// TaskRegistry.cpp
#include <algorithm>
#include <fstream>
#include <sstream>
#include <vector>
#include <string_view>
#include "Oscar.h"

#pragma message("TaskRegistry.cpp REV: registry v0.6")

TaskRegistry::TaskRegistry()
   : m_logDir("oscar_tasks") // directory for per-task logs
{
   if (!m_logDir.empty() && (m_logDir.back() == '/' || m_logDir.back() == '\\')) {
      m_logDir.pop_back();
   }
   if (!m_logDir.empty()) EnsureDir(m_logDir);
}

std::shared_ptr<TaskRegistry::TaskEntry>
TaskRegistry::FindOrCreate_(const std::string& taskId, int64_t nowMs, bool* outCreated)
{
   std::lock_guard<std::mutex> g(m_mapMutex);

   auto it = m_tasks.find(taskId);
   if (it != m_tasks.end()) {
      if (outCreated) *outCreated = false;
      return it->second;
   }

   auto e = std::make_shared<TaskEntry>();
   e->s.id = taskId;
   e->s.status = TaskStatus::Started;
   e->s.seq = 0;
   e->s.createdMs = nowMs;
   e->s.lastSeenMs = nowMs;

   m_tasks.emplace(taskId, e);
   if (outCreated) *outCreated = true;
   return e;
}

TaskSnapshot TaskRegistry::CreateOrGet(const std::string& taskId,
   std::string name,
   int boardsNumber,
   int64_t nowMs)
{
   bool created = false;
   auto entry = FindOrCreate_(taskId, nowMs, &created);

   std::lock_guard<std::mutex> lk(entry->m);
   auto& s = entry->s;
   s.lastSeenMs = nowMs;

   if (created) {
      s.name = std::move(name);
      s.boardsNumber = boardsNumber;
   } else {
      // T3 fix-ready behavior: fill missing metadata without overwriting meaningful existing values
      if (s.name.empty() && !name.empty()) s.name = std::move(name);
      if (s.boardsNumber == 0 && boardsNumber != 0) s.boardsNumber = boardsNumber;
   }

   return s;
}

TaskSnapshot TaskRegistry::ApplyEvent(const OwlEvent& ev, int64_t nowMs)
{
   const std::string& taskId = ev.task_id;
   auto entry = FindOrCreate_(taskId, nowMs, nullptr);

   std::lock_guard<std::mutex> lk(entry->m);
   auto& s = entry->s;
   s.lastSeenMs = nowMs;

   if (IsTerminal(s.status) && ev.type != OwlEventType::Log) {
      return s;
   }

   // monotonic seq guard
   if (ev.seq <= s.seq) {
      return s;
   }
   s.seq = ev.seq;

   if (ev.percent >= 0) {
      s.percent = std::clamp(ev.percent, 0, 100);
      if (s.status == TaskStatus::Started && s.percent > 0) s.status = TaskStatus::InProgress;
   }
   if (!ev.message.empty()) s.message = ev.message;

   if (ev.type == OwlEventType::Done) {
      s.status = TaskStatus::Done;
      s.percent = 100;
      if (!ev.resultJson.empty()) s.resultJson = ev.resultJson;
   } else if (ev.type == OwlEventType::Fail) {
      s.status = TaskStatus::Failed;
      if (!ev.message.empty()) s.errorText = ev.message;
   } else if (ev.type == OwlEventType::Progress) {
      if (s.status == TaskStatus::Started) s.status = TaskStatus::InProgress;
   }

   // Append to per-task log (tiny, safe)
   AppendLogLine(LogPathForTask(taskId), ev.seq, ev.type, ev.percent, ev.message, ev.resultJson);
   return s;
}

std::optional<GetResult> TaskRegistry::Get(const std::string& taskId, const GetOptions& opt) const
{
   std::shared_ptr<TaskEntry> entry;
   {
      std::lock_guard<std::mutex> g(m_mapMutex);
      auto it = m_tasks.find(taskId);
      if (it == m_tasks.end()) return std::nullopt;
      entry = it->second; // copy shared_ptr => safe after unlock
   }

   GetResult out;
   {
      std::lock_guard<std::mutex> lk(entry->m);
      out.snapshot = entry->s;
   }

   if (opt.includeLogTail) {
      out.logTail = ReadTail(LogPathForTask(taskId), opt.logTailBytes);
   }
   return out;
}

