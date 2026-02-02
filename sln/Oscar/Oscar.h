#pragma once
#include "httplib.h"
#include "json.hpp"
#include "TaskRegistry.h"

using json = nlohmann::json;

struct SConfig {
   bool isVerbose = false;
   std::string localHost = "127.0.0.1";
   std::string logPath = "oscar_log.txt";
   std::string tasksHelloRoute = "/tasks/hello";
   std::string tasksPostRoute = "/tasks";
   std::string tasksGetListRoute = "/tasks";
   std::string tasksGetOneRoute = R"(/tasks/([A-Za-z0-9\-_]+))";
   std::string tasksDeleteRoute = R"(/tasks/([A-Za-z0-9\-_]+))";
   std::string walrusHelloRoute = "/oscar/hello";
   std::string clubEventRoute = "/oscar/event";
   std::string clubStopRoute = "/griffins/club/stop";
   int threadPoolSize = 8;
};

struct TaskState {
   uint64_t last_seq = 0;
   uint64_t last_seen_ms = 0;
};

struct SServer {
   std::mutex mx;
   std::unordered_map<std::string, TaskState> tasks;
   std::ofstream log;

   bool AttemptHttpRun(int port);

   SServer();
   ~SServer() { LogFlush(); }
private:
   static SServer* _this;
   TaskRegistry reg;
   void LogOpen(const std::string& logPath);
   void LogLine(const std::string& line);
   void LogFlush();

   static void OutwardsHello(const httplib::Request&, httplib::Response& res);
   static void HelloWalrus(const httplib::Request& req, httplib::Response& res);
   static void HearClubEvent(const httplib::Request& req, httplib::Response& res);
   static void TasksGetList(const httplib::Request& req, httplib::Response& res);
   static void TasksPost(const httplib::Request& req, httplib::Response& res);
   static void TasksGetOne(const httplib::Request& req, httplib::Response& res);
   static void TasksDeleteOne(const httplib::Request& req, httplib::Response& res);

   void VerboseOut(OwlEvent& ev);
};

extern SConfig config;
extern SServer srv;

// helpers
const char* TaskStatusToStr(TaskStatus st);
std::string_view OwlEventTypeToStrView(OwlEventType t);
json SnapshotToJson(const TaskSnapshot& s, bool includeLogTail, const std::optional<std::string>& logTail);
DeleteMode ParseDeleteMode(const httplib::Request& req);
GetOptions ParseGetOptions(const httplib::Request& req);
uint64_t NowUnixMs();
void PrintLine(const std::string& line);
bool GetArgValue(int argc, char** argv, const char* key, std::string& out);
bool HasArg(int argc, char** argv, const char* key);
void EnsureDir(const std::string& d); 
void FillConfig(int argc, char** argv);
int GetHttpPort(int argc, char** argv);
std::string SafeTaskId(const httplib::Request& req);
bool ShouldDropBySeq(const std::string& taskId, uint64_t seq, uint64_t now);

