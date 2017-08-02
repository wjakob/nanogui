/*
    src/popup.cpp -- Simple popup widget which is attached to another given
    window (can be nested)

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/popup.h>
#include <nanogui/theme.h>
#include <nanogui/opengl.h>
#include <nanogui/screen.h>
#include <nanogui/serializer/core.h>

NAMESPACE_BEGIN(nanogui)

static const int kArrowSize = 15;

Popup::Popup(Widget *parent, Window *parentWindow, bool showArrow)
    : Window(parent, ""), mParentWindow(parentWindow),
      mAnchorPos(Vector2i::Zero()), mAnchorOffset(0), mSide(Side::Right), mShowArrow(showArrow) {
}

int Popup::actualAnchorOffset() const {
    if (mSide == Side::Left || mSide == Side::Right) {
        return (mAnchorOffset > 0 ? mAnchorOffset : 30); // top (with optional offset)
    }
    else {
        return mSize.x() / 2 + mAnchorOffset; // middle (with optional offset).
    }
}

void Popup::performLayout(NVGcontext *ctx) {
    if (mParentWindow)
        refreshRelativePlacement();
    if (mLayout || mChildren.size() != 1) {
        Widget::performLayout(ctx);
    } else {
        mChildren[0]->setPosition(Vector2i::Zero());
        mChildren[0]->setSize(mSize);
        mChildren[0]->performLayout(ctx);
    }
    if (mSide == Side::Left)
        mAnchorPos[0] -= size()[0];
    else if (mSide == Side::Top)
        mAnchorPos[1] -= size()[1];
}

void Popup::refreshRelativePlacement() {
    mParentWindow->refreshRelativePlacement();
    mVisible &= mParentWindow->visibleRecursive();

    Vector2i arrowOffset = Vector2i(0, 0);
    if ( mShowArrow ) {
        switch(mSide) {
            case Side::Left:
                arrowOffset.x() -= kArrowSize;
                break;
            case Side::Right:
                arrowOffset.x() += kArrowSize;
                break;
            case Side::Top:
                arrowOffset.y() -= kArrowSize;
                break;
            case Side::Bottom:
                arrowOffset.y() += kArrowSize;
                break;
        }
    }

    mPos = mParentWindow->position() + mAnchorPos + arrowOffset;

    if (mSide == Side::Left || mSide == Side::Right)
        mPos -= Vector2i(0, actualAnchorOffset());
    else
        mPos -= Vector2i(actualAnchorOffset(), 0);
}

void Popup::draw(NVGcontext* ctx) {
    refreshRelativePlacement();

    if (!mVisible)
        return;

    int ds = mTheme->mWindowDropShadowSize, cr = mTheme->mWindowCornerRadius;

    nvgSave(ctx);
    nvgResetScissor(ctx);

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

    /* Draw window */
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x(), mPos.y(), mSize.x(), mSize.y(), cr);
    nvgFillColor(ctx, mTheme->mWindowPopup);
    nvgFill(ctx);

    if(mShowArrow) {
        nvgBeginPath(ctx);

        Vector2i base = mPos;
        if (mSide == Side::Left || mSide == Side::Right) {
            base += Vector2i(0, actualAnchorOffset());

            int sign = -1;
            if (mSide == Side::Left) {
                base.x() += mSize.x();
                sign = 1;
            }

            nvgMoveTo(ctx, base.x() + kArrowSize*sign, base.y());
            nvgLineTo(ctx, base.x() - 1*sign, base.y() - kArrowSize);
            nvgLineTo(ctx, base.x() - 1*sign, base.y() + kArrowSize);
        }
        else {
            base += Vector2i(actualAnchorOffset(), 0);

            int sign = -1;
            if (mSide == Side::Top) {
                base.y() += mSize.y();
                sign = 1;
            }

            nvgMoveTo(ctx, base.x(), base.y() + kArrowSize*sign);
            nvgLineTo(ctx, base.x() + kArrowSize, base.y() - 1*sign);
            nvgLineTo(ctx, base.x() - kArrowSize, base.y() - 1*sign);
        }

        nvgFill(ctx);
    }

    nvgRestore(ctx);

    Widget::draw(ctx);
}

void Popup::save(Serializer &s) const {
    Window::save(s);
    s.set("anchorPos", mAnchorPos);
    s.set("anchorOffset", mAnchorOffset);
    s.set("side", mSide);
}

bool Popup::load(Serializer &s) {
    if (!Window::load(s)) return false;
    if (!s.get("anchorPos", mAnchorPos)) return false;
    if (!s.get("side", mSide)) return false;
    // "new" fields
    const std::vector<std::string> keys = s.keys();
    if (std::find(keys.begin(), keys.end(), "anchorOffset") != keys.end()) {
        if (!s.get("anchorOffset", mAnchorOffset)) return false;
    }
    return true;
}

NAMESPACE_END(nanogui)
