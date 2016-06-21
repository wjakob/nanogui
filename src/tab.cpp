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
#include <nanogui/theme.h>
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
    // Automatically sets the parent.
    mContent->addChild(tab);
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
    mHeader->setPosition({ 0, 0 });
    mContent->setPosition({contentBorder, mHeader->size().y() + contentBorder});
   
}

Vector2i Tab::preferredSize(NVGcontext* ctx) const {
    auto contentSize = mContent->preferredSize(ctx);
    auto headerSize = mHeader->preferredSize(ctx);
    auto borderSize = Vector2i(2 * contentBorder, 2 * contentBorder);
    Vector2i tabPreferredSize = contentSize + borderSize + Vector2i(0, headerSize.y());
    return tabPreferredSize;
}

void Tab::draw(NVGcontext* ctx) {  
    Widget::draw(ctx);
    // Draw a border around the content
    int xBorder = mPos.x();
    int yBorder = mPos.y() + mHeader->size().y();
    int wBorder = mSize.x();
    int hBorder = mContent->size().y() + contentBorder;
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, xBorder, yBorder, wBorder, hBorder, mTheme->mWindowCornerRadius);
    nvgStrokeColor(ctx, mTheme->mBorderDark);
    nvgStroke(ctx);
    // Remove the border separating the tab header from the contents.
    nvgBeginPath(ctx);
    nvgMoveTo(ctx, xBorder, yBorder);
    nvgLineTo(ctx, xBorder + wBorder, yBorder);
    nvgStrokeColor(ctx,  mTheme->mWindowFillFocused);
    nvgStroke(ctx);

}

NAMESPACE_END(nanogui)

//Color(1.0f, 0.0f, 0.0f, 1.0f)