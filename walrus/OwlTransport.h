// OwlTransport.h
#pragma once
#include <string>
#include <memory>
#include <atomic>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <chrono>
#include <cstdio>

#pragma message("OwlTransport.h REV: hello v0.5")

// http relevant slice.
struct OwlConfig {
   bool isHttp = true;
   std::string host = "127.0.0.1";
   int port = 3042;
   std::string helloPath = "/oscar/hello";
   std::string eventPath = "/oscar/event";
   bool autoSpawn = true;
   int helloRetries = 2;
   int helloRetryMs = 100;
   size_t queueMax = 256;
};

// Minimal event model (maps cleanly to your owl.Show/Silent/OnProgress/OnDone).
enum class OwlEventType { Log, Progress, Done };

struct OwlEvent {
   OwlEventType type{};
   std::string task_id;
   std::string message;   // for Log/Done
   int percent = -1;      // for Progress
   uint64_t seq = 0;
   uint64_t unix_ms = 0;
   bool silent = false;   // for Log
};

class IOwlTransport {
public:
   virtual ~IOwlTransport() = default;
   virtual bool InitAndHandshake() = 0;
   virtual void Enqueue(const OwlEvent& e) = 0; // fire-and-forget
   virtual void Flush(std::chrono::milliseconds maxWait) = 0;
   virtual void Shutdown() = 0;
};

// Factory
std::unique_ptr<IOwlTransport> CreateOwlTransport();
