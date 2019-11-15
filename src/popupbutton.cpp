/*
    src/popupbutton.cpp -- Button which launches a popup widget

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/popupbutton.h>
#include <nanogui/theme.h>
#include <nanovg.h>
#include <nanogui/serializer/core.h>

NAMESPACE_BEGIN(nanogui)

PopupButton::PopupButton(Widget *parent, const std::string &caption, int buttonIcon)
    : Button(parent, caption, buttonIcon) {

    mChevronIcon = mTheme->mPopupChevronRightIcon;

    setFlags(Flag::ToggleButton | Flag::PopupButton);

    Window *parentWindow = window();
    mPopup = parentWindow->parent()->add<Popup>(window());
    mPopup->setSize(Vector2i(320, 250));
    mPopup->setVisible(false);

    mIconExtraScale = 0.8f;// widget override
}

PopupButton::~PopupButton() {
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

        nvgFontSize(ctx, (mFontSize < 0 ? mTheme->mButtonFontSize : mFontSize) * icon_scale());
        nvgFontFace(ctx, "icons");
        nvgFillColor(ctx, mEnabled ? textColor : mTheme->mDisabledTextColor);
        nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);

        float iw = nvgTextBounds(ctx, 0, 0, icon.data(), nullptr, nullptr);
        Vector2f iconPos(0, mPos.y() + mSize.y() * 0.5f - 1);

        if (mPopup->side() == Popup::Right)
            iconPos[0] = mPos.x() + mSize.x() - iw - 8;
        else
            iconPos[0] = mPos.x() + 8;

        nvgText(ctx, iconPos.x(), iconPos.y(), icon.data(), nullptr);
    }
}

void PopupButton::performLayout(NVGcontext *ctx) {
    Widget::performLayout(ctx);

    const Window *parentWindow = window();

    int posY = absolutePosition().y() - parentWindow->position().y() + mSize.y() /2;
    if (mPopup->side() == Popup::Right)
        mPopup->setAnchorPos(Vector2i(parentWindow->width() + 15, posY));
    else
        mPopup->setAnchorPos(Vector2i(0 - 15, posY));
}

void PopupButton::setSide(Popup::Side side) {
    if (mPopup->side() == Popup::Right &&
        mChevronIcon == mTheme->mPopupChevronRightIcon)
        setChevronIcon(mTheme->mPopupChevronLeftIcon);
    else if (mPopup->side() == Popup::Left &&
             mChevronIcon == mTheme->mPopupChevronLeftIcon)
        setChevronIcon(mTheme->mPopupChevronRightIcon);
    mPopup->setSide(side);
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
