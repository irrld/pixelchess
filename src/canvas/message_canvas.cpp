#include "message_canvas.h"
#include "menu_canvas.h"

MessageCanvas::MessageCanvas(gApp* root, const std::string& message)
    : gBaseCanvas(root), message(std::move(message)) {
  this->root = root;
}

MessageCanvas::~MessageCanvas() {}

void MessageCanvas::setup() {
  int buttons_x = getWidth() / 2;
  int buttons_y = getHeight() / 2 + 150;
  back_button = new Button("BACK", buttons_x, buttons_y, 180, 60);
}

void MessageCanvas::update() {
  back_button->Update();
  if (back_button->GetState() == ButtonState::Pressed) {
    back_button->Lock();
    root->setCurrentCanvas(new MenuCanvas(root));
  }
}

void MessageCanvas::draw() {
  clearColor(0x4c, 0x54, 0x65);
  // buttons
  back_button->Draw();
  // logo
  RenderUtil::DrawFont(message, getWidth() / 2, 300);
  root->DrawCursor();
}

void MessageCanvas::mouseMoved(int x, int y) {
  root->SetCursorPos(x, y);
  back_button->OnMouseMoved(x, y);
}

void MessageCanvas::mousePressed(int x, int y, int button) {
  root->SetCursorType(CursorType::kHandClosed);
  back_button->OnMousePressed(x, y);
}

void MessageCanvas::mouseReleased(int x, int y, int button) {
  root->SetCursorType(CursorType::kArrow);
  back_button->OnMouseReleased(x, y);
}
