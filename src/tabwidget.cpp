/*
    nanogui/tabwidget.h -- A wrapper around the widgets TabHeader and StackedWidget
    which hooks the two classes together.

    The tab widget was contributed by Stefan Ivanov.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/tabwidget.h>
#include <nanogui/tabheader.h>
#include <nanogui/stackedwidget.h>
#include <nanogui/theme.h>
#include <nanogui/opengl.h>
#include <algorithm>

NAMESPACE_BEGIN(nanogui)

TabWidget::TabWidget(Widget* parent)
    : Widget(parent), mHeader(new TabHeader(this)), mContent(new StackedWidget(this)) {
    mHeader->setCallback([this](int i) { mContent->setSelectedIndex(i); 
    /// TODO: if (mCallback) mCallback(i); 
    });
}

void TabWidget::setActiveTab(int tabIndex) {
    mHeader->setActiveTab(tabIndex);
    mContent->setSelectedIndex(tabIndex);
}

int TabWidget::activeTab() const {
    assert(mHeader->activeTab() == mContent->selectedIndex());
    return mContent->selectedIndex();
}

int TabWidget::tabCount() const {
    assert(mContent->childCount() == mHeader->tabCount());
    return mHeader->tabCount(); 
}

void TabWidget::appendTab(Widget* tab, const std::string& name) {
    mContent->addChild(tab);
    mHeader->appendTab(name);
    assert(mHeader->tabCount() == mContent->childCount());
}

void TabWidget::prependTab(Widget * tab, const std::string & tabLabel) {
    mHeader->prependTab(tabLabel);
    mContent->prependChild(tab);
    assert(mHeader->tabCount() == mContent->childCount());
}

void TabWidget::insertTab(int index, Widget *tab, const std::string & tablabel) {
    mHeader->insertTab(index, tablabel);
    mContent->insertChild(index, tab);
    assert(mHeader->tabCount() == mContent->childCount());
}

int TabWidget::tabLabelIndex(const std::string& tabLabel)
{
    return mHeader->tabIndex(tabLabel);
}

const Widget * TabWidget::getTab(const std::string& tabName) const {
    int index = mHeader->tabIndex(tabName);
    if (index == mContent->childCount())
        return nullptr;
    return mContent->children()[index];
}

Widget* TabWidget::getTab(const std::string& tabName) {
    int index = mHeader->tabIndex(tabName);
    if (index == mContent->childCount())
        return nullptr;
    return mContent->children()[index];
}

bool TabWidget::removeTab(const std::string& tabName) {
    int index = mHeader->removeTab(tabName);
    if (index == mHeader->tabCount())
        return false;
    mContent->removeChild(index);
    return true;
}

void TabWidget::removeTab(int index) {
    assert(mHeader->tabCount() == mContent->childCount() && mContent->childCount() < index);
    mHeader->removeTab(index);
    mContent->removeChild(index);
}

const std::string & TabWidget::tabLabelAt(int index) const
{
    return mHeader->tabLabelAt(index);
}

void TabWidget::performLayout(NVGcontext* ctx) {
    int headerHeight = mHeader->preferredSize(ctx).y();
    mHeader->setPosition({ 0, 0 });
    mHeader->setSize({ mSize.x(), headerHeight });
    mHeader->performLayout(ctx);
    mContent->setPosition({ contentBorder, headerHeight + contentBorder });
    mContent->setSize({ mSize.x() - 2 * contentBorder, mSize.y() - 2*contentBorder - headerHeight });
    mContent->performLayout(ctx);
}

Vector2i TabWidget::preferredSize(NVGcontext* ctx) const {
    auto contentSize = mContent->preferredSize(ctx);
    auto headerSize = mHeader->preferredSize(ctx);
    auto borderSize = Vector2i(2 * contentBorder, 2 * contentBorder);
    Vector2i tabPreferredSize = contentSize + borderSize + Vector2i(0, headerSize.y());
    return tabPreferredSize;
}

void TabWidget::draw(NVGcontext* ctx) {  
    Widget::draw(ctx);   
    // Draw a border around the content.
    int xBorder = mPos.x();
    int yBorder = mPos.y() + mHeader->size().y();
    int wBorder = mSize.x();
    int hBorder = mContent->size().y() + contentBorder;
    // Draw dark border.
    // Draw light border.
    float darkOffset = 0.0f;
    float lightOffset = 1.0f;
    drawBorder(ctx, darkOffset, mTheme->mBorderDark);
    drawBorder(ctx, lightOffset, mTheme->mBorderLight);

    //nvgBeginPath(ctx);
    //nvgMoveTo(ctx, xBorder + TabHeader::controlWidth, yBorder);
    //nvgLineTo(ctx, xBorder, yBorder);
    //nvgLineTo(ctx, xBorder, yBorder + hBorder);
    //nvgLineTo(ctx, xBorder + wBorder, yBorder + hBorder);
    //nvgLineTo(ctx, xBorder + wBorder, yBorder);
    //nvgLineTo(ctx, xBorder + wBorder - TabHeader::controlWidth, yBorder);
    //auto tabButtonsArea = mHeader->visibleButtonArea();
    //nvgLineTo(ctx, xBorder + tabButtonsArea.second.x(), yBorder);
    //nvgStrokeColor(ctx, mTheme->mBorderDark);
    ///// TODO: fix stroke width, and make Button more resistant to stroke width changes
    //nvgStrokeWidth(ctx, 0.3f * contentBorder);
    //nvgStroke(ctx);

}

void TabWidget::drawBorder(NVGcontext* ctx, float offset, const Color& borderColor) {
    // Draw a border around the content.
    int xBorder = mPos.x();
    int yBorder = mPos.y() + mHeader->size().y();
    int wBorder = mSize.x();
    int hBorder = mContent->size().y() + contentBorder;

    nvgBeginPath(ctx);
    nvgMoveTo(ctx, xBorder + TabHeader::controlWidth, yBorder + offset);
    nvgLineTo(ctx, xBorder + offset, yBorder + offset);
    nvgLineTo(ctx, xBorder + offset, yBorder + hBorder - offset);
    nvgLineTo(ctx, xBorder + wBorder - offset, yBorder + hBorder - offset);
    nvgLineTo(ctx, xBorder + wBorder - offset, yBorder + offset);
    nvgLineTo(ctx, xBorder + wBorder - TabHeader::controlWidth - offset, yBorder + offset);
    auto tabButtonsArea = mHeader->visibleButtonArea();
    nvgLineTo(ctx, xBorder + tabButtonsArea.second.x() - offset, yBorder + offset);
    nvgStrokeColor(ctx, borderColor);
    /// TODO: fix stroke width, and make Button more resistant to stroke width changes
    nvgStrokeWidth(ctx, 1.0f);
    nvgStroke(ctx);
}

NAMESPACE_END(nanogui)