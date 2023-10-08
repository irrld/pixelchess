
#pragma once

#include <thread>
#include "app/app.h"
#include "app/button.h"
#include "gBaseCanvas.h"
#include "gImage.h"
#include "util.h"

class LoadingCanvas : public gBaseCanvas {
 public:
  LoadingCanvas(gApp* root);
  virtual ~LoadingCanvas();

  void setup();
  void update();
  void draw();

  void keyPressed(int key);
  void keyReleased(int key);
  void charPressed(unsigned int codepoint);
  void mouseMoved(int x, int y );
  void mouseDragged(int x, int y, int button);
  void mousePressed(int x, int y, int button);
  void mouseReleased(int x, int y, int button);
  void mouseScrolled(int x, int y);
  void mouseEntered();
  void mouseExited();
  void windowResized(int w, int h);

  void showNotify();
  void hideNotify();

 private:
  gApp* root;
  gImage brand_logo_;
  int brand_width_, brand_height_, brand_x_, brand_y_;
  gImage glist_logo_;
  int glist_width_, glist_height_, glist_x_, glist_y_;
  double delta_;
};