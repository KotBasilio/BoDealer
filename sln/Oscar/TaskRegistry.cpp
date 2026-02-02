// TaskRegistry.cpp
#include "TaskRegistry.h"
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

TaskSnapshot TaskRegistry::CreateOrGet(const std::string& taskId,
   std::string name,
   int boardsNumber,
   int64_t nowMs)
{
   std::unique_ptr<TaskEntry>* slot = nullptr;
   {
      std::lock_guard<std::mutex> g(m_mapMutex);
      auto it = m_tasks.find(taskId);
      if (it == m_tasks.end()) {
         auto e = std::make_unique<TaskEntry>();
         e->s.id = taskId;
         e->s.name = std::move(name);
         e->s.boardsNumber = boardsNumber;
         e->s.status = TaskStatus::Started;
         e->s.seq = 0;
         e->s.createdMs = nowMs;
         e->s.lastSeenMs = nowMs;
         auto [insIt, _] = m_tasks.emplace(taskId, std::move(e));
         slot = &insIt->second;
      } else {
         slot = &it->second;
      }
   }

   // Update mutable fields under per-task lock (only if we created; else leave as-is)
   std::lock_guard<std::mutex> lk((*slot)->m);
   (*slot)->s.lastSeenMs = nowMs;
   return (*slot)->s;
}

TaskSnapshot TaskRegistry::ApplyEvent(const OwlEvent& ev, int64_t nowMs)
{
   const std::string& taskId = ev.task_id;

   // Ensure task exists (if Walter races before UI GET/POST, we still accept)
   std::unique_ptr<TaskEntry>* slot = nullptr;
   {
      std::lock_guard<std::mutex> g(m_mapMutex);
      auto it = m_tasks.find(taskId);
      if (it == m_tasks.end()) {
         auto e = std::make_unique<TaskEntry>();
         e->s.id = taskId;
         e->s.name = ""; // unknown unless UI set
         e->s.boardsNumber = 0;
         e->s.status = TaskStatus::Started;
         e->s.seq = 0;
         e->s.createdMs = nowMs;
         e->s.lastSeenMs = nowMs;
         auto [insIt, _] = m_tasks.emplace(taskId, std::move(e));
         slot = &insIt->second;
      } else {
         slot = &it->second;
      }
   }

   std::lock_guard<std::mutex> lk((*slot)->m);
   auto& s = (*slot)->s;
   s.lastSeenMs = nowMs;

   // Ignore updates after terminal unless it's just logging (optional)
   if (IsTerminal(s.status) && ev.type != OwlEventType::Log) {
      return s;
   }

   // Enforce monotonic seq (idempotent-ish)
   if (ev.seq <= s.seq) {
      return s;
   }
   s.seq = ev.seq;

   // Update snapshot
   if (ev.percent >= 0) {
      s.percent = std::clamp(ev.percent, 0, 100);
      if (s.status == TaskStatus::Started && s.percent > 0) {
         s.status = TaskStatus::InProgress;
      }
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
   // ev.type == "log" -> snapshot unchanged aside from message/seq

   // Append to per-task log (tiny, safe)
   AppendLogLine(LogPathForTask(taskId), ev.seq, ev.type, ev.percent, ev.message, ev.resultJson);
   return s;
}

std::optional<GetResult> TaskRegistry::Get(const std::string& taskId, const GetOptions& opt) const
{
   std::unique_ptr<TaskEntry>* slot = nullptr;
   {
      std::lock_guard<std::mutex> g(m_mapMutex);
      auto it = m_tasks.find(taskId);
      if (it == m_tasks.end()) return std::nullopt;
      slot = const_cast<std::unique_ptr<TaskEntry>*>(&it->second);
   }

   GetResult out;
   {
      std::lock_guard<std::mutex> lk((*slot)->m);
      out.snapshot = (*slot)->s;
   }

   if (opt.includeLogTail) {
      out.logTail = ReadTail(LogPathForTask(taskId), opt.logTailBytes);
   }
   return out;
}

std::optional<TaskSnapshot> TaskRegistry::Delete(const std::string& taskId, DeleteMode mode, int64_t nowMs)
{
   std::unique_ptr<TaskEntry>* slot = nullptr;
   {
      std::lock_guard<std::mutex> g(m_mapMutex);
      auto it = m_tasks.find(taskId);
      if (it == m_tasks.end()) return std::nullopt;
      slot = &it->second;
   }

   TaskSnapshot snap;
   bool shouldEvict = false;

   {
      std::lock_guard<std::mutex> lk((*slot)->m);
      auto& s = (*slot)->s;
      s.lastSeenMs = nowMs;

      if (!IsTerminal(s.status)) {
         // Running: cancellation request
         s.status = TaskStatus::Canceled;
         s.cancelRequested = true;
         s.message = "Canceled by user";
         // We do NOT evict immediately by default; keep until UI acks or TTL sweeps,
         // unless you prefer aggressive eviction.
         shouldEvict = (mode == DeleteMode::Ack); // courtesy: if they explicitly ack, evict
      } else {
         // Terminal: allow eviction
         shouldEvict = true;
      }
      snap = s;
   }

   if (shouldEvict) {
      std::lock_guard<std::mutex> g(m_mapMutex);
      m_tasks.erase(taskId);
      // optional: delete log file here if you want
      // ::remove(LogPathForTask(taskId).c_str());
   }
   return snap;
}

size_t TaskRegistry::SweepByLastSeen(int64_t nowMs, int64_t maxAgeMs)
{
   std::vector<std::string> toDrop;
   {
      std::lock_guard<std::mutex> g(m_mapMutex);
      toDrop.reserve(m_tasks.size());
      for (auto& kv : m_tasks) {
         auto& entry = kv.second;
         std::lock_guard<std::mutex> lk(entry->m);
         if (nowMs - entry->s.lastSeenMs > maxAgeMs) {
            toDrop.push_back(kv.first);
         }
      }
      for (auto& id : toDrop) m_tasks.erase(id);
   }
   return toDrop.size();
}

bool TaskRegistry::IsTerminal(TaskStatus st)
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

TaskRegistry::ListResult TaskRegistry::ListAll() const
{
   ListResult out;
   std::lock_guard<std::mutex> g(m_mapMutex);
   out.totalElements = m_tasks.size();
   out.tasks.reserve(m_tasks.size());

   for (auto& kv : m_tasks) {
      auto& entry = kv.second;
      std::lock_guard<std::mutex> lk(entry->m);
      out.tasks.push_back(entry->s);
   }
   return out;
}
