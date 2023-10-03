#include "Util.h"
#include <iostream>

gFont* RenderUtil::font_;
gFont* RenderUtil::font_bold_;
gImage* RenderUtil::button_;
gImage* RenderUtil::button_pressed_;
gImage* RenderUtil::button_hover_;
gImage* RenderUtil::progress_bar_;

void RenderUtil::Setup() {
  font_ = new gFont();
  font_bold_ = new gFont();
  font_->loadFont("PixeloidMono.ttf", 24, false, 80);
  font_bold_->loadFont("PixeloidSansBold.ttf", 24, false, 80);
  button_ = new gImage();
  button_->loadImage("button.png");
  button_->setFiltering(2, 2);
  button_pressed_ = new gImage();
  button_pressed_->loadImage("button_pressed.png");
  button_pressed_->setFiltering(2, 2);
  button_hover_ = new gImage();
  button_hover_->loadImage("button_hover.png");
  button_hover_->setFiltering(2, 2);
  progress_bar_ = new gImage();
  progress_bar_->loadImage("progress_bar.png");
  progress_bar_->setFiltering(2, 2);
}

void RenderUtil::DrawFont(const std::string& text, int x, int y, const gColor& color, bool bold) {
  auto*& render_font = bold ? font_bold_ : font_;
  DrawFont(render_font, text, x - render_font->getStringWidth(text) / 2.0f, y - render_font->getStringHeight(text) / 2.0f, color);
}

void RenderUtil::DrawFont(gFont* render_font, const std::string& text, int x, int y, const gColor& color) {
  const gColor& og_color = gRenderObject::getRenderer()->getColor();
  gRenderObject::getRenderer()->setColor({color.r - color.r / 2.0f, color.g - color.g / 2.0f, color.b - color.b / 2.0f, color.a});
  render_font->drawText(text, x, y + render_font->getStringHeight(text) / 6.5f);
  gRenderObject::getRenderer()->setColor(color);
  render_font->drawText(text, x, y);
  gRenderObject::getRenderer()->setColor(og_color);
}

void RenderUtil::DrawButton(const std::string& text, int x, int y, int width, int height, ButtonState state) {
  //    int width = GetStringWidth(text) + 8 * kPixelScale;
  //    int height = GetStringHeight(text) + 8 * kPixelScale;
  auto*& button = state == ButtonState::Pressed ? button_pressed_ : state == ButtonState::Hover ? button_hover_ : button_;
  button->draw(x - width / 2.0f, y - height / 2.0f, width, height);
  DrawFont(text, x - 3, y + (2 * kPixelScale) + (state == ButtonState::Pressed ? kPixelScale : 0.0f), {0xeb / 255.0f, 0xef / 255.0f, 0xf6 / 255.0f});
}

void RenderUtil::DrawProgress(int x, int y, int percentage, int width) {
  if (percentage > 100) {
    percentage = 100;
  }
  int pixels = (percentage * width) / 100;
  int scaled_width = pixels * kPixelScale;
  int height = progress_bar_->getHeight() * kPixelScale;
  progress_bar_->drawSub(x, y,
                         scaled_width, height,
                         0, 0,
                         3, 4);
  if (percentage < 100) {
    if (percentage != 0) {
      progress_bar_->drawSub(x + scaled_width - kPixelScale, y,
                             kPixelScale,
                             height,
                             3, 0,
                             1, 4);
    }
    progress_bar_->drawSub(x + scaled_width, y,
                           width * kPixelScale - scaled_width, height,
                           4, 0,
                           4, 4);
  }
}