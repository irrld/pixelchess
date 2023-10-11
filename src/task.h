
#pragma once

#include <string>
#include "app/app.h"
#include "net/chess_connection.h"

class Task {
 public:
  virtual ~Task() {}
  virtual bool Update() = 0;
  virtual Ref<Task> Next() = 0;
  virtual const std::string& GetTitle() = 0;
};

class HostTask : public Task {
 public:
  HostTask(gApp* root);
  ~HostTask();
  bool Update() override;
  Ref<Task> Next() override;

  const std::string& GetTitle() override;
 private:
  std::string title;
  gApp* root;
  Ref<znet::Server> server;
  Ref<std::thread> thread;
  bool completed = false;
  Ref<ChessConnectionLocal> connection;
  bool wait = false;
};

class JoinTask : public Task {
 public:
  JoinTask(gApp* root, std::string host, int port);
  ~JoinTask();
  bool Update() override;
  Ref<Task> Next() override;

  const std::string& GetTitle() override;
 private:
  std::string title;
  std::string host;
  int port;
  gApp* root;
  Ref<znet::Client> client_;
  Ref<std::thread> thread_;
  bool completed = false;
  Ref<ChessConnectionNetwork> connection;
  bool wait = false;
};