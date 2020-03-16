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
#include <type_traits>

NAMESPACE_BEGIN(nanogui)

using FloatPair = std::pair<float, float>;
DECLSETTERARGS(SliderObservable, FloatObservable)
DECLSETTERILIST(SliderRange, FloatPair)
DECLSETTER(SliderCallback, std::function<void(float)>)


/**
 * \class Slider slider.h nanogui/slider.h
 *
 * \brief Fractional slider widget with mouse control.
 */
class NANOGUI_EXPORT Slider : public Widget {
public:
    RTTI_CLASS_UID(Slider)
    RTTI_DECLARE_INFO(Slider)

    explicit Slider(Widget *parent);

    using Widget::set;
    template<typename... Args>
    Slider(Widget* parent, const Args&... args)
      : Slider(parent) { set<Slider, Args...>(args...); }

    float value() const { return mValue; }
    void setValue(float value) { mValue = value; }

    FloatObservable observable() { return mValue; }
    void setObservable(FloatObservable value) { mValue = value; }

    const Color &highlightColor() const { return mHighlightColor; }
    void setHighlightColor(const Color &highlightColor) { mHighlightColor = highlightColor; }

    std::pair<float, float> range() const { return mRange; }
    void setRange(std::pair<float, float> range) { mRange = range; }

    void setValueColorVisible(bool v) { mShowValueWithColor = v; }
    void setValueColor(const Color& c) { mValueColor = c; }

    bool tabstop(CanTabStop) const override { return true; }

    std::pair<float, float> highlightedRange() const { return mHighlightedRange; }
    void setHighlightedRange(std::pair<float, float> highlightedRange) { mHighlightedRange = highlightedRange; }

    std::function<void(float)> callback() const { return mCallback; }
    void setCallback(const std::function<void(float)> &callback) { mCallback = callback; }

    std::function<void(float)> finalCallback() const { return mFinalCallback; }
    void setFinalCallback(const std::function<void(float)> &callback) { mFinalCallback = callback; }

    Vector2i preferredSize(NVGcontext *ctx) const override;
    bool mouseDragEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) override;
    bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override;
    void draw(NVGcontext* ctx) override;
    void save(Json::value &save) const override;
    bool load(Json::value &save) override;

protected:
    FloatObservable mValue;
    std::function<void(float)> mCallback;
    std::function<void(float)> mFinalCallback;
    std::pair<float, float> mRange;
    std::pair<float, float> mHighlightedRange;
    Color mHighlightColor;
    Color mValueColor;
    bool mShowValueWithColor = true;

public:
  PROPSETTER(InitialValue, setValue)
  PROPSETTER(SliderObservable, setObservable)
  PROPSETTER(SliderRange, setRange)
  PROPSETTER(SliderCallback, setCallback)
};

NAMESPACE_END(nanogui)
