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
class ToolButton : public Button {
public:
    ToolButton(Widget *parent, int icon,
           const std::string &caption = "")
        : Button(parent, caption, icon) {
        setFlags(Flags::RadioButton | Flags::ToggleButton);
        setFixedSize(Vector2i(25, 25));
    }
};

NAMESPACE_END(nanogui)
