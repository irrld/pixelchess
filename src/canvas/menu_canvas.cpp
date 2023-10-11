#include "menu_canvas.h"
#include "join_canvas.h"
#include "message_canvas.h"
#include "task.h"
#include "task_canvas.h"

MenuCanvas::MenuCanvas(gApp* root) : gBaseCanvas(root) {
  this->root = root;
}

MenuCanvas::~MenuCanvas() {
}

void MenuCanvas::setup() {
  int buttons_x = getWidth() / 2;
  int buttons_y = getHeight() / 2 + 100;
  join_button = new Button("JOIN", buttons_x, buttons_y, 180, 60);
  host_button = new Button("HOST", buttons_x, buttons_y + 70, 180, 60);
  quit_button = new Button("QUIT", buttons_x, buttons_y + 140, 180, 60);
  logo_font.loadFont("PixeloidSansBold.ttf", 64, false, 80);
  fade_in_ = !root->loaded_;
  root->loaded_ = true;
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

  join_button->Update();
  host_button->Update();
  quit_button->Update();
  if (host_button->GetState() == ButtonState::Pressed) {
    host_button->Lock();
    root->setCurrentCanvas(new TaskCanvas(root, CreateRef<HostTask>(root)));
  } else if (join_button->GetState() == ButtonState::Pressed) {
    join_button->Lock();
    root->setCurrentCanvas(new JoinCanvas(root));
  } else if (quit_button->GetState() == ButtonState::Pressed) {
    quit_button->Lock();
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
  join_button->Draw();
  host_button->Draw();
  quit_button->Draw();
  // logo
  RenderUtil::DrawFont(&logo_font, "Chess Tacos", getWidth() / 2 - logo_font.getStringWidth("Chess Tacos") / 2, 150, 8);
  setColor(og_color);
  RenderUtil::DrawFont(kVersion, 0, getHeight() - 33, {100, 100, 100}, false, true);
  root->DrawCursor();
}

void MenuCanvas::mouseMoved(int x, int y) {
  root->SetCursorPos(x, y);
  if (fade_in_) {
    return;
  }
  join_button->OnMouseMoved(x, y);
  host_button->OnMouseMoved(x, y);
  quit_button->OnMouseMoved(x, y);
}

void MenuCanvas::mousePressed(int x, int y, int button) {
  root->SetCursorType(CursorType::kHandClosed);
  if (fade_in_) {
    return;
  }
  join_button->OnMousePressed(x, y);
  host_button->OnMousePressed(x, y);
  quit_button->OnMousePressed(x, y);
}

void MenuCanvas::mouseReleased(int x, int y, int button) {
  root->SetCursorType(CursorType::kArrow);
  if (fade_in_) {
    return;
  }
  join_button->OnMouseReleased(x, y);
  host_button->OnMouseReleased(x, y);
  quit_button->OnMouseReleased(x, y);
}

