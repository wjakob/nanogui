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
            if (contains(p)) {
                mChecked = !mChecked;
                if (mCallback)
                    mCallback(mChecked);
            }
            mPushed = false;
        }
        return true;
    }
    return false;
}

Vector2i CheckBox::preferredSize(NVGcontext *ctx) const {
    nvgFontSize(ctx, (float)fontSize());
    nvgFontFace(ctx, "sans");
    Vector2i prefSize( nvgTextBounds(ctx, 0, 0, mCaption.c_str(), nullptr, nullptr) + 1.8f * fontSize(),
                       fontSize() * 1.3f);

    if (mFixedSize.x() > 0)
      prefSize.x() = mFixedSize.x();
    if (mFixedSize.y() > 0)
      prefSize.y() = mFixedSize.y();

    return prefSize;
}

void CheckBox::draw(NVGcontext *ctx) 
{
    Widget::draw(ctx);

    nvgFontSize(ctx, (float)fontSize());
    nvgFontFace(ctx, "sans");
    nvgFillColor(ctx, mEnabled ? mTheme->mTextColor : mTheme->mDisabledTextColor);
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgText(ctx, mPos + Vector2i( 1.6f * fontSize(), mSize.y() * 0.5f ), mCaption);

    const Color& pushedColor = mPushedColor.notW(mTheme->mCheckboxPushedColor);
    const Color& bgColor = mChecked
                              ? mCheckedColor.notW(mTheme->mCheckboxCheckedColor)
                              : mUncheckedColor.notW(mTheme->mCheckboxUncheckedColor);

    NVGpaint bg = nvgBoxGradient(ctx, mPos.x() + 1.5f, mPos.y() + 1.5f,
                                 mSize.y() - 2.0f, mSize.y() - 2.0f, 3, 3,
                                 mPushed ? pushedColor : bgColor,
                                 Color(0, 0, 0, 180));

    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos + Vector2i{ 1, 1 }, mSize.yy() - Vector2i{ 2, 2 }, 3);
    nvgFillPaint(ctx, bg);
    nvgFill(ctx);

    if (mChecked) {
        nvgFontFaceSize(ctx, "icons", mSize.y() * icon_scale());
        nvgFillColor(ctx, mEnabled ? mTheme->mIconColor : mTheme->mDisabledTextColor);
        nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
        nvgText(ctx, mPos.x() + mSize.y() * 0.5f + 1,
                     mPos.y() + mSize.y() * 0.5f, utf8(mTheme->mCheckBoxIcon).data(),
                nullptr);
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
