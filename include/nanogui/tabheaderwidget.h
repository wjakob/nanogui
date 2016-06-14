/*
    nanogui/tabheaderwidget.h -- Widget used to control tabs

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#pragma once

#include <nanogui/widget.h>

NAMESPACE_BEGIN(nanogui)

class NANOGUI_EXPORT TabHeaderWidget : public Widget {
public:

    TabHeaderWidget(nanogui::Widget* parent, StackedWidget* stackedWidget);

    void setActiveTab(unsigned int tabIndex);
    unsigned int activeTab() const;

    virtual void performLayout(NVGcontext* /* ctx */) override;
    virtual Vector2i preferredSize(NVGcontext* ctx) const override;

private:
    StackedWidget* mStackedWidget;
    unsigned int mActiveTab {};
};



NAMESPACE_END(nanogui)