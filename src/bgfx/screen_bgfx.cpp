/*
    src/screen.cpp -- Top-level widget and interface between NanoGUI and GLFW

    A significant redesign of this code was contributed by Christian Schueller.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/screen.h>
#include <nanogui/theme.h>
#include <nanogui/window.h>
#include <nanogui/popup.h>
#include <nanovg.h>
#include <map>
#include <iostream>

NAMESPACE_BEGIN(nanogui)

std::map<void *, Screen *> __nanogui_screens;
void appForEachScreen(std::function<void(Screen*)> f)
{
  for (auto kv : __nanogui_screens)
    f(kv.second);
}

Screen::~Screen() {
    __nanogui_screens.erase(mHwWindow);
}

intptr_t Screen::createStandardCursor(int) { return 0; }

Screen::Screen()
    : Widget(nullptr), mHwWindow(nullptr), mNVGContext(nullptr),
      mCursor(Cursor::Arrow), mBackground(0.3f, 0.3f, 0.32f, 1.f),
      mShutdownOnDestruct(false), mFullscreen(false) {
    memset(mCursors, 0, sizeof(GLFWcursor *) * (int) Cursor::CursorCount);
}

Screen::Screen(const Vector2i &size, const std::string &caption, bool resizable,
               bool fullscreen, int colorBits, int alphaBits, int depthBits,
               int stencilBits, int nSamples,
               unsigned int glMajor, unsigned int glMinor)
    : Widget(nullptr), mHwWindow(nullptr), mNVGContext(nullptr),
      mCursor(Cursor::Arrow), mBackground(0.3f, 0.3f, 0.32f, 1.f), mCaption(caption),
      mShutdownOnDestruct(false), mFullscreen(fullscreen)
{
  mSize = size;
}

void Screen::drawAll() {
    drawContents();
    drawWidgets();
}

bool appIsShouldCloseScreen(Screen* screen) { return false; }
void Screen::setVisible(bool visible) { Widget::setVisible(visible); }
bool appPostEmptyEvent() { return false; }
bool appWaitEvents(void) { return false; }
bool appPollEvents() { return false; }
void Screen::_internalSetCursor(int) {}
void Screen::setClipboardString(const std::string & text) {}
std::string Screen::getClipboardString(void) { return ""; }

void Screen::initialize(void *handle, bool shutdownOnDestruct)
{
  mNVGContext = (NVGcontext*)handle;
  mHwWindow = handle;
  __nanogui_screens[handle] = this;

  mShutdownOnDestruct = shutdownOnDestruct;
  mPixelRatio = mSize.x() / mSize.y();
  _setupStartParams();
}

NAMESPACE_END(nanogui)