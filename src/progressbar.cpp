/*
    src/progressbar.cpp -- Standard widget for visualizing progress

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/progressbar.h>
#include <nanovg.h>
#include <nanogui/serializer/core.h>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(ProgressBar, Widget)

ProgressBar::ProgressBar(Widget *parent)
    : Widget(parent), mValue(0.0f) {}

Vector2i ProgressBar::preferredSize(NVGcontext *) const {
    return Vector2i(70, 12);
}

void ProgressBar::draw(NVGcontext* ctx) {
    Widget::draw(ctx);

    NVGpaint paint = nvgBoxGradient(
        ctx, mPos.x() + 1, mPos.y() + 1,
        mSize.x()-2, mSize.y(), 3, 4, Color(0, 32), Color(0, 92));
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x(), mPos.y(), mSize.x(), mSize.y(), 3);
    nvgFillPaint(ctx, paint);
    nvgFill(ctx);

    float value = std::min(std::max(0.0f, mValue), 1.0f);
    int barPos = (int) std::round((mSize.x() - 2) * value);

    paint = nvgBoxGradient(
        ctx, mPos.x(), mPos.y(),
        barPos+1.5f, mSize.y()-1, 3, 4,
        Color(220, 100), Color(128, 100));

    nvgBeginPath(ctx);
    nvgRoundedRect(
        ctx, mPos.x()+1, mPos.y()+1,
        barPos, mSize.y()-2, 3);
    nvgFillPaint(ctx, paint);
    nvgFill(ctx);
}

void ProgressBar::save(Serializer &s) const {
    Widget::save(s);
    s.set("value", mValue);
}

bool ProgressBar::load(Serializer &s) {
    if (!Widget::load(s))
        return false;
    if (!s.get("value", mValue))
        return false;
    return true;
}

Vector2i CircleProgressBar::preferredSize(NVGcontext *) const {
  return Vector2i(70, 70);
}

void CircleProgressBar::draw(NVGcontext* ctx) {
  Widget::draw(ctx);

  float a0 = 0.0f;
  float a1 = nvgDegToRad(360.f * mValue);
  float r = (std::min(width(), height()) / 2) * mRadius;
  float r0 = r;
  float r1 = r * mLineWidth;
  float ax, ay, bx, by;

  Vector2i center = mPos + (mSize / 2);
  NVGpaint paint;

  nvgSave(ctx);

  Color gradTop = mTheme->mButtonGradientTopUnfocused;
  Color gradBot = mTheme->mButtonGradientBotUnfocused;

  nvgBeginPath(ctx);
  nvgRoundedRect(ctx, mPos.x() + 1, mPos.y() + 1.0f, mSize.x() - 2,
                      mSize.y() - 2, mTheme->mButtonCornerRadius - 1);

  NVGpaint bg = nvgLinearGradient(ctx, mPos.x(), mPos.y(), mPos.x(),
                                       mPos.y() + mSize.y(),
                                       gradTop.mul_a(0.3f), gradBot.mul_a(0.3f));

  nvgFillPaint(ctx, bg);
  nvgFill(ctx);

  nvgBeginPath(ctx);
  nvgArc(ctx, center.x(), center.y(), r0, a0, a1, NVG_CW);
  nvgArc(ctx, center.x(), center.y(), r1, a1, a0, NVG_CCW);
  nvgClosePath(ctx);
  ax = center.x() + cosf(a0) * (r0 + r1)*0.5f;
  ay = center.y() + sinf(a0) * (r0 + r1)*0.5f;
  bx = center.x() + cosf(a1) * (r0 + r1)*0.5f;
  by = center.y() + sinf(a1) * (r0 + r1)*0.5f;

  paint = nvgLinearGradient(ctx, ax, ay, bx, by, mColorStart, mColorEnd);
  nvgFillPaint(ctx, paint);
  nvgFill(ctx);

  nvgRestore(ctx);
}

CircleProgressBar::CircleProgressBar(Widget *parent)
  : ProgressBar(parent) {}

NAMESPACE_END(nanogui)
