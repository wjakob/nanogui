/*
    src/label.cpp -- Text label with an arbitrary font, color, and size

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/label.h>
#include <nanogui/theme.h>
#include <nanovg.h>
#include <nanogui/serializer/core.h>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(Label, Widget)

Label::Label(Widget *parent)
    : Widget(parent), mCaption(""), mFont("sans") {
}

void Label::setTheme(Theme *theme) {
    Widget::setTheme(theme);
}

int Label::fontSize() const { return mFontSize > 0 ? mFontSize : mTheme->mStandardFontSize;  }

Vector2i Label::preferredSize(NVGcontext *ctx) const {
  if (mCaption == "")
  {
    if (mFixedSize.x() > 0 || mFixedSize.y() > 0)
      return mFixedSize;

    return Vector2i::Zero();
  }

  nvgFontFaceSize(ctx, mFont.c_str(), fontSize());
  if (mFixedSize.x() > 0 || mFixedSize.y() > 0) 
  {
      float bounds[4];
      nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
      nvgTextBounds(ctx, 0, 0, mCaption.c_str(), nullptr, bounds);
      const_cast<Label*>(this)->mTextRealSize = Vector2i(bounds[2] - bounds[0], bounds[3] - bounds[1] );
      return mFixedSize.fillZero(mTextRealSize);
  } 
  else 
  {
      nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
      int tw = nvgTextBounds(ctx, 0, 0, mCaption.c_str(), nullptr, nullptr) + 2;
      int th = fontSize();
      const_cast<Label*>(this)->mTextRealSize = Vector2i(tw, th);
      return mMinSize.cwiseMax(tw, th);
  }
}

void Label::draw(NVGcontext *ctx) 
{
    nvgFontFaceSize(ctx, mFont.c_str(), fontSize());
    const Color& color = enabled() 
                            ? mColor.notW(mTheme->mTextColor)
                            : mDisabledColor.notW(mTheme->mLabelTextDisabledColor);

    nvgFillColor(ctx, color);

    //int halign = (mFixedSize.x() > 0 ? (1 << mTextHAlign) : (1 << TextHAlign::hLeft));
    //int valign = (mFixedSize.y() > 0 ? (1 << mTextVAlign) : (1 << TextVAlign::vTop));

    Vector2i opos;
    switch (mTextHAlign)
    {
    case TextHAlign::hCenter: opos.x() = (mSize.x() - mTextRealSize.x()) / 2; break;
    case TextHAlign::hRight: opos.x() = (mSize.x() - mTextRealSize.x()); break;
    }

    switch (mTextVAlign)
    {
    case TextVAlign::vMiddle: opos.y() = (mSize.y() - mTextRealSize.y()) / 2; break;
    case TextVAlign::vBottom: opos.y() = (mSize.y() - mTextRealSize.y()); break;
    }

    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    if (mFixedSize.x() > 0)
      nvgTextBox(ctx, mPos.x() + opos.x(), mPos.y() + opos.y(), mFixedSize.x(), mCaption.c_str(), nullptr);
    else
      nvgText(ctx, mPos + opos, mCaption);

    Widget::draw(ctx);
}

void Label::save(Serializer &s) const {
    Widget::save(s);
    s.set("caption", mCaption);
    s.set("font", mFont);
    s.set("color", mColor);
}

bool Label::load(Serializer &s) {
    if (!Widget::load(s)) return false;
    if (!s.get("caption", mCaption)) return false;
    if (!s.get("font", mFont)) return false;
    if (!s.get("color", mColor)) return false;
    return true;
}

NAMESPACE_END(nanogui)
