// Oscar.h 
#pragma once
#include "httplib.h"
#include "json.hpp"
#include "TaskRegistry.h"

#pragma message("Oscar.h  REV: registry v0.8")

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

class SServer {
   static SServer* _this;
   std::mutex mx;
   std::ofstream log;
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
   void PrintLine(const std::string& line);

   void HandleClubEvent(OwlEvent& ev, httplib::Response& res);
   std::string HandleHelloWalrus(const httplib::Request& req);

public:
   SServer();
   ~SServer();
   bool AttemptHttpRun(int port);
};

extern SConfig config;
extern SServer srv;

// helpers
std::string_view OwlEventTypeToStrView(OwlEventType t);
json SnapshotToJson(const TaskSnapshot& s, bool includeLogTail, const std::optional<std::string>& logTail);
DeleteMode ParseDeleteMode(const httplib::Request& req);
GetOptions ParseGetOptions(const httplib::Request& req);
uint64_t NowUnixMs();
bool GetArgValue(int argc, char** argv, const char* key, std::string& out);
void EnsureDir(const std::string& d); 

