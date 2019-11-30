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


namespace {
const int MIN_VALUE = 0;
const int MAX_VALUE = 100;
} // namespace


NAMESPACE_BEGIN(nanogui)

ToleranceBar::ToleranceBar(Widget* parent, const std::function<void(int)>& callback)
    : Widget(parent)
    , mCallback(callback)
{
}

bool ToleranceBar::mouseMotionEvent(const Vector2i& p, const Vector2i& /* rel */, int /* button */, int /* modifiers */)
{
    if (mBtnPressed)
        updateValue(valueForPosition(p));

    return true;
}

bool ToleranceBar::mouseButtonEvent(const Vector2i& p, int /* button */, bool down, int /* modifiers */)
{
    if (inFigure(p))
    {
        mBtnPressed = down;
        updateValue(valueForPosition(p));
    }
    else if (!down)
        mBtnPressed = false;

    return true;
}

bool ToleranceBar::scrollEvent(const Vector2i& p, const Vector2f& rel)
{
    if (inFigure(p))
    {
        const int step = rel.y() < 0 ? -1 : 1;
        updateValue(mValue + step);
    }

    return true;
}

void ToleranceBar::drawFrame(NVGcontext* ctx, float x, float y, float w, float h)
{
    nvgFillColor(ctx, mTheme->mToleranceBarBgColor);
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
    nvgLineTo(ctx, x + w / MAX_VALUE * mValue, y + h - (h / MAX_VALUE * mValue));
    nvgLineTo(ctx, x + w / MAX_VALUE * mValue, y + h);
    nvgClosePath(ctx);
    nvgFillPaint(ctx, bg);
    nvgFill(ctx);
}

void ToleranceBar::draw(NVGcontext* ctx)
{
    const float x = mPos.x();
    const float y = mPos.y();

    const float height = mSize.y() - 1;
    const float width = mSize.x() - 1;

    drawFrame(ctx, x, y, width, height);
    drawContent(ctx, x + 1, y + 1, width - 2, height - 2);

    Widget::draw(ctx);
}

int ToleranceBar::valueForPosition(const Vector2i& pos) const
{
    const int value = (pos - mPos).x() * MAX_VALUE / mSize.x();

    return clamp(value, MIN_VALUE, MAX_VALUE);
}

bool ToleranceBar::inFigure(const Vector2i& pos) const
{
    const float scale = static_cast<float>(mSize.y()) / mSize.x();
    const auto widgetPos = pos - mPos;

    return widgetPos.y() > mSize.y() - scale * widgetPos.x();
}

void ToleranceBar::updateValue(int value)
{
    mValue = clamp(value, MIN_VALUE, MAX_VALUE);

    if (mCallback)
        mCallback(mValue);
}

NAMESPACE_END(nanogui)
