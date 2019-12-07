/*
    nanogui/progressbar.h -- Standard widget for visualizing progress

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
 * \class ProgressBar progressbar.h nanogui/progressbar.h
 *
 * \brief Standard widget for visualizing progress.
 */

class NANOGUI_EXPORT ProgressBar : public Widget {
public:
    RTTI_CLASS_UID("PRGB")
    RTTI_DECLARE_INFO(ProgressBar)

    explicit ProgressBar(Widget *parent);

    using Widget::set;
    template<typename... Args>
    ProgressBar(Widget* parent, const Args&... args)
      : ProgressBar(parent) {  set<Widget, Args...>(args...); }

    float value() { return mValue; }
    void setValue(float value) { mValue = value; }

    Vector2i preferredSize(NVGcontext *ctx) const override;
    void draw(NVGcontext* ctx) override;

    virtual void save(Serializer &s) const override;
    virtual bool load(Serializer &s) override;
protected:
    float mValue;
public:
    PROPSETTER(FloatValue,setValue)
};

class NANOGUI_EXPORT CircleProgressBar : public ProgressBar {
public:
  CircleProgressBar(Widget *parent);

  using ProgressBar::set;
  template<typename... Args>
  CircleProgressBar(Widget* parent, const Args&... args)
    : CircleProgressBar(parent) { set<Widget, Args...>(args...); }

  Vector2i preferredSize(NVGcontext *ctx) const override;
  void draw(NVGcontext* ctx) override;

private:
  float mRadius = 0.9f;
  float mLineWidth = 0.75f;

  Color mColorStart = Color(0, 0xff, 0, 0x80);
  Color mColorEnd = Color(0, 0xff, 0, 0xff);
};

NAMESPACE_END(nanogui)
