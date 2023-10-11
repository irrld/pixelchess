#pragma once

#include <thread>

#include "gBaseCanvas.h"
#include "gImage.h"

#include "net/chess_connection.h"
#include "task.h"
#include "app/app.h"
#include "app/button.h"
#include "color.h"
#include "util.h"
#include "ver.h"

class MenuCanvas : public gBaseCanvas {
 public:
  MenuCanvas(gApp* root);
  virtual ~MenuCanvas();

  void setup();
  void update();
  void draw();

  void keyPressed(int key) {}
  void keyReleased(int key) {}
  void charPressed(unsigned int codepoint) {}
  void mouseMoved(int x, int y);
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
  Button* join_button;
  Button* host_button;
  Button* quit_button;
  gFont logo_font;
  bool fade_in_;
  RGB fade_color_;
  RGB back_color_;
  RGB clear_color_;
  double fade_in_timer_;
};
