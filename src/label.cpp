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
#include <nanogui/opengl.h>
#include <nanogui/serializer/core.h>

NAMESPACE_BEGIN(nanogui)

Label::Label(Widget *parent, const std::string &caption, const std::string &font, int fontSize)
    : Widget(parent), mCaption(caption), mFont(font), mTextTruncation(TextTruncation::None),
      mTextAlignment(TextAlignment::Left) {
    if (mTheme) {
        mFontSize = mTheme->mStandardFontSize;
        mColor = mTheme->mTextColor;
    }
    if (fontSize >= 0) mFontSize = fontSize;
}

void Label::setTheme(Theme *theme) {
    Widget::setTheme(theme);
    if (mTheme) {
        mFontSize = mTheme->mStandardFontSize;
        mColor = mTheme->mTextColor;
    }
}

Vector2i Label::preferredSize(NVGcontext *ctx) const {
    if (mCaption == "")
        return Vector2i::Zero();
    nvgFontFace(ctx, mFont.c_str());
    nvgFontSize(ctx, fontSize());
    if (mFixedSize.x() > 0) {
        float bounds[4];
        nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
        nvgTextBoxBounds(ctx, mPos.x(), mPos.y(), mFixedSize.x(), mCaption.c_str(), nullptr, bounds);
        return Vector2i(mFixedSize.x(), bounds[3] - bounds[1]);
    } else {
        nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
        return Vector2i(
            nvgTextBounds(ctx, 0, 0, mCaption.c_str(), nullptr, nullptr) + 2,
            fontSize()
        );
    }
}

void Label::draw(NVGcontext *ctx) {
    Widget::draw(ctx);
    nvgFontFace(ctx, mFont.c_str());
    nvgFontSize(ctx, fontSize());
    nvgFillColor(ctx, mColor);

    std::string caption = truncateText(ctx, mCaption, mTextTruncation, width());

    if (mFixedSize.x() > 0) {
        Vector2f textOrigin = Vector2f(mPos.x(), mPos.y());
        int textAlign = NVG_ALIGN_LEFT;

        if (mTextAlignment == TextAlignment::Center) {
            textAlign = NVG_ALIGN_CENTER;
            textOrigin.x() += mFixedSize.x() * 0.5f;
        } else if (mTextAlignment == TextAlignment::Right) {
            textAlign = NVG_ALIGN_RIGHT;
            textOrigin.x() += mFixedSize.x();
        }

        nvgTextAlign(ctx, textAlign | NVG_ALIGN_TOP);
        nvgTextBox(ctx, textOrigin.x(), textOrigin.y(), mFixedSize.x(), caption.c_str(), nullptr);
    } else {
        Vector2f textOrigin = Vector2f(mPos.x(), mPos.y() + mSize.y() * 0.5f);
        int textAlign = NVG_ALIGN_LEFT;

        if (mTextAlignment == TextAlignment::Center) {
            textAlign = NVG_ALIGN_CENTER;
            textOrigin.x() += mSize.x() * 0.5f;
        } else if (mTextAlignment == TextAlignment::Right) {
            textAlign = NVG_ALIGN_RIGHT;
            textOrigin.x() += mSize.x();
        }

        nvgTextAlign(ctx, textAlign | NVG_ALIGN_MIDDLE);
        nvgText(ctx, textOrigin.x(), textOrigin.y(), caption.c_str(), nullptr);
    }
}

void Label::save(Serializer &s) const {
    Widget::save(s);
    s.set("caption", mCaption);
    s.set("font", mFont);
    s.set("color", mColor);
    s.set("textTruncation", mTextTruncation);
    s.set("textAlignment", mTextAlignment);
}

bool Label::load(Serializer &s) {
    if (!Widget::load(s)) return false;
    if (!s.get("caption", mCaption)) return false;
    if (!s.get("font", mFont)) return false;
    if (!s.get("color", mColor)) return false;

    // "new" fields
    const std::vector<std::string> keys = s.keys();
    if (std::find(keys.begin(), keys.end(), "textTruncation") != keys.end()) {
        if (!s.get("textTruncation", mTextTruncation)) return false;
    }
    if (std::find(keys.begin(), keys.end(), "textAlignment") != keys.end()) {
        if (!s.get("textAlignment", mTextAlignment)) return false;
    }
    return true;
}

NAMESPACE_END(nanogui)
