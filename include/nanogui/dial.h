/*
    nanogui/dial.h -- Fractional dial widget with mouse control

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

DECLSETTER(DialCallback, std::function<void(float)>)
DECLSETTER(DialFinalCallback, std::function<void(float)>)
/**
 * \class Dial dial.h nanogui/dial.h
 *
 * \brief Fractional dial widget with mouse control.
 */
class NANOGUI_EXPORT Dial : public Widget {
public:
    RTTI_CLASS_UID(Dial)
    RTTI_DECLARE_INFO(Dial)
    WIDGET_COMMON_FUNCTIONS(Dial)

    explicit Dial(Widget *parent);

    using Widget::set;
    template<typename... Args>
    Dial(Widget* parent, const Args&... args)
      : Dial(parent) { set<Dial, Args...>(args...); }

    float value() const { return mValue; }
    void setValue(float value) { mValue = value; }

    const Color &highlightColor() const { return mHighlightColor; }
    void setHighlightColor(const Color &highlightColor) { mHighlightColor = highlightColor; }

    std::pair<float, float> range() const { return mRange; }
    void setRange(std::pair<float, float> range) { mRange = range; }

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
    void save(Json::value &s) const override;
    bool load(Json::value &s) override;

protected:
    float mValue;
    std::function<void(float)> mCallback;
    std::function<void(float)> mFinalCallback;
    std::pair<float, float> mRange;
    std::pair<float, float> mHighlightedRange;
    Color mHighlightColor;
public:
  PROPSETTER(FloatValue, setValue)
  PROPSETTER(DialCallback, setCallback)
  PROPSETTER(DialFinalCallback, setFinalCallback)
};

namespace elm { using Dial = Element<Dial>; }

NAMESPACE_END(nanogui)
