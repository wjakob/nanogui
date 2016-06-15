/*
    nanogui/stacked.cpp -- Widget used to stack widgets on top
    of each other. Only the active widget is visible.

    The stacked widget was contributed by Stefan Ivanov.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/stacked.h>

NAMESPACE_BEGIN(nanogui)

StackedWidget::StackedWidget(nanogui::Widget * parent)
    : Widget(parent)
{}

void StackedWidget::setActiveTab(unsigned int tabIndex) {
    assert(tabIndex < mChildren.size());
    mChildren[mActiveTab]->setVisible(false);
    mActiveTab = tabIndex;
    mChildren[mActiveTab]->setVisible(true);
}

unsigned int StackedWidget::activeTab() const {
    return mActiveTab;
}

void StackedWidget::performLayout(NVGcontext* ctx) {
    for (auto child : mChildren) {
        child->setPosition(Vector2i::Zero());
        child->setSize(mSize);
    }
    Widget::performLayout(ctx);
}

Vector2i StackedWidget::preferredSize(NVGcontext * ctx) const {
    Vector2i size = Vector2i::Zero();
    // Retrieve the maximum possible width and height among the widgets.
    for (auto child : mChildren)
        size = size.cwiseMax(child->preferredSize(ctx));
    return size + Vector2i(2 * borderSize, headerSize + 2 * borderSize);

}

void StackedWidget::addChild(Widget * widget) {
    Widget::addChild(widget);
    widget->setVisible(false);
}


NAMESPACE_END(nanogui)