
#pragma once

#include <memory>
#include "gFont.h"
#include "gImage.h"
#include "znet/znet.h"

using znet::Ref;
using znet::CreateRef;

inline float Lerp(float a, float b, float t) {
  return a + (b - a) * std::min(std::max(std::abs(t), 0.0f), 1.0f);
}

enum class ButtonState {
  None,
  Hover,
  Pressed
};

class RenderUtil {
 public:
  static const int kPixelScale = 5;

  static void Setup();

  static void DrawFont(const std::string& text, int x, int y, const gColor& color = {1.0f, 1.0f, 1.0f}, bool bold = false);
  static void DrawFont(gFont* render_font, const std::string& text, int x, int y, const gColor& color = {1.0f, 1.0f, 1.0f});

  static void DrawButton(const std::string& text, int x, int y, int width, int height, ButtonState state = ButtonState::None);
  static void DrawProgress(int x, int y, int percentage, int width);
  static void DrawRect(int x, int y, int width, int height);

  static int GetStringWidth(const std::string& text, bool bold = false) {
    return bold ? font_bold_->getStringWidth(text) : font_->getStringWidth(text);
  }

  static int GetStringHeight(const std::string& text, bool bold = false) {
    return bold ? font_bold_->getStringHeight(text) : font_->getStringHeight(text);
  }

 private:
  static gFont* font_;
  static gFont* font_bold_;
  static gImage* button_;
  static gImage* button_hover_;
  static gImage* button_pressed_;
  static gImage* progress_bar_;
  static gImage* panel_;
};