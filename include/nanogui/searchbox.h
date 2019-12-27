/*
    nanogui/searchbox.h -- Fancy text box with builtin regular
    expression-based validation

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    Searchbox was developed by dalerank <dalerankn8@gmail.com>
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/textbox.h>
#include <cstdio>
#include <sstream>
#include <limits>

NAMESPACE_BEGIN(nanogui)

class NANOGUI_EXPORT SearchBox : public TextBox
{
public:
  RTTI_CLASS_UID("SRCH")
  RTTI_DECLARE_INFO(SearchBox)

  explicit SearchBox(Widget* parent);

  void draw(NVGcontext* ctx) override;

  int getCornerRadius() const override;
  void performLayout(NVGcontext* ctx) override;

  void clearText();
};

NAMESPACE_END(nanogui)