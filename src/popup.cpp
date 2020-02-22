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
#include <nanovg.h>
#include <nanogui/saveload.h>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(Popup, Window)

Popup::Popup(Widget *parent, Window *parentWindow)
    : Window(parent, ""), mParentWindow(parentWindow),
      mAnchorPos(Vector2i::Zero()), mAnchorHeight(30), mSide(Side::Right) {
}

void Popup::performLayout(NVGcontext *ctx) {
    if (mLayout || mChildren.size() != 1) {
        Widget::performLayout(ctx);
    } else {
        mChildren[0]->setPosition(Vector2i::Zero());
        mChildren[0]->setSize(mSize);
        mChildren[0]->performLayout(ctx);
    }
    if (mSide == Side::Left)
        mAnchorPos[0] -= size()[0];
}

void Popup::refreshRelativePlacement() {
    mParentWindow->refreshRelativePlacement();
    mVisible &= mParentWindow->visibleRecursive();
    mPos = mParentWindow->position() + mAnchorPos - Vector2i(0, mAnchorHeight);
}

int Popup::getHeaderHeight() const 
{
  if (auto layt = GroupLayout::cast(layout()))
    return layt->margin();
  return 0;
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
    nvgRect(ctx, mPos - Vector2i(ds), mSize + Vector2i(2 * ds));
    nvgRoundedRect(ctx, mPos.x(), mPos.y(), mSize.x(), mSize.y(), cr);
    nvgPathWinding(ctx, NVG_HOLE);
    nvgFillPaint(ctx, shadowPaint);
    nvgFill(ctx);

    /* Draw window */
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x(), mPos.y(), mSize.x(), mSize.y(), cr);

    Vector2i base = mPos + Vector2i(0, mAnchorHeight);
    int sign = -1;
    if (mSide == Side::Left) {
        base.x() += mSize.x();
        sign = 1;
    }

    nvgMoveTo(ctx, base.x() + 15*sign, base.y());
    nvgLineTo(ctx, base.x() - 1*sign, base.y() - 15);
    nvgLineTo(ctx, base.x() - 1*sign, base.y() + 15);

    nvgFillColor(ctx, mTheme->mWindowPopup);
    nvgFill(ctx);
    nvgRestore(ctx);

    Widget::draw(ctx);
}

void Popup::save(Json::value &s) const 
{
    Window::save(s);
    auto obj = s.get_obj();

    obj["anchorPos"] = json().set(mAnchorPos).name("Anchor pos");
    obj["anchorHeight"] = json().set(mAnchorHeight).name("Anchor height");
    obj["side"] = json().set<int>(mSide).name("Side");

    s = Json::value(obj);
}

bool Popup::load(Json::value &save) 
{
    Window::load(save);
    json s{ save.get_obj() };

    mAnchorPos = s.get<Vector2i>("anchorPos");
    mAnchorHeight = s.get<int>("anchorHeight");
    mSide = (Side)s.get<int>("side");
    return true;
}

NAMESPACE_END(nanogui)
