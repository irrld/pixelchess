#include "join_canvas.h"
#include "menu_canvas.h"
#include "task_canvas.h"

JoinCanvas::JoinCanvas(gApp* root) : gBaseCanvas(root) {
  this->root = root;
}

JoinCanvas::~JoinCanvas() {

}

void JoinCanvas::setup() {
  int x = getWidth() / 2;
  int y = getHeight() / 2 - 50;
  ip_textbox = new Textbox("IP Address", x, y, 300, 75);
  ip_textbox->SetCallback([this](const std::string& ip) {
    Join(ip);
  });
  ip_textbox->SetText("localhost");
  join_button = new Button("JOIN", x, y + 100, 300, 60);
  back_button = new Button("BACK", x, y + 170, 300, 60);
}

void JoinCanvas::update() {
  ip_textbox->Update();
  join_button->Update();
  back_button->Update();
  if (join_button->GetState() == ButtonState::Pressed) {
    join_button->Lock();
    Join(ip_textbox->GetText());
  }
  if (back_button->GetState() == ButtonState::Pressed) {
    back_button->Lock();
    root->setCurrentCanvas(new MenuCanvas(root));
  }
}

void JoinCanvas::draw() {
  clearColor(CreateRGB(0x4c5465));
  ip_textbox->Draw();
  join_button->Draw();
  back_button->Draw();
  root->DrawCursor();
}

void JoinCanvas::keyPressed(int key) {
  ip_textbox->OnKeyPress(key);
}

void JoinCanvas::keyReleased(int key) {
  ip_textbox->OnKeyRelease(key);
}

void JoinCanvas::charPressed(unsigned int codepoint) {
  ip_textbox->OnKeyChar(codepoint);
}

void JoinCanvas::mouseMoved(int x, int y) {
  root->SetCursorPos(x, y);
  ip_textbox->OnMouseMoved(x, y);
  join_button->OnMouseMoved(x, y);
  back_button->OnMouseMoved(x, y);
}

void JoinCanvas::mousePressed(int x, int y, int button) {
  root->SetCursorPos(x, y);
  root->SetCursorType(CursorType::kHandClosed);
  ip_textbox->OnMousePressed(x, y);
  join_button->OnMousePressed(x, y);
  back_button->OnMousePressed(x, y);
}

void JoinCanvas::mouseReleased(int x, int y, int button) {
  root->SetCursorPos(x, y);
  root->SetCursorType(CursorType::kArrow);
  ip_textbox->OnMouseReleased(x, y);
  join_button->OnMouseReleased(x, y);
  back_button->OnMouseReleased(x, y);
}

void JoinCanvas::Join(const std::string& ip) {
  gLogi("gCanvas") << "Joining to " << ip;
  root->setCurrentCanvas(new TaskCanvas(root, CreateRef<JoinTask>(root, ip, 44025)));
}