/*
* gApp.cpp
*
*  Created on: May 6, 2020
*      Author: noyan
*/

#include "gApp.h"
#include "GameCanvas.h"
#include "MenuCanvas.h"

gApp::gApp() {
}

gApp::gApp(int argc, char **argv) : gBaseApp(argc, argv) {
}

gApp::~gApp() {
}

void gApp::setup() {
 RenderUtil::Setup();
 MenuCanvas* cnv = new MenuCanvas(this);
 appmanager->setCurrentCanvas(cnv);
}

void gApp::update() {
}
