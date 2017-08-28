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

PopupButton::PopupButton(Widget *parent, const std::string &caption, int buttonIcon)
    : Button(parent, caption, buttonIcon),
      mChevronIcon(ENTYPO_ICON_CHEVRON_SMALL_RIGHT) {

    setFlags(Flags::ToggleButton | Flags::PopupButton);

    Window *parentWindow = window();
    mPopup = new Popup(parentWindow->parent(), window());
    mPopup->setSize(Vector2i(320, 250));
    mPopup->setVisible(false);
}

Vector2i PopupButton::preferredSize(NVGcontext *ctx) const {
    Vector2i size = Button::preferredSize(ctx);

    if (mChevronIcon) {
        float iw = 0.0f;
        float fontSize = mFontSize == -1 ? mTheme->mButtonFontSize : mFontSize;
        if (nvgIsFontIcon(mChevronIcon)) {
            fontSize *= 1.5f;
            nvgFontFace(ctx, "icons");
            nvgFontSize(ctx, fontSize);
            iw = nvgTextBounds(ctx, 0, 0, utf8(mChevronIcon).data(), nullptr, nullptr)
                + mSize.y() * 0.15f;
        } else {
            int w, h;
            fontSize *= 0.9f;
            nvgImageSize(ctx, mChevronIcon, &w, &h);
            iw = w * fontSize / h;
        }
        size.x() += iw + 20;
    }

    return size;
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
        Vector2f iconPos(0, mPos.y() + mSize.y() * 0.5f);

        if (mPopup->side() == Popup::Left)
            iconPos[0] = mPos.x() + 8;
        else
            iconPos[0] = mPos.x() + mSize.x() - iw - 8;

        nvgText(ctx, iconPos.x(), iconPos.y(), icon.data(), nullptr);
    }
}

void PopupButton::performLayout(NVGcontext *ctx) {
    Widget::performLayout(ctx);

    const Window *parentWindow = window();

    if (mPopup->side() == Popup::Left || mPopup->side() == Popup::Right) {
        int posY = absolutePosition().y() - parentWindow->position().y() + mSize.y() /2;
        if (mPopup->side() == Popup::Right)
            mPopup->setAnchorPos(Vector2i(parentWindow->width(), posY));
        else
            mPopup->setAnchorPos(Vector2i(0, posY));
    }
    else {
        int posY = absolutePosition().y() - parentWindow->position().y();
        if (mPopup->side() == Popup::Bottom)
            posY += mSize.y();

        mPopup->setAnchorPos(Vector2i(absolutePosition().x() - parentWindow->position().x() + mSize.x() / 2, posY));
    }
}

void PopupButton::setSide(Popup::Side side) {
    int icon = 0;

    switch(side) {
        case Popup::Right:
            icon = ENTYPO_ICON_CHEVRON_SMALL_RIGHT;
            break;
        case Popup::Left:
            icon = ENTYPO_ICON_CHEVRON_SMALL_LEFT;
            break;
        case Popup::Top:
            icon = ENTYPO_ICON_CHEVRON_SMALL_UP;
            break;
        case Popup::Bottom:
            icon = ENTYPO_ICON_CHEVRON_SMALL_DOWN;
            break;
    }

    if (icon != mChevronIcon)
        setChevronIcon(icon);

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
