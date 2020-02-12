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
#include <nanogui/window.h>
#include <nanogui/popup.h>
#include <set>
#include <nanovg.h>
#include <algorithm>
#include <iostream>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(Screen, Object)

void Screen::addChild(int index, Widget * widget)
{
  Widget::addChild(index, widget);
}

void Screen::needPerformLayout(Widget* w)
{
  if (w == this)
    mWidgetsNeedUpdateGlobal = true;
  else
    mWidgetsNeedUpdate.emplace_back(w);
}

/// Return a pointer to the underlying nanoVG draw context
NVGcontext *Screen::nvgContext() { return __nanogui_context; }

void Screen::_setupStartParams()
{
    mVisible = true;
    setTheme<Theme>();
    mMousePos = Vector2i::Zero();
    mMouseState = mModifiers = 0;
    mDragActive = false;
    mLastInteraction = getTimeFromStart();
    mProcessEvents = true;

    for (int i=0; i < (int) Cursor::CursorCount; ++i)
      mCursors[i] = createStandardCursor(i);
}

void Screen::drawAll() 
{
  drawContents();
  drawWidgets();
}

Screen::~Screen() 
{
  for (int i = 0; i < (int)Cursor::CursorCount; ++i) {
    if (mCursors[i])
      __nanogui_destroy_cursor(mCursors[i]);
  }
}

intptr_t Screen::createStandardCursor(int shape) { return __nanogui_create_cursor(shape); }

Screen::Screen()
  : Widget(nullptr),
  mCursor(Cursor::Arrow), mBackground(0.3f, 0.3f, 0.32f, 1.f),
  mShutdownOnDestruct(false), mFullscreen(false)
{
  memset(mCursors, 0, (int)Cursor::CursorCount * sizeof(intptr_t));
}

Screen::Screen(const Vector2i &size, const std::string &caption, bool fullscreen)
  : Screen()
{
  mFullscreen = fullscreen;
  mCaption = caption;
  mSize = size;
  mVisible = true;

  _setupStartParams();
}

void Screen::setSizeFBSize(const Vector2i& size, const Vector2i& )
{
#if defined(_WIN32) || defined(__linux__)
  mSize = (size / mPixelRatio).cast<int>();
  mFBSize = (size * mPixelRatio).cast<int>();
#else
  /* Recompute pixel ratio on OSX */
  if (size[0])
    mPixelRatio = (float)mFBSize[0] / (float)mSize[0];
#endif
}

void Screen::setVisible(bool visible) { mVisible = visible; }

void Screen::setCaption(const std::string &caption) { mCaption = caption; }

void Screen::setSize(const Vector2i &size) { Widget::setSize(size); }

bool Screen::resizeCallbackEvent(int, int) {
  Vector2i fbSize, size;
  if (mResizeHwHandler)
    mResizeHwHandler(this, size, fbSize);

#if defined(_WIN32) || defined(__linux__)
  size = (size.cast<float>() / mPixelRatio).cast<int>();
#endif

  if (fbSize == Vector2i(0, 0) || size == Vector2i(0, 0))
    return false;

  mFBSize = fbSize; mSize = size;
  mLastInteraction = getTimeFromStart();

  return resizeEvent(mSize);
}

