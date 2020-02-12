/*
    src/checkbox.cpp -- Two-state check box widget

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/checkbox.h>
#include <nanovg.h>
#include <nanogui/theme.h>
#include <nanogui/serializer/core.h>
#include <nanogui/serializer/json.h>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(CheckBox, Widget)

CheckBox::CheckBox(Widget *parent, const std::string &caption,
                   std::function<void(bool)> callback,
                   bool initial)
    : Widget(parent), 
      mCaption(caption), 
      mPushed(false),
      mCallback(callback) 
{
  mIconExtraScale = 1.2f;// widget override
  mChecked = initial;
}

bool CheckBox::mouseButtonEvent(const Vector2i &p, int button, bool down,
                                int modifiers) {
    Widget::mouseButtonEvent(p, button, down, modifiers);
    if (!mEnabled)
        return false;

    if (isMouseButtonLeft(button)) {
        if (down) {
            mPushed = true;
        } 
        else if (mPushed) 
        {
            if (contains(p))
              toggleCheck();           
            mPushed = false;
        }
        return true;
    }
    return false;
}

void CheckBox::toggleCheck()
{
  mChecked = !mChecked;
  if (mCallback)
    mCallback(mChecked);
}

Vector2i CheckBox::preferredSize(NVGcontext *ctx) const 
{
    nvgFontFaceSize(ctx, "sans", fontSize());
    Vector2i prefSize( nvgTextBounds(ctx, 0, 0, mCaption.c_str(), nullptr, nullptr) + 1.8f * fontSize(),
                       fontSize() * 1.3f);

    prefSize += Vector2i::Constant(*theme()->framePaddingTop);
    prefSize = mFixedSize.fillZero(prefSize);

    return prefSize;
}

bool CheckBox::keyboardEvent(int key, int scancode, int action, int mods)
{
  if (!focused())
    return false;

  if (isKeyboardActionPress(action)
      || isKeyboardActionRepeat(action))
  {
    if (isKeyboardKey(key, "SPCE") || isKeyboardKey(key, "ENTR"))
    {
      toggleCheck();
      return true;
    }
  }

  return Widget::keyboardEvent(key, scancode, action, mods);
}

void CheckBox::draw(NVGcontext *ctx) 
{
    Widget::draw(ctx);

    nvgFontFaceSize(ctx, "sans", fontSize());
    nvgFillColor(ctx, mEnabled ? mTheme->mTextColor : mTheme->mDisabledTextColor);
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgText(ctx, mPos + Vector2i( 0.6f * fontSize() + height() + *theme()->innerSpacingCommon, 
                                  height() / 2 ), 
            mCaption);

    const Color& pushedColor = mPushedColor.notW(mTheme->mCheckboxPushedColor);
    const Color& bgColor = mChecked
                              ? mCheckedColor.notW(mTheme->mCheckboxCheckedColor)
                              : mUncheckedColor.notW(mTheme->mCheckboxUncheckedColor);

    NVGpaint bg = nvgBoxGradient(ctx, mPos.x() + 1.5f, mPos.y() + 1.5f,
                                 height() - 2.0f, height() - 2.0f, 3, 3,
                                 mPushed ? pushedColor : bgColor,
                                 Color(0, 0, 0, 180));

    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos + Vector2i{ 1, 1 }, mSize.yy() - Vector2i{ 2, 2 }, 3);
    nvgFillPaint(ctx, bg);
    nvgFill(ctx);

    if (mChecked) {
        nvgFontFaceSize(ctx, "icons", height() * icon_scale());
        nvgFillColor(ctx, mEnabled ? mTheme->mIconColor : mTheme->mDisabledTextColor);
        nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
        nvgText(ctx, mPos + size().yy() / 2 + Vector2i{ 1, 0 }, utf8(mTheme->mCheckBoxIcon).data());
    }
}

void CheckBox::save(Json::value &save) const {
  Widget::save(save);
  Json::object obj = save.get_obj();
  obj["caption"] = Json::hobject().$("value", mCaption).$("type", "string").$("name", "Caption");
  obj["pushed"] = Json::hobject().$("value", mPushed).$("type", "boolean").$("name", "Pushed");
  obj["checked"] = Json::hobject().$("value", mChecked).$("type", "boolean").$("name", "Checked");
}

bool CheckBox::load(Json::value &save) {
  Widget::load(save);
  auto c = save.get("caption"); mCaption = c.get_str("value");
  auto ph = save.get("pushed"); mPushed = ph.get_bool("value");
  auto ch = save.get("checked"); mChecked = ch.get_bool("value");
  return true;
}

NAMESPACE_END(nanogui)
