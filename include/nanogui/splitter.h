/*
NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch> and
dalerank <dalerankn8@gmail.com> The widget drawing code is based 
on the NanoVG demo application by Mikko Mononen.

All rights reserved. Use of this source code is governed by a
BSD-style license that can be found in the LICENSE.txt file.
*/
/**
* \file nanogui/splitter.h
*
* \brief Defines the splitter widget
*/

#pragma once

#include <nanogui/widget.h>

NAMESPACE_BEGIN(nanogui)

DECLSETTER(SplitterDir, Orientation)

class NANOGUI_EXPORT Splitter : public Widget 
{
public:
  RTTI_CLASS_UID(Splitter)
  RTTI_DECLARE_INFO(Splitter)

  explicit Splitter(Widget* parent, Orientation dir);

  using Widget::set;
  template<typename... Args>
  Splitter(Widget* parent, const Args&... args)
    : Widget(parent)
  { set<Splitter, Args...>(args...); }

  void draw(NVGcontext* ctx) override;

  bool mouseDragEvent(const Vector2i &, const Vector2i &rel,
    int buttons, int /* modifiers */) override;

  /// Compute the size needed to fully display the label
  Vector2i preferredSize(NVGcontext *ctx) const override;
  void setDirection(Orientation dir) { mOrientation = dir; }

protected:
  Orientation mOrientation = Orientation::Vertical;

public:
  PROPSETTER(SplitterDir, setDirection)
};
namespace elm { using Splitter = Element<Splitter>; }

NAMESPACE_END(nanogui)