void Screen::drawWidgets() {
    if (!mVisible)
        return;
    if (mWidgetsNeedUpdateGlobal)
    {
      mWidgetsNeedUpdate.clear();
      mWidgetsNeedUpdateGlobal = false;

      performLayout();
    }
    else if (!mWidgetsNeedUpdate.empty())
    {
      std::set<Widget*> ws;
      for (auto w : mWidgetsNeedUpdate)
        ws.insert(w);

      for (auto c: mWidgetsNeedUpdate)
      {
        for (auto it = ws.begin(); it != ws.end(); )
        {
          if (c->areParentsContain(*it))
            it = ws.erase(it);
          else it++;
        }
      }

      mWidgetsNeedUpdate.clear();
      for (auto& c : ws)
        c->performLayout(nvgContext());
    }

    if (mPrepareFrameFunc)
      mPrepareFrameFunc(this);

    nvgBeginFrame(nvgContext(), mSize.x(), mSize.y(), mPixelRatio);

    draw(nvgContext());
    afterDraw(nvgContext());

    /*if (mFocusRequested)
    {
      updateFocus(mFocusRequested);
      mFocusRequested = nullptr;
    }*/

    double elapsed = getTimeFromStart() - mLastInteraction;

    if (elapsed > 0.5f) {
        /* Draw tooltips */
        const Widget *widget = findWidget(mMousePos);
        if (widget && !widget->tooltip().empty()) {
            int tooltipWidth = 150;

            float bounds[4];
            nvgFontFaceSize(nvgContext(), "sans", 15.0f);
            nvgTextAlign(nvgContext(), NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
            nvgTextLineHeight(nvgContext(), 1.1f);
            Vector2i pos = widget->absolutePosition() +
                           Vector2i(widget->width() / 2, widget->height() + 10);

            nvgTextBounds(nvgContext(), pos.x(), pos.y(),
                            widget->tooltip().c_str(), nullptr, bounds);
            int h = (bounds[2] - bounds[0]) / 2;
            if (h > tooltipWidth / 2) {
                nvgTextAlign(nvgContext(), NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
                nvgTextBoxBounds(nvgContext(), pos.x(), pos.y(), tooltipWidth,
                                widget->tooltip().c_str(), nullptr, bounds);

                h = (bounds[2] - bounds[0]) / 2;
            }
            nvgGlobalAlpha(nvgContext(), std::min<float>(1.0, 2 * (elapsed - 0.5f)) * 0.8);

            nvgBeginPath(nvgContext());
            nvgFillColor(nvgContext(), theme()->mTooltipBackgroundColor.mul_a(theme()->mTooltipOpacity));
            nvgRoundedRect(nvgContext(), bounds[0] - 4 - h, bounds[1] - 4,
                           (int) (bounds[2] - bounds[0]) + 8,
                           (int) (bounds[3] - bounds[1]) + 8, 3);

            int px = (int) ((bounds[2] + bounds[0]) / 2) - h;
            nvgMoveTo(nvgContext(), px, bounds[1] - 10);
            nvgLineTo(nvgContext(), px + 7, bounds[1] + 1);
            nvgLineTo(nvgContext(), px - 7, bounds[1] + 1);
            nvgFill(nvgContext());

            nvgFillColor(nvgContext(), theme()->mTooltipTextColor.mul_a(theme()->mTooltipOpacity));
            nvgFontBlur(nvgContext(), 0.0f);
            nvgTextBox(nvgContext(), pos.x() - h, pos.y(), tooltipWidth,
                       widget->tooltip().c_str(), nullptr);
        }
    }

    nvgEndFrame(nvgContext());
}

bool Screen::resizeEvent(const Vector2i& size) {
    if (mResizeCallback) {
        mResizeCallback(size);
        return true;
    }
    return false;
}

bool Screen::cursorPosCallbackEvent(double x, double y) {
    Vector2i p((int) x, (int) y);

#if defined(_WIN32) || defined(__linux__)
    p = (p.cast<float>() / mPixelRatio).cast<int>();
#endif

    bool ret = false;
    mLastInteraction = getTimeFromStart();
#if NANOGUI_USING_EXCEPTIONS
    try {
#endif
        p -= Vector2i(1, 2);

        if (!mDragActive) {
            Widget *widget = findWidget(p);
            if (widget != nullptr && widget->cursor() != mCursor) {
                mCursor = widget->cursor();
                if (mHwCursorSetter)
                  mHwCursorSetter(mCursors[(int)mCursor]);
            }
        } else {
            ret = mDragWidget->mouseDragEvent(
                p - mDragWidget->parent()->absolutePosition(), p - mMousePos,
                mMouseState, mModifiers);
        }

        if (!ret)
            ret = mouseMotionEvent(p, p - mMousePos, mMouseState, mModifiers);

        mMousePos = p;

        return ret;
#if NANOGUI_USING_EXCEPTIONS
    } catch (const std::exception &e) {
        std::cerr << "Caught exception in event handler: " << e.what() << std::endl;
        return false;
    }
#endif
}

bool Screen::mouseButtonCallbackEvent(int button, int action, int modifiers) {
    mModifiers = modifiers;
    mLastInteraction = getTimeFromStart();
#if NANOGUI_USING_EXCEPTIONS
    try {
#endif
        if (!mFocusPath.empty()) {
            const Window *dwindow = Window::cast(mFocusPath.back());
            if (dwindow && dwindow->modal()) {
                if (!dwindow->contains(mMousePos))
                    return false;
            }
        }

        if (isMouseActionPress(action))
            mMouseState |= 1 << button;
        else
            mMouseState &= ~(1 << button);

        auto dropWidget = findWidget(mMousePos);
        if (mDragActive && isMouseActionRelease(action) &&
            dropWidget != mDragWidget)
            mDragWidget->mouseButtonEvent(
                mMousePos - mDragWidget->parent()->absolutePosition(), button,
                false, mModifiers);

        if (dropWidget != nullptr && dropWidget->cursor() != mCursor) {
            mCursor = dropWidget->cursor();
            if (mHwCursorSetter)
              mHwCursorSetter(mCursors[(int)mCursor]);
        }

        if ( isMouseActionPress(action) && (isMouseButtonLeft(button) || isMouseButtonRight(button))) {
            mDragWidget = findWidget(mMousePos);
            if (mDragWidget == this)
                mDragWidget = nullptr;
            mDragActive = mDragWidget != nullptr;
            if (!mDragActive)
                updateFocus(nullptr);
        } else {
            mDragActive = false;
            mDragWidget = nullptr;
        }

        return mouseButtonEvent(mMousePos, button, isMouseActionPress(action), mModifiers);
#if NANOGUI_USING_EXCEPTIONS
    } catch (const std::exception &e) {
        std::cerr << "Caught exception in event handler: " << e.what() << std::endl;
        return false;
    }
#endif
}

void resolveTabSequence(Widget* w, std::vector<Widget*>& arr)
{
  if (w->visible())
  {
    if (w->tabstop(Widget::TabStopSelf))
      arr.push_back(w);

    if (w->tabstop(Widget::TabStopChildren))
    {
      for (auto& c : w->children())
        resolveTabSequence(c, arr);
    }
  }
}

bool Screen::keyCallbackEvent(int key, int scancode, int action, int mods) 
{
    mLastInteraction = getTimeFromStart();

    bool resolved = false;
    if (!mFocusPath.empty())
      resolved = mFocusPath.front()->keyboardEvent(key, scancode, action, mods);

    if (!resolved)
    {
      if (isKeyboardActionPress(action) || isKeyboardActionRepeat(action)) 
      {
        Widget* selected = getCurrentSelection();
        bool kbup = isKeyboardKey(key, "KBUP");
        bool kbdown = isKeyboardKey(key, "DOWN");
        if (kbup || kbdown) 
        {
          std::vector<Widget*> tabSequence;
          resolveTabSequence(selected->window(), tabSequence);
          
          Widget* focusRequested = nullptr;
          if (kbdown)
          {
            auto it = std::find(tabSequence.begin(), tabSequence.end(), selected);
            if (it != tabSequence.end())
              focusRequested = (it + 1) != tabSequence.end() ? *(it + 1) : nullptr;
          }
          else
          {
            auto it = std::find(tabSequence.rbegin(), tabSequence.rend(), selected);
            if (it != tabSequence.rend())
              focusRequested = (it + 1) != tabSequence.rend() ? *(it + 1) : nullptr;
          }

          if (focusRequested)
            updateFocus(focusRequested);
        }
      }
    }

    return resolved;
}

bool Screen::charCallbackEvent(unsigned int codepoint) {
    mLastInteraction = getTimeFromStart();
    if (!mFocusPath.empty())
      return mFocusPath.front()->keyboardCharacterEvent(codepoint);

    return false;
}

bool Screen::dropCallbackEvent(int count, const char **filenames) {
    std::vector<std::string> arg(count);
    for (int i = 0; i < count; ++i)
        arg[i] = filenames[i];
    return dropEvent(arg);
}

bool Screen::scrollCallbackEvent(double x, double y) {
    mLastInteraction = getTimeFromStart();
        if (!mFocusPath.empty()) {
            const Window *window = Window::cast(mFocusPath.back());
            if (window && window->modal()) {
                if (!window->contains(mMousePos))
                    return false;
            }
        }
        return scrollEvent(mMousePos, Vector2f(x, y));
}

Widget* Screen::getCurrentSelection() const { return mSelectedWidget; }

void Screen::updateFocus(Widget *widget) {
    // Save old focus path
    auto oldFocusPath = mFocusPath;
    mFocusPath.clear();
    // Generate new focus path
    Widget *window = nullptr;
    mSelectedWidget = nullptr;
    if (widget)
    {
      while (widget != this) {
        if (!mSelectedWidget)
          mSelectedWidget = widget;

        mFocusPath.push_back(widget);
        if (Window::cast(widget))
          window = widget;
        widget = widget->parent();
      }
    }
    // Send unfocus events to widgets losing focus.
    for (auto w : oldFocusPath) {
        if (!w->focused() || std::find(mFocusPath.begin(), mFocusPath.end(), w) != mFocusPath.end())
            continue;
        w->focusEvent(false);
    }
    // Send focus events to widgets gaining focus.
    for (auto it = mFocusPath.rbegin(); it != mFocusPath.rend(); ++it)
        (*it)->focusEvent(true);

    if (window)
        moveWindowToFront((Window *) window);
}

void Screen::disposeWindow(Window *window) {
    if (std::find(mFocusPath.begin(), mFocusPath.end(), window) != mFocusPath.end())
        mFocusPath.clear();
    if (mDragWidget == window)
        mDragWidget = nullptr;
    removeChild(window);
}

void Screen::centerWindow(Window *window) {
    if (window->size() == Vector2i::Zero()) {
        window->setSize(window->preferredSize(nvgContext()));
        window->performLayout(nvgContext());
    }
    window->setPosition((mSize - window->size()) / 2);
}

void Screen::moveWindowToFront(Window *window) {
  if (!isMyChild(window))
    return;

  mChildren.erase(std::remove(mChildren.begin(), mChildren.end(), window), mChildren.end());
  mChildren.push_back(window);
  /* Brute force topological sort (no problem for a few windows..) */
  bool changed = false;
  do {
      size_t baseIndex = 0;
      for (size_t index = 0; index < mChildren.size(); ++index)
          if (mChildren[index] == window)
              baseIndex = index;
      changed = false;
      for (size_t index = 0; index < mChildren.size(); ++index) {
          Popup *pw = Popup::cast(mChildren[index]);
          if (pw && pw->parentWindow() == window && index < baseIndex) {
              moveWindowToFront(pw);
              changed = true;
              break;
          }
      }
  } while (changed);
}

NAMESPACE_END(nanogui)
