#include "message_canvas.h"
#include "menu_canvas.h"

MessageCanvas::MessageCanvas(gApp* root, std::string message) : gBaseCanvas(root), message(message) {
  this->root = root;
}

MessageCanvas::~MessageCanvas() {
}

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

void MessageCanvas::keyPressed(int key) {
  //	gLogi("gCanvas") << "keyPressed:" << key;
}

void MessageCanvas::keyReleased(int key) {
  //	gLogi("gCanvas") << "keyReleased:" << key;
}

void MessageCanvas::charPressed(unsigned int codepoint) {
  //	gLogi("gCanvas") << "charPressed:" << gCodepointToStr(codepoint);
}

void MessageCanvas::mouseMoved(int x, int y) {
  //	gLogi("gCanvas") << "mouseMoved" << ", x:" << x << ", y:" << y;
  back_button->OnMouseMoved(x, y);
}

void MessageCanvas::mouseDragged(int x, int y, int button) {
  //	gLogi("gCanvas") << "mouseDragged" << ", x:" << x << ", y:" << y << ", b:" << button;
}

void MessageCanvas::mousePressed(int x, int y, int button) {
  //	gLogi("gCanvas") << "mousePressed" << ", x:" << x << ", y:" << y << ", b:" << button;
  back_button->OnMousePressed(x, y);
}

void MessageCanvas::mouseReleased(int x, int y, int button) {
  //	gLogi("gCanvas") << "mouseReleased" << ", button:" << button;
  back_button->OnMouseReleased(x, y);
}

void MessageCanvas::mouseScrolled(int x, int y) {
  //	gLogi("gCanvas") << "mouseScrolled" << ", x:" << x << ", y:" << y;
}

void MessageCanvas::mouseEntered() {

}

void MessageCanvas::mouseExited() {

}

void MessageCanvas::windowResized(int w, int h) {

}

void MessageCanvas::showNotify() {

}

void MessageCanvas::hideNotify() {

}