std::optional<TaskSnapshot> TaskRegistry::Delete(const std::string& taskId, DeleteMode mode, int64_t nowMs)
{
   std::shared_ptr<TaskEntry> entry;
   {
      std::lock_guard<std::mutex> g(m_mapMutex);
      auto it = m_tasks.find(taskId);
      if (it == m_tasks.end()) return std::nullopt;
      entry = it->second;
   }

   TaskSnapshot snap;
   bool shouldEvict = false;

   {
      std::lock_guard<std::mutex> lk(entry->m);
      auto& s = entry->s;
      s.lastSeenMs = nowMs;

      if (!IsTerminal(s.status)) {
         // v0.5 behavior: optimistic cancel
         s.status = TaskStatus::Canceled;
         s.cancelRequested = true;
         s.message = "Canceled by user";

         // courtesy semantics: explicit ack can force eviction
         shouldEvict = (mode == DeleteMode::Ack);
      } else {
         shouldEvict = true;
      }
      snap = s;
   }

   if (shouldEvict) {
      std::lock_guard<std::mutex> g(m_mapMutex);
      auto it = m_tasks.find(taskId);
      // avoid erasing a new task if the ID got re-created/reused somehow
      if (it != m_tasks.end() && it->second == entry) {
         m_tasks.erase(it);
      }
   }
   return snap;
}

size_t TaskRegistry::SweepByLastSeen(int64_t nowMs, int64_t maxAgeMs)
{
   // Copy snapshot of entries first (short map lock), then evaluate outside if desired.
   // Minimal v0.5: do it in two phases.
   std::vector<std::pair<std::string, std::shared_ptr<TaskEntry>>> items;
   {
      std::lock_guard<std::mutex> g(m_mapMutex);
      items.reserve(m_tasks.size());
      for (auto& kv : m_tasks) items.push_back(kv);
   }

   std::vector<std::string> toDrop;
   toDrop.reserve(items.size());
   for (auto& kv : items) {
      const auto& id = kv.first;
      const auto& entry = kv.second;
      std::lock_guard<std::mutex> lk(entry->m);
      if (nowMs - entry->s.lastSeenMs > maxAgeMs) {
         toDrop.push_back(id);
      }
   }

   if (toDrop.empty()) return 0;

   size_t dropped = 0;
   {
      std::lock_guard<std::mutex> g(m_mapMutex);
      for (const auto& id : toDrop) {
         auto it = m_tasks.find(id);
         if (it != m_tasks.end()) {
            m_tasks.erase(it);
            ++dropped;
         }
      }
   }
   return dropped;
}

TaskRegistry::ListResult TaskRegistry::ListAll() const
{
   std::vector<std::shared_ptr<TaskEntry>> entries;
   {
      std::lock_guard<std::mutex> g(m_mapMutex);
      entries.reserve(m_tasks.size());
      for (auto& kv : m_tasks) entries.push_back(kv.second);
   }

   ListResult out;
   out.totalElements = entries.size();
   out.tasks.reserve(entries.size());

   for (auto& e : entries) {
      std::lock_guard<std::mutex> lk(e->m);
      out.tasks.push_back(e->s);
   }
   return out;
}

bool IsTerminal(TaskStatus st)
{
   return st == TaskStatus::Done || st == TaskStatus::Failed || st == TaskStatus::Canceled;
}

std::string TaskRegistry::SanitizeForFilename(const std::string& taskId)
{
   std::string out;
   out.reserve(taskId.size());
   for (unsigned char c : taskId) {
      const bool ok =
         (c >= '0' && c <= '9') ||
         (c >= 'a' && c <= 'z') ||
         (c >= 'A' && c <= 'Z') ||
         (c == '-') || (c == '_');
      out.push_back(ok ? char(c) : '_');
   }
   if (out.size() > 96) out.resize(96);
   return out;
}

std::string TaskRegistry::LogPathForTask(const std::string& taskId) const
{
   return m_logDir + "/task_" + SanitizeForFilename(taskId) + ".log";
}

void TaskRegistry::AppendLogLine(const std::string& path, uint64_t seq, const OwlEventType type,
   int percent, const std::string& msg, const std::string& json)
{
   std::ofstream f(path, std::ios::binary | std::ios::app);
   if (!f) return;

   // Record-oriented line; still readable, easy to tail/parse.
   // You can swap '\n' for '\x1E' (RS) if you want strict delimiter behavior.
   f << "seq=" << seq
      << " type=" << OwlEventTypeToStrView(type);
   if (percent >= 0) f << " pct=" << percent;
   if (!msg.empty())  f << " msg=" << msg;
   if (!json.empty()) f << " json=" << json;
   f << "\n";
}

std::optional<std::string> TaskRegistry::ReadTail(const std::string& path, size_t tailBytes)
{
   std::ifstream f(path, std::ios::binary);
   if (!f) return std::nullopt;

   f.seekg(0, std::ios::end);
   std::streamoff len = f.tellg();
   if (len <= 0) return std::string{};

   std::streamoff start = std::max<std::streamoff>(0, len - (std::streamoff)tailBytes);
   f.seekg(start, std::ios::beg);

   std::string buf;
   buf.resize((size_t)(len - start));
   f.read(buf.data(), (std::streamsize)buf.size());
   return buf;
}
