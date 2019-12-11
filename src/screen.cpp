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
  widgetsNeedUpdate.emplace_back(w);
}

void Screen::_setupStartParams()
{
    mVisible = true;
    setTheme(new Theme(mNVGContext));
    mMousePos = Vector2i::Zero();
    mMouseState = mModifiers = 0;
    mDragActive = false;
    mLastInteraction = getTimeFromStart();
    mProcessEvents = true;

    for (int i=0; i < (int) Cursor::CursorCount; ++i)
      mCursors[i] = createStandardCursor(i);

    /// Fixes retina display-related font rendering issue (#185)
    nvgBeginFrame(mNVGContext, mSize.x(), mSize.y(), mPixelRatio);
    nvgEndFrame(mNVGContext);
}

void Screen::drawWidgets() {
    if (!mVisible)
        return;

    if (!widgetsNeedUpdate.empty())
    {
      std::set<Widget*> ws;
      for (auto w : widgetsNeedUpdate)
        ws.insert(w);

      for (auto c: widgetsNeedUpdate)
      {
        for (auto it = ws.begin(); it != ws.end(); )
        {
          if (c->areParentsContain(*it))
            it = ws.erase(it);
          else it++;
        }
      }

      widgetsNeedUpdate.clear();
      for (auto& c : ws)
        c->performLayout(mNVGContext);
    }

    _drawWidgetsBefore();

    nvgBeginFrame(mNVGContext, mSize[0], mSize[1], mPixelRatio);

    draw(mNVGContext);
    afterDraw(mNVGContext);

    double elapsed = getTimeFromStart() - mLastInteraction;

    if (elapsed > 0.5f) {
        /* Draw tooltips */
        const Widget *widget = findWidget(mMousePos);
        if (widget && !widget->tooltip().empty()) {
            int tooltipWidth = 150;

            float bounds[4];
            nvgFontFace(mNVGContext, "sans");
            nvgFontSize(mNVGContext, 15.0f);
            nvgTextAlign(mNVGContext, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
            nvgTextLineHeight(mNVGContext, 1.1f);
            Vector2i pos = widget->absolutePosition() +
                           Vector2i(widget->width() / 2, widget->height() + 10);

            nvgTextBounds(mNVGContext, pos.x(), pos.y(),
                            widget->tooltip().c_str(), nullptr, bounds);
            int h = (bounds[2] - bounds[0]) / 2;
            if (h > tooltipWidth / 2) {
                nvgTextAlign(mNVGContext, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
                nvgTextBoxBounds(mNVGContext, pos.x(), pos.y(), tooltipWidth,
                                widget->tooltip().c_str(), nullptr, bounds);

                h = (bounds[2] - bounds[0]) / 2;
            }
            nvgGlobalAlpha(mNVGContext,
                           std::min<float>(1.0, 2 * (elapsed - 0.5f)) * 0.8);

            nvgBeginPath(mNVGContext);
            nvgFillColor(mNVGContext, theme()->mTooltipBackgroundColor.mul_a(theme()->mTooltipOpacity));
            nvgRoundedRect(mNVGContext, bounds[0] - 4 - h, bounds[1] - 4,
                           (int) (bounds[2] - bounds[0]) + 8,
                           (int) (bounds[3] - bounds[1]) + 8, 3);

            int px = (int) ((bounds[2] + bounds[0]) / 2) - h;
            nvgMoveTo(mNVGContext, px, bounds[1] - 10);
            nvgLineTo(mNVGContext, px + 7, bounds[1] + 1);
            nvgLineTo(mNVGContext, px - 7, bounds[1] + 1);
            nvgFill(mNVGContext);

            nvgFillColor(mNVGContext, theme()->mTooltipTextColor.mul_a(theme()->mTooltipOpacity));
            nvgFontBlur(mNVGContext, 0.0f);
            nvgTextBox(mNVGContext, pos.x() - h, pos.y(), tooltipWidth,
                       widget->tooltip().c_str(), nullptr);
        }
    }

    nvgEndFrame(mNVGContext);
}

bool Screen::keyboardEvent(int key, int scancode, int action, int modifiers) {
    if (mFocusPath.size() > 0) {
        for (auto it = mFocusPath.rbegin() + 1; it != mFocusPath.rend(); ++it)
            if ((*it)->focused() && (*it)->keyboardEvent(key, scancode, action, modifiers))
                return true;
    }

    return false;
}

bool Screen::keyboardCharacterEvent(unsigned int codepoint) {
    if (mFocusPath.size() > 0) {
        for (auto it = mFocusPath.rbegin() + 1; it != mFocusPath.rend(); ++it)
            if ((*it)->focused() && (*it)->keyboardCharacterEvent(codepoint))
                return true;
    }
    return false;
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
                _internalSetCursor((int)mCursor);
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
        if (mFocusPath.size() > 1) {
            const Window *dwindow = mFocusPath[mFocusPath.size() - 2]->cast<Window>();
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
            _internalSetCursor( (int)mCursor);
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

bool Screen::keyCallbackEvent(int key, int scancode, int action, int mods) {
    mLastInteraction = getTimeFromStart();
    return keyboardEvent(key, scancode, action, mods);
}

bool Screen::charCallbackEvent(unsigned int codepoint) {
    mLastInteraction = getTimeFromStart();
    return keyboardCharacterEvent(codepoint);
}

bool Screen::dropCallbackEvent(int count, const char **filenames) {
    std::vector<std::string> arg(count);
    for (int i = 0; i < count; ++i)
        arg[i] = filenames[i];
    return dropEvent(arg);
}

bool Screen::scrollCallbackEvent(double x, double y) {
    mLastInteraction = getTimeFromStart();
        if (mFocusPath.size() > 1) {
            const Window *window = mFocusPath[mFocusPath.size() - 2]->cast<Window>();
            if (window && window->modal()) {
                if (!window->contains(mMousePos))
                    return false;
            }
        }
        return scrollEvent(mMousePos, Vector2f(x, y));
}

void Screen::updateFocus(Widget *widget) {
    // Save old focus path
    auto oldFocusPath = mFocusPath;
    mFocusPath.clear();
    // Generate new focus path
    Widget *window = nullptr;
    while (widget) {
        mFocusPath.push_back(widget);
        if (widget->cast<Window>())
            window = widget;
        widget = widget->parent();
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
        window->setSize(window->preferredSize(mNVGContext));
        window->performLayout(mNVGContext);
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
          Popup *pw = mChildren[index]->cast<Popup>();
          if (pw && pw->parentWindow() == window && index < baseIndex) {
              moveWindowToFront(pw);
              changed = true;
              break;
          }
      }
  } while (changed);
}


NAMESPACE_END(nanogui)
