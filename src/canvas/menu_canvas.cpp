/*
* gCanvas.cpp
*
*  Created on: May 6, 2020
*      Author: noyan
*/

#include "menu_canvas.h"
#include <cmath>
#include "game_canvas.h"
#include "message_canvas.h"

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
  fade_in_ = !root->loaded_;
  root->loaded_ = true;
  fade_in_timer_ = 0.0f;
  fade_color_ = RGB{0x1c / 255.0f, 0x1e / 255.0f, 0x25 / 255.0f};
  back_color_ = RGB{0x4c / 255.0f, 0x54 / 255.0f, 0x65 / 255.0f};
  clear_color_ = back_color_;
}

void MenuCanvas::update() {
  fade_in_timer_ += appmanager->getElapsedTime();
  if (fade_in_) {
    clear_color_ = interpolateColors(fade_color_, back_color_, std::min(fade_in_timer_, 1.0));
    if (fade_in_timer_ >= 1.5f) {
      fade_in_ = false;
    }
    return;
  }

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
  clearColor(gColor{clear_color_.r, clear_color_.g, clear_color_.b});
  if (fade_in_ && fade_in_timer_ < 0.5f) {
    return;
  }
  const gColor& og_color = renderer->getColor();
  if (fade_in_) {
    setColor(255, 255, 255, std::min((int) ((fade_in_timer_ - 0.5f) * 255.0f), 255));
  }
  if (loading) {
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
    setColor(og_color);
    root->DrawCursor();
    return;
  }
  // buttons
  join_button->Draw();
  host_button->Draw();
  settings_button->Draw();
  // logo
  RenderUtil::DrawFont(&logo_font, "Chess Tacos", getWidth() / 2 - logo_font.getStringWidth("Chess Tacos") / 2, 150);
  setColor(og_color);
  root->DrawCursor();
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
  root->SetCursorPos(x, y);
  if (loading) {
    return;
  }
  join_button->OnMouseMoved(x, y);
  host_button->OnMouseMoved(x, y);
  settings_button->OnMouseMoved(x, y);
}

void MenuCanvas::mouseDragged(int x, int y, int button) {
  //	gLogi("gCanvas") << "mouseDragged" << ", x:" << x << ", y:" << y << ", b:" << button;
  root->SetCursorPos(x, y);
  root->SetCursorType(CursorType::kHandClosed);
}

void MenuCanvas::mousePressed(int x, int y, int button) {
  //	gLogi("gCanvas") << "mousePressed" << ", x:" << x << ", y:" << y << ", b:" << button;
  root->SetCursorType(CursorType::kHandClosed);
  if (loading) {
    return;
  }
  join_button->OnMousePressed(x, y);
  host_button->OnMousePressed(x, y);
  settings_button->OnMousePressed(x, y);
}

void MenuCanvas::mouseReleased(int x, int y, int button) {
  //	gLogi("gCanvas") << "mouseReleased" << ", button:" << button;
  root->SetCursorType(CursorType::kArrow);
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

