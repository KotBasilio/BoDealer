// OwlTransportHttp.cpp
#define  _CRT_SECURE_NO_WARNINGS
#include "waCrossPlatform.h"
#include "httplib.h" // cpp-httplib (https://github.com/yhirose/cpp-httplib)
#include HEADER_SLEEP
#include HEADER_CURSES
#include "waDoubleDeal.h"

#include <atomic>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <cstdio>

#pragma message("OwlTransportHttp.cpp REV: hello v1.0")

static uint64_t now_unix_ms()
{
   using namespace std::chrono;
   return (uint64_t)duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

static std::string json_escape(const std::string& s)
{
   std::string out;
   out.reserve(s.size() + 8);
   for (char c : s) {
      switch (c) {
      case '\\': out += "\\\\"; break;
      case '\"': out += "\\\""; break;
      case '\n': out += "\\n"; break;
      case '\r': out += "\\r"; break;
      case '\t': out += "\\t"; break;
      default:
         if ((unsigned char)c < 0x20) {
            char buf[8];
            std::snprintf(buf, sizeof(buf), "\\u%04x", (unsigned)c);
            out += buf;
         } else out += c;
      }
   }
   return out;
}

static const char* type_to_str(OwlEventType t)
{
   switch (t) {
   case OwlEventType::Log: return "log";
   case OwlEventType::Progress: return "progress";
   case OwlEventType::Done: return "done";
   default: return "unknown";
   }
}

static std::string event_to_json(const OwlEvent& e)
{
   // Minimal JSON payload. Keep it stable; easy for OO to parse.
   std::string js;
   js.reserve(256 + e.message.size());
   js += "{";
   js += "\"type\":\""; js += type_to_str(e.type); js += "\",";
   js += "\"task_id\":\""; js += json_escape(e.task_id); js += "\",";
   js += "\"seq\":"; js += std::to_string(e.seq); js += ",";
   js += "\"unix_ms\":"; js += std::to_string(e.unix_ms); js += ",";
   js += "\"silent\":"; js += (e.silent ? "true" : "false"); js += ",";
   js += "\"percent\":"; js += std::to_string(e.percent); js += ",";
   js += "\"message\":\""; js += json_escape(e.message); js += "\"";
   js += "}";
   return js;
}

class OwlHttpTransport final : public IOwlTransport {
public:
   OwlHttpTransport() = default;
   ~OwlHttpTransport() override { Shutdown(); }

   bool InitHeated() override 
   {
      // fill class members
      cfg_ = config.cowl;
      task_id_ = config.TaskID;
      client_ = std::make_unique<httplib::Client>(cfg_.host, cfg_.port);
      client_->set_keep_alive(true);
      client_->set_read_timeout(0, 200000);   // 200ms
      client_->set_write_timeout(0, 200000);

      // check for Oscar presence
      if (!hello_probe()) {
         return false;
      }

      // ok to start sender
      StartSenderThread();
      return true;
   }

   virtual bool HandshakeAttempt() override 
   { 
      // may retry hello a bit (Oscar boot time)
      if (!hello_probe()) {
         bool ok = false;
         for (int i = 0; i < cfg_.helloRetries; ++i) {
            std::this_thread::sleep_for(std::chrono::milliseconds(cfg_.helloRetryMs));
            if (hello_probe()) { ok = true; break; }
         }
         if (!ok) {// might still start worker; it would drop/skip sends.
            std::fprintf(stderr, "[owl/http] Oscar not responding on %s:%d\n",
               cfg_.host.c_str(), cfg_.port);
            return false;
         }
      }

      // ok to start sender
      StartSenderThread();
      return true;
   }

   void Enqueue(const OwlEvent& e) override
   {
      OwlEvent ev = e;
      ev.task_id = task_id_;
      ev.unix_ms = ev.unix_ms ? ev.unix_ms : now_unix_ms();

      // seq is assigned here to preserve order for OO.
      ev.seq = ++seq_;

      {
         std::lock_guard<std::mutex> lk(mx_);
         // bounded queue: drop policy
         if (q_.size() >= cfg_.queueMax) {
            // Drop oldest "progress" first if possible; else drop oldest.
            bool dropped = false;
            for (auto it = q_.begin(); it != q_.end(); ++it) {
               if (it->type == OwlEventType::Progress) { q_.erase(it); dropped = true; break; }
            }
            if (!dropped) q_.pop_front();
         }
         q_.push_back(std::move(ev));
      }
      cv_.notify_one();
   }

   void Flush(std::chrono::milliseconds maxWait) override
   {
      auto deadline = std::chrono::steady_clock::now() + maxWait;
      while (std::chrono::steady_clock::now() < deadline) {
         {
            std::lock_guard<std::mutex> lk(mx_);
            if (q_.empty()) return;
         }
         std::this_thread::sleep_for(std::chrono::milliseconds(5));
      }
   }

   void Shutdown() override
   {
      bool wasRunning = running_.exchange(false);
      if (wasRunning) {
         cv_.notify_all();
         if (worker_.joinable()) worker_.join();
      }
      client_.reset();
   }

private:
   bool hello_probe()
   {
      handshake_text_.clear();
      std::string path = cfg_.helloPath + "?task_id=" + task_id_;

      auto res = client_->Get(path.c_str());
      if (!res) return false;
      if (res->status != 200) return false;

      // Whatever OO returns, we keep as "handshake text" for now.
      // If it's JSON, you can parse it later; for now just print body.
      handshake_text_ = "[Oscar hello] " + res->body;
      return true;
   }

   void StartSenderThread()
   {
      running_.store(true);
      worker_ = std::thread([this]() { this->worker_loop(); });

      // Print handshake info similarly to your pipe version (best-effort).
      if (!handshake_text_.empty()) {
         std::fprintf(stdout, "%s\n", handshake_text_.c_str());
      }
   }

   void worker_loop()
   {
      while (running_.load()) {
         OwlEvent ev;
         {
            std::unique_lock<std::mutex> lk(mx_);
            cv_.wait_for(lk, std::chrono::milliseconds(50), [&]() {
               return !running_.load() || !q_.empty();
               });
            if (!running_.load()) break;
            if (q_.empty()) continue;
            ev = std::move(q_.front());
            q_.pop_front();
         }

         // POST event (best-effort, no blocking retries here)
         if (!client_) continue;
         std::string body = event_to_json(ev);
         std::string path = (ev.type == OwlEventType::Done) ? cfg_.donePath : cfg_.eventPath;
         auto res = client_->Post(path.c_str(), body, "application/json");
         // If Oscar is down, we just drop quietly (still "fire-and-forget").
         // If you want mild resilience, you can requeue on certain errors.
         (void)res;
      }
   }

private:
   OwlConfig cfg_;
   std::string task_id_;
   std::string handshake_text_;

   std::unique_ptr<httplib::Client> client_;

   std::atomic<bool> running_{ false };
   std::thread worker_;

   std::mutex mx_;
   std::condition_variable cv_;
   std::deque<OwlEvent> q_;

   std::atomic<uint64_t> seq_{ 0 };
};

// ---- Null transport stub: black hole ----
class OwlNullTransport final : public IOwlTransport {
public:
   bool InitHeated() override { return true; }
   bool HandshakeAttempt() override { return true; }
   void Enqueue(const OwlEvent& /*e*/) override {}
   void Flush(std::chrono::milliseconds /*maxWait*/) override {}
   void Shutdown() override {}
};

std::unique_ptr<IOwlTransport> CreateOwlTransport()
{
   if (config.cowl.isHttp) return std::make_unique<OwlHttpTransport>();
   return std::make_unique<OwlNullTransport>();
}
