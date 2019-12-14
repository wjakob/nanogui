/*
    src/toolbutton.cpp -- [Normal/Toggle/Radio/Popup] Button widget

    This file was developed by Dalerank <dalerankn8@gmail.com>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/button.h>
#include <nanogui/theme.h>
#include <nanovg.h>
#include <nanogui/common.h>
#include <nanogui/serializer/json.h>
#include <nanogui/serializer/core.h>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(Button, Widget)
RTTI_IMPLEMENT_INFO(LedButton, Button)
RTTI_IMPLEMENT_INFO(LinkButton, Button)

Button::Button(Widget *parent, const std::string &caption, int icon)
    : Widget(parent), mCaption(caption), mIcon(icon),
      mIconPosition(IconPosition::LeftCentered), mPushed(false),
      mBackgroundColor(Color(0, 0)),
      mTextColor(Color(0, 0))
{
  setFlags(NormalButton);
  setDrawFlags(DrawAll);
}

Vector2i Button::preferredSize(NVGcontext *ctx) const {
    int fontSize = mFontSize == -1 ? mTheme->mButtonFontSize : mFontSize;
    nvgFontSize(ctx, (float)fontSize);
    nvgFontFace(ctx, "sans-bold");
    float tw = nvgTextBounds(ctx, 0,0, mCaption.c_str(), nullptr, nullptr);
    float iw = 0.0f, ih = (float)fontSize;

    if (mIcon) {
        if (nvgIsFontIcon(mIcon)) {
            ih *= icon_scale();
            nvgFontFace(ctx, "icons");
            nvgFontSize(ctx, ih);
            iw = nvgTextBounds(ctx, 0, 0, utf8(mIcon).data(), nullptr, nullptr)
                + mSize.y() * 0.15f;
        } else {
            int w, h;
            ih *= 0.9f;
            nvgImageSize(ctx, mIcon, &w, &h);
            iw = w * ih / h;
        }
    }
    return Vector2i((int)(tw + iw) + 20, fontSize + 10);
}

bool Button::mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) {
    Widget::mouseButtonEvent(p, button, down, modifiers);
    /* Temporarily increase the reference count of the button in case the
       button causes the parent window to be destructed */
    ref<Button> self = this;

    if (isMouseButtonLeft(button) && mEnabled) {
        bool pushedBackup = mPushed;
        if (down) {
            if (haveFlag(RadioButton)) {
                if (mButtonGroup.empty()) {
                    for (auto widget : parent()->children()) {
                        Button *b = Button::cast(widget);
                        if (b != this && b && (b->haveFlag(RadioButton)) && b->mPushed) {
                            b->mPushed = false;
                            if (b->mChangeCallback)
                                b->mChangeCallback(false);
                        }
                    }
                } else {
                    for (auto b : mButtonGroup) {
                        if (b != this && (b->haveFlag(RadioButton)) && b->mPushed) {
                            b->mPushed = false;
                            if (b->mChangeCallback)
                                b->mChangeCallback(false);
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
                            b->mChangeCallback(false);
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
          mChangeCallback(mPushed);
        }

        return true;
    }
    return false;
}

Color Button::getTextColor() const 
{
  if (mPushed)
    return mPressedTextColor.w() == 0 ? mTheme->mButtonPressedTextColor : mPressedTextColor;
  else
  {
    if (mMouseFocus)
      return mHoverTextColor.w() == 0 ? mTheme->mButtonHoverTextColor : mTextColor;
    else
      return mTextColor.w() == 0 ? mTheme->mTextColor : mTextColor;
  }
}

void Button::draw(NVGcontext *ctx) {
    Widget::draw(ctx);

    if (haveDrawFlag(DrawBody))
    {
      NVGcolor gradTop = mTheme->mButtonGradientTopUnfocused;
      NVGcolor gradBot = mTheme->mButtonGradientBotUnfocused;

      if (mPushed) {
        gradTop = mTheme->mButtonGradientTopPushed;
        gradBot = mTheme->mButtonGradientBotPushed;
      }
      else if (mMouseFocus && mEnabled) {
        gradTop = mTheme->mButtonGradientTopFocused;
        gradBot = mTheme->mButtonGradientBotFocused;
      }

      nvgBeginPath(ctx);

      nvgRoundedRect(ctx, mPos.x() + 1, mPos.y() + 1.0f, mSize.x() - 2,
        mSize.y() - 2, mTheme->mButtonCornerRadius - 1);

      if (mBackgroundColor.w() != 0) {
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

      NVGpaint bg = nvgLinearGradient(ctx, mPos.x(), mPos.y(), mPos.x(),
        mPos.y() + mSize.y(), gradTop, gradBot);

      nvgFillPaint(ctx, bg);
      nvgFill(ctx);
    }

    if (haveDrawFlag(DrawBorder))
    {
      nvgBeginPath(ctx);
      nvgStrokeWidth(ctx, 1.0f);
      nvgRoundedRect(ctx, mPos.x() + 0.5f, mPos.y() + (mPushed ? 0.5f : 1.5f), mSize.x() - 1,
        mSize.y() - 1 - (mPushed ? 0.0f : 1.0f), mTheme->mButtonCornerRadius);
      nvgStrokeColor(ctx, mTheme->mBorderLight);
      nvgStroke(ctx);

      nvgBeginPath(ctx);
      nvgRoundedRect(ctx, mPos.x() + 0.5f, mPos.y() + 0.5f, mSize.x() - 1,
        mSize.y() - 2, mTheme->mButtonCornerRadius);
      nvgStrokeColor(ctx, mTheme->mBorderDark);
      nvgStroke(ctx);
    }

    int fontSize = mFontSize == -1 ? mTheme->mButtonFontSize : mFontSize;
    nvgFontSize(ctx, fontSize);
    nvgFontFace(ctx, "sans-bold");
    float tw = nvgTextBounds(ctx, 0,0, mCaption.c_str(), nullptr, nullptr);

    Vector2f center = mPos.cast<float>() + mSize.cast<float>() * 0.5f;
    Vector2f textPos(center.x() - tw * 0.5f, center.y() - 1);
    NVGcolor textColor = getTextColor();

    if (!mEnabled)
        textColor = mTheme->mDisabledTextColor;

    if (mIcon && haveDrawFlag(DrawIcon)) {
        auto icon = utf8(mIcon);

        float iw, ih = fontSize;
        if (nvgIsFontIcon(mIcon)) {
            ih *= icon_scale();
            nvgFontSize(ctx, ih);
            nvgFontFace(ctx, "icons");
            iw = nvgTextBounds(ctx, 0, 0, icon.data(), nullptr, nullptr);
        } else {
            int w, h;
            ih *= 0.9f;
            nvgImageSize(ctx, mIcon, &w, &h);
            iw = w * ih / h;
        }
        if (mCaption != "")
            iw += mSize.y() * 0.15f;
        nvgFillColor(ctx, textColor);
        nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
        Vector2f iconPos = center;
        iconPos.y() -= 1;

        if (mIconPosition == IconPosition::LeftCentered) {
            iconPos.x() -= (tw + iw) * 0.5f;
            textPos.x() += iw * 0.5f;
        } else if (mIconPosition == IconPosition::RightCentered) {
            textPos.x() -= iw * 0.5f;
            iconPos.x() += tw * 0.5f;
        } else if (mIconPosition == IconPosition::Left) {
            iconPos.x() = mPos.x() + 8;
        } else if (mIconPosition == IconPosition::Right) {
            iconPos.x() = mPos.x() + mSize.x() - iw - 8;
        }

        if (nvgIsFontIcon(mIcon)) {
            nvgText(ctx, iconPos.x(), iconPos.y()+1, icon.data(), nullptr);
        } else {
            NVGpaint imgPaint = nvgImagePattern(ctx,
                    iconPos.x(), iconPos.y() - ih/2, iw, ih, 0, mIcon, mEnabled ? 0.5f : 0.25f);

            nvgFillPaint(ctx, imgPaint);
            nvgFill(ctx);
        }
    }

    if (haveDrawFlag(DrawText))
    {
      nvgFontSize(ctx, fontSize);
      nvgFontFace(ctx, "sans-bold");
      nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
      nvgFillColor(ctx, mTheme->mTextColorShadow);
      nvgText(ctx, textPos.x(), textPos.y(), mCaption.c_str(), nullptr);
      nvgFillColor(ctx, textColor);
      nvgText(ctx, textPos.x(), textPos.y() + 1, mCaption.c_str(), nullptr);
    }
}

void Button::save(Serializer &s) const {
    Widget::save(s);
    s.set("caption", mCaption);
    s.set("icon", mIcon);
    s.set("iconPosition", (int) mIconPosition);
    s.set("pushed", mPushed);
    s.set("flags", mFlags);
    s.set("backgroundColor", mBackgroundColor);
    s.set("textColor", mTextColor);
}

void Button::save(Json::value &save) const {
  Widget::save(save);
  Json::object obj = save.get_obj();
  obj["caption"] = Json::hobject().$("value", mCaption).$("type", "string").$("name", "Caption");
  obj["icon"] = Json::hobject().$("value", mIcon).$("type", "integer").$("name", "Icon");
  obj["iconPosition"] = Json::hobject().$("value", (int)mIconPosition).$("type", "integer").$("name", "Icon position");
  obj["pushed"] = Json::hobject().$("value", mPushed).$("type", "boolean").$("name", "Icon");
  obj["backgroundColor"] = Json::hobject().$("color", mBackgroundColor.toInt()).$("type", "color").$("name", "Background color");
  obj["textColor"] = Json::hobject().$("color", mTextColor.toInt()).$("type", "color").$("name", "Text color");

  save = Json::value(obj);
}

bool Button::load(Json::value &save) {
  Widget::load(save);
  auto c = save.get("caption"); mCaption = c.get_str("value");
  auto i = save.get("icon"); mIcon = i.get_int("value");
  auto ip = save.get("iconPosition"); mIconPosition = (IconPosition)ip.get_int("value");
  auto ph = save.get("pushed"); mPushed = ph.get_bool("value");
  auto bg = save.get("backgroundColor"); mBackgroundColor = Color(bg.get_int("color"));
  auto tc = save.get("textColor"); mTextColor = Color(tc.get_int("color"));
  return true;
}

bool Button::load(Serializer &s) {
  if (!Widget::load(s)) return false;
  if (!s.get("caption", mCaption)) return false;
  if (!s.get("icon", mIcon)) return false;
  if (!s.get("iconPosition", mIconPosition)) return false;
  if (!s.get("pushed", mPushed)) return false;
  if (!s.get("flags", mFlags)) return false;
  if (!s.get("backgroundColor", mBackgroundColor)) return false;
  if (!s.get("textColor", mTextColor)) return false;
  return true;
}

LinkButton::LinkButton(Widget* parent)
  : Button(parent)
{
  setDrawFlags(DrawText);
}

void LinkButton::draw(NVGcontext* ctx)
{
  Button::draw(ctx);
}

Color LinkButton::getTextColor() const
{
  if (mPushed)
    return mPressedTextColor.w() == 0 ? mTheme->mLinkPressedTextColor : mPressedTextColor;
  else
  {
    if (mMouseFocus)
      return mHoverTextColor.w() == 0 ? mTheme->mLinkHoverTextColor : mTextColor;
    else
      return mTextColor.w() == 0 ? mTheme->mLinkTextColor : mTextColor;
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
