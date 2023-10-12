
#pragma once

#include <string>
#include "app/app.h"
#include "net/chess_connection.h"

class Task {
 public:
  virtual ~Task() {}
  virtual bool Update() = 0;
  virtual bool CanStop() = 0;
  virtual void Stop() = 0;
  virtual Ref<Task> Next() = 0;
  virtual const std::string& GetTitle() = 0;
};

class HostTask : public Task {
 public:
  HostTask(gApp* root);
  ~HostTask();
  bool Update() override;
  bool CanStop() override;
  void Stop() override;
  Ref<Task> Next() override;

  const std::string& GetTitle() override;
 private:
  void Cleanup();
  std::string title_;
  gApp* root_;
  Ref<znet::Server> server_;
  Ref<std::thread> thread_;
  bool completed_ = false;
  bool was_completed_ = false;
  Ref<ChessConnectionLocal> connection_;
};

class JoinTask : public Task {
 public:
  JoinTask(gApp* root, std::string host, int port);
  ~JoinTask();
  bool Update() override;
  bool CanStop() override;
  void Stop() override;
  Ref<Task> Next() override;

  const std::string& GetTitle() override;
 private:
  void Cleanup();
  std::string title_;
  std::string host_;
  int port_;
  gApp* root_;
  Ref<znet::Client> client_;
  Ref<std::thread> thread_;
  bool completed_ = false;
  bool was_completed_ = false;
  Ref<ChessConnectionNetwork> connection_;
};