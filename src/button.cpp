/*
    src/toolbutton.cpp -- [Normal/Toggle/Radio/Popup] Button widget

    This file was developed by Dalerank <dalerankn8@gmail.com>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/toolbutton.h>
#include <nanogui/theme.h>
#include <nanovg.h>
#include <nanogui/saveload.h>
#include <nanogui/entypo.h>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(Button, Widget)
RTTI_IMPLEMENT_INFO(LedButton, Button)
RTTI_IMPLEMENT_INFO(LinkButton, Button)
RTTI_IMPLEMENT_INFO(RadioButton, Button)
RTTI_IMPLEMENT_INFO(UpDownButton, Button)

UpDownButton::UpDownButton(Widget* parent)
  : Widget(parent,
           WidgetBoxLayout{ Orientation::Horizontal, Alignment::Fill, 2, 2})
{
  mDown = &toolbutton(Icon{ ENTYPO_ICON_LEFT_1 }, ButtonFlags{ Button::NormalButton },
    ButtonCallback{ [&] {
      if (mCallback) mCallback(false); 
      mActivateTime = getTimeFromStart() + mActivateInterval;
    }
  });
  mUp = &toolbutton(Icon{ ENTYPO_ICON_RIGHT_1 }, ButtonFlags{ Button::NormalButton },
    ButtonCallback{ [&] {
      if (mCallback) mCallback(true); 
      mActivateTime = getTimeFromStart() + mActivateInterval;
    }
  });
}

void UpDownButton::setUpIcon(int icon) { if (mUp) mUp->setIcon(icon); }
void UpDownButton::setDownIcon(int icon) { if (mDown) mDown->setIcon(icon); }

void UpDownButton::afterDraw(NVGcontext* ctx)
{
  Widget::afterDraw(ctx);

  int curState = mDown->pushed() ? -1 : 
                 mUp->pushed() ? 1 : 
                 0;
  if (curState != 0 && mLastState == curState)
  {
    float curtime = getTimeFromStart();
    if (mActivateTime < curtime && mLastUpdate + mRepeatInverval < curtime)
    {
      mCallback(curState == 1);
      mLastUpdate = curtime;
    }
  }
  mLastState = curState;
}

Button::Button(Widget *parent)
    : Widget(parent), mCaption("Untitled"), mIcon(0),
      mIconAlign(IconAlign::LeftCentered), 
      mBackgroundColor(Color(0, 0)),
      mTextColor(Color(0, 0))
{
  setFlags(NormalButton);
  setDrawFlags(DrawAll);
  setTextStyle(StyleTextNone);
  mCaptionSize = { -1.f, -1.f };
  mBorderSize = -1;
  mPushed = false;
}

Vector2f Button::getCaptionSize(NVGcontext *ctx)
{
  if (mCaptionSize.x() < 0)
    mCaptionSize = nvgTextBounds(ctx, 0, 0, mCaption.c_str(), nullptr);
 
  return mCaptionSize;
}

Vector2i Button::preferredSize(NVGcontext *ctx) const 
{
    int fontSize = mFontSize == -1 ? mTheme->mButtonFontSize : mFontSize;
    nvgFontFaceSize(ctx, "sans-bold", fontSize);
    auto capsize = const_cast<Button*>(this)->getCaptionSize(ctx);
    float iw = 0.0f, ih = (float)fontSize;

    if (mIcon) 
    {
        if (nvgIsFontIcon(mIcon)) 
        {
            ih *= icon_scale();
            nvgFontFaceSize(ctx, "icons", ih);
            iw = nvgTextBounds(ctx, 0, 0, utf8(mIcon).data(), nullptr, nullptr) + mSize.y() * 0.15f;
        }
        else 
        {
            int w, h;
            ih *= 0.9f;
            nvgImageSize(ctx, mIcon, &w, &h);
            iw = w * ih / h;
        }
    }
    return Vector2i((int)(capsize.x() + iw) + 20 + (*theme()->framePaddingLeft * 2), fontSize + 10);
}

bool Button::mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) {
    Widget::mouseButtonEvent(p, button, down, modifiers);
    /* Temporarily increase the reference count of the button in case the
       button causes the parent window to be destructed */
    ref<Button> self = this;

    if (isMouseButtonLeft(button) && mEnabled) {
        bool pushedBackup = mPushed;
        if (down) {
            if (haveFlag(Button::RadioButton)) {
                if (mButtonGroup.empty()) {
                    for (auto widget : parent()->children()) {
                        Button *b = Button::cast(widget);
                        if (b != this && b && (b->haveFlag(Button::RadioButton)) && b->mPushed) {
                            b->mPushed = false;
                            if (b->mChangeCallback)
                                b->mChangeCallback(this);
                        }
                    }
                } else {
                    for (auto b : mButtonGroup) {
                        if (b != this && (b->haveFlag(Button::RadioButton)) && b->mPushed) {
                            b->mPushed = false;
                            if (b->mChangeCallback)
                                b->mChangeCallback(this);
                        }
                    }
                }
            }
            if (haveFlag(PopupButton)) {
                for (auto widget : parent()->children()) {
                    Button *b = Button::cast(widget);
                    if (b != this && b && (b->haveFlag(PopupButton)) && b->mPushed) {
                        b->mPushed = false;
                        if (b->mChangeCallback)
                            b->mChangeCallback(this);
                    }
                }
            }
            if (haveFlag(ToggleButton))
                mPushed = !mPushed;
            else
                mPushed = true;
        }
        else if (mPushed) {
            if (contains(p))
            {
                beforeDoCallback();
                if (mCallback)
                  mCallback();
            }
            if (haveFlag(NormalButton))
                mPushed = false;
        }

        if (pushedBackup != mPushed && mChangeCallback)
        {
          beforeDoChangeCallback(mPushed);
          mChangeCallback(this);
        }

        return true;
    }
    return false;
}

