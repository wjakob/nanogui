/*
    nanogui/tabheaderwidget.h -- Widget used to control tabs

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/tabheaderwidget.h>

NAMESPACE_BEGIN(nanogui)


TabHeaderWidget::TabHeaderWidget(nanogui::Widget * parent, StackedWidget * stackedWidget) 
    : Widget(parent), mStackedWidget(stackedWidget) 
{}

void TabHeaderWidget::setActiveTab(unsigned int /*tabIndex*/)
{

}

unsigned int TabHeaderWidget::activeTab() const
{
    return {};
}

void TabHeaderWidget::performLayout(NVGcontext *)
{

}

Vector2i TabHeaderWidget::preferredSize(NVGcontext * /*ctx*/) const
{
    return {};
}

NAMESPACE_END(nanogui)