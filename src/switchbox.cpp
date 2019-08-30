/*
    src/checkbox.cpp -- Two-state check box widget

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/switchbox.h>
#include <nanogui/opengl.h>
#include <nanogui/theme.h>
#include <nanogui/serializer/core.h>

NAMESPACE_BEGIN(nanogui)

SwitchBox::SwitchBox(Widget *parent, Alignment align, const std::string &caption,
                   const std::function<void(bool) > &callback)
    : CheckBox(parent, caption, callback), mAlign(align) {

    mIconExtraScale = 1.2f;// widget override
}

Vector2i SwitchBox::preferredSize(NVGcontext *ctx) const {
    if (mFixedSize != Vector2i::Zero())
        return mFixedSize;

    if (mAlign == Alignment::Horizontal)
    {
      nvgFontSize(ctx, fontSize());
      nvgFontFace(ctx, "sans");
      return Vector2i(
        nvgTextBounds(ctx, 0, 0, mCaption.c_str(), nullptr, nullptr) +
        1.8f * fontSize(),
        fontSize() * 1.8f);
    }
    else
    {
      nvgFontSize(ctx, fontSize());
      nvgFontFace(ctx, "sans");
      return Vector2i(
        nvgTextBounds(ctx, 0, 0, mCaption.c_str(), nullptr, nullptr) +
        1.8f * fontSize(),
        fontSize() * 3.6f);
    }
}

void SwitchBox::draw(NVGcontext *ctx) 
{
  if (mChecked)
  {
    if (path < 1.0f)
      path += 0.1f;
  }
  else
  {
    if (path > 0) path -= 0.1f;
    if (path < 0) path = 0;
  }

  Vector2f center = mPos.cast<float>() + mSize.cast<float>() * 0.5f;
  Vector2f knobPos;
  float kr, startX, startY, widthX, heightY;
  if (mAlign == Alignment::Horizontal)
  {
    kr = (int)(mSize.y() * 0.4f);
    startX = mPos.x() + mSize.y() * 0.1f;
    heightY = mSize.y() * 0.8;

    startY = (mPos.y() + (mSize.y() - heightY) / 2) + 1;
    widthX = (mSize.y() * 1.5);

    knobPos = Vector2f(startX + kr + path * (widthX - 2 * kr), center.y() + 0.5f);
  }
  else
  {
    kr = (int)(mSize.y() * 0.2f);
    startX = mPos.x() + mSize.y() * 0.05f + 1;
    heightY = mSize.y() * 0.8;

    startY = (mPos.y() + (mSize.y() - heightY) / 2);
    widthX = (mSize.y() * 0.4f);

    knobPos = Vector2f(startX + kr, startY + path * (heightY - 2 * kr) + kr);
  }

  NVGpaint bg = nvgBoxGradient(ctx, startX, startY, widthX, heightY, 3, 3,
    Color(0, mEnabled ? 32 : 10),
    Color(0, mEnabled ? 128 : 210));

  nvgBeginPath(ctx);
  nvgRoundedRect(ctx, startX, startY, widthX, heightY, kr);
  nvgFillPaint(ctx, bg);

  nvgBeginPath(ctx);
  nvgStrokeWidth(ctx, 1.0f);
  nvgRoundedRect(ctx, startX + 0.5f, startY + 0.5f, widthX - 1, heightY - 1, kr);
  nvgStrokeColor(ctx, mTheme->mBorderLight);
  nvgStroke(ctx);
  nvgFill(ctx);

  nvgBeginPath(ctx);
  nvgRoundedRect(ctx, startX + 0.5f, startY + 0.5f, widthX - 1, heightY - 2, kr);
  nvgStrokeColor(ctx, mTheme->mBorderDark);
  nvgStroke(ctx);

  NVGpaint knob = nvgLinearGradient(ctx, mPos.x(), center.y() - kr, mPos.x(), center.y() + kr,
    mTheme->mBorderLight, mTheme->mBorderMedium);
  NVGpaint knobReverse = nvgLinearGradient(ctx, mPos.x(), center.y() - kr, mPos.x(), center.y() + kr,
    mTheme->mBorderMedium, mTheme->mBorderLight);

  nvgBeginPath(ctx);
  nvgCircle(ctx, knobPos.x(), knobPos.y(), kr * 0.9);
  nvgStrokeColor(ctx, Color(0, 200));
  nvgFillPaint(ctx, knob);
  nvgStroke(ctx);
  nvgFill(ctx);
  nvgBeginPath(ctx);
  nvgCircle(ctx, knobPos.x(), knobPos.y(), kr * 0.7);
  nvgFillColor(ctx, Color(120, mEnabled ? 255 : 100));
  nvgStrokePaint(ctx, knobReverse);
  nvgStroke(ctx);
  nvgFill(ctx);

#if NANOGUI_SHOW_WIDGET_BOUNDS
  nvgStrokeWidth(ctx, 1.0f);
  nvgBeginPath(ctx);
  nvgRect(ctx, mPos.x(), mPos.y(), mSize.x(), mSize.y());
  nvgStrokeColor(ctx, nvgRGBA(255, 0, 0, 255));
  nvgStroke(ctx);
#endif

  nvgFontSize(ctx, fontSize());
  nvgFontFace(ctx, "sans");
  nvgFillColor(ctx, mEnabled ? mTheme->mTextColor : mTheme->mDisabledTextColor);
  nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
  nvgText(ctx, mPos.x() + 1.6f * fontSize(), mPos.y() + mSize.y() * 0.5f, mCaption.c_str(), nullptr);

  Widget::draw(ctx);
}

NAMESPACE_END(nanogui)
