// OsServer.cpp

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <chrono>

#include "httplib.h"
#include "json.hpp"

#pragma message("OsServer.cpp REV: hello v1.0")

using json = nlohmann::json;

struct SConfig {
   std::string logPath;
};

struct TaskState {
   uint64_t last_seq = 0;
   uint64_t last_seen_ms = 0;
};

struct SServer {
   std::mutex mx;
   std::unordered_map<std::string, TaskState> tasks;
   std::ofstream log;
};

static SConfig config;
static SServer srv;


static uint64_t NowUnixMs()
{
   using namespace std::chrono;
   return (uint64_t)duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

static void LogLine(const std::string& line)
{
   std::lock_guard<std::mutex> lk(srv.mx);
   if (srv.log.is_open()) {
      srv.log << line << "\n";
      srv.log.flush();
   }
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

static int getHttpPort(int argc, char** argv)
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
   if (seq != 0 && seq <= st.last_seq) return true;
   if (seq != 0) st.last_seq = seq;
   return false;
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

static void ListenerEvent(const httplib::Request& req, httplib::Response& res)
{
   const auto now = NowUnixMs();

   try {
      json j = json::parse(req.body);

      const std::string type = j.value("type", "log");
      const std::string taskId = j.value("task_id", "unknown");
      const uint64_t seq = j.value("seq", (uint64_t)0);
      const uint64_t unixMs = j.value("unix_ms", (uint64_t)0);
      const bool silent = j.value("silent", false);
      const int percent = j.value("percent", -1);
      const std::string msg = j.value("message", "");

      if (ShouldDropBySeq(taskId, seq, now)) {
         res.status = 200;
         res.set_content("dup", "text/plain");
         return;
      }

      std::string line;
      line.reserve(msg.size() + 128);

      line += "[";
      line += taskId;
      line += "] ";
      line += type;

      if (type == "progress" && percent >= 0) {
         line += " ";
         line += std::to_string(percent);
         line += "%";
      }

      if (!msg.empty()) {
         line += ": ";
         line += msg;
      }

      LogLine(line);
      if (!silent) PrintLine(line);

      res.status = 200;
      res.set_content("ok", "text/plain");
   }
   catch (const std::exception& e) {
      res.status = 400;
      res.set_content(std::string("bad json: ") + e.what(), "text/plain");
   }
}

static void FillConfig(int argc, char** argv)
{
   std::string logPath;
   if (GetArgValue(argc, argv, "-logresult", logPath)) {
      config.logPath = logPath;
      srv.log.open(config.logPath, std::ios::out | std::ios::app);
      if (!srv.log.is_open()) {
         std::cerr << "Oscar: failed to open log file: " << config.logPath << "\n";
      }
   }
}

bool OscarAttemptHttpRun(int argc, char** argv)
{
   int port = getHttpPort(argc, argv);
   if (port <= 0) {
      return false;
   }

   FillConfig(argc, argv);

   PrintLine("Oscar (http-only) ready for creation.");
   httplib::Server svr;

   svr.new_task_queue = [] {
       return new httplib::ThreadPool(8);
   };

   svr.Get("/oscar/hello", ListenerHello);
   svr.Post("/oscar/event", ListenerEvent);

   svr.Post("/oscar/stop", [&](const httplib::Request&, httplib::Response& res) {
      res.set_content("stopping", "text/plain");
      svr.stop();
   });

   PrintLine("Oscar starts listening on 127.0.0.1:" + std::to_string(port));
   if (!svr.listen("127.0.0.1", port)) {
      std::cerr << "Oscar: listen failed on 127.0.0.1:" << port << "\n";
      return false;
   }

   return true;
}
