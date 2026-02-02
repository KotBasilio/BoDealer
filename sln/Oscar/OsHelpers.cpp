// OsHelpers.cpp
#include "TaskRegistry.h"

#include <iostream>
#include <fstream>
#include <atomic>

#include "Oscar.h"

#pragma message("OsHelpers.cpp REV: registry v0.6")

using json = nlohmann::json;

#if defined(_WIN32)
#include <direct.h>
void EnsureDir(const std::string& d) { _mkdir(d.c_str()); }
#else
#include <sys/stat.h>
void EnsureDir(const std::string& d) { ::mkdir(d.c_str(), 0755); }
#endif

const char* TaskStatusToStr(TaskStatus st)
{
   switch (st) {
      case TaskStatus::Started:    return "STARTED";
      case TaskStatus::InProgress: return "IN_PROGRESS";
      case TaskStatus::Done:       return "DONE";
      case TaskStatus::Failed:     return "FAILED";
      case TaskStatus::Canceled:   return "CANCELED";
      default:                     return "UNKNOWN";
   }
}

std::string_view OwlEventTypeToStrView(OwlEventType t)
{
   switch (t) {
      case OwlEventType::Log:      return "log";
      case OwlEventType::Progress: return "progress";
      case OwlEventType::Done:     return "done";
      case OwlEventType::Fail:     return "fail";
      default:                     return "unknown";
   }
}

json SnapshotToJson(const TaskSnapshot& s, bool includeLogTail, const std::optional<std::string>& logTail)
{
   json j;
   j["id"] = s.id;
   j["name"] = s.name;
   j["boardsNumber"] = s.boardsNumber;
   j["status"] = TaskStatusToStr(s.status);
   j["seq"] = s.seq;
   j["percent"] = s.percent;
   j["message"] = s.message;

   if (!s.resultJson.empty()) j["resultJson"] = s.resultJson;
   if (!s.errorText.empty())  j["errorText"] = s.errorText;
   j["cancelRequested"] = s.cancelRequested;

   j["createdMs"] = s.createdMs;
   j["lastSeenMs"] = s.lastSeenMs;

   if (includeLogTail) {
      j["logTail"] = logTail.has_value() ? *logTail : "";
   }
   return j;
}

DeleteMode ParseDeleteMode(const httplib::Request& req)
{
   if (!req.has_param("mode")) return DeleteMode::Auto;
   auto m = req.get_param_value("mode");
   if (m == "cancel") return DeleteMode::Cancel;
   if (m == "ack")    return DeleteMode::Ack;
   return DeleteMode::Auto;
}

GetOptions ParseGetOptions(const httplib::Request& req)
{
   GetOptions opt;
   opt.includeLogTail = req.has_param("log_req") && req.get_param_value("log_req") == "1";
   // You can also allow ?tail=8192 if you want, but keeping minimal:
   return opt;
}

uint64_t NowUnixMs()
{
   using namespace std::chrono;
   return (uint64_t)duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

void PrintLine(const std::string& line)
{
   std::lock_guard<std::mutex> lk(srv.mx);
   std::cout << line << std::endl;
}

bool GetArgValue(int argc, char** argv, const char* key, std::string& out)
{
   for (int i = 1; i + 1 < argc; ++i) {
      if (std::string(argv[i]) == key) {
         out = argv[i + 1];
         return true;
      }
   }
   return false;
}

bool HasArg(int argc, char** argv, const char* key)
{
   for (int i = 1; i < argc; ++i) {
      if (std::string(argv[i]) == key) return true;
   }
   return false;
}

void FillConfig(int argc, char** argv)
{
   std::string logPath;
   if (GetArgValue(argc, argv, "-logresult", logPath)) {
      config.logPath = logPath;
   }

   if (HasArg(argc, argv, "-verbose")) {
      config.isVerbose = true;
   }
}

int GetHttpPort(int argc, char** argv)
{
   std::string portStr;
   if (!GetArgValue(argc, argv, "-http", portStr)) return -1;
   try { return std::stoi(portStr); }
   catch (...) { return -1; }
}

std::string SafeTaskId(const httplib::Request& req)
{
   auto it = req.get_param_value("task_id");
   if (!it.empty()) return it;
   return "unknown";
}

bool ShouldDropBySeq(const std::string& taskId, uint64_t seq, uint64_t now)
{
   std::lock_guard<std::mutex> lk(srv.mx);
   auto& st = srv.tasks[taskId];
   st.last_seen_ms = now;
   if (seq != 0) {
      if (seq <= st.last_seq) return true;
      st.last_seq = seq;
   }
   return false;
}

bool OwlEvent::AttemptParse(const std::string& body)
{
   try {
      json j = json::parse(body);

      auto valType = j.value("type", "log");
      type = valType == "progress" ? OwlEventType::Progress :
             valType == "done" ? OwlEventType::Done : 
             OwlEventType::Log;
      task_id = j.value("task_id", "unknown");
      seq = j.value("seq", (uint64_t)0);
      unix_ms = j.value("unix_ms", (uint64_t)0);
      silent = j.value("silent", false);
      percent = j.value("percent", -1);
      message = j.value("message", "");

      return true;
   }
   catch (const std::exception& e) {
      std::cerr << "OwlEvent::AttemptParse: JSON parse error: " << e.what() << "\n";
      return false;
   }
}

void VerboseOut(OwlEvent& ev)
{
   if (!config.isVerbose) {
      std::cout << '.';
      if (!(ev.seq & 0x03f)) {
         std::cout << std::endl;
      }
      srv.LogLine(ev.message);
      return;
   }

   std::string line;
   line.reserve(ev.message.size() + 128);

   line += "[";
   line += ev.task_id;
   line += "] ";
   line += ev.type == OwlEventType::Progress ? "progress" :
      ev.type == OwlEventType::Done ? "done" : "log";

   if (ev.type == OwlEventType::Progress && ev.percent >= 0) {
      line += " ";
      line += std::to_string(ev.percent);
      line += "%";
   }

   if (!ev.message.empty()) {
      line += ": ";
      line += ev.message;
   }

   srv.LogLine(ev.message);
   if (!ev.silent) PrintLine(line);
}

void SServer::LogOpen(const std::string& logPath)
{
   log.open(logPath, std::ios::out | std::ios::app);
   if (!log.is_open()) {
      std::cerr << "HTTP --> failed to open log file: " << logPath << "\n";
   } else {
      std::cout << "HTTP --> logging to file: " << logPath << "\n";
   }
}

void SServer::LogLine(const std::string& line)
{
   std::lock_guard<std::mutex> lk(mx);
   if (log.is_open()) {
      log << line;
   }
}

void SServer::LogFlush()
{
   std::lock_guard<std::mutex> lk(mx);
   if (log.is_open()) {
      log.flush();
   }
}
