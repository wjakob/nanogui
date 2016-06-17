/*
    nanogui/tab.h -- A wrapper around the widgets TabHeader and Stacked
    which hooks the two classes together.

    The tab widget was contributed by Stefan Ivanov.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/tab.h>
#include <nanogui/tabheader.h>
#include <nanogui/stacked.h>
#include <nanogui/opengl.h>
#include <algorithm>

NAMESPACE_BEGIN(nanogui)

Tab::Tab(Widget* parent)
    : Widget(parent), mHeader(new TabHeader(this)), mContent(new Stacked(this))
{
    mHeader->setCallback([this](int i) {mContent->setActiveTab(i); });
}

void Tab::setActiveTab(int tabIndex) {
    mHeader->setActiveTab(tabIndex);
    mContent->setActiveTab(tabIndex);
}

int Tab::activeTab() const {
    assert(mHeader->activeTab() == mContent->activeTab());
    return mContent->activeTab();
}

void Tab::addTab(Widget* tab, const std::string& name) {
    mContent->addChild(tab);
    tab->setParent(mContent);
    mHeader->addTabLabel(name);
    assert(mHeader->labelCount() == mContent->childCount());
}

const Widget * Tab::getTab(const std::string& tabName) const {
    int index = mHeader->tabLabelIndex(tabName);
    if (index == mContent->childCount())
        return nullptr;
    return mContent->children()[index];
}

Widget* Tab::getTab(const std::string& tabName) {
    int index = mHeader->tabLabelIndex(tabName);
    if (index == mContent->childCount())
        return nullptr;
    return mContent->children()[index];
}

bool Tab::removeTab(const std::string& tabName) {
    int index = mHeader->removeTabLabel(tabName);
    if (index == mHeader->labelCount())
        return false;
    mContent->removeChild(index);
    return true;
}

void Tab::performLayout(NVGcontext* ctx) {
    mHeader->setFixedWidth(mSize.x());
    Widget::performLayout(ctx);
}

Vector2i Tab::preferredSize(NVGcontext* ctx) const {
    auto contentSize = mContent->preferredSize(ctx);
    auto headerSize = mHeader->preferredSize(ctx);
    auto borderSize = Vector2i(2 * contentBorder, 2 * contentBorder);
    Vector2i tabPreferredSize = contentSize + borderSize + Vector2i(0, headerSize.y());
    return tabPreferredSize;
}

void Tab::draw(NVGcontext* ctx) {  
    nvgTranslate(ctx, mPos.x(), mPos.y());
    mHeader->draw(ctx);
    nvgTranslate(ctx, contentBorder, mHeader->size().y());
    mContent->draw(ctx);
    nvgTranslate(ctx, -(mPos.x() + contentBorder), -(mPos.y() + mHeader->size().y()));
}

NAMESPACE_END(nanogui)