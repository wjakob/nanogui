/*
    nanogui/slider.cpp -- Fractional slider widget with mouse control

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/slider.h>
#include <nanogui/theme.h>
#include <nanovg.h>
#include <nanogui/serializer/core.h>
#include <nanogui/serializer/json.h>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(Slider, Widget)

Slider::Slider(Widget *parent)
    : Widget(parent), mRange(0.f, 1.f),
      mHighlightedRange(0.f, 0.f) 
{
  mValue = 0.f;
  mHighlightColor = Color(255, 80, 80, 70);
}

Vector2i Slider::preferredSize(NVGcontext *) const {
    return Vector2i(70, 16);
}

bool Slider::mouseDragEvent(const Vector2i &p, const Vector2i & /* rel */,
                            int /* button */, int /* modifiers */) {
    if (!mEnabled)
        return false;

    const float kr = (int) (mSize.y() * 0.4f), kshadow = 3;
    const float startX = kr + kshadow + mPos.x() - 1;
    const float widthX = mSize.x() - 2 * (kr + kshadow);

    float value = (p.x() - startX) / widthX;
    value = value * (mRange.second - mRange.first) + mRange.first;
    mValue = std::min(std::max(value, mRange.first), mRange.second);
    if (mCallback)
        mCallback(mValue);
    return true;
}

bool Slider::mouseButtonEvent(const Vector2i &p, int /* button */, bool down, int /* modifiers */) {
    if (!mEnabled)
        return false;

    const float kr = (int) (mSize.y() * 0.4f), kshadow = 3;
    const float startX = kr + kshadow + mPos.x() - 1;
    const float widthX = mSize.x() - 2 * (kr + kshadow);

    float value = (p.x() - startX) / widthX;
    value = value * (mRange.second - mRange.first) + mRange.first;
    mValue = std::min(std::max(value, mRange.first), mRange.second);
    if (mCallback)
        mCallback(mValue);
    if (mFinalCallback && !down)
        mFinalCallback(mValue);
    return true;
}

void Slider::draw(NVGcontext* ctx) {
    Vector2f center = mPos.cast<float>() + mSize.cast<float>() * 0.5f;
    float kr = (int) (mSize.y() * 0.4f), kshadow = 3;

    float startX = kr + kshadow + mPos.x();
    float widthX = mSize.x() - 2*(kr+kshadow);

    Vector2f knobPos(startX + (mValue - mRange.first) /
            (mRange.second - mRange.first) * widthX,
            center.y() + 0.5f);

    NVGpaint bg = nvgBoxGradient(
        ctx, startX, center.y() - 3 + 1, widthX, 6, 3, 3,
        Color(0, mEnabled ? 32 : 10), Color(0, mEnabled ? 128 : 210));

    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, startX, center.y() - 3 + 1, widthX, 6, 2);
    nvgFillPaint(ctx, bg);
    nvgFill(ctx);

    if (mHighlightedRange.second != mHighlightedRange.first) {
        nvgBeginPath(ctx);
        nvgRoundedRect(ctx, startX + mHighlightedRange.first * mSize.x(),
                       center.y() - kshadow + 1,
                       widthX *
                           (mHighlightedRange.second - mHighlightedRange.first),
                       kshadow * 2, 2);
        nvgFillColor(ctx, mHighlightColor);
        nvgFill(ctx);
    }

    if (mShowValueWithColor)
    {
      nvgBeginPath(ctx);
      nvgRoundedRect(ctx, startX, center.y() - kshadow + 1,
                          knobPos.x() - startX, kshadow * 2, 2);
      nvgFillColor(ctx, mValueColor.notW(mTheme->mSliderValueColor));
      nvgFill(ctx);
    }

    NVGpaint knobShadow =
        nvgRadialGradient(ctx, knobPos.x(), knobPos.y(), kr - kshadow,
                          kr + kshadow, Color(0, 64), mTheme->mTransparent);

    nvgBeginPath(ctx);
    nvgRect(ctx, knobPos.x() - kr - 5, knobPos.y() - kr - 5, kr * 2 + 10,
            kr * 2 + 10 + kshadow);
    nvgCircle(ctx, knobPos.x(), knobPos.y(), kr);
    nvgPathWinding(ctx, NVG_HOLE);
    nvgFillPaint(ctx, knobShadow);
    nvgFill(ctx);

    NVGpaint knob = nvgLinearGradient(ctx,
        mPos.x(), center.y() - kr, mPos.x(), center.y() + kr,
        mTheme->mBorderLight, mTheme->mBorderMedium);
    NVGpaint knobReverse = nvgLinearGradient(ctx,
        mPos.x(), center.y() - kr, mPos.x(), center.y() + kr,
        mTheme->mBorderMedium,
        mTheme->mBorderLight);

    nvgBeginPath(ctx);
    nvgCircle(ctx, knobPos.x(), knobPos.y(), kr);
    nvgStrokeColor(ctx, mTheme->mBorderDark);
    nvgFillPaint(ctx, knob);
    nvgStroke(ctx);
    nvgFill(ctx);
    nvgBeginPath(ctx);
    nvgCircle(ctx, knobPos.x(), knobPos.y(), kr/2);
    nvgFillColor(ctx, Color(150, mEnabled ? 255 : 100));
    nvgStrokePaint(ctx, knobReverse);
    nvgStroke(ctx);
    nvgFill(ctx);
}

void Slider::save(Json::value &save) const {
  Widget::save(save);
  Json::object obj = save.get_obj();
  obj["value"] = Json::hobject().$("value", mValue).$("type", "float").$("name", "Value");
  obj["range"] = Json::hobject().$("min", mRange.first).$("max", mRange.second).$("type", "range").$("name", "Range");
  obj["highlightedRange"] = Json::hobject().$("min", mHighlightedRange.first).$("max", mHighlightedRange.second).$("type", "range").$("name", "Highligh range color");
  obj["highlightColor"] = Json::hobject().$("color", mHighlightColor.toInt()).$("type", "color").$("name", "Highlight color");
}

bool Slider::load(Json::value &save) {
  Widget::load(save);
  mValue = save.get_float("value"); 
  auto r = save.get("range"); mRange = { r.get_float("min"), r.get_float("max") };
  auto hr = save.get("highlightedRange"); mHighlightedRange = { hr.get_float("min"), hr.get_float("max") };
  auto bg = save.get("highlightColor"); mHighlightColor = Color(bg.get_int("color"));

  return true;
}

NAMESPACE_END(nanogui)
