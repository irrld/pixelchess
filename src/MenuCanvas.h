/*
* gCanvas.h
*
*  Created on: May 6, 2020
*      Author: noyan
*/

#ifndef GCANVAS_H_
#define GCANVAS_H_

#include <net/ChessConnection.h>
#include <thread>
#include "Util.h"
#include "app/Button.h"
#include "app/gApp.h"
#include "gBaseCanvas.h"
#include "gImage.h"

class Task {
 public:
  virtual ~Task() {}
  virtual bool Update() = 0;
  virtual Ref<Task> Next() = 0;
  virtual const std::string& GetTitle() = 0;
};

class HostTask : public Task {
 public:
  HostTask(gApp* root);
  ~HostTask();
  bool Update() override;
  Ref<Task> Next() override;

  const std::string& GetTitle() override;
 private:
  std::string title;
  gApp* root;
  Ref<znet::Server> server;
  Ref<std::thread> thread;
  bool completed = false;
  Ref<ChessConnectionLocal> connection;
};

class JoinTask : public Task {
 public:
  JoinTask(gApp* root, std::string host, int port);
  ~JoinTask();
  bool Update() override;
  Ref<Task> Next() override;

  const std::string& GetTitle() override;
 private:
  std::string title;
  std::string host;
  int port;
  gApp* root;
  Ref<znet::Client> server;
  Ref<std::thread> thread;
  bool completed = false;
  Ref<ChessConnectionNetwork> connection;
};

class MenuCanvas : public gBaseCanvas {
 public:
  MenuCanvas(gApp* root);
  virtual ~MenuCanvas();

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
  Button* join_button;
  Button* host_button;
  Button* settings_button;
  gFont logo_font;
  gImage black_pieces;
  gImage white_pieces;
  bool loading;
  double loading_anim;
  std::string title;
  Ref<Task> task;
};

#endif /* GCANVAS_H_ */
