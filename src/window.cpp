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

Window::Window(Widget *parent, const std::string &title)
    : Widget(parent), mTitle(title), mButtonPanel(nullptr), mModal(false), mDrag(false),
    mMinSize(Vector2i::Zero()), mResizeDir(Vector2i::Zero()) { }

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
    if (mMinSize == Vector2i::Zero()) {
        mMinSize = mSize;
    }
}

void Window::draw(NVGcontext *ctx) {
    int ds = mTheme->mWindowDropShadowSize, cr = mTheme->mWindowCornerRadius;
    int hh = mTheme->mWindowHeaderHeight;

    if(mResize) performLayout(ctx);
    updateResizeRectPositions();
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

bool Window::mouseDragEvent(const Vector2i &, const Vector2i &rel,
                            int button, int /* modifiers */) {

    if (mDrag && (button & (1 << GLFW_MOUSE_BUTTON_1)) != 0) {
        mPos += rel;
        mPos = mPos.cwiseMax(Vector2i::Zero());
        mPos = mPos.cwiseMin(parent()->size() - mSize);
        return true;
    } else if (mResize && (button & (1 << GLFW_MOUSE_BUTTON_1)) != 0) {
        if (mResizeDir.x() > 0) {
            mSize.x() += rel[0];
        } else if (mResizeDir.x() < 0) {
            mSize[0] += -rel[0];
            if (mSize[0] > mMinSize[0]) mPos[0] += rel[0];
        }

        if (mResizeDir.y() > 0) {
            mSize[1] += rel[1];
        } else if (mResizeDir.y() < 0) {
            mSize[1] += -rel[1];
            if (mSize[1] > mMinSize[1]) mPos[1] += rel[1];
        }
        mSize = mSize.cwiseMax(mMinSize);
        return true;
    }
    return false;
}

bool Window::mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) {
    if (Widget::mouseButtonEvent(p, button, down, modifiers))
        return true;
    if (button == GLFW_MOUSE_BUTTON_1) {
        mDrag = down && (p.y() - mPos.y()) < mTheme->mWindowHeaderHeight;
        mResize = false;
        if (!mDrag && down) {
            for (int i = 0; i < mResizeRects.size(); i++) {
                if (mResizeRects[i].collisionPoint(p)) {
                    mResize = true;
                    switch (i) {
                        case BottomRight:
                            mResizeDir.x() = 1;
                            mResizeDir.y() = 1;
                            break;
                        case BottomLeft:
                            mResizeDir.x() = -1;
                            mResizeDir.y() = 1;
                            break;
                        case Bottom:
                            mResizeDir.x() = 0;
                            mResizeDir.y() = 1;
                            break;
                        case Left:
                            mResizeDir.x() = -1;
                            mResizeDir.y() = 0;
                            break;
                        case Right:
                            mResizeDir.x() = 1;
                            mResizeDir.y() = 0;
                            break;
                    }
                    break;
                }
            }
            if (mFixedSize.x() != 0) {
                mResizeDir.x() = 0;
            }

            if (mFixedSize.y() != 0) {
                mResizeDir.y() = 0;
            }
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

void Window::updateResizeRectPositions() {
    mResizeRects[BottomRight].x() = absolutePosition().x() + (width() - mResizeRects[BottomRight].w());
    mResizeRects[BottomRight].y() = absolutePosition().y() + (height() - mResizeRects[BottomRight].h());
    mResizeRects[BottomRight].w() = mTheme->mResizeRectangleCornerSize.x();
    mResizeRects[BottomRight].h() = mTheme->mResizeRectangleCornerSize.y();

    mResizeRects[BottomLeft].x() = absolutePosition().x();
    mResizeRects[BottomLeft].y() = absolutePosition().y() + (height() - mResizeRects[BottomLeft].h());
    mResizeRects[BottomLeft].w() = mTheme->mResizeRectangleCornerSize.x();
    mResizeRects[BottomLeft].h() = mTheme->mResizeRectangleCornerSize.y();

    mResizeRects[Bottom].w() = width() - (mResizeRects[BottomLeft].w() + mResizeRects[BottomRight].w());
    mResizeRects[Bottom].h() = mTheme->mResizeRectangleBottomSideHeight;
    mResizeRects[Bottom].x() = absolutePosition().x() + mResizeRects[BottomLeft].w();
    mResizeRects[Bottom].y() = absolutePosition().y() + (height() - mResizeRects[Bottom].h());

    mResizeRects[Right].w() = mTheme ->mResizeRectangleLeftRightSidesWidth;
    mResizeRects[Right].h() = height() - (mTheme->mWindowHeaderHeight + mResizeRects[BottomRight].h());
    mResizeRects[Right].x() = absolutePosition().x() + (width() - mResizeRects[Right].w());
    mResizeRects[Right].y() = absolutePosition().y() + mTheme->mWindowHeaderHeight;

    mResizeRects[Left].w() =  mTheme ->mResizeRectangleLeftRightSidesWidth;
    mResizeRects[Left].h() =  height() - (mTheme->mWindowHeaderHeight + mResizeRects[BottomLeft].h());
    mResizeRects[Left].x() = absolutePosition().x();
    mResizeRects[Left].y() = absolutePosition().y() + mTheme->mWindowHeaderHeight;
}

NAMESPACE_END(nanogui)
