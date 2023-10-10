/*
* gApp.cpp
*
*  Created on: May 6, 2020
*      Author: noyan
*/

#include "app.h"
#include "canvas/loading_canvas.h"
#include "canvas/game_canvas.h"

gApp::gApp() {
}

gApp::gApp(int argc, char **argv) : gBaseApp(argc, argv) {
}

gApp::~gApp() {
}

void gApp::setup() {
  RenderUtil::Setup();
  cursor_ = new gImage();
  cursor_->loadImage("cursor.png");
  cursor_->setFiltering(2, 2);
  cursor_height_ = cursor_->getHeight();
  cursor_width_ = 16;
  //auto* cnv = new GameCanvas(this, CreateRef<ChessConnectionDummy>(), nullptr);
  auto* cnv = new LoadingCanvas(this);
  appmanager->setCurrentCanvas(cnv);
  appmanager->getWindow()->setCursorMode(0x00034002); // GLFW_CURSOR_HIDDEN
  appmanager->setTargetFramerate(1000);
}

void gApp::update() {
}

void gApp::stop() {
  auto* canvas = appmanager->getCurrentCanvas();
  if (canvas != nullptr) {
    canvas->hideNotify();
    delete canvas;
  }
}

void gApp::SetCursorPos(int x, int y) {
  cursor_pos_x_ = x;
  cursor_pos_y_ = y;
}

void gApp::SetCursorType(CursorType type) {
  cursor_type_ = type;
}

void gApp::DrawCursor() {
  cursor_->drawSub(cursor_pos_x_ - cursor_width_ * 2, cursor_pos_y_ - cursor_height_,
                   cursor_width_ * 4, cursor_height_ * 4,
                   cursor_type_ * 16, 0,
                   cursor_width_, cursor_height_);
}