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
#include <nanogui/serializer/json.h>
#include <nanogui/serializer/core.h>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(ToolButton, Button)
RTTI_IMPLEMENT_INFO(ToggleButton, Button)

Color ToggleButton::getIconColor() const
{
  return mPushed
          ? (mActiveColor.w() == 0
               ? theme()->mToggleButtonActiveColor
               : mActiveColor)
          : (mInactiveColor.w() == 0
               ? theme()->mToggleButtonInactiveColor
               : mInactiveColor);
}

ToggleButton::ToggleButton(Widget *parent, int icon)
  : Button(parent, std::string(""), icon)
{
  setFlags(Flag::ToggleButton);
  setDrawFlags(DrawFlag::DrawIcon);

  setFixedSize(25, 25);
}

void ToggleButton::draw(NVGcontext* ctx)
{
  Button::draw(ctx);
}

NAMESPACE_END(nanogui)
