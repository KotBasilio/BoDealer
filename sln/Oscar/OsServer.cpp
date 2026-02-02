// OsServer.cpp
#include "TaskRegistry.h"

#include <iostream>
#include <fstream>
#include <atomic>

#include "httplib.h"
#include "json.hpp"
#include "Oscar.h"

#pragma message("OsServer.cpp REV: registry v0.5")

using json = nlohmann::json;

struct SConfig {
   std::string localHost = "127.0.0.1";
   std::string logPath = "oscar_log.txt";
   bool isVerbose = false;
};

struct TaskState {
   uint64_t last_seq = 0;
   uint64_t last_seen_ms = 0;
};

struct SServer {
   std::mutex mx;
   std::unordered_map<std::string, TaskState> tasks;
   std::ofstream log;

   void LogOpen(const std::string& logPath)
   {
      log.open(logPath, std::ios::out | std::ios::app);
      if (!log.is_open()) {
         std::cerr << "HTTP --> failed to open log file: " << logPath << "\n";
      } else {
         std::cout << "HTTP --> logging to file: " << logPath << "\n";
      }
   }

   void LogLine(const std::string& line)
   {
      std::lock_guard<std::mutex> lk(mx);
      if (log.is_open()) {
         log << line;
      }
   }

   void LogFlush()
   {
      std::lock_guard<std::mutex> lk(mx);
      if (log.is_open()) {
         log.flush();
      }
   }

   ~SServer()
   {
      LogFlush();
   }
};

static SConfig config;
static SServer srv;
static TaskRegistry g_tasks("oscar_tasks"); // directory for per-task logs

static const char* TaskStatusToStr(TaskStatus st)
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

static json SnapshotToJson(const TaskSnapshot& s, bool includeLogTail, const std::optional<std::string>& logTail)
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

static DeleteMode ParseDeleteMode(const httplib::Request& req)
{
   if (!req.has_param("mode")) return DeleteMode::Auto;
   auto m = req.get_param_value("mode");
   if (m == "cancel") return DeleteMode::Cancel;
   if (m == "ack")    return DeleteMode::Ack;
   return DeleteMode::Auto;
}

static GetOptions ParseGetOptions(const httplib::Request& req)
{
   GetOptions opt;
   opt.includeLogTail = req.has_param("log_req") && req.get_param_value("log_req") == "1";
   // You can also allow ?tail=8192 if you want, but keeping minimal:
   return opt;
}

