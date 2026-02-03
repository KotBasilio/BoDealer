// TaskRegistry.h
#pragma once
#include <cstdint>
#include <mutex>
#include <unordered_map>
#include <optional>
#include "OwlTransport.h"

#pragma message("TaskRegistry.h REV: registry v0.8")

enum class TaskStatus : uint8_t {
   Started,
   InProgress,
   Done,
   Failed,
   Canceled
};
bool IsTerminal(TaskStatus st);

struct TaskSnapshot {
   std::string id;
   std::string name;
   int boardsNumber = 0;

   TaskStatus status = TaskStatus::Started;

   // stream / progress
   uint64_t seq = 0;           // last accepted event seq
   int percent = 0;            // 0..100
   std::string message;        // last message
   std::string resultJson;     // optional: final result payload (JSON as string)
   std::string errorText;      // optional: failure diagnostics

   // lifecycle bookkeeping
   int64_t createdMs = 0;
   int64_t lastSeenMs = 0;

   // cooperative cancel support (Oscar -> Walter)
   bool cancelRequested = false;
};

enum class DeleteMode : uint8_t {
   Auto,    // your default: "I don't need it"; server decides cancel vs evict
   Cancel,  // courtesy / sanity flavor
   Ack      // courtesy / sanity flavor
};

struct GetOptions {
   bool includeLogTail = false; // wire to ?log_req=1
   size_t logTailBytes = 4096;  // small by default
};

struct GetResult {
   TaskSnapshot snapshot;
   std::optional<std::string> logTail; // present only if requested and available
};

// event ingest disposition (this replaces SServer::tasks[] dedupe responsibility)
enum class IngestDisposition : uint8_t {
   Accepted,   // applied & logged
   Duplicate,  // dropped due to seq <= last seq (the old tasks[] behavior)
   Ignored     // dropped for other reasons (e.g. terminal task ignoring non-log)
};

struct ApplyEventResult {
   IngestDisposition disposition = IngestDisposition::Ignored;
   TaskSnapshot snapshot;
};

class TaskRegistry {
   // LOGS
   std::string m_logDir;
   static std::string SanitizeForFilename(const std::string& taskId);
   std::string LogPathForTask(const std::string& taskId) const;
   static void AppendLogLine(const std::string& path, uint64_t seq, const OwlEventType type, int percent, const std::string& msg, const std::string& json);
   static std::optional<std::string> ReadTail(const std::string& path, size_t tailBytes);

   // MUTEX SET: 
   // -- global map mutex only for map structure; 
   // -- per-task mutex for task content.
   struct TaskEntry
   {
      mutable std::mutex m;
      TaskSnapshot s;
   };
   std::unordered_map<std::string, std::shared_ptr<TaskEntry>> m_tasks;
   mutable std::mutex m_mapMutex;

   std::shared_ptr<TaskEntry> FindOrCreate_(const std::string& taskId, int64_t nowMs, bool* outCreated);

public:
   explicit TaskRegistry();

   // touch/register a task without changing anything else (used by /oscar/hello)
   TaskSnapshot Touch(const std::string& taskId, int64_t nowMs);

   // UI side: create task (or return existing)
   TaskSnapshot CreateOrGet(const std::string& taskId,
      std::string name,
      int boardsNumber,
      int64_t nowMs);

   // Walter -> Oscar: apply streamed event
   // Returns returns whether event was accepted / duplicate / ignored
   ApplyEventResult ApplyEvent(const OwlEvent& ev, int64_t nowMs);

   // UI side: get current snapshot (optionally with log tail)
   std::optional<GetResult> Get(const std::string& taskId, const GetOptions& opt) const;

   // UI side: delete/ack/cancel. Returns snapshot if task existed.
   // - If running: marks CANCELED + cancelRequested=true (cooperative cancel).
   // - If terminal (DONE/FAILED/CANCELED): evicts immediately for Ack/Auto; for Cancel it also evicts.
   std::optional<TaskSnapshot> Delete(const std::string& taskId, DeleteMode mode, int64_t nowMs);

   // Optional hygiene: evict tasks not seen for too long (e.g. after crash)
   // Returns number evicted.
   size_t SweepByLastSeen(int64_t nowMs, int64_t maxAgeMs);

   struct ListResult {
      std::vector<TaskSnapshot> tasks;
      size_t totalElements = 0;
   };
   ListResult ListAll() const;

};
