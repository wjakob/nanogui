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
#include <nanogui/screen.h>
#include <nanovg.h>
#include <nanogui/saveload.h>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(Label, Widget)

Label::Label(Widget *parent)
    : Widget(parent), mCaption(""), mFont("sans") {
}

void Label::setTheme(Theme *theme) {
    Widget::setTheme(theme);
}

int Label::fontSize() const { return mFontSize > 0 ? mFontSize : mTheme->mStandardFontSize;  }

void Label::setCaption(const std::string& caption)
{
  mCaption = caption;
  if (auto s = screen())
    s->needPerformLayout(parent());
}

Vector2i Label::preferredSize(NVGcontext *ctx) const {
  if (mCaption == "")
  {
    if (mFixedSize.x() > 0 || mFixedSize.y() > 0)
      return mFixedSize;

    return Vector2i::Zero();
  }

  nvgFontFaceSize(ctx, mFont.c_str(), fontSize());
  if (mFixedSize.x() > 0 || mTextWidthBreak) 
  {
    const_cast<Label*>(this)->mTextRealSize = nvgTextBoxBounds(ctx, 0, 0, 
                                                               mTextWidthBreak ? mTextWidthBreak : mFixedSize.x(), 
                                                               mCaption.c_str()).cast<int>();
    return mFixedSize.fillZero(mTextRealSize);
  } 
  else if (mTextWrapped)
  {
    const_cast<Label*>(this)->mTextRealSize = nvgTextBoxBounds(ctx, 0, 0,
                                                               parent()->width(),
                                                               mCaption.c_str()).cast<int>();
    return mTextRealSize;
  }
  else 
  {
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    Vector2f tsize = nvgTextBounds(ctx, 0, 0, mCaption.c_str()) + Vector2f(2, 0);
    int th = fontSize();
    const_cast<Label*>(this)->mTextRealSize = Vector2i(tsize.x(), th);
    return mMinSize.cwiseMax(tsize.x(), th);
  }
}

void Label::draw(NVGcontext *ctx) 
{
    if (mBackgroundColor.w() > 0)
    {
      nvgBeginPath(ctx);
      nvgRoundedRect(ctx, mPos + Vector2i{ 1, 1 }, mSize - Vector2i{ 2, 2 }, 3);

      nvgFillColor(ctx, mBackgroundColor);
      nvgFill(ctx);
    }

    nvgFontFaceSize(ctx, mFont.c_str(), fontSize());
    const Color& color = enabled() 
                            ? mColor.notW(mTheme->mTextColor)
                            : mDisabledColor.notW(mTheme->mLabelTextDisabledColor);

    nvgFillColor(ctx, color);

    //int halign = (mFixedSize.x() > 0 ? (1 << mTextHAlign) : (1 << TextHAlign::hLeft));
    //int valign = (mFixedSize.y() > 0 ? (1 << mTextVAlign) : (1 << TextVAlign::vTop));

    Vector2i opos;
    switch (mTextAlign.h)
    {
    case TextHAlign::hLeft: opos.x() = mTextOffset.x(); break;
    case TextHAlign::hCenter: opos.x() = (mSize.x() - mTextRealSize.x()) / 2 + mTextOffset.x(); break;
    case TextHAlign::hRight: opos.x() = (mSize.x() - mTextRealSize.x()) - mTextOffset.x(); break;
    }

    switch (mTextAlign.v)
    {
    case TextVAlign::vMiddle: opos.y() = (mSize.y() - mTextRealSize.y()) / 2 + mTextOffset.y(); break;
    case TextVAlign::vBottom: opos.y() = (mSize.y() - mTextRealSize.y()) - mTextOffset.y(); break;
    case TextVAlign::vTop: opos.y() = mTextOffset.y(); break;
    }

    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    if (mFixedSize.x() > 0 || mTextWidthBreak > 0)
      nvgTextBox(ctx, mPos.x() + opos.x(), mPos.y() + opos.y(), 
                 mTextWidthBreak > 0 ? mTextWidthBreak : mFixedSize.x(), mCaption.c_str(), nullptr);
    else if (mTextWrapped)
      nvgTextBox(ctx, mPos.x() + mTextOffset.x(), mPos.y() + mTextOffset.y(),
                 width() - mTextOffset.x(), mCaption.c_str(), nullptr);
    else
      nvgText(ctx, mPos + opos, mCaption);

    Widget::draw(ctx);
}

void Label::save(Json::value &s) const {
  Widget::save(s);
  auto obj = s.get_obj();

  obj["caption"] = json().set(mCaption).name("Caption");
  obj["font"] = json().set(mFont).name("Font");
  obj["color"] = json().set(mColor).name("Color");

  s = Json::value(obj);
}

bool Label::load(Json::value &save) {
  Widget::load(save);
  json s{ save.get_obj() };

  mCaption = s.get<std::string>("caption");
  mFont = s.get<std::string>("font");
  mColor = s.get<Color>("color");
  return true;
}

NAMESPACE_END(nanogui)
