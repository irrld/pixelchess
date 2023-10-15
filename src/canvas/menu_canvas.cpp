#include "menu_canvas.h"
#include "join_canvas.h"
#include "message_canvas.h"
#include "task.h"
#include "task_canvas.h"

MenuCanvas::MenuCanvas(gApp* root) : gBaseCanvas(root) {
  this->root_ = root;
}

MenuCanvas::~MenuCanvas() {
}

void MenuCanvas::setup() {
  int buttons_x = getWidth() / 2;
  int buttons_y = getHeight() / 2 + 100;
  join_button_ = new Button("JOIN", buttons_x, buttons_y, 180, 60);
  host_button_ = new Button("HOST", buttons_x, buttons_y + 70, 180, 60);
  quit_button_ = new Button("QUIT", buttons_x, buttons_y + 140, 180, 60);
  logo_font_.loadFont("PixeloidSansBold.ttf", 64, false, 80);
  fade_in_ = !root_->loaded_;
  root_->loaded_ = true;
  fade_in_timer_ = 0.0f;
  fade_color_ = CreateRGB(0x1c1e25);
  back_color_ = CreateRGB(0x4c5465);
  clear_color_ = back_color_;
}

void MenuCanvas::update() {
  fade_in_timer_ += appmanager->getElapsedTime();
  if (fade_in_) {
    clear_color_ = InterpolateColors(fade_color_, back_color_,
                                     std::min(fade_in_timer_, 1.0));
    if (fade_in_timer_ >= 1.5f) {
      fade_in_ = false;
    }
    return;
  }

  join_button_->Update();
  host_button_->Update();
  quit_button_->Update();
  if (host_button_->GetState() == ButtonState::Pressed) {
    host_button_->Lock();
    root_->setCurrentCanvas(new TaskCanvas(root_, CreateRef<HostTask>(root_)));
  } else if (join_button_->GetState() == ButtonState::Pressed) {
    join_button_->Lock();
    root_->setCurrentCanvas(new JoinCanvas(root_));
  } else if (quit_button_->GetState() == ButtonState::Pressed) {
    quit_button_->Lock();
    appmanager->stop();
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
  // buttons
  join_button_->Draw();
  host_button_->Draw();
  quit_button_->Draw();
  // logo
  RenderUtil::DrawFont(&logo_font_, "Chess Tacos", getWidth() / 2 - logo_font_.getStringWidth("Chess Tacos") / 2, 150, 8);
  setColor(og_color);
  RenderUtil::DrawFont(RenderUtil::font_, kVersion, 0, getHeight() - 10, 0, {100, 100, 100});
  root_->DrawCursor();
}

void MenuCanvas::mouseMoved(int x, int y) {
  root_->SetCursorPos(x, y);
  if (fade_in_) {
    return;
  }
  join_button_->OnMouseMoved(x, y);
  host_button_->OnMouseMoved(x, y);
  quit_button_->OnMouseMoved(x, y);
}

void MenuCanvas::mousePressed(int x, int y, int button) {
  root_->SetCursorType(CursorType::kHandClosed);
  if (fade_in_) {
    return;
  }
  join_button_->OnMousePressed(x, y);
  host_button_->OnMousePressed(x, y);
  quit_button_->OnMousePressed(x, y);
}

void MenuCanvas::mouseReleased(int x, int y, int button) {
  root_->SetCursorType(CursorType::kArrow);
  if (fade_in_) {
    return;
  }
  join_button_->OnMouseReleased(x, y);
  host_button_->OnMouseReleased(x, y);
  quit_button_->OnMouseReleased(x, y);
}

