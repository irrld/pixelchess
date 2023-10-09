/*
* gApp.h
*
*  Created on: May 6, 2020
*      Author: noyan
*/

#ifndef GAPP_H_
#define GAPP_H_

#include "gBaseApp.h"

enum CursorType {
  kArrow,
  kHand,
  kHandClosed
};

class gApp : public gBaseApp {
 public:
  gApp();
  gApp(int argc, char **argv);
  ~gApp();

  void setup();
  void update();

  void stop();

  void SetCursorPos(int x, int y);
  void SetCursorType(CursorType type);
  void DrawCursor();

  bool loaded_ = false;

 private:
  gImage* cursor_;
  CursorType cursor_type_ = kArrow;
  int cursor_width_, cursor_height_;
  int cursor_pos_x_ = -100, cursor_pos_y_ = -100;

};

#endif /* GAPP_H_ */
