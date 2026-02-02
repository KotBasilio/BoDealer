// OsServer.cpp
#include <iostream>
#include <fstream>
#include <atomic>

#include "Oscar.h"

#pragma message("OsServer.cpp REV: registry v0.6")

// GET /tasks/hello
void SServer::OutwardsHello(const httplib::Request&, httplib::Response& res)
{
   res.set_content("Pipeline Design for UI-Oscar–Walter v0.6", "text/plain");
}

// GET /oscar/hello
void SServer::HelloWalrus(const httplib::Request& req, httplib::Response& res)
{
   const auto taskId = SafeTaskId(req);
   const auto now = NowUnixMs();
   {
      std::lock_guard<std::mutex> lk(_this->mx);
      _this->tasks[taskId].last_seen_ms = now;
   }

   std::string body = "Hi, Walrus. Your task is registered; task_id=" + taskId;
   res.set_content(body, "text/plain");
}

// POST /oscar/event
void SServer::HearClubEvent(const httplib::Request& req, httplib::Response& res)
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

   _this->VerboseOut(ev);

   _this->reg.ApplyEvent(ev, now);

   res.status = 200;
   res.set_content("ok", "text/plain");
}

// GET /tasks
void SServer::TasksGetList(const httplib::Request& req, httplib::Response& res)
{
   // minimal v0: ignore filters; you can add limit/offset later
   auto lst = _this->reg.ListAll();

   json out;
   out["totalElements"] = lst.totalElements;
   out["tasks"] = json::array();

   for (const auto& s : lst.tasks) {
      out["tasks"].push_back(SnapshotToJson(s, false, std::nullopt));
   }

   res.status = 200;
   res.set_content(out.dump(), "application/json");
}

// POST /tasks
// Accepts JSON like:
// { "task_id": "...", "name":"...", "boardsNumber": 777 }
// or { "id": "...", ... }
void SServer::TasksPost(const httplib::Request& req, httplib::Response& res)
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

   auto snap = _this->reg.CreateOrGet(taskId, std::move(name), boardsNumber, now);

   json out = SnapshotToJson(snap, false, std::nullopt);
   res.status = 200;
   res.set_content(out.dump(), "application/json");
}

// GET /tasks/:id  (implemented via regex route)
void SServer::TasksGetOne(const httplib::Request& req, httplib::Response& res)
{
   const auto opt = ParseGetOptions(req);

   // Expect first capture group to be id
   if (req.matches.size() < 2) {
      res.status = 400;
      res.set_content("missing id", "text/plain");
      return;
   }
   const std::string taskId = req.matches[1];

   auto got = _this->reg.Get(taskId, opt);
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
void SServer::TasksDeleteOne(const httplib::Request& req, httplib::Response& res)
{
   const auto now = NowUnixMs();
   const auto mode = ParseDeleteMode(req);

   if (req.matches.size() < 2) {
      res.status = 400;
      res.set_content("missing id", "text/plain");
      return;
   }
   const std::string taskId = req.matches[1];

   auto snap = _this->reg.Delete(taskId, mode, now);
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

bool SServer::AttemptHttpRun(int port)
{
   // create
   LogOpen(config.logPath);
   PrintLine("Oscar server is ready for creation.");
   httplib::Server htsvr;
   htsvr.new_task_queue = [=] {
       return new httplib::ThreadPool(config.threadPoolSize);
   };

   // route inwards to walruses
   htsvr.Get(config.walrusHelloRoute, HelloWalrus);
   htsvr.Post(config.clubEventRoute, HearClubEvent);

   // route outwards to UI
   htsvr.Get(config.tasksHelloRoute, OutwardsHello);
   htsvr.Post(config.tasksPostRoute, TasksPost);
   htsvr.Get(config.tasksGetListRoute, TasksGetList);
   htsvr.Get(config.tasksGetOneRoute, TasksGetOne);
   htsvr.Delete(config.tasksDeleteRoute, TasksDeleteOne);

   // route to club exit
   htsvr.Post(config.clubStopRoute, [&](const httplib::Request&, httplib::Response& res) {
      std::cout << "\nOscar puts on his hat,";
      res.set_content("stopping", "text/plain");
      htsvr.stop();
      std::cout << " then nods";
   });

   // listen
   PrintLine("Oscar starts listening on " + config.localHost + ":" + std::to_string(port));
   if (!htsvr.listen(config.localHost.c_str(), port)) {
      std::cerr << "Oscar: listen failed on " << config.localHost << ":" << port << "\n";
      return false;
   }
   std::cout << " and walks out..." << std::endl;

   return true;
}
