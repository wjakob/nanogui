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

/* TODO Rename Stacked -> StackedWidget ;) */
class NANOGUI_EXPORT StackedWidget : public Widget {
public:
    StackedWidget(Widget* parent);

    void setSelectedIndex(int tabIndex);
    int selectedIndex() const;

    /// Inserts a child at the end of the child collection.
    void appendChild(Widget* widget);
    /// Inserts a child at the beginning of the child collection.
    void prependChild(Widget* widget);
    /// Inserts a child into the child collection at the specific index.
    void insertChild(int index, Widget* widget);
    /// Removes the child with the specified index in the children vector.
    void removeChild(int index);
    /// Retrieves the child at the specific position in the child array.
    const Widget* childAt(int index) const;
    
    
    virtual void performLayout(NVGcontext* ctx) override;
    virtual Vector2i preferredSize(NVGcontext* ctx) const override;
    virtual void addChild(Widget* widget) override;

private:
    int mSelectedIndex = 0;
};



NAMESPACE_END(nanogui)