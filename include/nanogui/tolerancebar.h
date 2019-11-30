/*
    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

/**
 * \file nanogui/tolerancebar.h
 *
 * \brief ToleranceBar box Widget.
 */

#pragma once

#include <nanogui/widget.h>


NAMESPACE_BEGIN(nanogui)

/**
 * \class ToleranceBar tolerancebar.h nanogui/tolerancebar.h
 *
 * \brief ToleranceBar box widget.
 */
class NANOGUI_EXPORT ToleranceBar : public Widget
{
public:
    /**
     * Adds a ToleranceBar to the specified ``parent``.
     *
     * \param parent
     *     The Widget to add this ToleranceBar to.
     *
     * \param callback
     *     If provided, the callback to execute when the ToleranceBar is changed.
     *     Default parameter function does nothing.
     */
    explicit ToleranceBar(Widget* parent, const std::function<void(int)>& callback = std::function<void(bool)>());

    bool mouseMotionEvent(const Vector2i& p, const Vector2i& rel, int button, int modifiers) override;
    bool mouseButtonEvent(const Vector2i& p, int button, bool down, int modifiers) override;
    bool scrollEvent(const Vector2i& p, const Vector2f& rel) override;

    void draw(NVGcontext* ctx) override;

    void setBackgroundColor(const Color& c)
    {
        mBackgroundColor = c;
    }

protected:
    int mValue = 0;
    bool mBtnPressed = false;

    float path = 0.f;
    Color mBackgroundColor;
    std::function<void(bool)> mCallback;

private:
    void drawFrame(NVGcontext* ctx, float x, float y, float w, float h);
    void drawContent(NVGcontext* ctx, float x, float y, float w, float h);

    int valueForPosition(const Vector2i& pos) const;
    bool inFigure(const Vector2i& pos) const;

    void updateValue(int value);

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

NAMESPACE_END(nanogui)
