#include <nanogui/checkbox.h>
#include <nanogui/opengl.h>
#include <nanogui/theme.h>
#include <nanogui/entypo.h>

NANOGUI_NAMESPACE_BEGIN

CheckBox::CheckBox(Widget *parent, const std::string &caption,
         const std::function<void(bool)> &callback)
    : Widget(parent), mCaption(caption), mPushed(false), mChecked(false),
      mCallback(callback) {}

bool CheckBox::mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) {
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
    nvgFontSize(ctx, mTheme->mStandardFontSize);
    nvgFontFace(ctx, "sans");
    return Vector2i(
        nvgTextBounds(ctx, 0,0, mCaption.c_str(), nullptr, nullptr) + 28,
        std::max(mTheme->mStandardFontSize, 20)
    );
}

void CheckBox::draw(NVGcontext *ctx) {
    Widget::draw(ctx);

    nvgFontSize(ctx, mTheme->mStandardFontSize);
    nvgFontFace(ctx, "sans");
    nvgFillColor(ctx, mEnabled ? mTheme->mTextColor : mTheme->mDisabledTextColor);
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgText(ctx, mPos.x() + 28, mPos.y() + mSize.y() * 0.5f, mCaption.c_str(), nullptr);

    NVGpaint bg = nvgBoxGradient(ctx,
        mPos.x(),
        mPos.y() + mSize.y() * 0.5 - 8,
        18, 18, 3, 3,
        mPushed ? Color(0, 100) : Color(0, 32), Color(0,0,0,180));

    nvgBeginPath(ctx);
    nvgRoundedRect(ctx,
        mPos.x(),
        mPos.y() + mSize.y() * 0.5 - 9, 18, 18, 3);
    nvgFillPaint(ctx, bg);
    nvgFill(ctx);

    if (mChecked) {
        nvgFontSize(ctx, 40);
        nvgFontFace(ctx, "icons");
        nvgFillColor(ctx, mEnabled ? mTheme->mIconColor : mTheme->mDisabledTextColor);
        nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
        nvgText(ctx, mPos.x() + 9 + 1, mPos.y() + mSize.y() * 0.5f,
                utf8(ENTYPO_ICON_CHECK).data(), nullptr);
    }
}

NANOGUI_NAMESPACE_END
