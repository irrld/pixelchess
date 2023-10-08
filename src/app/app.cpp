/*
* gApp.cpp
*
*  Created on: May 6, 2020
*      Author: noyan
*/

#include "app.h"
#include <loading_canvas.h>
#include "game_canvas.h"
#include "menu_canvas.h"

gApp::gApp() {
}

gApp::gApp(int argc, char **argv) : gBaseApp(argc, argv) {
}

gApp::~gApp() {
}

void gApp::setup() {
 RenderUtil::Setup();
// auto* cnv = new GameCanvas(this, CreateRef<ChessConnectionDummy>(), nullptr);
 auto* cnv = new LoadingCanvas(this);
 appmanager->setCurrentCanvas(cnv);
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