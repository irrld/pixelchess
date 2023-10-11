
#pragma once

#include <app/textbox.h>
#include "app/app.h"
#include "app/button.h"
#include "color.h"
#include "gBaseCanvas.h"
#include "task.h"

class JoinCanvas : public gBaseCanvas {
 public:
  JoinCanvas(gApp* root);
  virtual ~JoinCanvas();

  void setup();
  void update();
  void draw();

  void keyPressed(int key);
  void keyReleased(int key);
  void charPressed(unsigned int codepoint);
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

  void Join(const std::string& ip);

 private:
  gApp* root;
  Textbox* ip_textbox;
  Button* join_button;
  Button* back_button;
};