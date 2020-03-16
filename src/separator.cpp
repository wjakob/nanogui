/*
    src/label.cpp -- Text label with an arbitrary font, color, and size

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    Updated by dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/separator.h>
#include <nanogui/theme.h>
#include <nanovg.h>
#include <nanogui/saveload.h>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(Line, Widget)
RTTI_IMPLEMENT_INFO(UpdateHandler, Widget)
RTTI_IMPLEMENT_INFO(RemoveTimer, Widget)

Line::Line(Widget *parent)
    : Widget(parent) {
  mDrawFlags = Horizontal | CenterV | CenterH;
}

Vector2i Line::preferredSize(NVGcontext *) const
{
  //Vector2i ps = parent() ? parent()->size() : Vector2i{ 1, 1 };
  if (haveDrawFlag(Line::Horizontal))
    return{ 0, mLineWidth >= 0 ? mLineWidth : theme()->separatorWidth };
  else
    return{ mLineWidth >= 0 ? mLineWidth : theme()->separatorWidth, 0 };
}

void Line::performLayout(NVGcontext *ctx)
{
  Vector2i psize = parent() ? parent()->size() : Vector2i{ 1, 1 };
  Vector2i pref = preferredSize(ctx);
  
  if (haveDrawFlag(Line::Horizontal))
    pref.x() = mRelSize.x() > 0 ? mRelSize.x() * psize.x() : psize.x();
  else
    pref.y() = mRelSize.y() > 0 ? mRelSize.y() * psize.y() : psize.y();

  if (haveDrawFlag(Line::Left))
    mPos.x() = 0;
  else if (haveDrawFlag(Line::Rigth))
    mPos.x() = (psize - pref).x();
  else if (haveDrawFlag(Line::CenterH))
    mPos.x() = (psize - pref).x() / 2;

  if (haveDrawFlag(Line::Top))
    mPos.y() = 0;
  else if (haveDrawFlag(Line::Bottom))
    mPos.y() = (psize - pref).y();
  else if (haveDrawFlag(Line::CenterV))
    mPos.y() = (psize - pref).y() / 2;

  if (isSubElement())
    setSize(pref);

  Widget::performLayout(ctx);
}

void Line::draw(NVGcontext* ctx)
{
  nvgBeginPath(ctx);
  nvgStrokeWidth(ctx, (mLineWidth >= 0 ? mLineWidth : theme()->separatorWidth));
  nvgStrokeColor(ctx, mBackgroundColor.notW(theme()->separatorColor));
  if (haveDrawFlag(Line::Vertical)) {
    auto center = mPos + mSize._x0() / 2;
    nvgLine(ctx, center, center + mSize._0y());
  }
  else
  {
    auto center = mPos + mSize._0y() / 2;
    nvgLine(ctx, center, center + mSize._x0());
  }
  nvgStroke(ctx);

  Widget::draw(ctx);
}

NAMESPACE_END(nanogui)