/*
    nanogui/toolbutton.h -- Simple radio+toggle button with an icon

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/button.h>

NAMESPACE_BEGIN(nanogui)

/**
 * \class ToolButton toolbutton.h nanogui/toolbutton.h
 *
 * \brief Simple radio+toggle button with an icon.
 */
class NANOGUI_EXPORT ToolButton : public Button {
public:
    RTTI_CLASS_UID("TLBT")
    RTTI_DECLARE_INFO(ToolButton)

    explicit ToolButton(Widget *parent, int icon, const std::string &caption = "")
        : Button(parent, caption, icon)
    {
        setFlags(Flag::RadioButton | Flag::ToggleButton);
        setFixedSize(Vector2i(25, 25));
    }

    std::string wtypename() const override { return "toolbutton"; }

    using Button::set;
    template<typename... Args>
    ToolButton(Widget* parent, const Args&... args)
      : ToolButton(parent, -1, std::string("")) { set<ToolButton, Args...>(args...); }
};

class NANOGUI_EXPORT ToggleButton : public Button {
public:
  RTTI_CLASS_UID("TGBT")
  RTTI_DECLARE_INFO(ToggleButton)

  explicit ToggleButton(Widget *parent, int icon);
  std::string wtypename() const override { return "togglebutton"; }

  using Button::set;
  template<typename... Args>
  ToggleButton(Widget* parent, const Args&... args)
    : ToggleButton(parent, -1) { set<ToggleButton, Args...>(args...); }

  Color getIconColor() const override;
  void draw(NVGcontext* ctx) override;

private:
  Color mActiveColor, mInactiveColor;
};

NAMESPACE_END(nanogui)
