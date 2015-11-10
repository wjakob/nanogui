/*
    src/screen.cpp -- Top-level widget and interface between NanoGUI and GLFW

    A significant redesign of this code was contributed by Christian Schueller.

    NanoGUI was developed by Wenzel Jakob <wenzel@inf.ethz.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/screen_core.h>
#include <nanogui/theme.h>
#include <nanogui/popup.h>
#include <iostream>

/* Allow enforcing the GL2 implementation of NanoVG */
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>

NAMESPACE_BEGIN(nanogui)

ScreenCore::ScreenCore()
    : Widget(nullptr), mNVGContext(nullptr), mCursor(Cursor::Arrow) { }

ScreenCore::~ScreenCore() {
    release();
}

void ScreenCore::release() {
    if (mNVGContext)
        nvgDeleteGL3(mNVGContext);
}

void ScreenCore::init(const Vector2i &s, float pRatio) {
    mSize = s;
    mPixelRatio = pRatio;

#ifdef NDEBUG
    mNVGContext = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_ANTIALIAS);
#else
    mNVGContext = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_ANTIALIAS | NVG_DEBUG);
#endif
    if (mNVGContext == nullptr)
        throw std::runtime_error("Could not initialize NanoVG!");

    mTheme = new Theme(mNVGContext);
    mMousePos = Vector2i::Zero();
    mMouseState = mModifiers = 0;
    mDragActive = false;
    mLastInteraction = glfwGetTime();
    mProcessEvents = true;
}

void ScreenCore::setPixelRatio(float pRatio) {
    mPixelRatio = pRatio;
    mLastInteraction = glfwGetTime();
}

