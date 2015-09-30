#include <nanogui/slider.h>
#include <nanogui/theme.h>
#include <nanogui/opengl.h>

NANOGUI_NAMESPACE_BEGIN

Slider::Slider(Widget *parent)
    : Widget(parent), mValue(0.0f), mUnsafeRange(std::make_pair(0.f, 0.f)) {}

Vector2i Slider::preferredSize(NVGcontext *) const {
    return Vector2i(70, 12);
}

bool Slider::mouseDragEvent(const Vector2i &p, const Vector2i & /* rel */,
                            int /* button */, int /* modifiers */) {
    if (!mEnabled)
        return false;
    mValue = std::min(std::max((p.x() - mPos.x()) / (float) mSize.x(), (float) 0.0f), (float) 1.0f);
    if (mCallback)
        mCallback(mValue);
    return true;
}

bool Slider::mouseButtonEvent(const Vector2i &p, int /* button */, bool down, int /* modifiers */) {
    if (!mEnabled)
        return false;
    mValue = std::min(std::max((p.x() - mPos.x()) / (float) mSize.x(), (float) 0.0f), (float) 1.0f);
    if (mCallback)
        mCallback(mValue);
    if (mFinalCallback && !down)
        mFinalCallback(mValue);
    return true;
}

void Slider::draw(NVGcontext* ctx) {
    Vector2f center = mPos.cast<float>() + mSize.cast<float>() * 0.5f;
    Vector2f knobPos(mPos.x() + mValue * mSize.x(), center.y() + 0.5f);
    float kr = (int)(mSize.y()*0.5f);
    NVGpaint bg = nvgBoxGradient(ctx,
        mPos.x(), center.y() - 3 + 1, mSize.x(), 6, 3, 3, Color(0, mEnabled ? 32 : 10), Color(0, mEnabled ? 128 : 210));

    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x(), center.y() - 3 + 1, mSize.x(), 6, 2);
    nvgFillPaint(ctx, bg);
    nvgFill(ctx);

    if (mUnsafeRange.second != mUnsafeRange.first) {
        nvgBeginPath(ctx);
        nvgRoundedRect(ctx, mPos.x() + mUnsafeRange.first * mSize.x(), center.y() - 3 + 1, mSize.x() * (mUnsafeRange.second-mUnsafeRange.first), 6, 2);
        nvgFillColor(ctx, nvgRGBA(255, 80, 80, 70));
        nvgFill(ctx);
    }

    NVGpaint knobShadow = nvgRadialGradient(ctx,
        knobPos.x(), knobPos.y(), kr-3, kr+3, Color(0, 64), mTheme->mTransparent);

    nvgBeginPath(ctx);
    nvgRect(ctx, knobPos.x() - kr - 5, knobPos.y() - kr - 5, kr*2+10, kr*2+10+3);
    nvgCircle(ctx, knobPos.x(), knobPos.y(), kr);
    nvgPathWinding(ctx, NVG_HOLE);
    nvgFillPaint(ctx, knobShadow);
    nvgFill(ctx);

    NVGpaint knob = nvgLinearGradient(ctx,
        mPos.x(), center.y() - kr, mPos.x(), center.y() + kr,
        mTheme->mBorderLight, mTheme->mBorderMedium);
    NVGpaint knobReverse = nvgLinearGradient(ctx,
        mPos.x(), center.y() - kr, mPos.x(), center.y() + kr,
        mTheme->mBorderMedium,
        mTheme->mBorderLight);

    nvgBeginPath(ctx);
    nvgCircle(ctx, knobPos.x(), knobPos.y(), kr);
    nvgStrokeColor(ctx, mTheme->mBorderDark);
    nvgFillPaint(ctx, knob);
    nvgStroke(ctx);
    nvgFill(ctx);
    nvgBeginPath(ctx);
    nvgCircle(ctx, knobPos.x(), knobPos.y(), kr/2);
    nvgFillColor(ctx, Color(150, mEnabled ? 255 : 100));
    nvgStrokePaint(ctx, knobReverse);
    nvgStroke(ctx);
    nvgFill(ctx);
}

NANOGUI_NAMESPACE_END
