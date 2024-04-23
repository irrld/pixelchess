
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

  virtual const std::string& title() = 0;

  bool is_failed() { return is_failed_; }

  const std::string& fail_reason() { return fail_reason_; }

 protected:
  bool is_failed_ = false;
  std::string fail_reason_ = "";
};

class HostTask : public Task {
 public:
  HostTask(gApp* root);
  ~HostTask();
  bool Update() override;
  bool CanStop() override;
  void Stop() override;
  Ref<Task> Next() override;

  const std::string& title() override { return title_; }

 private:
  void Cleanup();
  std::string title_;
  gApp* root_;
  Ref<znet::Server> server_;
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

  const std::string& title() override { return title_; }
 private:
  void Cleanup();
  std::string title_;
  std::string host_;
  int port_;
  gApp* root_;
  Ref<znet::Client> client_;
  bool completed_ = false;
  bool was_completed_ = false;
  Ref<ChessConnectionNetwork> connection_;
};