void ScreenCore::drawWidgets() {
    nvgBeginFrame(mNVGContext, mSize[0], mSize[1], mPixelRatio);

    draw(mNVGContext);

    double elapsed = glfwGetTime() - mLastInteraction;

    if (elapsed > 0.5f) {
        /* Draw tooltips */
        const Widget *widget = findWidget(mMousePos);
        if (widget && !widget->tooltip().empty()) {
            int tooltipWidth = 150;

            float bounds[4];
            nvgFontFace(mNVGContext, "sans");
            nvgFontSize(mNVGContext, 15.0f);
            nvgTextAlign(mNVGContext, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
            nvgTextLineHeight(mNVGContext, 1.1f);
            Vector2i pos = widget->absolutePosition() +
                           Vector2i(widget->width() / 2, widget->height() + 10);

            nvgTextBoxBounds(mNVGContext, pos.x(), pos.y(), tooltipWidth,
                             widget->tooltip().c_str(), nullptr, bounds);

            nvgGlobalAlpha(mNVGContext,
                           std::min(1.0, 2 * (elapsed - 0.5f)) * 0.8);

            nvgBeginPath(mNVGContext);
            nvgFillColor(mNVGContext, Color(0, 255));
            int h = (bounds[2] - bounds[0]) / 2;
            nvgRoundedRect(mNVGContext, bounds[0] - 4 - h, bounds[1] - 4,
                           (int) (bounds[2] - bounds[0]) + 8,
                           (int) (bounds[3] - bounds[1]) + 8, 3);

            int px = (int) ((bounds[2] + bounds[0]) / 2) - h;
            nvgMoveTo(mNVGContext, px, bounds[1] - 10);
            nvgLineTo(mNVGContext, px + 7, bounds[1] + 1);
            nvgLineTo(mNVGContext, px - 7, bounds[1] + 1);
            nvgFill(mNVGContext);

            nvgFillColor(mNVGContext, Color(255, 255));
            nvgFontBlur(mNVGContext, 0.0f);
            nvgTextBox(mNVGContext, pos.x() - h, pos.y(), tooltipWidth,
                       widget->tooltip().c_str(), nullptr);
        }
    }

    nvgEndFrame(mNVGContext);
}

bool ScreenCore::keyboardEvent(int key, int scancode, int action, int modifiers) {
    if (mFocusPath.size() > 0) {
        for (auto it = mFocusPath.rbegin() + 1; it != mFocusPath.rend(); ++it)
            if ((*it)->focused() && (*it)->keyboardEvent(key, scancode, action, modifiers))
                return true;
    }

    return false;
}

bool ScreenCore::keyboardCharacterEvent(unsigned int codepoint) {
    if (mFocusPath.size() > 0) {
        for (auto it = mFocusPath.rbegin() + 1; it != mFocusPath.rend(); ++it)
            if ((*it)->focused() && (*it)->keyboardCharacterEvent(codepoint))
                return true;
    }
    return false;
}

bool ScreenCore::cursorPosCallbackEvent(double x, double y) {
    Vector2i p((int) x, (int) y);
    bool ret = false;
    mLastInteraction = glfwGetTime();
    try {
        p -= Vector2i(1, 2);

        if (!mDragActive) {
            Widget *widget = findWidget(p);
            if (widget != nullptr && widget->cursor() != mCursor) {
                mCursor = widget->cursor();
                setCursorAppearance((int) mCursor);
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
    } catch (const std::exception &e) {
        std::cerr << "Caught exception in event handler: " << e.what() << std::endl;
        abort();
    }

    return false;
}

bool ScreenCore::mouseButtonCallbackEvent(int button, int action, int modifiers) {
    mModifiers = modifiers;
    mLastInteraction = glfwGetTime();
    try {
        if (mFocusPath.size() > 1) {
            const Window *window =
                dynamic_cast<Window *>(mFocusPath[mFocusPath.size() - 2]);
            if (window && window->modal()) {
                if (!window->contains(mMousePos))
                    return false;
            }
        }

        if (action == GLFW_PRESS)
            mMouseState |= 1 << button;
        else
            mMouseState &= ~(1 << button);

        auto dropWidget = findWidget(mMousePos);
        if (mDragActive && action == GLFW_RELEASE &&
            dropWidget != mDragWidget)
            mDragWidget->mouseButtonEvent(
                mMousePos - mDragWidget->parent()->absolutePosition(), button,
                false, mModifiers);

        if (dropWidget != nullptr && dropWidget->cursor() != mCursor) {
            mCursor = dropWidget->cursor();
            setCursorAppearance((int) mCursor);
        }

        if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_1) {
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

        return mouseButtonEvent(mMousePos, button, action == GLFW_PRESS,
                                mModifiers);
    } catch (const std::exception &e) {
        std::cerr << "Caught exception in event handler: " << e.what() << std::endl;
        abort();
    }

    return false;
}

bool ScreenCore::keyCallbackEvent(int key, int scancode, int action, int mods) {
    mLastInteraction = glfwGetTime();
    try {
        return keyboardEvent(key, scancode, action, mods);
    } catch (const std::exception &e) {
        std::cerr << "Caught exception in event handler: " << e.what() << std::endl;
        abort();
    }
}

bool ScreenCore::charCallbackEvent(unsigned int codepoint) {
    mLastInteraction = glfwGetTime();
    try {
        return keyboardCharacterEvent(codepoint);
    } catch (const std::exception &e) {
        std::cerr << "Caught exception in event handler: " << e.what()
                  << std::endl;
        abort();
    }
}

bool ScreenCore::scrollCallbackEvent(double x, double y) {
    mLastInteraction = glfwGetTime();
    try {
        if (mFocusPath.size() > 1) {
            const Window *window =
                dynamic_cast<Window *>(mFocusPath[mFocusPath.size() - 2]);
            if (window && window->modal()) {
                if (!window->contains(mMousePos))
                    return false;
            }
        }
        return scrollEvent(mMousePos, Vector2f(x, y));
    } catch (const std::exception &e) {
        std::cerr << "Caught exception in event handler: " << e.what()
                  << std::endl;
        abort();
    }

    return false;
}

void ScreenCore::updateFocus(Widget *widget) {
    for (auto w: mFocusPath) {
        if (!w->focused())
            continue;
        w->focusEvent(false);
    }
    mFocusPath.clear();
    Widget *window = nullptr;
    while (widget) {
        mFocusPath.push_back(widget);
        if (dynamic_cast<Window *>(widget))
            window = widget;
        widget = widget->parent();
    }
    for (auto it = mFocusPath.rbegin(); it != mFocusPath.rend(); ++it)
        (*it)->focusEvent(true);

    if (window)
        moveWindowToFront((Window *) window);
}

void ScreenCore::disposeWindow(Window *window) {
    if (std::find(mFocusPath.begin(), mFocusPath.end(), window) != mFocusPath.end())
        mFocusPath.clear();
    if (mDragWidget == window)
        mDragWidget = nullptr;
    removeChild(window);
}

void ScreenCore::centerWindow(Window *window) {
    if (window->size() == Vector2i::Zero()) {
        window->setSize(window->preferredSize(mNVGContext));
        window->performLayout(mNVGContext);
    }
    window->setPosition((mSize - window->size()) / 2);
}

void ScreenCore::moveWindowToFront(Window *window) {
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
            Popup *pw = dynamic_cast<Popup *>(mChildren[index]);
            if (pw && pw->parentWindow() == window && index < baseIndex) {
                moveWindowToFront(pw);
                changed = true;
                break;
            }
        }
    } while (changed);
}

NAMESPACE_END(nanogui)
