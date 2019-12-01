/*
    nanogui/stackedwidget.cpp -- Widget used to stack widgets on top
    of each other. Only the active widget is visible.

    The stacked widget was contributed by Stefan Ivanov.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/stackedwidget.h>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(StackedWidget, Widget)

StackedWidget::StackedWidget(nanogui::Widget *parent)
    : Widget(parent) { }

void StackedWidget::setSelectedIndex(int index) {
    assert(index < childCount());
    if (mSelectedIndex >= 0)
        mChildren[mSelectedIndex]->setVisible(false);
    mSelectedIndex = index;
    mChildren[mSelectedIndex]->setVisible(true);
}

int StackedWidget::selectedIndex() const {
    return mSelectedIndex;
}

void StackedWidget::performLayout(NVGcontext *ctx) {
    for (auto child : mChildren) {
        child->setPosition(Vector2i::Zero());
        child->setSize(mSize);
        child->performLayout(ctx);
    }
}

Vector2i StackedWidget::preferredSize(NVGcontext *ctx) const {
    Vector2i size = Vector2i::Zero();
    for (auto child : mChildren)
        size = size.cwiseMax(child->preferredSize(ctx));
    return size;
}

void StackedWidget::addChild(int index, Widget *widget) {
    if (mSelectedIndex >= 0)
        mChildren[mSelectedIndex]->setVisible(false);
    Widget::addChild(index, widget);
    widget->setVisible(true);
    setSelectedIndex(index);
}

NAMESPACE_END(nanogui)
