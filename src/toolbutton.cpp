/*
    src/toolbutton.cpp -- [Normal/Toggle/Radio/Popup] Button widget

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    Toolbutoon.cpp file was developed by Dalerank <dalerankn8@gmail.com>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/toolbutton.h>
#include <nanovg.h>
#include <nanogui/common.h>
#include <nanogui/saveload.h>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(ToolButton, Button)
RTTI_IMPLEMENT_INFO(ToggleButton, Button)

Vector2i ToolButton::preferredSize(NVGcontext *ctx) const 
{
  int side = 0;
  if (mFontSize > 0) side = mFontSize * 1.2;
  else side = theme()->toolButtonSide;

  return{ side, side };
}

Color ToggleButton::getIconColor() const
{
  return mPushed
          ? mActiveColor.notW(theme()->mToggleButtonActiveColor)
          : mInactiveColor.notW(theme()->mToggleButtonInactiveColor);
}

Vector2i ToggleButton::preferredSize(NVGcontext *ctx) const 
{
  int side = 0;
  if (mFontSize > 0) side = mFontSize * 1.2;
  else side = theme()->toolButtonSide;

  return { side, side };
}

ToggleButton::ToggleButton(Widget *parent, int icon)
  : Button(parent, std::string(), icon)
{
  setFlags(Flag::ToggleButton);
  setDrawFlags(DrawFlag::DrawIcon);
}

void ToggleButton::draw(NVGcontext* ctx)
{
  Button::draw(ctx);
}

NAMESPACE_END(nanogui)
