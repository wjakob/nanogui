#include <nanogui/button.h>
#include <nanogui/theme.h>
#include <nanogui/opengl.h>

NANOGUI_NAMESPACE_BEGIN

Button::Button(Widget *parent, const std::string &caption, int icon)
    : Widget(parent), mCaption(caption), mIcon(icon),
      mIconPosition(LeftCentered), mPushed(false),
      mButtonFlags(NormalButton), mBackgroundColor(Color(0, 0)),
          mTextColor(Color(0, 0)), mFontSize(-1) {
}

Vector2i Button::preferredSize(NVGcontext *ctx) const {
    nvgFontSize(ctx, mTheme->mButtonFontSize);
    nvgFontFace(ctx, "sans-bold");
    float tw = nvgTextBounds(ctx, 0,0, mCaption.c_str(), nullptr, nullptr);
    float iw = 0.0f, ih = mFontSize == -1 ? mTheme->mButtonFontSize : mFontSize;

    if (mIcon) {
        if (nvgIsFontIcon(mIcon)) {
            ih *= 1.5f;
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
    return Vector2i(
        (int)(tw + iw) + 20,
        (mFontSize == -1 ? mTheme->mButtonFontSize : mFontSize) + 10);
}

bool Button::mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) {
    Widget::mouseButtonEvent(p, button, down, modifiers);

    if (button == GLFW_MOUSE_BUTTON_1 && mEnabled) {
        bool pushedBackup = mPushed;
        if (down) {
            if (mButtonFlags & RadioButton) {
                if (mButtonGroup.empty()) {
                    for (auto widget : parent()->children()) {
                        Button *button = dynamic_cast<Button *>(widget);
                        if (button != this && button && button->buttonFlags() & RadioButton)
                            button->mPushed = false;
                    }
                } else {
                    for (auto button : mButtonGroup) {
                        if (button != this && button->buttonFlags() & RadioButton)
                            button->mPushed = false;
                    }
                }
            }
            if (mButtonFlags & PopupButton) {
                for (auto widget : parent()->children()) {
                    Button *button = dynamic_cast<Button *>(widget);
                    if (button != this && button && button->buttonFlags() & PopupButton)
                        button->mPushed = false;
                }
            }
            if (mButtonFlags & ToggleButton)
                mPushed = !mPushed;
            else
                mPushed = true;
        } else if (mPushed) {
            if (contains(p) && mCallback)
                mCallback();
            if (mButtonFlags & NormalButton)
                mPushed = false;
        }
        if (pushedBackup != mPushed && mChangeCallback)
            mChangeCallback(mPushed);

        return true;
    }
    return false;
}

void Button::draw(NVGcontext *ctx) {
    Widget::draw(ctx);

    NVGcolor gradTop = mTheme->mButtonGradientTopUnfocused;
    NVGcolor gradBot = mTheme->mButtonGradientBotUnfocused;

    if (mPushed) {
        gradTop = mTheme->mButtonGradientTopPushed;
        gradBot = mTheme->mButtonGradientBotPushed;
    } else if (mMouseFocus && mEnabled) {
        gradTop = mTheme->mButtonGradientTopFocused;
        gradBot = mTheme->mButtonGradientBotFocused;
    }

    nvgBeginPath(ctx);

    nvgRoundedRect(ctx, mPos.x() + 1, mPos.y() + 1.0f, mSize.x() - 2,
                   mSize.y() - 2, mTheme->mButtonCornerRadius - 1);


    if (mBackgroundColor.w() != 0) {
        nvgFillColor(ctx, mBackgroundColor);
        nvgFill(ctx);
        gradTop.a = gradBot.a = mEnabled ? 0.90f : 0.95f;
    }

    NVGpaint bg = nvgLinearGradient(ctx, mPos.x(), mPos.y(), mPos.x(),
                                    mPos.y() + mSize.y(), gradTop, gradBot);

    nvgFillPaint(ctx, bg);
    nvgFill(ctx);

    nvgBeginPath(ctx);

    nvgRoundedRect(ctx, mPos.x() + 0.5f, mPos.y() + (mPushed ? 0.5f : 1.5f), mSize.x() - 1,
                   mSize.y() - 1 - (mPushed ? 0.0f : 1.0f), mTheme->mButtonCornerRadius);
    nvgStrokeColor(ctx, mTheme->mBorderLight);
    nvgStroke(ctx);

    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x() + 0.5f, mPos.y() + 0.5f, mSize.x() - 1,
                   mSize.y() - 2, mTheme->mButtonCornerRadius);
    nvgStrokeColor(ctx, mTheme->mBorderDark);
    nvgStroke(ctx);

    nvgFontSize(ctx, mFontSize == -1 ? mTheme->mButtonFontSize : mFontSize);
    nvgFontFace(ctx, "sans-bold");
    float tw = nvgTextBounds(ctx, 0,0, mCaption.c_str(), nullptr, nullptr);

    Vector2f center = mPos.cast<float>() + mSize.cast<float>() * 0.5f;
    Vector2f textPos(center.x() - tw * 0.5f, center.y() - 1);
    NVGcolor textColor =
        mTextColor.w() == 0 ? mTheme->mTextColor : mTextColor;
    if (!mEnabled)
        textColor = mTheme->mDisabledTextColor;

    if (mIcon) {
        auto icon = utf8(mIcon);

        float iw, ih = mFontSize == -1 ? mTheme->mButtonFontSize : mFontSize;
        if (nvgIsFontIcon(mIcon)) {
            ih *= 1.5f;
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

        if (mIconPosition == LeftCentered) {
            iconPos.x() -= (tw + iw) * 0.5f;
            textPos.x() += iw * 0.5f;
        } else if (mIconPosition == RightCentered) {
            textPos.x() -= iw * 0.5f;
            iconPos.x() += tw * 0.5f;
        } else if (mIconPosition == Left) {
            iconPos.x() = mPos.x() + 8;
        } else if (mIconPosition == Right) {
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

    nvgFontSize(ctx, mFontSize == -1 ? mTheme->mButtonFontSize : mFontSize);
    nvgFontFace(ctx, "sans-bold");
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgFillColor(ctx, mTheme->mTextColorShadow);
    nvgText(ctx,  textPos.x(), textPos.y() -1, mCaption.c_str(), nullptr);
    nvgFillColor(ctx, textColor);
    nvgText(ctx,  textPos.x(), textPos.y(), mCaption.c_str(), nullptr);
}

NANOGUI_NAMESPACE_END
