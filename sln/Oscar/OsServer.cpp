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

#pragma message("OsServer.cpp REV: hello v0.9")

using json = nlohmann::json;

static std::string g_logPath;
static std::ofstream g_log;
static std::mutex g_mx;

struct TaskState {
   uint64_t last_seq = 0;
   uint64_t last_seen_ms = 0;
};

static std::unordered_map<std::string, TaskState> g_tasks;

static uint64_t now_unix_ms()
{
   using namespace std::chrono;
   return (uint64_t)duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

static void log_line(const std::string& line)
{
   std::lock_guard<std::mutex> lk(g_mx);
   if (g_log.is_open()) {
      g_log << line << "\n";
      g_log.flush();
   }
}

static void print_line(const std::string& line)
{
   std::lock_guard<std::mutex> lk(g_mx);
   std::cout << line << std::endl;
}

static bool get_arg_value(int argc, char** argv, const char* key, std::string& out)
{
   for (int i = 1; i + 1 < argc; ++i) {
      if (std::string(argv[i]) == key) {
         out = argv[i + 1];
         return true;
      }
   }
   return false;
}

static bool has_arg(int argc, char** argv, const char* key)
{
   for (int i = 1; i < argc; ++i) {
      if (std::string(argv[i]) == key) return true;
   }
   return false;
}

static int get_http_port(int argc, char** argv)
{
   std::string portStr;
   if (!get_arg_value(argc, argv, "-http", portStr)) return -1;
   try { return std::stoi(portStr); }
   catch (...) { return -1; }
}

static std::string safe_task_id(const httplib::Request& req)
{
   auto it = req.get_param_value("task_id");
   if (!it.empty()) return it;
   return "unknown";
}

static bool should_drop_by_seq(const std::string& task_id, uint64_t seq, uint64_t now)
{
   std::lock_guard<std::mutex> lk(g_mx);
   auto& st = g_tasks[task_id];
   st.last_seen_ms = now;
   if (seq != 0 && seq <= st.last_seq) return true;
   if (seq != 0) st.last_seq = seq;
   return false;
}

static void listener_hello(const httplib::Request& req, httplib::Response& res)
{
   const auto task_id = safe_task_id(req);
   const auto now = now_unix_ms();

   {
      std::lock_guard<std::mutex> lk(g_mx);
      g_tasks[task_id].last_seen_ms = now;
   }

   // Keep body simple for now (you can JSON later).
   std::string body = "Oscar ready v0.7; task_id=" + task_id;
   res.set_content(body, "text/plain");
}

static void listener_event(const httplib::Request& req, httplib::Response& res)
{
   const auto now = now_unix_ms();

   try {
      json j = json::parse(req.body);

      const std::string type = j.value("type", "log");
      const std::string task_id = j.value("task_id", "unknown");
      const uint64_t seq = j.value("seq", (uint64_t)0);
      const uint64_t unix_ms = j.value("unix_ms", (uint64_t)0);
      const bool silent = j.value("silent", false);
      const int percent = j.value("percent", -1);
      const std::string msg = j.value("message", "");

      if (should_drop_by_seq(task_id, seq, now)) {
         res.status = 200;
         res.set_content("dup", "text/plain");
         return;
      }

      // Format line for console and/or log
      std::string line;
      line.reserve(msg.size() + 128);

      line += "[";
      line += task_id;
      line += "] ";
      line += type;

      if (type == "progress" && percent >= 0) {
         line += " ";
         line += std::to_string(percent);
         line += "%";
      }

      // timestamp (optional)
      if (unix_ms != 0) {
         line += " @";
         line += std::to_string(unix_ms);
      }

      if (!msg.empty()) {
         line += ": ";
         line += msg;
      }

      // Always log if log file is enabled; print only if not silent
      log_line(line);
      if (!silent) print_line(line);

      res.status = 200;
      res.set_content("ok", "text/plain");
   }
   catch (const std::exception& e) {
      res.status = 400;
      res.set_content(std::string("bad json: ") + e.what(), "text/plain");
   }
}

bool OscarAttemptHttpRun(int argc, char** argv)
{
   // Only real switches we care about:
   //   -http <port>   (required for HTTP-only mode)
   //   -logresult <path>
   int port = get_http_port(argc, argv);
   if (port <= 0) {
      return false;
   }

   std::string logPath;
   if (get_arg_value(argc, argv, "-logresult", logPath)) {
      g_logPath = logPath;
      g_log.open(g_logPath, std::ios::out | std::ios::app);
      if (!g_log.is_open()) {
         std::cerr << "Oscar: failed to open log file: " << g_logPath << "\n";
      }
   }

   print_line("Oscar (http-only) ready for creation.");
   httplib::Server svr;

   // Optional: small thread pool (cpp-httplib supports a custom task queue).
   svr.new_task_queue = [] {
       return new httplib::ThreadPool(8);
   };

   // Listeners
   svr.Get("/oscar/hello", listener_hello);
   svr.Post("/oscar/event", listener_event);

   // Main loop
   print_line("Oscar starts listening on 127.0.0.1:" + std::to_string(port));
   if (!svr.listen("127.0.0.1", port)) { // Bind loopback explicitly (avoids firewall drama).
      std::cerr << "Oscar: listen failed on 127.0.0.1:" << port << "\n";
      return false;
   }

   // normal exit
   return true;
}
