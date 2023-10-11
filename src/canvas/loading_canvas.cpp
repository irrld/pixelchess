#include "loading_canvas.h"
#include "menu_canvas.h"

LoadingCanvas::LoadingCanvas(gApp* root) : gBaseCanvas(root) {
  this->root = root;
}

LoadingCanvas::~LoadingCanvas() {
}

void LoadingCanvas::setup() {
  brand_logo_.loadImage("teoncreative_logo.png");
  brand_width_ = brand_logo_.getWidth() / 4;
  brand_height_ = brand_logo_.getHeight() / 4;
  brand_x_ = (getWidth() - brand_width_) / 2;
  brand_y_ = (getHeight() - brand_height_) / 2;
  glist_logo_.loadImage("glistengine_logo.png");
  glist_width_ = glist_logo_.getWidth();
  glist_height_ = glist_logo_.getHeight();
  glist_x_ = (getWidth() - glist_width_) / 2;
  glist_y_ = (getHeight() - glist_height_) / 2;
  delta_ = 0.0f;
}

void LoadingCanvas::update() {
  delta_ += appmanager->getElapsedTime();
}

void LoadingCanvas::draw() {
  clearColor(CreateRGB(0x1c1e25));
  const gColor& old_color = renderer->getColor();
  if (delta_ < 3) {
    int opacity = delta_ * delta_ * 255;
    if (delta_ > 2) {
        opacity = 255 - (delta_ - 2) * (delta_ - 2) * 255;
    }
    renderer->setColor(255, 255, 255, std::min(opacity, 255));
    brand_logo_.draw(brand_x_, brand_y_, brand_width_, brand_height_);
  } else if (delta_ < 6) {
    double delta = this->delta_ - 3;
    int opacity = delta * delta * 255;
    if (delta > 2) {
        opacity = 255 - (delta - 2) * (delta - 2) * 255;
    }
    renderer->setColor(255, 255, 255, std::min(opacity, 255));
    glist_logo_.draw(glist_x_, glist_y_, glist_width_, glist_height_);
  } else {
    root->setCurrentCanvas(new MenuCanvas(root));
  }
  renderer->setColor(old_color);
}

void LoadingCanvas::keyPressed(int key) {
  //	gLogi("gCanvas") << "keyPressed:" << key;
}

void LoadingCanvas::keyReleased(int key) {
  //	gLogi("gCanvas") << "keyReleased:" << key;
  delta_ = 6;
}

void LoadingCanvas::charPressed(unsigned int codepoint) {
  //	gLogi("gCanvas") << "charPressed:" << gCodepointToStr(codepoint);
}

void LoadingCanvas::mouseMoved(int x, int y) {
  //	gLogi("gCanvas") << "mouseMoved" << ", x:" << x << ", y:" << y;
}

void LoadingCanvas::mouseDragged(int x, int y, int button) {
  //	gLogi("gCanvas") << "mouseDragged" << ", x:" << x << ", y:" << y << ", b:" << button;
}

void LoadingCanvas::mousePressed(int x, int y, int button) {
  //	gLogi("gCanvas") << "mousePressed" << ", x:" << x << ", y:" << y << ", b:" << button;
}

void LoadingCanvas::mouseReleased(int x, int y, int button) {
  //	gLogi("gCanvas") << "mouseReleased" << ", button:" << button;
  delta_ = 6;
}

void LoadingCanvas::mouseScrolled(int x, int y) {
  //	gLogi("gCanvas") << "mouseScrolled" << ", x:" << x << ", y:" << y;
}

void LoadingCanvas::mouseEntered() {

}

void LoadingCanvas::mouseExited() {

}

void LoadingCanvas::windowResized(int w, int h) {

}

void LoadingCanvas::showNotify() {

}

void LoadingCanvas::hideNotify() {

}