/*
    src/popupbutton.cpp -- Button which launches a popup widget

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/popupbutton.h>
#include <nanogui/entypo.h>
#include <nanogui/theme.h>
#include <nanogui/opengl.h>
#include <nanogui/serializer/core.h>

NAMESPACE_BEGIN(nanogui)

PopupButton::PopupButton(Widget *parent, const std::string &caption,
                         int buttonIcon, int chevronIcon)
    : Button(parent, caption, buttonIcon), mChevronIcon(chevronIcon) {

    setFlags(Flags::ToggleButton | Flags::PopupButton);

    Window *parentWindow = window();
    mPopup = new Popup(parentWindow->parent(), window());
    mPopup->setSize(Vector2i(320, 250));
    mPopup->setVisible(false);
}

Vector2i PopupButton::preferredSize(NVGcontext *ctx) const {
    return Button::preferredSize(ctx) + Vector2i(15, 0);
}

void PopupButton::draw(NVGcontext* ctx) {
    if (!mEnabled && mPushed)
        mPushed = false;

    mPopup->setVisible(mPushed);
    Button::draw(ctx);

    if (mChevronIcon) {
        auto icon = utf8(mChevronIcon);
        NVGcolor textColor =
            mTextColor.w() == 0 ? mTheme->mTextColor : mTextColor;

        nvgFontSize(ctx, (mFontSize < 0 ? mTheme->mButtonFontSize : mFontSize) * 1.5f);
        nvgFontFace(ctx, "icons");
        nvgFillColor(ctx, mEnabled ? textColor : mTheme->mDisabledTextColor);
        nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);

        float iw = nvgTextBounds(ctx, 0, 0, icon.data(), nullptr, nullptr);
        Vector2f iconPos(mPos.x() + mSize.x() - iw - 8,
                         mPos.y() + mSize.y() * 0.5f - 1);

        nvgText(ctx, iconPos.x(), iconPos.y(), icon.data(), nullptr);
    }
}

void PopupButton::performLayout(NVGcontext *ctx) {
    Widget::performLayout(ctx);

    const Window *parentWindow = window();

    mPopup->setAnchorPos(Vector2i(parentWindow->width() + 15,
        absolutePosition().y() - parentWindow->position().y() + mSize.y() /2));
}

void PopupButton::save(Serializer &s) const {
    Button::save(s);
    s.set("chevronIcon", mChevronIcon);
}

bool PopupButton::load(Serializer &s) {
    if (!Button::load(s))
        return false;
    if (!s.get("chevronIcon", mChevronIcon))
        return false;
    return true;
}

NAMESPACE_END(nanogui)