Color Button::getTextColor() const 
{
  if (!mEnabled)
    return mTheme->mDisabledTextColor;

  if (mPushed)
    return mPressedTextColor.notW(mTheme->mButtonPressedTextColor);
  else
  {
    if (mMouseFocus) return mHoverTextColor.notW(mTheme->mButtonHoverTextColor);
    else return mTextColor.notW(mTheme->mTextColor);
  }
}

Color Button::getIconColor() const 
{ 
  if (mPushed && mPushedIconColor.w() > 0)
    return mPushedIconColor;
  if (mMouseFocus && mIconHoveredColor.w() > 0)
    return mIconHoveredColor;
  return mIconColor.notW(getTextColor()); 
}

void Button::draw(NVGcontext *ctx) 
{
  float cr = mCornerRadius < 0.f ? mTheme->mButtonCornerRadius : mCornerRadius;
    if (haveDrawFlag(DrawBody))
    {
      NVGcolor gradTop;
      NVGcolor gradBot;

      nvgBeginPath(ctx);
      nvgRoundedRect(ctx, mPos + Vector2i{ 1, 1 }, mSize - Vector2i{ 2, 2 }, cr - 1 );

      if (mBackgroundHoverColor.w() != 0)
      {
        gradTop = Color::transparent;
        gradBot = Color::transparent;
        
        if (mPushed || (mMouseFocus && mEnabled))
        {
          gradTop = mBackgroundHoverColor;
          gradBot = mBackgroundHoverColor;
        }

        nvgFillColor(ctx, mBackgroundColor);
        nvgFill(ctx);
      }
      else
      {
        gradTop = mTheme->mButtonGradientTopUnfocused;
        gradBot = mTheme->mButtonGradientBotUnfocused;
        if (mPushed)
        {
          gradTop = mTheme->mButtonGradientTopPushed;
          gradBot = mTheme->mButtonGradientBotPushed;
        }
        else if (mMouseFocus && mEnabled)
        {
          gradTop = mTheme->mButtonGradientTopFocused;
          gradBot = mTheme->mButtonGradientBotFocused;
        }

        if (mBackgroundColor.w() != 0)
        {
          nvgFillColor(ctx, Color(mBackgroundColor.rgb(), 1.f));
          nvgFill(ctx);
          if (mPushed) {
            gradTop.a = gradBot.a = 0.8f;
          }
          else {
            double v = 1 - mBackgroundColor.w();
            gradTop.a = gradBot.a = mEnabled ? v : v * .5f + .5f;
          }
        }
      }

      NVGpaint bg = nvgLinearGradient(ctx, mPos.x(), mPos.y(), 
                                           mPos.x(), mPos.y() + mSize.y(), 
                                           gradTop, gradBot);

      nvgFillPaint(ctx, bg);
      nvgFill(ctx);
    }

    if (haveDrawFlag(DrawBorder))
    {
      float bs = mBorderSize >= 0 ? mBorderSize : theme()->buttonBorderSize;

      nvgBeginPath(ctx);
      nvgStrokeWidth(ctx, bs);
      nvgRoundedRect(ctx, mPos + Vector2f{ bs - 0.5f, bs + (mPushed ? -.5f : .5f) }, 
                          mSize - Vector2f{ bs, bs + (mPushed ? 0.0f : 1.0f)}, cr);
      nvgStrokeColor(ctx, mBorderColor.notW(theme()->mBorderLight));
      nvgStroke(ctx);

      nvgBeginPath(ctx);
      nvgRoundedRect(ctx, mPos + Vector2f{ bs - 0.5f, bs - 0.5f }, 
                          mSize - Vector2f{ bs, bs + 1 }, cr);
      nvgStrokeColor(ctx, mBorderColor.notW(theme()->mBorderDark));
     
      nvgStroke(ctx);
    }

    int fontSize = mFontSize == -1 ? mTheme->mButtonFontSize : mFontSize;
    const char* fontName = (mFontName.empty() ? theme()->buttonFont : mFontName).c_str();
    nvgFontFaceSize(ctx, fontName, fontSize);
    auto capsize = getCaptionSize(ctx);

    Vector2f textPos = { 0, 0 };
    switch (mTextAlign.h)
    {
    case TextHAlign::hCenter: textPos.x() = (mSize.x() - mCaptionSize.x()) / 2; break;
    case TextHAlign::hRight: textPos.x() = (mSize.x() - mCaptionSize.x()); break;
    default: break;
    }

    switch (mTextAlign.v)
    {
    case TextVAlign::vMiddle: textPos.y() = (mSize.y() - mCaptionSize.y()) / 2; break;
    case TextVAlign::vBottom: textPos.y() = (mSize.y() - mCaptionSize.y()); break;
    case TextVAlign::vTop: textPos.y() = 0; break;
    }
    NVGcolor textColor = getTextColor();

    if (mIcon && haveDrawFlag(DrawIcon)) 
    {
        auto icon = utf8(mIcon);
        Color iconColor = getIconColor();

        float iw, ih = fontSize;
        if (nvgIsFontIcon(mIcon)) {
            ih *= icon_scale();
            nvgFontFaceSize(ctx, "icons", ih);
            iw = nvgTextBounds(ctx, 0, 0, icon.data(), nullptr, nullptr);
        } else {
            int w, h;
            ih *= 0.9f;
            nvgImageSize(ctx, mIcon, &w, &h);
            iw = w * ih / h;
        }
        if (mCaption != "")
            iw += mSize.y() * 0.15f;
        nvgFillColor(ctx, iconColor);
        nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
        Vector2f iconPos = (mPos + mSize / 2).cast<float>();;
        iconPos.y() -= 1;

        if (mIconAlign == IconAlign::LeftCentered) 
        {
            iconPos.x() -= (capsize.x() + iw) * 0.5f;
            textPos.x() += iw * 0.5f;
        } 
        else if (mIconAlign == IconAlign::RightCentered) 
        {
            textPos.x() -= iw * 0.5f;
            iconPos.x() += capsize.x() * 0.5f;
        } 
        else if (mIconAlign == IconAlign::Left) 
        {
            iconPos.x() = mPos.x() + 8;
        } 
        else if (mIconAlign == IconAlign::Right) 
        {
            iconPos.x() = mPos.x() + mSize.x() - iw - 8;
        }

        if (nvgIsFontIcon(mIcon)) 
        {
          nvgText(ctx, iconPos + Vector2f{ 0, 1 }, icon.data());
        } 
        else 
        {
            NVGpaint imgPaint = nvgImagePattern(ctx, iconPos.x(), iconPos.y() - ih/2, 
                                                iw, ih, 0, mIcon, mEnabled ? 0.5f : 0.25f);
            nvgFillPaint(ctx, imgPaint);
            nvgFill(ctx);
        }
    }

    if (haveDrawFlag(DrawCaption))
    {
      nvgFontFaceSize(ctx, fontName, fontSize);

      nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
      if (haveTextStyle(StyleTextShadow))
      {
        nvgFillColor(ctx, mTheme->mTextColorShadow);
        nvgText(ctx, mPos + textPos, mCaption);
      }

      nvgFillColor(ctx, textColor);
      nvgText(ctx, mPos + textPos + Vector2f{ 0, 1 }, mCaption);

      if (haveTextStyle(StyleTextUnderline))
      {
        nvgBeginPath(ctx);
        nvgStrokeColor(ctx, textColor);
        nvgStrokeWidth(ctx, 1.f);
        Vector2f tofs(0, height() - capsize.y());
        nvgLine(ctx, mPos + textPos + capsize._0y(), mPos + textPos + capsize);
        nvgStroke(ctx);
      }
    }

    Widget::draw(ctx);
}

