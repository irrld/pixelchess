#include "util.h"
#include <iostream>

gFont* RenderUtil::font_;
gFont* RenderUtil::font_bold_;
gImage* RenderUtil::button_;
gImage* RenderUtil::button_pressed_;
gImage* RenderUtil::button_hover_;
gImage* RenderUtil::progress_bar_;
gImage* RenderUtil::panel_;

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
  panel_ = new gImage();
  panel_->loadImage("panel.png");
  panel_->setFiltering(2, 2);
}

void RenderUtil::DrawFont(const std::string& text, int x, int y, const gColor& color, bool bold, bool absolute) {
  auto*& render_font = bold ? font_bold_ : font_;
  DrawFont(render_font, text, x - (absolute ? 0.0f : render_font->getStringWidth(text) / 2.0f), y + render_font->getStringHeight("A"), 3, color);
}

void RenderUtil::DrawFont(gFont* render_font, const std::string& text, int x, int y, int shadow_distance, const gColor& color) {
  const gColor& og_color = gRenderObject::getRenderer()->getColor();
  float alpha = og_color.a * color.a;
  if (shadow_distance > 0) {
    gRenderObject::getRenderer()->setColor({color.r - color.r / 2.0f, color.g - color.g / 2.0f, color.b - color.b / 2.0f, alpha});
    render_font->drawText(text, x, y + shadow_distance);
  }
  gRenderObject::getRenderer()->setColor({color.r, color.g, color.b, alpha});
  render_font->drawText(text, x, y);
  gRenderObject::getRenderer()->setColor(og_color);
}

void RenderUtil::DrawButton(const std::string& text, int x, int y, int width, int height, ButtonState state) {
  auto*& button = state == ButtonState::Pressed ? button_pressed_ : state == ButtonState::Hover ? button_hover_ : button_;
  button->draw(x - width / 2.0f, y - height / 2.0f, width, height);
  DrawFont(text, x - 3, y - height / 2.0f + 8, {0xeb / 255.0f, 0xef / 255.0f, 0xf6 / 255.0f});
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

void RenderUtil::DrawRect(int x, int y, int width, int height) {
  if (width % 4 != 0) {
    width += 4 - width % 4;
  }
  if (height % 4 != 0) {
    height += 4 - height % 4;
  }
  int scaled_width = width;
  int scaled_height = height;
  int dx = x;
  int dy = y;
  int size = kPixelScale * 4;
  int woffset = scaled_width - size;
  int hoffset = scaled_height - size;

  // fill
  int fill_width = woffset / 2;
  int fill_height = hoffset / 2 + size;
  // top left
  panel_->drawSub(dx + size, dy + size, fill_width, fill_height - size, 3, 3, 1, 1);
  panel_->drawSub(dx, dy + size, size, fill_height - size, 0, 3, 4, 1);
  panel_->drawSub(dx + size, dy, fill_width, size, 3, 0, 1, 4);

  // top right
  panel_->drawSub(dx + size + fill_width, dy + size, fill_width, fill_height - size, 4, 3, 1, 1);
  panel_->drawSub(dx + fill_width * 2, dy + size, size, fill_height - size, 4, 3, 4, 1);
  panel_->drawSub(dx + size + fill_width, dy, fill_width, size, 4, 0, 1, 4);

  // bottom left
  panel_->drawSub(dx + size, dy + fill_height, fill_width, fill_height - size * 2, 3, 4, 1, 1);
  panel_->drawSub(dx, dy + fill_height, size, fill_height - size * 2, 0, 4, 4, 1);
  panel_->drawSub(dx + size, dy + fill_height * 2 - size * 2, fill_width, size, 3, 4, 1, 4);

  // bottom right
  panel_->drawSub(dx + size + fill_width, dy + fill_height, fill_width, fill_height - size * 2, 4, 4, 1, 1);
  panel_->drawSub(dx + fill_width * 2, dy + fill_height, size, fill_height - size * 2, 4, 4, 4, 1);
  panel_->drawSub(dx + size + fill_width, dy + fill_height * 2 - size * 2, fill_width, size, 4, 4, 1, 4);

  // corners
  panel_->drawSub(dx, dy, size, size, 0, 0, 4, 4);
  panel_->drawSub(dx + woffset, dy, size, size, 4, 0, 4, 4);
  panel_->drawSub(dx, dy + hoffset, size, size, 0, 4, 4, 4);
  panel_->drawSub(dx + woffset, dy + hoffset, size, size, 4, 4, 4, 4);
}