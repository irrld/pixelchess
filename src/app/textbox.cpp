
#include "textbox.h"
#include <color.h>
#include <locale>
#include <codecvt>

Textbox::Textbox(std::string placeholder, int x, int y, int width, int height) : placeholder_(std::move(placeholder)), x_(x - width / 2.0f), y_(y - height / 2.0f), width_(width), height_(height) {
  last_backspace_ = 0.0f;
}

void Textbox::Draw() {
  RenderUtil::DrawRect(x_, y_, width_, height_);
  int padding = RenderUtil::kPixelScale * 2;
  int inner_padding = RenderUtil::kPixelScale;
  int bottom = RenderUtil::kPixelScale * 3;
  const gColor& color = gRenderObject::getRenderer()->getColor();
  gRenderObject::getRenderer()->setColor(CreateRGB(0x6b7488));
  gDrawRectangle(x_ + padding, y_ + padding, width_ - padding * 2, height_ - padding * 2 - bottom, true);
  gRenderObject::getRenderer()->setColor(color);
  int text_pos_x = x_ + padding + inner_padding;
  int text_pos_y = y_ + padding + inner_padding;
  RenderUtil::DrawFont(text_, text_pos_x, text_pos_y, CreateRGB(0xffffff), true, true);
  uint64_t dt = delta_ * 1000;
  if (dt % 1000 < 500) {
    int width = RenderUtil::GetStringWidth(text_.substr(0, cursor_end_), true);
    gDrawLine(text_pos_x + width + 5,
              text_pos_y + 3,
              text_pos_x + width + 5,
              text_pos_y + RenderUtil::GetStringHeight("A", true) + 3, 2.0f);
  }
}

void Textbox::Update() {
  delta_ += appmanager->getElapsedTime();
  if (delta_ - last_backspace_ > 0.2) {
    if (keys_[G_KEY_BACKSPACE] && text_.length() > 0) {
      SetText(text_.substr(0, text_.length() - 1));
    } else if (keys_[G_KEY_LEFT] && cursor_end_ > 0) {
      cursor_end_--;
    } else if (keys_[G_KEY_RIGHT] && cursor_end_ < text_.length()) {
      cursor_end_++;
    }
    last_backspace_ = delta_;
  }
}

void Textbox::OnKeyPress(int keycode) {
  keys_[keycode] = true;
}

void Textbox::OnKeyRelease(int keycode) {
  keys_[keycode] = false;
  if (keycode == G_KEY_BACKSPACE) {
    last_backspace_ = delta_ + 0.2f;
    if (text_.length() > 0) {
      SetText(text_.substr(0, text_.length() - 1));
    }
  } else if (keycode == G_KEY_LEFT && cursor_end_ > 0) {
    cursor_end_--;
  } else if (keycode == G_KEY_RIGHT && cursor_end_ < text_.length()) {
    cursor_end_++;
  } else if (keycode == G_KEY_ENTER && callback_) {
    callback_(text_);
  }
}

void Textbox::OnKeyChar(unsigned char key) {
  if (key == '\r' || key == '\n' || key < 0) {
    return;
  }
  SetText(text_ + (char)key);
}

void Textbox::OnMouseMoved(int x, int y) {

}

void Textbox::OnMousePressed(int x, int y) {

}

void Textbox::OnMouseReleased(int x, int y) {

}

void Textbox::SetText(const std::string& text) {
  text_ = text;
  cursor_start_ = text_.length();
  cursor_end_ = text_.length();
}