/*
    src/tolerancebar.cpp -- ToleranceBar box widget

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/tolerancebar.h>

#include <nanogui/serializer/core.h>
#include <nanogui/theme.h>
#include <nanovg.h>


NAMESPACE_BEGIN(nanogui)

ToleranceBar::ToleranceBar(Widget* parent, const std::function<void(int)>& callback)
    : Widget(parent)
    , mCallback(callback)
{
    mIconExtraScale = 1.2f; // widget override
}

bool ToleranceBar::mouseMotionEvent(const Vector2i& p, const Vector2i& /* rel */, int button, int /* modifiers */)
{
    if (button != 0)
        mValue = valueForPosition(p);

    return true;
}

bool ToleranceBar::mouseButtonEvent(const Vector2i& p, int /* button */, bool /* down */, int /* modifiers */)
{
    mValue = valueForPosition(p);

    if (mCallback)
        mCallback(mValue);

    return true;
}

bool ToleranceBar::scrollEvent(const Vector2i& /* p */, const Vector2f& rel)
{
    mValue = rel.y() < 0 ? std::max(mValue - 1, 0) : std::min(mValue + 1, 100);

    if (mCallback)
        mCallback(mValue);

    return true;
}

void ToleranceBar::drawFrame(NVGcontext* ctx, float x, float y, float w, float h)
{
    nvgFillColor(ctx,  mTheme->mToleranceBarBgColor);
    nvgStrokeColor(ctx, mTheme->mToleranceBarBorderColor);
    nvgStrokeWidth(ctx, 1);

    nvgBeginPath(ctx);
    nvgMoveTo(ctx, x, y + h);
    nvgLineTo(ctx, x + w, y);
    nvgLineTo(ctx, x + w, y + h);
    nvgClosePath(ctx);

    nvgFill(ctx);
    nvgStroke(ctx);
}

void ToleranceBar::drawContent(NVGcontext* ctx, float x, float y, float w, float h)
{
    const NVGpaint bg
        = nvgLinearGradient(ctx, x + w / 4, y, x + w, y, mTheme->mToleranceBarLowColor, mTheme->mToleranceBarHighColor);

    nvgBeginPath(ctx);
    nvgMoveTo(ctx, x, y + h);
    nvgLineTo(ctx, x + w / 100. * mValue, y + h - (h / 100. * mValue));
    nvgLineTo(ctx, x + w / 100. * mValue, y + h);
    nvgClosePath(ctx);
    nvgFillPaint(ctx, bg);
    nvgFill(ctx);
}

void ToleranceBar::draw(NVGcontext* ctx)
{
    const float startX = mPos.x();
    const float startY = mPos.y();

    const float height = mSize.y() - 1;
    const float width  = mSize.x() - 1;

    drawFrame(ctx, startX, startY, width, height);
    drawContent(ctx, startX, startY, width, height);

    Widget::draw(ctx);
}

int ToleranceBar::valueForPosition(const Vector2i& pos)
{
    return (pos - mPos).x() * 100 / mSize.x();
}

NAMESPACE_END(nanogui)
