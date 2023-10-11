#include "task.h"
#include "canvas/message_canvas.h"
#include "canvas/game_canvas.h"


HostTask::HostTask(gApp* root) : root(root) {
  title = "Creating server...";
  znet::ServerConfig config{"127.0.0.1", 44025};
  server = CreateRef<znet::Server>(config);
  completed = false;
  wait = false;
  thread = CreateRef<std::thread>([&, root]() {
    znet::Result result;
    title = "Binding server...";
    if ((result = server->Bind()) != znet::Result::Success) {
      wait = true;
      gLogi("HostTask") << "Failed to bind server! Result: " << (int) result;
      root->setCurrentCanvas(new MessageCanvas(root, "Failed to bind server!"));
      return;
    }
    title = "Waiting for opponent...";
    connection = CreateRef<ChessConnectionLocal>(server);
    connection->SetOnConnect([this]() {
      completed = true;
    });
    connection->SetOnOpponentDisconnect([root]() {
      // changing the canvas will close and cleanup the server
      // no need to do here
      root->setCurrentCanvas(new MessageCanvas(root, "Opponent disconnected from the game!"));
    });
    if ((result = server->Listen()) != znet::Result::Completed) {
      wait = true;
      gLogi("HostTask") << "Failed to listen server! Result: " << (int) result;
      root->setCurrentCanvas(new MessageCanvas(root, "Failed to listen server!"));
      return;
    }
  });
}

HostTask::~HostTask() {
  if (wait) {
    if (thread && thread->joinable()) {
      thread->join();
    }
  }
}

bool HostTask::Update() {
  if (completed) {
    root->setCurrentCanvas(new GameCanvas(root, connection, thread));
    completed = false;
  }
  return completed;
}

Ref<Task> HostTask::Next() {
  return nullptr;
}

const std::string& HostTask::GetTitle() {
  return title;
}

JoinTask::JoinTask(gApp* root, std::string host, int port) : root(root), host(host), port(port) {
  znet::ClientConfig config{host, port};
  client_ = CreateRef<znet::Client>(config);
  completed = false;
  wait = false;
  thread_ = CreateRef<std::thread>([root, host, port, this]() {
    connection = CreateRef<ChessConnectionNetwork>(client_);
    connection->SetOnConnect([this]() {
      title = "Loading the board...";
      completed = true;
    });
    title = "Connecting to " + host + ":" + std::to_string(port) + "...";
    znet::Result result;
    if ((result = client_->Bind()) != znet::Result::Success) {
      wait = true;
      gLogi("HostTask") << "Failed to bind client! Result: " << (int) result;
      root->setCurrentCanvas(new MessageCanvas(root, "Failed to bind client!"));
      return;
    }
    result = client_->Connect();
    if (result != znet::Result::Completed) {
      wait = true;
      gLogi("HostTask") << "Failed to connect to the server! Result: " << (int) result;
      root->setCurrentCanvas(new MessageCanvas(root, "Failed to connect to the server!"));
      return;
    }
    root->setCurrentCanvas(new MessageCanvas(root, "Opponent disconnected from the game!"));
  });
}

JoinTask::~JoinTask() {
  if (wait) {
    if (thread_ && thread_->joinable()) {
      thread_->join();
    }
  }
}

bool JoinTask::Update() {
  if (completed) {
    // wait until board data arrives
    if (!connection->GetBoardData()) {
      return false;
    }
    root->setCurrentCanvas(new GameCanvas(root, connection, thread_));
    completed = false;
  }
  return completed;
}

Ref<Task> JoinTask::Next() {
  return nullptr;
}

const std::string& JoinTask::GetTitle() {
  return title;
}