void Button::save(Json::value &save) const 
{
  Widget::save(save);
  auto obj = save.get_obj();
  obj["caption"] = json().set(mCaption).name("Caption");
  obj["icon"] = json().set(mIcon).name("Icon");
  obj["iconPosition"] = json().set((int)mIconAlign).name("Icon align");
  obj["pushed"] = json().set(mPushed).name("Pushed");
  obj["backgroundColor"] = json().set(mBackgroundColor).name("Background color");
  obj["textColor"] = json().set(mTextColor).name("Text color");

  save = Json::value(obj);
}

bool Button::load(Json::value &save) 
{
  Widget::load(save);
  json s{ save.get_obj() };
  mCaption = s.get<std::string>("caption");
  mIcon = s.get<int>("icon");
  mIconAlign = (IconAlign)s.get<int>("iconAlign");
  mPushed = s.get<bool>("pushed");
  mBackgroundColor = s.get<Color>("backgroundColor");
  mTextColor = s.get<Color>("textColor");
  return true;
}

LinkButton::LinkButton(Widget* parent)
  : Button(parent)
{
  setDrawFlags(DrawCaption);
  setTextStyle(StyleTextUnderline);
  setCursor(Cursor::Hand);
}

void LinkButton::draw(NVGcontext* ctx)
{
  Button::draw(ctx);
}

