#pragma once
#include "httplib.h"
#include "json.hpp"

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

   void LogOpen(const std::string& logPath);
   void LogLine(const std::string& line);
   void LogFlush();

   ~SServer() { LogFlush(); }
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
void VerboseOut(OwlEvent& ev);

