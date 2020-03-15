/*
    nanogui/separator.h -- Text line with an arbitrary color, and size

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    Updated by Dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/widget.h>

NAMESPACE_BEGIN(nanogui)

/**
 * \class Line separator.h nanogui/separator.h
 *
 * \brief Basic line widget.
 *
 * The color and some optioons can be customized. When \ref Widget::setFixedWidth()
 * is used, the line is wrapped when it surpasses the specified width.
 */

DECLSETTER(LineWidth, int)

class NANOGUI_EXPORT UpdateHandler : public Widget 
{
  std::function<void(Widget*)> handler = nullptr;
public:
  RTTI_CLASS_UID(UpdateHandler)
  RTTI_DECLARE_INFO(UpdateHandler)

  UpdateHandler(const std::function<void (Widget*)>& f)
    : Widget(nullptr), handler(f)
  {}

  void afterDraw(NVGcontext*) override
  {
    if (!handler)
      return;
    handler(parent());
  }
};

class NANOGUI_EXPORT RemoveTimer : public Widget
{
public:
  RTTI_CLASS_UID(RemoveTimer)
  RTTI_DECLARE_INFO(RemoveTimer)

  float seconds2remove = 0;
  float startTimeSec = 0;

  RemoveTimer(float seconds)
    : Widget(nullptr), seconds2remove(seconds)
  {
    startTimeSec = getTimeFromStart();
  }

  void afterDraw(NVGcontext*) override
  {
    if (getTimeFromStart() - startTimeSec > seconds2remove)
      parent()->removeLater();
  }
};

class NANOGUI_EXPORT Line : public Widget 
{
public:
    RTTI_CLASS_UID(Line)
    RTTI_DECLARE_INFO(Line)

    enum DrawFlag {
      Horizontal = 1 << 0,
      Vertical = 1 << 1,
      Bottom = 1 << 2,
      Top = 1 << 3,
      Left = 1 << 4,
      Rigth = 1 << 5,
      CenterH = 1 << 6,
      CenterV = 1 << 7
    };

    explicit Line(Widget* parent);

    using Widget::set;
    template<typename... Args>
    Line(Widget* parent, const Args&... args)
       : Line(parent) { set<Line, Args...>(args...); }

    void setDrawFlags(int flags) { mDrawFlags = flags; }
    bool haveDrawFlag(int flag) const { return (mDrawFlags & flag) == flag; }

    void setBackgroundColor(const Color& color) { mBackgroundColor = color; }
    void setLineWidth(int w) { mLineWidth = w; }

    Vector2i preferredSize(NVGcontext *ctx) const override;
    void performLayout(NVGcontext *ctx) override;
    void draw(NVGcontext* ctx) override;

protected:
    Color mBackgroundColor;
    int mDrawFlags;
    int mLineWidth = -1;

public:
    PROPSETTER(DrawFlags, setDrawFlags)
    PROPSETTER(BackgroundColor, setBackgroundColor)
    PROPSETTER(LineWidth, setLineWidth)
};

NAMESPACE_END(nanogui)