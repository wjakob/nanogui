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

Line::Line(Widget *parent)
    : Widget(parent) {
  mDrawFlags = Horizontal | CenterV | CenterH;
}

Vector2i Line::preferredSize(NVGcontext *ctx) const
{
  int w = parent() ? parent()->width() : 1;
  return { static_cast<int>(mRelSize.x() > 0 ? mRelSize.x() * w : w),
           mLineWidth >= 0 ? mLineWidth : theme()->separatorWidth };
}

void Line::performLayout(NVGcontext *ctx)
{
  Vector2i pref = preferredSize(ctx);
  Vector2i psize = parent() ? parent()->size() : Vector2i{1, 1};

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

  Widget::performLayout(ctx);
}

void Line::draw(NVGcontext* ctx)
{
  nvgBeginPath(ctx);
  nvgStrokeWidth(ctx, (mLineWidth >= 0 ? mLineWidth : theme()->separatorWidth));
  nvgStrokeColor(ctx, mBackgroundColor.notW(theme()->separatorColor));
  nvgLine(ctx, mPos, mPos + mSize);
  nvgStroke(ctx);

  Widget::draw(ctx);
}

NAMESPACE_END(nanogui)