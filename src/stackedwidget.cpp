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

StackedWidget::StackedWidget(nanogui::Widget * parent)
    : Widget(parent)
{}

void StackedWidget::setSelectedIndex(int tabIndex) {
    assert(tabIndex < childCount());
    mChildren[mSelectedIndex]->setVisible(false);
    mSelectedIndex = tabIndex;
    mChildren[mSelectedIndex]->setVisible(true);
}

int StackedWidget::selectedIndex() const {
    return mSelectedIndex;
}

void StackedWidget::appendChild(Widget * widget) {
    mChildren.push_back(widget);
    widget->incRef();
    widget->setParent(this);
    widget->setTheme(mTheme);
    widget->setVisible(false);
    setSelectedIndex(childCount() - 1);
}

void StackedWidget::prependChild(Widget* widget) {
    mChildren.insert(mChildren.begin(), widget);
    widget->incRef();
    widget->setParent(this);
    widget->setTheme(mTheme);
    widget->setVisible(false);
    setSelectedIndex(0);
}

void StackedWidget::insertChild(int index, Widget* widget) {
    assert(index < childCount());
    widget->incRef();
    widget->setParent(this);
    widget->setTheme(mTheme);
    widget->setVisible(false);
    mChildren.insert(std::next(mChildren.begin(), index), widget);
    setSelectedIndex(index);
}

void StackedWidget::removeChild(int index) {
    assert(index < childCount());
    // Decrease reference count.
    auto forRemoval = mChildren[index];
    if (forRemoval)
        forRemoval->decRef();
    mChildren.erase(std::next(mChildren.begin(), index));
}

const Widget* StackedWidget::childAt(int index) const {
    assert(index < childCount());
    return mChildren[index];
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
    for (auto child : mChildren)
        size = size.cwiseMax(child->preferredSize(ctx));
    return size;

}

void StackedWidget::addChild(Widget * widget) {
    Widget::addChild(widget);
    widget->setVisible(false);
    // TODO: Remove this call so as not to mess with pop up buttons.
    setSelectedIndex(childCount()-1);
}


NAMESPACE_END(nanogui)