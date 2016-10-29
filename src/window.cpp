/*
    src/window.cpp -- Top-level window widget

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/window.h>
#include <nanogui/theme.h>
#include <nanogui/opengl.h>
#include <nanogui/screen.h>
#include <nanogui/layout.h>
#include <nanogui/serializer/core.h>

NAMESPACE_BEGIN(nanogui)

Window::Window(Widget *parent, const std::string &title, bool resizable)
    : Widget(parent), mTitle(title), mButtonPanel(nullptr), mModal(false), mDrag(false),
    mResizeDir(Vector2i::Zero()), mMinSize(Vector2i::Zero()), mResizable(resizable)  { }

Vector2i Window::preferredSize(NVGcontext *ctx) const {
    if (mButtonPanel)
        mButtonPanel->setVisible(false);
    Vector2i result = Widget::preferredSize(ctx);
    if (mButtonPanel)
        mButtonPanel->setVisible(true);

    nvgFontSize(ctx, 18.0f);
    nvgFontFace(ctx, "sans-bold");
    float bounds[4];
    nvgTextBounds(ctx, 0, 0, mTitle.c_str(), nullptr, bounds);

    return result.cwiseMax(Vector2i(
        bounds[2]-bounds[0] + 20, bounds[3]-bounds[1]
    ));
}

Widget *Window::buttonPanel() {
    if (!mButtonPanel) {
        mButtonPanel = new Widget(this);
        mButtonPanel->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 4));
    }
    return mButtonPanel;
}

void Window::performLayout(NVGcontext *ctx) {
    if (!mButtonPanel) {
        Widget::performLayout(ctx);
    } else {
        mButtonPanel->setVisible(false);
        Widget::performLayout(ctx);
        for (auto w : mButtonPanel->children()) {
            w->setFixedSize(Vector2i(22, 22));
            w->setFontSize(15);
        }
        mButtonPanel->setVisible(true);
        mButtonPanel->setSize(Vector2i(width(), 22));
        mButtonPanel->setPosition(Vector2i(width() - (mButtonPanel->preferredSize(ctx).x() + 5), 3));
        mButtonPanel->performLayout(ctx);
    }
    if (mMinSize == Vector2i::Zero())
        mMinSize = mSize;
}

void Window::draw(NVGcontext *ctx) {
    int ds = mTheme->mWindowDropShadowSize, cr = mTheme->mWindowCornerRadius;
    int hh = mTheme->mWindowHeaderHeight;

    if (mResize) performLayout(ctx);

    /* Draw window */
    nvgSave(ctx);
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x(), mPos.y(), mSize.x(), mSize.y(), cr);

    nvgFillColor(ctx, mMouseFocus ? mTheme->mWindowFillFocused
                                  : mTheme->mWindowFillUnfocused);
    nvgFill(ctx);

    /* Draw a drop shadow */
    NVGpaint shadowPaint = nvgBoxGradient(
        ctx, mPos.x(), mPos.y(), mSize.x(), mSize.y(), cr*2, ds*2,
        mTheme->mDropShadow, mTheme->mTransparent);

    nvgBeginPath(ctx);
    nvgRect(ctx, mPos.x()-ds,mPos.y()-ds, mSize.x()+2*ds, mSize.y()+2*ds);
    nvgRoundedRect(ctx, mPos.x(), mPos.y(), mSize.x(), mSize.y(), cr);
    nvgPathWinding(ctx, NVG_HOLE);
    nvgFillPaint(ctx, shadowPaint);
    nvgFill(ctx);

    if (!mTitle.empty()) {
        /* Draw header */
        NVGpaint headerPaint = nvgLinearGradient(
            ctx, mPos.x(), mPos.y(), mPos.x(),
            mPos.y() + hh,
            mTheme->mWindowHeaderGradientTop,
            mTheme->mWindowHeaderGradientBot);

        nvgBeginPath(ctx);
        nvgRoundedRect(ctx, mPos.x(), mPos.y(), mSize.x(), hh, cr);

        nvgFillPaint(ctx, headerPaint);
        nvgFill(ctx);

        nvgBeginPath(ctx);
        nvgRoundedRect(ctx, mPos.x(), mPos.y(), mSize.x(), hh, cr);
        nvgStrokeColor(ctx, mTheme->mWindowHeaderSepTop);

        nvgSave(ctx);
        nvgIntersectScissor(ctx, mPos.x(), mPos.y(), mSize.x(), 0.5f);
        nvgStroke(ctx);
        nvgResetScissor(ctx);
        nvgRestore(ctx);

        nvgBeginPath(ctx);
        nvgMoveTo(ctx, mPos.x() + 0.5f, mPos.y() + hh - 1.5f);
        nvgLineTo(ctx, mPos.x() + mSize.x() - 0.5f, mPos.y() + hh - 1.5);
        nvgStrokeColor(ctx, mTheme->mWindowHeaderSepBot);
        nvgStroke(ctx);

        nvgFontSize(ctx, 18.0f);
        nvgFontFace(ctx, "sans-bold");
        nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);

        nvgFontBlur(ctx, 2);
        nvgFillColor(ctx, mTheme->mDropShadow);
        nvgText(ctx, mPos.x() + mSize.x() / 2,
                mPos.y() + hh / 2, mTitle.c_str(), nullptr);

        nvgFontBlur(ctx, 0);
        nvgFillColor(ctx, mFocused ? mTheme->mWindowTitleFocused
                                   : mTheme->mWindowTitleUnfocused);
        nvgText(ctx, mPos.x() + mSize.x() / 2, mPos.y() + hh / 2 - 1,
                mTitle.c_str(), nullptr);
    }

    nvgRestore(ctx);
    Widget::draw(ctx);
}

