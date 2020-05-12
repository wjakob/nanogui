/*
    nanogui/spinner.h -- Load spinner with an arbitrary color, radius and size

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/widget.h>
#include <nanogui/separator.h>

NAMESPACE_BEGIN(nanogui)

/**
 * \class Spinner spinner.h nanogui/spinner.h
 *
 * \brief Load spinner widget.
 *
 * The font and color can be customized. When \ref Widget::setFixedWidth()
 * is used, the text is wrapped when it surpasses the specified width.
 */

DECLSETTER(SpinnerSpeed,float)
DECLSETTER(SpinnerRadius,float)
DECLSETTER(BackgroundW, float)

class NANOGUI_EXPORT Spinner : public Widget {
public:
  RTTI_CLASS_UID(Spinner)
  RTTI_DECLARE_INFO(Spinner)

  Spinner(Widget* parent);
  void draw(NVGcontext* ctx) override;
   /// Compute the size needed to fully display the label
  Vector2i preferredSize(NVGcontext *ctx) const override;

  void setSpeed(float speed) { mSpeed = speed; }
  void setBackgroundColor(const Color& top, const Color& bottom) {
     mBackgroundTop = top;
     mBackgroundBottom = bottom;
     mBackgroundW = 1.f;
  }

  void setBackgroundColor(const Color& c) { setBackgroundColor(c, c); }
  void setBackgroundW(float w) { mBackgroundW = w; }
  void setRadius(float r) { mRadius = r; }

  using Widget::set;

  template<typename... Args>
  Spinner(Widget* parent, const Args&... args)
     : Spinner(parent) { set<Spinner, Args...>(args...); }
private:
  float mSpeed = 1.f;
  float mRadius = 0.75f;
  float mBackgroundW = 0.3f;
  Color mBackgroundTop, mBackgroundBottom;

public:
  PROPSETTER(SpinnerSpeed,setSpeed)
  PROPSETTER(SpinnerRadius,setRadius)
  PROPSETTER(BackgroundW, setBackgroundW)
  PROPSETTER(BackgroundColor,setBackgroundColor)
  PROPSETTER(RemoveAfterSec, addChild)
};

namespace elm { using Spinner = Element<Spinner>; }

NAMESPACE_END(nanogui)
