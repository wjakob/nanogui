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
#include <nanogui/serializer/core.h>

NAMESPACE_BEGIN(nanogui)

Popup::Popup(Widget *parent, Window *parentWindow)
    : Window(parent, ""), mParentWindow(parentWindow),
      mAnchorPos(Vector2i::Zero()), mAnchorHeight(30), mPopupSide(PopupSide::RIGHTSIDE) {
}

void Popup::performLayout(NVGcontext *ctx) {
    if (mLayout || mChildren.size() != 1) {
        Widget::performLayout(ctx);
    } else {
        mChildren[0]->setPosition(Vector2i::Zero());
        mChildren[0]->setSize(mSize);
        mChildren[0]->performLayout(ctx);
    }
    if(mPopupSide == PopupSide::LEFTSIDE){
        mAnchorPos[0] -= size()[0];
    }
}

void Popup::refreshRelativePlacement() {
    mParentWindow->refreshRelativePlacement();
    mVisible &= mParentWindow->visibleRecursive();
    mPos = mParentWindow->position() + mAnchorPos - Vector2i(0, mAnchorHeight);
}

void Popup::draw(NVGcontext* ctx) {
    refreshRelativePlacement();

    if (!mVisible)
        return;

    int ds = mTheme->mWindowDropShadowSize, cr = mTheme->mWindowCornerRadius;

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

    if(mPopupSide == PopupSide::RIGHTSIDE){
        nvgMoveTo(ctx, mPos.x()-15,mPos.y()+mAnchorHeight);
        nvgLineTo(ctx, mPos.x()+1,mPos.y()+mAnchorHeight-15);
        nvgLineTo(ctx, mPos.x()+1,mPos.y()+mAnchorHeight+15);
    }else{
        nvgMoveTo(ctx, mPos.x()+mSize.x()+15,mPos.y()+mAnchorHeight);
        nvgLineTo(ctx, mPos.x()+mSize.x()-1,mPos.y()+mAnchorHeight-15);
        nvgLineTo(ctx, mPos.x()+mSize.x()-1,mPos.y()+mAnchorHeight+15);
    }

    nvgFillColor(ctx, mTheme->mWindowPopup);
    nvgFill(ctx);

    Widget::draw(ctx);
}

void Popup::save(Serializer &s) const {
    Window::save(s);
    s.set("anchorPos", mAnchorPos);
    s.set("anchorHeight", mAnchorHeight);
    s.set("popupSide", (int)mPopupSide);
}

bool Popup::load(Serializer &s) {
    if (!Window::load(s)) return false;
    if (!s.get("anchorPos", mAnchorPos)) return false;
    if (!s.get("anchorHeight", mAnchorHeight)) return false;
    int dummy;
    if (!s.get("popupSide", dummy)) return false;
    mPopupSide = (PopupSide)dummy;
    return true;
}

NAMESPACE_END(nanogui)
