
#pragma once

#include <string>
#include <utility>
#include "gAppManager.h"
#include "util.h"

class Textbox {
 public:
  Textbox(std::string placeholder, int x, int y, int width, int height);

  void Draw();

  void Update();

  void OnKeyPress(int keycode);

  void OnKeyRelease(int keycode);

  void OnKeyChar(unsigned char key);

  void OnMouseMoved(int x, int y);

  void OnMousePressed(int x, int y);

  void OnMouseReleased(int x, int y);

  bool CheckCollision(int x, int y) {
    int wh = width_ / 2;
    int hh = height_ / 2;
    return x >= x_ - wh && x <= x_ + wh && y >= y_ - hh && y <= y_ + hh;
  }

  void SetText(const std::string& text);

  const std::string& GetText() const { return text_; }

  void SetCallback(std::function<void(const std::string&)> callback) {
    callback_ = std::move(callback);
  }
 private:
  std::string placeholder_;
  std::string text_;
  int x_;
  int y_;
  int width_;
  int height_;
  bool active_;
  int cursor_start_;
  int cursor_end_;
  double delta_;
  double last_backspace_;
  std::unordered_map<int, bool> keys_;
  std::function<void(const std::string&)> callback_;
};
