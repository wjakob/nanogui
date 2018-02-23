/*
    src/checkbox.cpp -- Two-state check box widget

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/checkbox.h>
#include <nanogui/opengl.h>
#include <nanogui/theme.h>
#include <nanogui/serializer/core.h>

NAMESPACE_BEGIN(nanogui)

CheckBox::CheckBox(Widget *parent, const std::string &caption,
                   const std::function<void(bool) > &callback, const std::string &font)
    : FontWidget(parent, font, false), mCaption(caption), mPushed(false),
      mChecked(false), mCallback(callback) {

    if (mTheme)
        mIconExtraScale = mTheme->mCheckBoxIconExtraScale;
    else
        mIconExtraScale = Theme::defaultCheckBoxIconExtraScale();
}

bool CheckBox::mouseButtonEvent(const Vector2i &p, int button, bool down,
                                int modifiers) {
    Widget::mouseButtonEvent(p, button, down, modifiers);
    if (!mEnabled)
        return false;

    if (button == GLFW_MOUSE_BUTTON_1) {
        if (down) {
            mPushed = true;
        } else if (mPushed) {
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
    if (mFixedSize != Vector2i::Zero())
        return mFixedSize;
    nvgFontSize(ctx, fontSize());
    nvgFontFace(ctx, mFont.c_str());
    return Vector2i(
        nvgTextBounds(ctx, 0, 0, mCaption.c_str(), nullptr, nullptr) +
            1.8f * fontSize(),
        fontSize() * 1.3f);
}

void CheckBox::draw(NVGcontext *ctx) {
    Widget::draw(ctx);

    nvgFontSize(ctx, fontSize());
    nvgFontFace(ctx, mFont.c_str());
    nvgFillColor(ctx,
                 mEnabled ? mTheme->mTextColor : mTheme->mDisabledTextColor);
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgText(ctx, mPos.x() + 1.6f * fontSize(), mPos.y() + mSize.y() * 0.5f,
            mCaption.c_str(), nullptr);

    NVGpaint bg = nvgBoxGradient(ctx, mPos.x() + 1.5f, mPos.y() + 1.5f,
                                 mSize.y() - 2.0f, mSize.y() - 2.0f, 3, 3,
                                 mPushed ? Color(0, 100) : Color(0, 32),
                                 Color(0, 0, 0, 180));

    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x() + 1.0f, mPos.y() + 1.0f, mSize.y() - 2.0f,
                   mSize.y() - 2.0f, 3);
    nvgFillPaint(ctx, bg);
    nvgFill(ctx);

    if (mChecked) {
        nvgFontSize(ctx, mSize.y() * icon_scale());
        nvgFontFace(ctx, mIconFont.c_str());
        nvgFillColor(ctx, mEnabled ? mTheme->mIconColor
                                   : mTheme->mDisabledTextColor);
        nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
        nvgText(ctx, mPos.x() + mSize.y() * 0.5f + 0.5f * icon_scale(),
                mPos.y() + mSize.y() * 0.5f, utf8(mTheme->mCheckBoxIcon).data(),
                nullptr);
    }
}

void CheckBox::save(Serializer &s) const {
    FontWidget::save(s);
    s.set("caption", mCaption);
    s.set("pushed", mPushed);
    s.set("checked", mChecked);
}

bool CheckBox::load(Serializer &s) {
    if (!FontWidget::load(s)) return false;
    if (!s.get("caption", mCaption)) return false;
    if (!s.get("pushed", mPushed)) return false;
    if (!s.get("checked", mChecked)) return false;
    return true;
}

NAMESPACE_END(nanogui)
