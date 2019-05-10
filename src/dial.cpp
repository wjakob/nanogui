/*
    nanogui/dial.cpp -- Fractional dial widget with mouse control

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/dial.h>
#include <nanogui/theme.h>
#include <nanogui/opengl.h>
#include <nanogui/serializer/core.h>
#include <Eigen/Geometry>

using Eigen::Rotation2Df;

NAMESPACE_BEGIN(nanogui)

Dial::Dial(Widget *parent)
    : Widget(parent), mValue(0.0f), mRange(0.f, 1.f),
      mHighlightedRange(0.f, 0.f) {
    mHighlightColor = Color(255, 80, 80, 70);
}

Vector2i Dial::preferredSize(NVGcontext *) const {
    return Vector2i(40, 40);
}

bool Dial::mouseDragEvent(const Vector2i &p, const Vector2i & /* rel */,
                            int /* button */, int /* modifiers */) {
    if (!mEnabled)
        return false;

    Vector2f pos = (p - mPos - mSize/2).cast<float>();
    float value = 0.5f + 0.5f*atan2f(pos.x(), -pos.y())/NVG_PI;
    value = -0.1f + 1.2f*value;

    value = value * (mRange.second - mRange.first) + mRange.first;
    mValue = std::min(std::max(value, mRange.first), mRange.second);
    if (mCallback)
        mCallback(mValue);

    return true;
}

bool Dial::mouseButtonEvent(const Vector2i &p, int /* button */, bool down, int /* modifiers */) {
    if (!mEnabled)
        return false;

    if (down) {
        Vector2f pos = (p - mPos - mSize/2).cast<float>();
        float kr = 0.5f * (mSize.y() * 0.4f);

        if (pos.squaredNorm() >= kr*kr) {
            float value = 0.5f + 0.5f*atan2f(pos.x(), -pos.y())/NVG_PI;
            value = -0.1f + 1.2f*value;

            value = value * (mRange.second - mRange.first) + mRange.first;
            mValue = std::min(std::max(value, mRange.first), mRange.second);
        }
        if (mCallback)
            mCallback(mValue);
    } else if (mFinalCallback) {
        mFinalCallback(mValue);
    }

    return true;
}

void Dial::draw(NVGcontext* ctx) {
    Vector2f center = mPos.cast<float>() + mSize.cast<float>() * 0.5f;
    float kr = (int) (mSize.y() * 0.4f), kshadow = 2;

    Vector2f dialPos(center.x(), center.y() + 0.5f);

    NVGpaint dial = nvgLinearGradient(ctx,
        mPos.x(), center.y() - kr, mPos.x(), center.y() + kr,
        mTheme->mBorderLight, mTheme->mBorderMedium);
    NVGpaint dialReverse = nvgLinearGradient(ctx,
        mPos.x(), center.y() - kr, mPos.x(), center.y() + kr,
        mTheme->mBorderMedium,
        mTheme->mBorderLight);

    NVGpaint dialFace = nvgRadialGradient(ctx,
        dialPos.x(), dialPos.y(), kr - kshadow,
        kr + kshadow, Color(150, mEnabled ? 255 : 100), mTheme->mTransparent);

    if (mHighlightedRange.second != mHighlightedRange.first) {
        float a0 = 0.5f*NVG_PI + 2.0f*NVG_PI*(0.1f + 0.8f * mHighlightedRange.first);
        float a1 = 0.5f*NVG_PI + 2.0f*NVG_PI*(0.1f + 0.8f * mHighlightedRange.second);

        nvgBeginPath(ctx);
        nvgArc(ctx, dialPos.x(), dialPos.y(), kr, a0, a1, NVG_CW);
        nvgArc(ctx, dialPos.x(), dialPos.y(), kr + 2*kshadow, a1, a0, NVG_CCW);
        nvgFillColor(ctx, mHighlightColor);
        nvgFill(ctx);
    }

    nvgBeginPath(ctx);
    nvgCircle(ctx, dialPos.x(), dialPos.y(), kr);
    nvgStrokeColor(ctx, mTheme->mBorderDark);
    nvgFillPaint(ctx, dial);
    nvgStroke(ctx);
    nvgFill(ctx);
    nvgBeginPath(ctx);
    nvgCircle(ctx, dialPos.x(), dialPos.y(), kr - kshadow);
    nvgFillPaint(ctx, dialFace);
    nvgStrokePaint(ctx, dialReverse);
    nvgStroke(ctx);
    nvgFill(ctx);

    Vector2f notchPos(0.0f, 0.8f*(kr - 1.5f*kshadow));
    float value = (mValue - mRange.first)/(mRange.second - mRange.first);
    float theta = 2.0f*NVG_PI*(0.1f + 0.8f*value);
    Rotation2Df t(theta);
    notchPos = t*notchPos;
    notchPos += dialPos;

    nvgBeginPath(ctx);
    nvgCircle(ctx, notchPos.x(), notchPos.y(), 0.15f*kr);
    nvgFillColor(ctx, Color(mEnabled ? 50 : 100, 150));
    nvgStrokePaint(ctx, dial);
    nvgStroke(ctx);
    nvgFill(ctx);
}

void Dial::save(Serializer &s) const {
    Widget::save(s);
    s.set("value", mValue);
    s.set("range", mRange);
}

bool Dial::load(Serializer &s) {
    if (!Widget::load(s)) return false;
    if (!s.get("value", mValue)) return false;
    if (!s.get("range", mRange)) return false;
    return true;
}

NAMESPACE_END(nanogui)
