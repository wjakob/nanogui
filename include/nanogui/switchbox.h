/*
    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/**
 * \file nanogui/switchbox.h
 *
 * \brief Two-state SwitchBox box Widget.
 */

#pragma once

#include <nanogui/checkbox.h>

NAMESPACE_BEGIN(nanogui)

/**
 * \class SwitchBox checkbox.h nanogui/checkbox.h
 *
 * \brief Two-state SwitchBox box widget.
 *
 * \remark
 *     This class overrides \ref nanogui::Widget::mIconExtraScale to be ``1.2f``,
 *     which affects all subclasses of this Widget.  Subclasses must explicitly
 *     set a different value if needed (e.g., in their constructor).
 */
DECLSETTER(SwitchboxAlign, int)

class NANOGUI_EXPORT SwitchBox : public CheckBox 
{
public:
  RTTI_CLASS_UID(SwitchBox)
  RTTI_DECLARE_INFO(SwitchBox)

  enum class Alignment {
    Horizontal,
    Vertical
  };
  /**
    * Adds a SwitchBox to the specified ``parent``.
    *
    * \param parent
    *     The Widget to add this SwitchBox to.
    *
    * \param caption
    *     The caption text of the SwitchBox (default ``"Untitled"``).
    *
    * \param callback
    *     If provided, the callback to execute when the SwitchBox is checked or
    *     unchecked.  Default parameter function does nothing.  See
    *     \ref nanogui::SwitchBox::mPushed for the difference between "pushed"
    *     and "checked".
    */
  explicit SwitchBox(Widget *parent);

  using CheckBox::set;
  template<typename... Args>
  SwitchBox(Widget* parent, const Args&... args)
    : SwitchBox(parent) { set<SwitchBox, Args...>(args...); }

  /// The preferred size of this SwitchBox.
  Vector2i preferredSize(NVGcontext *ctx) const override;

  /// Draws this SwitchBox.
  void draw(NVGcontext *ctx) override;

  void setAlignment(int align) { mAlign = (Alignment)align; }
  void setBackgroundColor(const Color& c) { mBackgroundColor = c; }

protected:
  Alignment mAlign = Alignment::Horizontal;
  float path = 0.f;

  Color mBackgroundColor;

public:
  PROPSETTER(SwitchboxAlign, setAlignment)
  PROPSETTER(BackgroundColor, setBackgroundColor)
};

namespace elm { using Switchbox = Element<SwitchBox>; }

NAMESPACE_END(nanogui)
