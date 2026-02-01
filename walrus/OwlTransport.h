// OwlTransport.h
#pragma once
#include <string>
#include <memory>
#include <chrono>

#pragma message("OwlTransport.h REV: hello v1.0")

// http relevant slice.
struct OwlConfig {
   bool isHttp = false;
   std::string host = "127.0.0.1";
   int port = 3042;
   std::string helloPath = "/oscar/hello";
   std::string eventPath = "/oscar/event";
   std::string donePath = "/oscar/stop";
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

   bool AttemptParse(const std::string& body);
};

class IOwlTransport {
public:
   virtual ~IOwlTransport() = default;
   virtual bool InitHeated() = 0;
   virtual bool HandshakeAttempt() = 0;
   virtual void Enqueue(const OwlEvent& e) = 0; // fire-and-forget
   virtual void Flush(std::chrono::milliseconds maxWait) = 0;
   virtual void Shutdown() = 0;
};

// Factory
std::unique_ptr<IOwlTransport> CreateOwlTransport();
