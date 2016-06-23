/*
    nanogui/stackedwidget.h -- Widget used to stack widgets on top
    of each other. Only the active widget is visible.

    The stacked widget was contributed by Stefan Ivanov.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#pragma once

#include <nanogui/widget.h>

NAMESPACE_BEGIN(nanogui)

class NANOGUI_EXPORT StackedWidget : public Widget {
public:
    StackedWidget(Widget* parent);

    void setSelectedIndex(int tabIndex);
    int selectedIndex() const;

    /// Retrieves the child at the specific position in the child array.
    const Widget* childAt(int index) const;
    
    
    virtual void performLayout(NVGcontext* ctx) override;
    virtual Vector2i preferredSize(NVGcontext* ctx) const override;
    virtual void addChild(int index, Widget* widget) override;

private:
    int mSelectedIndex = 0;
};



NAMESPACE_END(nanogui)