static uint64_t NowUnixMs()
{
   using namespace std::chrono;
   return (uint64_t)duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

static void PrintLine(const std::string& line)
{
   std::lock_guard<std::mutex> lk(srv.mx);
   std::cout << line << std::endl;
}

static bool GetArgValue(int argc, char** argv, const char* key, std::string& out)
{
   for (int i = 1; i + 1 < argc; ++i) {
      if (std::string(argv[i]) == key) {
         out = argv[i + 1];
         return true;
      }
   }
   return false;
}

static bool HasArg(int argc, char** argv, const char* key)
{
   for (int i = 1; i < argc; ++i) {
      if (std::string(argv[i]) == key) return true;
   }
   return false;
}

static void FillConfig(int argc, char** argv)
{
   std::string logPath;
   if (GetArgValue(argc, argv, "-logresult", logPath)) {
      config.logPath = logPath;
   }

   if (HasArg(argc, argv, "-verbose")) {
      config.isVerbose = true;
   }
}

static int GetHttpPort(int argc, char** argv)
{
   std::string portStr;
   if (!GetArgValue(argc, argv, "-http", portStr)) return -1;
   try { return std::stoi(portStr); }
   catch (...) { return -1; }
}

static std::string SafeTaskId(const httplib::Request& req)
{
   auto it = req.get_param_value("task_id");
   if (!it.empty()) return it;
   return "unknown";
}

static bool ShouldDropBySeq(const std::string& taskId, uint64_t seq, uint64_t now)
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

static void ListenerHello(const httplib::Request& req, httplib::Response& res)
{
   const auto taskId = SafeTaskId(req);
   const auto now = NowUnixMs();

   {
      std::lock_guard<std::mutex> lk(srv.mx);
      srv.tasks[taskId].last_seen_ms = now;
   }

   std::string body = "Oscar ready v0.7; task_id=" + taskId;
   res.set_content(body, "text/plain");
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

static void ListenerEvent(const httplib::Request& req, httplib::Response& res)
{
   const auto now = NowUnixMs();

   OwlEvent ev;
   if (!ev.AttemptParse(req.body)) {
      res.status = 400;
      res.set_content("bad json", "text/plain");
      return;
   }

   if (ShouldDropBySeq(ev.task_id, ev.seq, now)) {
      res.status = 200;
      res.set_content("dup", "text/plain");
      return;
   }

   VerboseOut(ev);

   g_tasks.ApplyEvent(ev, now);

   res.status = 200;
   res.set_content("ok", "text/plain");
}

static void TasksGetList(const httplib::Request& req, httplib::Response& res)
{
   // minimal v0: ignore filters; you can add limit/offset later
   auto lst = g_tasks.ListAll();

   json out;
   out["totalElements"] = lst.totalElements;
   out["tasks"] = json::array();

   for (const auto& s : lst.tasks) {
      out["tasks"].push_back(SnapshotToJson(s, false, std::nullopt));
   }

   res.status = 200;
   res.set_content(out.dump(), "application/json");
}

static void TasksHello(const httplib::Request&, httplib::Response& res)
{
   res.set_content("Tasks API ready v0.1", "text/plain");
}

// POST /tasks
// Accepts JSON like:
// { "task_id": "...", "name":"...", "boardsNumber": 777 }
// or { "id": "...", ... }
static void TasksPost(const httplib::Request& req, httplib::Response& res)
{
   const auto now = NowUnixMs();

   json j;
   try { j = json::parse(req.body); }
   catch (...) {
      res.status = 400;
      res.set_content("bad json", "text/plain");
      return;
   }

   const std::string taskId = j.value("task_id", j.value("id", std::string{}));
   if (taskId.empty()) {
      res.status = 400;
      res.set_content("missing task_id", "text/plain");
      return;
   }

   std::string name = j.value("name", std::string{});
   int boardsNumber = j.value("boardsNumber", 0);

   auto snap = g_tasks.CreateOrGet(taskId, std::move(name), boardsNumber, now);

   json out = SnapshotToJson(snap, false, std::nullopt);
   res.status = 200;
   res.set_content(out.dump(), "application/json");
}

// GET /tasks/:id  (implemented via regex route)
static void TasksGetOne(const httplib::Request& req, httplib::Response& res)
{
   const auto opt = ParseGetOptions(req);

   // Expect first capture group to be id
   if (req.matches.size() < 2) {
      res.status = 400;
      res.set_content("missing id", "text/plain");
      return;
   }
   const std::string taskId = req.matches[1];

   auto got = g_tasks.Get(taskId, opt);
   if (!got.has_value()) {
      res.status = 404;
      res.set_content("not found", "text/plain");
      return;
   }

   json out = SnapshotToJson(got->snapshot, opt.includeLogTail, got->logTail);
   res.status = 200;
   res.set_content(out.dump(), "application/json");
}

// DELETE /tasks/:id
static void TasksDeleteOne(const httplib::Request& req, httplib::Response& res)
{
   const auto now = NowUnixMs();
   const auto mode = ParseDeleteMode(req);

   if (req.matches.size() < 2) {
      res.status = 400;
      res.set_content("missing id", "text/plain");
      return;
   }
   const std::string taskId = req.matches[1];

   auto snap = g_tasks.Delete(taskId, mode, now);
   if (!snap.has_value()) {
      // idempotent delete is nice: treat as ok
      res.status = 200;
      res.set_content(R"({"ok":true,"alreadyDeleted":true})", "application/json");
      return;
   }

   json out = SnapshotToJson(*snap, false, std::nullopt);
   out["ok"] = true;
   out["deleteMode"] = (mode == DeleteMode::Cancel ? "cancel" : mode == DeleteMode::Ack ? "ack" : "auto");

   res.status = 200;
   res.set_content(out.dump(), "application/json");
}

bool OscarAttemptHttpRun(int argc, char** argv)
{
   // prep
   int port = GetHttpPort(argc, argv);
   if (port <= 0) {
      return false;
   }
   FillConfig(argc, argv);
   srv.LogOpen(config.logPath);
   PrintLine("Oscar (http-only) ready for creation.");

   // setup
   httplib::Server htsvr;
   htsvr.new_task_queue = [] {
       return new httplib::ThreadPool(8);
   };
   // -- routes to walruses
   htsvr.Get("/oscar/hello", ListenerHello);
   htsvr.Post("/oscar/event", ListenerEvent);
   htsvr.Post("/oscar/stop", [&](const httplib::Request&, httplib::Response& res) {
      res.set_content("stopping", "text/plain");
      htsvr.stop();
   });
   // -- routes outwards to UI
   htsvr.Get("/tasks/hello", TasksHello);
   htsvr.Post("/tasks", TasksPost);
   htsvr.Get("/tasks", TasksGetList);
   htsvr.Get(R"(/tasks/([A-Za-z0-9\-_]+))", TasksGetOne);
   htsvr.Delete(R"(/tasks/([A-Za-z0-9\-_]+))", TasksDeleteOne);

   // listen
   PrintLine("Oscar starts listening on " + config.localHost + ":" + std::to_string(port));
   if (!htsvr.listen(config.localHost.c_str(), port)) {
      std::cerr << "Oscar: listen failed on " << config.localHost << ":" << port << "\n";
      return false;
   }

   return true;
}

