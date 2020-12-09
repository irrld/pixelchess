
#pragma once

#include <string>
#include <utility>
#include "Util.h"
#include "gAppManager.h"

class Button {
 public:
  Button(std::string text, int x, int y, int width, int height, ButtonState state = ButtonState::None) : text_(std::move(text)), x_(x), y_(y), width_(width), height_(height), state_(state) {
  }

  void Draw() {
    RenderUtil::DrawButton(text_, x_, y_, width_, height_, state_);
  }

  void Update() {
    if (lock_timer_ > 0) {
      lock_timer_ -= appmanager->getElapsedTime();
    }
  }

  void Lock() {
    lock_timer_ = 20000;
    state_ = ButtonState::None;
  }

  void OnMouseMoved(int x, int y) {
    if (lock_timer_ > 0 || state_ == ButtonState::Pressed) {
      return;
    }
    if (CheckCollision(x, y)) {
      state_ = ButtonState::Hover;
    } else {
      state_ = ButtonState::None;
    }
  }

  void OnMousePressed(int x, int y) {
    if (lock_timer_ > 0) {
      return;
    }
    if (CheckCollision(x, y)) {
      state_ = ButtonState::Pressed;
    } else {
      state_ = ButtonState::None;
    }
  }

  void OnMouseReleased(int x, int y) {
    if (lock_timer_ > 0) {
      return;
    }
    if (CheckCollision(x, y)) {
      state_ = ButtonState::Hover;
      lock_timer_ = 0.1;
    } else {
      state_ = ButtonState::None;
    }
  }

  bool CheckCollision(int x, int y) {
    int wh = width_ / 2;
    int hh = height_ / 2;
    return x >= x_ - wh && x <= x_ + wh && y >= y_ - hh && y <= y_ + hh;
  }

  ButtonState GetState() const { return state_; }

 private:
  std::string text_;
  int x_;
  int y_;
  int width_;
  int height_;
  ButtonState state_;
  double lock_timer_ = 0;
};