Color LinkButton::getTextColor() const
{
  if (mPushed)
    return mPressedTextColor.notW(mTheme->mLinkPressedTextColor);
  else
  {
    if (mMouseFocus) return mHoverTextColor.notW(mTheme->mLinkHoverTextColor);
    else return mTextColor.notW(mTheme->mLinkTextColor);
  }
}

void nvgBezierTo(NVGcontext* ctx, float c1x, float c1y, float c2x, float c2y, float x, float y, float kw, float kh)
{
  nvgBezierTo(ctx, c1x * kw, c1y * kh, c2x * kw, c2y * kh, x * kw, y * kh);
}

LedButton::LedButton(Widget* parent, Mode mode, int w, int h)
  : Button(parent)
{
  setFixedSize({ w, h });
  setMode(mode);
}

void LedButton::draw(NVGcontext* ctx)
{
  Color outBg1, outBg2, inBg1, inBg2, outFlow1, outFlow2, inFlow1, inFlow2;
  Mode currentMode = circleCustom;

  outBg1 = Color(0xad, 0xad, 0xad, 0xff); outBg2 = Color(0xf0, 0xf0, 0xf0, 0xff);
  inBg1 = Color(0x82, 0x82, 0x82, 0xff);  inBg2 = Color(0x92, 0x92, 0x92, 0xff);
  inFlow1 = Color(0xff, 0xff, 0xff, 0xff); inFlow2 = Color(0xff, 0xff, 0xff, 0xff);

  switch (mMode)
  {
  case Mode::circleBlue:
    outFlow1 = Color(0x00, 0x03, 0x9a, 0xff);  outFlow2 = Color(0xaf, 0xa5, 0xff, 0xff);
    currentMode = circleCustom;
    break;
  case Mode::circleBlack:
    outFlow1 = Color(0x0, 0x0, 0x0, 0xff);  outFlow2 = Color(0x8c, 0x8c, 0x8c, 0xff);
    currentMode = circleCustom;
    break;
  case Mode::circleGreen:
    outFlow1 = Color(0x0f, 0x69, 0x00, 0xff); outFlow2 = Color(0xa3, 0xff, 0x87, 0xff);
    currentMode = circleCustom;
    break;
  case Mode::circleGray:
    outFlow1 = Color(0x5a, 0x5a, 0x5a, 0xff); outFlow2 = Color(0xa9, 0xa9, 0xa9, 0xff);
    currentMode = circleCustom;
    break;
  case Mode::circleOrange:
    outFlow1 = Color(0xda, 0x46, 0x15, 0xff); outFlow2 = Color(0xff, 0xc0, 0x6d, 0xff);
    currentMode = circleCustom;
    break;
  case Mode::circlePurple:
    outFlow1 = Color(0x87, 0x08, 0x83, 0xff); outFlow2 = Color(0xfe, 0x9a, 0xff, 0xff);
    currentMode = circleCustom;
    break;
  case Mode::circleRed:
    outFlow1 = Color(0xcf, 0x00, 0x00, 0xff); outFlow2 = Color(0xff, 0x8b, 0xa4, 0xff);
    currentMode = circleCustom;
    break;
  case Mode::circleYellow:
    outFlow1 = Color(0xd2, 0xcd, 0x00, 0xff); outFlow2 = Color(0xff, 0xfe, 0x8f, 0xff);
    currentMode = circleCustom;
    break;
  }
  nvgSave(ctx);

  switch (currentMode)
  {
  case circleCustom:
    float kw = 1. / 50. * width();
    float kh = 1. / 50. * height();

    nvgTranslate(ctx, mPos.x(), mPos.y());
    nvgBeginPath(ctx);
    auto bg1 = nvgLinearGradient(ctx, 23.4f * kw, 23.2f * kh, 42.4f * kw, 44.0f * kh, outBg1, outBg2);
    nvgMoveTo(ctx, 50 * kw, 25 * kh);
    nvgBezierTo(ctx, 50.f, 38.f, 38.8f, 50.f, 25, 50, kw, kh);
    nvgBezierTo(ctx, 11.2f, 50, 0.0f, 38.8f, 0.0f, 25, kw, kh);
    nvgBezierTo(ctx, 0.0f, 11.2f, 11.2f, 0.0f, 25, 0.0f, kw, kh);
    nvgBezierTo(ctx, 38.8f, 0.0f, 50, 11.2f, 50, 25, kw, kh);
    nvgFillPaint(ctx, bg1);
    nvgFill(ctx);

    nvgBeginPath(ctx);
    auto bg2 = nvgLinearGradient(ctx, 23.2f * kw, 23.2f * kh, 42.7f * kw, 43.7f * kh, inBg1, inBg2.mul_a(0.35f));
    nvgMoveTo(ctx, 45.2f * kw, 25.0f * kh);
    nvgBezierTo(ctx, 45.2f, 36.1f, 36.1f, 45.2f, 25.0f, 45.2f, kw, kh);
    nvgBezierTo(ctx, 13.9f, 45.2f, 4.8f, 36.1f, 4.8f, 25.0f, kw, kh);
    nvgBezierTo(ctx, 4.8f, 13.9f, 13.9f, 4.8f, 25.0f, 4.8f, kw, kh);
    nvgBezierTo(ctx, 36.1f, 4.8f, 45.2f, 13.9f, 45.2f, 25.0f, kw, kh);
    nvgClosePath(ctx);
    nvgFillPaint(ctx, bg2);
    nvgFill(ctx);

    nvgBeginPath(ctx);
    auto bg3 = nvgLinearGradient(ctx, 23.2f * kw, 23.2f * kh, 42.7f * kw, 43.8f * kh, outFlow1.mul_a(196 / 255.f), outFlow2);
    nvgMoveTo(ctx, 43.1f * kw, 25.0f * kh);
    nvgBezierTo(ctx, 43.1f, 35.0f, 35.0f, 43.1f, 25.0f, 43.1f, kw, kh);
    nvgBezierTo(ctx, 15.0f, 43.1f, 6.9f, 35.0f, 7.0f, 25.f, kw, kh);
    nvgBezierTo(ctx, 7.0f, 15.f, 15.0f, 7.0f, 25.f, 7.0f, kw, kh);
    nvgBezierTo(ctx, 35.f, 7.f, 43.1f, 15.0f, 43.1f, 25.f, kw, kh);
    nvgClosePath(ctx);
    nvgFillPaint(ctx, bg3);
    nvgFill(ctx);

    nvgBeginPath(ctx);
    auto bg4 = nvgLinearGradient(ctx, 23.4 * kw, 23.4 * kh, 44.0 * kw, 42.8 * kh, inFlow1.mul_a(0.4f), inFlow2.mul_a(0.87f));
    nvgMoveTo(ctx, 39.4f * kw, 19.0f * kh);
    nvgBezierTo(ctx, 39.4f, 25.1f, 33.f, 30.1f, 25.f, 30.1f, kw, kh);
    nvgBezierTo(ctx, 17.0f, 30.1f, 10.6f, 25.1f, 10.6f, 19.f, kw, kh);
    nvgBezierTo(ctx, 10.6f, 12.8f, 17.0f, 7.8f, 25.f, 7.8f, kw, kh);
    nvgBezierTo(ctx, 33.f, 7.8f, 39.4f, 12.8f, 39.4f, 19.f, kw, kh);
    nvgClosePath(ctx);
    nvgFillPaint(ctx, bg4);
    nvgFill(ctx);

    nvgRestore(ctx);
    break;
  }
}

NAMESPACE_END(nanogui)
