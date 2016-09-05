/*
    nanogui/slider.h -- Fractional slider widget with mouse control

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/widget.h>

NAMESPACE_BEGIN(nanogui)

/**
 * \class Slider slider.h nanogui/slider.h
 *
 * \brief Fractional slider widget with mouse control.
 */
class NANOGUI_EXPORT Slider : public Widget {
public:
    Slider(Widget *parent);

    float value() const { return mValue; }
    void setValue(float value) { mValue = value; }

    const Color &highlightColor() const { return mHighlightColor; }
    void setHighlightColor(const Color &highlightColor) { mHighlightColor = highlightColor; }

    std::pair<float, float> highlightedRange() const { return mHighlightedRange; }
    void setHighlightedRange(std::pair<float, float> highlightedRange) { mHighlightedRange = highlightedRange; }

    std::function<void(float)> callback() const { return mCallback; }
    void setCallback(const std::function<void(float)> &callback) { mCallback = callback; }

    std::function<void(float)> finalCallback() const { return mFinalCallback; }
    void setFinalCallback(const std::function<void(float)> &callback) { mFinalCallback = callback; }

    virtual Vector2i preferredSize(NVGcontext *ctx) const override;
    virtual bool mouseDragEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) override;
    virtual bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override;
    virtual void draw(NVGcontext* ctx) override;
    virtual void save(Serializer &s) const override;
    virtual bool load(Serializer &s) override;

protected:
    float mValue;
    std::function<void(float)> mCallback;
    std::function<void(float)> mFinalCallback;
    std::pair<float, float> mHighlightedRange;
    Color mHighlightColor;
};

NAMESPACE_END(nanogui)
