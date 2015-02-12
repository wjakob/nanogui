#include <nanogui/textbox.h>
#include <nanogui/opengl.h>
#include <nanogui/theme.h>

NANOGUI_NAMESPACE_BEGIN

TextBox::TextBox(Widget *parent, const std::string &value)
    : Widget(parent), mValue(value), mUnits(""), mUnitsImage(-1) {}

void TextBox::draw(NVGcontext* ctx) {
    NVGpaint bg = nvgBoxGradient(ctx,
        mPos.x() + 1, mPos.y() + 1 + 1.5f, mSize.x() - 2, mSize.y() - 2,
        3, 4, Color(255,32), Color(32, 32));

    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x() + 1, mPos.y() + 1 + 1.5f, mSize.x() - 2, mSize.y() - 2, 3);
    nvgFillPaint(ctx, bg);
    nvgFill(ctx);

    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x() + 0.5f, mPos.y() + 0.5f, mSize.x() - 1, mSize.y() - 1, 3.5f);
    nvgStrokeColor(ctx, Color(0,48));
    nvgStroke(ctx);

    nvgFontSize(ctx, mTheme->mStandardFontSize);
    nvgFontFace(ctx, "sans");
    float uw = 0, drawPos = mPos.x() + mSize.x() - mSize.y() * 0.5f;

    if (mUnitsImage > 0) {
        int w, h;
        nvgImageSize(ctx, mUnitsImage, &w, &h);
        float uh = mSize.y() * 0.4f;
        uw = w * uh / h;
        NVGpaint imgPaint = nvgImagePattern(
            ctx, mPos.x() + mSize.x() - mSize.y() * 0.3f - uw, mPos.y() + (mSize.y() -uh) * 0.5f,
            uw, uh, 0, mUnitsImage, mEnabled ? 0.7f : 0.35f);
        nvgBeginPath(ctx);
        nvgRect(ctx, mPos.x() + mSize.x() - mSize.y() * 0.3f - uw,
                mPos.y() + (mSize.y() - uh) * 0.5f, uw, uh);
        nvgFillPaint(ctx, imgPaint);
        nvgFill(ctx);
    } else if (!mUnits.empty()) {
        uw = nvgTextBounds(ctx, 0,0, mUnits.c_str(), nullptr, nullptr);
        nvgFillColor(ctx, Color(255, mEnabled ? 64 : 32));
        nvgTextAlign(ctx, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
        nvgText(ctx, mPos.x() + mSize.x() - mSize.y() * 0.3f,
                mPos.y() + mSize.y() * 0.5f, mUnits.c_str(), nullptr);
    } else {
        nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
        drawPos = mPos.x() + mSize.x() * 0.5f;
    }

    nvgFontSize(ctx, 20.0f);
    nvgFillColor(ctx, mEnabled ? mTheme->mTextColor : mTheme->mDisabledTextColor);
    if (uw > 0)
        nvgTextAlign(ctx, NVG_ALIGN_RIGHT | NVG_ALIGN_MIDDLE);
    nvgText(ctx, drawPos - uw,
            mPos.y() + mSize.y() * 0.5f, mValue.c_str(), nullptr);
}

NANOGUI_NAMESPACE_END
