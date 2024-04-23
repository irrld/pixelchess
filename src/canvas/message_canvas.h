
#pragma once

#include <thread>
#include "app/app.h"
#include "app/button.h"
#include "gBaseCanvas.h"
#include "gImage.h"
#include "util.h"

class MessageCanvas : public gBaseCanvas {
 public:
  MessageCanvas(gApp* root, const std::string& message);
  virtual ~MessageCanvas();

  void setup();
  void update();
  void draw();

  void keyPressed(int key) {}
  void keyReleased(int key) {}
  void charPressed(unsigned int codepoint) {}
  void mouseMoved(int x, int y );
  void mouseDragged(int x, int y, int button) {
    mouseMoved(x, y);
  }
  void mousePressed(int x, int y, int button);
  void mouseReleased(int x, int y, int button);
  void mouseScrolled(int x, int y) {}
  void mouseEntered() {}
  void mouseExited() {}
  void windowResized(int w, int h) {}

  void showNotify() {}
  void hideNotify() {}

 private:
  gApp* root;
  std::string message;
  Button* back_button;
};