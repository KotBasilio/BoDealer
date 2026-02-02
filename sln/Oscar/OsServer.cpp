// OsServer.cpp
#include "TaskRegistry.h"

#include <iostream>
#include <fstream>
#include <atomic>

#include "Oscar.h"

#pragma message("OsServer.cpp REV: registry v0.6")

SConfig config;
SServer srv;
static TaskRegistry g_tasks("oscar_tasks"); // directory for per-task logs

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

