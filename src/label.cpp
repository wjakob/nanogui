#include <nanogui/label.h>
#include <nanogui/theme.h>
#include <nanogui/opengl.h>

NANOGUI_NAMESPACE_BEGIN

Label::Label(Widget *parent, const std::string &caption, const std::string &font)
    : Widget(parent), mCaption(caption), mFont(font) {
    mFontSize = mTheme->mStandardFontSize;
    mColor = mTheme->mTextColor;
}

Vector2i Label::preferredSize(NVGcontext *ctx) const {
    if (mCaption == "")
        return Vector2i::Zero();
    nvgFontSize(ctx,fontSize());
    nvgFontFace(ctx, mFont.c_str());
    if (mFixedSize.x() > 0) {
        float bounds[4];
        nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
        nvgTextBoxBounds(ctx, 0, 0, mFixedSize.x(), mCaption.c_str(), nullptr, bounds);
        return Vector2i(
            mFixedSize.x(), bounds[3]-bounds[1]
        );
    } else {
        nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
        return Vector2i(
            nvgTextBounds(ctx, 0, 0, mCaption.c_str(), nullptr, nullptr),
            mTheme->mStandardFontSize
        );
    }
}

void Label::draw(NVGcontext *ctx) {
    Widget::draw(ctx);
    nvgFontSize(ctx, fontSize());
    nvgFontFace(ctx, mFont.c_str());
    nvgFillColor(ctx, mColor);
    if (mFixedSize.x() > 0) {
        nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
        nvgTextBox(ctx, mPos.x(), mPos.y(), mFixedSize.x(), mCaption.c_str(), nullptr);
    } else {
        nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
        nvgText(ctx, mPos.x(), mPos.y() + mSize.y() * 0.5f, mCaption.c_str(), nullptr);
    }
}

NANOGUI_NAMESPACE_END
