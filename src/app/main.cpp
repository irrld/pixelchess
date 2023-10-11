/*
 * Copyright (C) 2014 Nitra Games Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "app.h"
#include "gAppManager.h"

#ifdef WIN32
#include <windows.h>

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil) {
  gStartEngine(new gApp(), "Chess Tacos (Windows)", G_WINDOWMODE_APP, 1280, 720);
  return 0;
}
#else
int main(int argc, char **argv) {
  gStartEngine(new gApp(argc, argv), "Chess Tacos", G_WINDOWMODE_APP, 1280, 720);
  return 0;
}
#endif

