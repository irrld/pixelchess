/*
* gCanvas.cpp
*
*  Created on: May 6, 2020
*      Author: noyan
*/

#include "MenuCanvas.h"
#include <cmath>
#include "GameCanvas.h"
#include "MessageCanvas.h"

HostTask::HostTask(gApp* root) : root(root) {
  title = "Creating server...";
  znet::ServerConfig config{"127.0.0.1", 44025};
  server = CreateRef<znet::Server>(config);
  completed = false;
  thread = CreateRef<std::thread>([&]() {
    znet::Result result;
    title = "Binding server...";
    if ((result = server->Bind()) != znet::Result::Success) {
      gLogi("HostTask") << "Failed to bind server! Result: " << (int) result;
      root->setCurrentCanvas(new MessageCanvas(root, "Failed to bind server!"));
      return;
    }
    title = "Waiting for opponent...";
    connection = CreateRef<ChessConnectionLocal>(server);
    connection->SetOnConnect([this]() {
      completed = true;
    });
    if ((result = server->Listen()) != znet::Result::Success) {
      gLogi("HostTask") << "Failed to listen server! Result: " << (int) result;
      root->setCurrentCanvas(new MessageCanvas(root, "Failed to listen server!"));
      return;
    }
  });
}

HostTask::~HostTask() {
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
  thread_ = CreateRef<std::thread>([host, port, this]() {
    connection = CreateRef<ChessConnectionNetwork>(client_);
    connection->SetOnConnect([this]() {
      title = "Loading the board...";
      completed = true;
    });
    title = "Connecting to " + host + ":" + std::to_string(port) + "...";
    client_->Bind();
    client_->Connect();
  });
}

JoinTask::~JoinTask() {

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

MenuCanvas::MenuCanvas(gApp* root) : gBaseCanvas(root) {
  this->root = root;
}

MenuCanvas::~MenuCanvas() {
}

void MenuCanvas::setup() {
  int buttons_x = getWidth() / 2;
  int buttons_y = getHeight() / 2 + 50;
  join_button = new Button("JOIN", buttons_x, buttons_y, 180, 60);
  host_button = new Button("HOST", buttons_x, buttons_y + 70, 180, 60);
  settings_button = new Button("SETTINGS", buttons_x, buttons_y + 140, 180, 60);
  logo_font.loadFont("PixeloidSansBold.ttf", 64, false, 80);
  white_pieces.loadImage("pieces/WhitePieces-Sheet.png");
  white_pieces.setFiltering(2, 2);
  black_pieces.loadImage("pieces/BlackPieces-Sheet.png");
  black_pieces.setFiltering(2, 2);
  loading = false;
  loading_anim = 0;
  title = "";
  task = nullptr;
  appmanager->setTargetFramerate(120);
}

void MenuCanvas::update() {
  if (task) {
    title = task->GetTitle();
  } else {
    title = "";
  }
  if (loading) {
    loading_anim += appmanager->getElapsedTime();
    if (task && task->Update()) {
      task = task->Next();
    }
    return;
  }
  join_button->Update();
  host_button->Update();
  settings_button->Update();
  if (host_button->GetState() == ButtonState::Pressed) {
    host_button->Lock();
    loading = true;
    task = CreateRef<HostTask>(root);
  } else if (join_button->GetState() == ButtonState::Pressed) {
    join_button->Lock();
    loading = true;
    task = CreateRef<JoinTask>(root, "127.0.0.1", 44025);
  }
}

void MenuCanvas::draw() {
  if (loading) {
    clearColor(0x4c, 0x54, 0x65);
    int size = 80;
    int scale = 3;
    int offset_x = (getWidth() / 2) - (size / 2);
    int offset_y = (getHeight() / 2) + 50;
    for (int i = 0; i < 6; ++i) {
      double mp = std::min(1.0, loading_anim / 4.0);
      double delta = (loading_anim * 2.0) + (i * 1.1f * mp);
      int real_size = size * mp;
      int x = std::sin(delta) * real_size + offset_x;
      int y = std::cos(delta) * real_size + offset_y;
      white_pieces.drawSub(x + 8, y + 16, 16 * scale, 32 * scale, 16 * i, 0, 16, 32, 8, 16, (int) (delta * 50.0f) % 360);
    }
    RenderUtil::DrawFont(title, getWidth() / 2, 150, {1.0f, 1.0f, 1.0f}, true);
    return;
  }
  clearColor(0x4c, 0x54, 0x65);
  // buttons
  join_button->Draw();
  host_button->Draw();
  settings_button->Draw();
  // logo
  RenderUtil::DrawFont(&logo_font, "Chess Tacos", getWidth() / 2 - logo_font.getStringWidth("Chess Tacos") / 2, 150);
}

void MenuCanvas::keyPressed(int key) {
  //	gLogi("gCanvas") << "keyPressed:" << key;
}

void MenuCanvas::keyReleased(int key) {
  //	gLogi("gCanvas") << "keyReleased:" << key;
}

void MenuCanvas::charPressed(unsigned int codepoint) {
  //	gLogi("gCanvas") << "charPressed:" << gCodepointToStr(codepoint);
}

void MenuCanvas::mouseMoved(int x, int y) {
  //	gLogi("gCanvas") << "mouseMoved" << ", x:" << x << ", y:" << y;
  if (loading) {
    return;
  }
  join_button->OnMouseMoved(x, y);
  host_button->OnMouseMoved(x, y);
  settings_button->OnMouseMoved(x, y);
}

void MenuCanvas::mouseDragged(int x, int y, int button) {
  //	gLogi("gCanvas") << "mouseDragged" << ", x:" << x << ", y:" << y << ", b:" << button;
}

void MenuCanvas::mousePressed(int x, int y, int button) {
  //	gLogi("gCanvas") << "mousePressed" << ", x:" << x << ", y:" << y << ", b:" << button;
  if (loading) {
    return;
  }
  join_button->OnMousePressed(x, y);
  host_button->OnMousePressed(x, y);
  settings_button->OnMousePressed(x, y);
}

void MenuCanvas::mouseReleased(int x, int y, int button) {
  //	gLogi("gCanvas") << "mouseReleased" << ", button:" << button;
  if (loading) {
    return;
  }
  join_button->OnMouseReleased(x, y);
  host_button->OnMouseReleased(x, y);
  settings_button->OnMouseReleased(x, y);
}

void MenuCanvas::mouseScrolled(int x, int y) {
  //	gLogi("gCanvas") << "mouseScrolled" << ", x:" << x << ", y:" << y;
}

void MenuCanvas::mouseEntered() {

}

void MenuCanvas::mouseExited() {

}

void MenuCanvas::windowResized(int w, int h) {

}

void MenuCanvas::showNotify() {

}

void MenuCanvas::hideNotify() {

}

