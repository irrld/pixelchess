#include "task.h"
#include "canvas/message_canvas.h"
#include "canvas/game_canvas.h"


HostTask::HostTask(gApp* root) : root_(root) {
  title_ = "Creating server...";
  znet::ServerConfig config{"", 44025};
  server_ = CreateRef<znet::Server>(config);
  completed_ = false;
  is_failed_ = false;
  znet::Result result;
  title_ = "Binding server...";
  if ((result = server_->Bind()) != znet::Result::Success) {
    ZNET_LOG_ERROR("Failed to bind server! Result: {}", (int) result);
    is_failed_ = true;
    fail_reason_ = "Failed to bind server!";
    return;
  }
  title_ = "Waiting for opponent...";
  connection_ = CreateRef<ChessConnectionLocal>(server_);
  connection_->SetOnConnect([this]() {
    completed_ = true;
  });
  connection_->SetOnDisconnect([root](bool is_opponent) {
    // changing the canvas will close and cleanup the server
    // no need to do here
    root->setCurrentCanvas(new MessageCanvas(root, "You've been disconnected from the game!"));
  });
  if ((result = server_->Listen()) != znet::Result::Success) {
    ZNET_LOG_ERROR("Failed to listen server! Result: {}", (int) result);
    is_failed_ = true;
    fail_reason_ = "Failed to listen server!";
    return;
  }
}

HostTask::~HostTask() {
  Cleanup();
}

bool HostTask::Update() {
  if (completed_) {
    root_->setCurrentCanvas(new GameCanvas(root_, connection_));
    was_completed_ = true;
    completed_ = false;
    // we handed over the server to the game canvas
    // remove the references so the connection won't get closed
    server_ = nullptr;
    connection_ = nullptr;
  }
  return completed_;
}

bool HostTask::CanStop() {
  return !was_completed_ && !completed_;
}

void HostTask::Stop() {
  Cleanup();
}

void HostTask::Cleanup() {
  if (server_) {
    server_->Stop();
    server_->Wait();
  }
}

Ref<Task> HostTask::Next() {
  return nullptr;
}

JoinTask::JoinTask(gApp* root, std::string host, int port) : root_(root), host_(host), port_(port) {
  znet::ClientConfig config{host, port};
  client_ = CreateRef<znet::Client>(config);
  completed_ = false;
  connection_ = CreateRef<ChessConnectionNetwork>(client_);
  connection_->SetOnConnect([this]() {
    title_ = "Loading the board...";
    completed_ = true;
  });
  connection_->SetOnDisconnect([root](bool opponent) {
    root->setCurrentCanvas(new MessageCanvas(root, "You've been disconnected from the game!"));
  });
  title_ = "Connecting to " + host + ":" + std::to_string(port) + "...";
  znet::Result result;
  if ((result = client_->Bind()) != znet::Result::Success) {
    gLogi("HostTask") << "Failed to bind client! Result: " << (int) result;
    root->setCurrentCanvas(new MessageCanvas(root, "Failed to bind client!"));
    return;
  }
  if ((result = client_->Connect()) != znet::Result::Success) {
    gLogi("HostTask") << "Failed to connect to the server! Result: " << (int) result;
    root->setCurrentCanvas(new MessageCanvas(root, "Failed to connect to the server!"));
    return;
  }
  root->setCurrentCanvas(new MessageCanvas(root, "Opponent disconnected from the game!"));
}

JoinTask::~JoinTask() {
  Cleanup();
}

bool JoinTask::Update() {
  if (completed_) {
    // wait until board data arrives
    if (!connection_->GetBoardData()) {
      return false;
    }
    root_->setCurrentCanvas(new GameCanvas(root_, connection_));
    was_completed_ = true;
    completed_ = false;
    // we handed over the server to the game canvas
    // remove the references so the connection won't get closed
    client_ = nullptr;
    connection_ = nullptr;
  }
  return completed_;
}

bool JoinTask::CanStop() {
  return !was_completed_ && !completed_;
}

void JoinTask::Stop() {
  Cleanup();
}

void JoinTask::Cleanup() {
  if (client_) {
    client_->Disconnect();
    client_->Wait();
  }
}

Ref<Task> JoinTask::Next() {
  return nullptr;
}