void Window::dispose() {
    Widget *widget = this;
    while (widget->parent())
        widget = widget->parent();
    ((Screen *) widget)->disposeWindow(this);
}

void Window::center() {
    Widget *widget = this;
    while (widget->parent())
        widget = widget->parent();
    ((Screen *) widget)->centerWindow(this);
}

bool Window::mouseDragEvent(const Vector2i &p, const Vector2i &rel,
                            int button, int /* modifiers */) {

    if (mDrag && (button & (1 << GLFW_MOUSE_BUTTON_1)) != 0) {
        mPos += rel;
        mPos = mPos.cwiseMax(Vector2i::Zero());
        mPos = mPos.cwiseMin(parent()->size() - mSize);
        return true;
    } else if (mResizable && mResize && (button & (1 << GLFW_MOUSE_BUTTON_1)) != 0) {
        Vector2i &&lowerLeftCorner = mPos + mSize;
        Vector2i &lowerRightCorner = mPos;

        if (mResizeDir.x() == 1) {
            if ((rel.x() > 0 && p.x() >= lowerLeftCorner.x()) || (rel.x() < 0)) {
                mSize.x() += rel.x();
            }
        } else if (mResizeDir.x() == -1) {
            if ((rel.x() < 0 && p.x() <= lowerRightCorner.x()) ||
                    (rel.x() > 0)) {
                mSize.x() += -rel.x();
                mSize = mSize.cwiseMax(mMinSize);
                mPos = lowerLeftCorner - mSize;
            }
        }

        if (mResizeDir.y() == 1) {
            if ((rel.y() > 0 && p.y() >= lowerLeftCorner.y()) || (rel.y() < 0)) {
                mSize.y() += rel.y();
            }
        }
        mSize = mSize.cwiseMax(mMinSize);
        return true;
    }
    return false;
}

bool Window::mouseMotionEvent(const Eigen::Vector2i &p, const Eigen::Vector2i &rel, int button, int modifiers) {
    if (mResizable && mFixedSize.x() == 0 && checkHorizontalResize(p) != 0) {
        mCursor = Cursor::HResize;
    } else if (mResizable && mFixedSize.y() == 0 && checkVerticalResize(p) != 0) {
        mCursor = Cursor::VResize;
    } else {
        mCursor = Cursor::Arrow;
    }
    return false;
}

bool Window::mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) {
    if (Widget::mouseButtonEvent(p, button, down, modifiers))
        return true;

    if (button == GLFW_MOUSE_BUTTON_1) {
        mDrag = down && (p.y() - mPos.y()) < mTheme->mWindowHeaderHeight;
        mResize = false;
        if (mResizable && !mDrag && down) {
            mResizeDir.x() = (mFixedSize.x() == 0) ? checkHorizontalResize(p) : 0;
            mResizeDir.y() = (mFixedSize.y() == 0) ? checkVerticalResize(p) : 0;
            mResize = mResizeDir.x() != 0 || mResizeDir.y() != 0;
        }
        return true;
    }
    return false;
}

bool Window::scrollEvent(const Vector2i &p, const Vector2f &rel) {
    Widget::scrollEvent(p, rel);
    return true;
}

void Window::refreshRelativePlacement() {
    /* Overridden in \ref Popup */
}

void Window::save(Serializer &s) const {
    Widget::save(s);
    s.set("title", mTitle);
    s.set("modal", mModal);
}

bool Window::load(Serializer &s) {
    if (!Widget::load(s)) return false;
    if (!s.get("title", mTitle)) return false;
    if (!s.get("modal", mModal)) return false;
    mDrag = false;
    return true;
}

int Window::checkHorizontalResize(const Eigen::Vector2i &mousePos) {
    int offset = mTheme->mResizeAreaOffset;
    Vector2i lowerRightCorner = absolutePosition() + size();
    int headerLowerLeftCornerY = absolutePosition().y() + mTheme->mWindowHeaderHeight;

    if (mousePos.y() > headerLowerLeftCornerY && mousePos.x() <= absolutePosition().x() + offset &&
            mousePos.x() >= absolutePosition().x()) {
        return -1;
    } else if (mousePos.y() > headerLowerLeftCornerY && mousePos.x() >= lowerRightCorner.x() - offset &&
               mousePos.x() <= lowerRightCorner.x()) {
        return 1;
    }

    return 0;
}

int Window::checkVerticalResize(const Eigen::Vector2i &mousePos) {
    int offset = mTheme->mResizeAreaOffset;
    Vector2i lowerRightCorner = absolutePosition() + size();

    // Do not check for resize area on top of the window. It is to prevent conflict drag and resize event.
    if (mousePos.y() >= lowerRightCorner.y() - offset && mousePos.y() <= lowerRightCorner.y()) {
        return 1;
    }

    return 0;
}

NAMESPACE_END(nanogui)
