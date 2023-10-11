
#pragma once


#include "color.h"
#include "gBaseCanvas.h"
#include "app/app.h"
#include "app/button.h"
#include "task.h"

class TaskCanvas : public gBaseCanvas {
 public:
  TaskCanvas(gApp* root, Ref<Task> task);
  virtual ~TaskCanvas();

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
  double loading_anim;
  std::string title;
  Ref<Task> task;
  gImage white_pieces;
};