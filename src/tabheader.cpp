/*
    nanogui/tabheader.h -- Widget used to control tabs.

    The tab header widget was contributed by Stefan Ivanov.
            
    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/tabheader.h>
#include <nanogui/opengl.h>

NAMESPACE_BEGIN(nanogui)

TabHeaderWidget::TabHeaderWidget(nanogui::Widget* parent,
                                 const std::string& font, int fontSize)
    : Widget(parent), mFont(font) {
    if (fontSize >= 0)
        mFontSize = fontSize;
}

void TabHeaderWidget::setActiveTab(int tabIndex) {
    assert(tabIndex < mTabLabels.size());
    mActiveTab = tabIndex;
}

int TabHeaderWidget::activeTab() const {
    return mActiveTab;
}

void TabHeaderWidget::setCallback(const std::function<void(int)>& callback) {
    mCallback = callback;
}

const std::function<void(int)>& TabHeaderWidget::callback() const {
    return mCallback;
}

void TabHeaderWidget::addTab(const std::string& tabLable) {
    mTabLabels.push_back(tabLable);
}

std::string& TabHeaderWidget::tabLabelAt(size_type index) {
    assert(index < mTabLabels.size());
    return mTabLabels[index];
}

void TabHeaderWidget::performLayout(NVGcontext* ctx) {
    Widget::performLayout(ctx);
}

Vector2i TabHeaderWidget::preferredSize(NVGcontext* ctx) const {
    if (mTabLabels.empty()) {
        return Vector2i::Zero();
    }
    nvgFontFace(ctx, mFont.c_str());
    nvgFontSize(ctx, fontSize());
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    int width = 0;
    for (auto& tabLabel : mTabLabels) {
        width += nvgTextBounds(ctx, 0, 0, tabLabel.c_str(),
                               nullptr, nullptr) + tabMargin;
    }
    return Vector2i(width, buttonHeight);
}

bool TabHeaderWidget::mouseButtonEvent(const Vector2i & p, int button, bool down, int modifiers) {
    Widget::mouseButtonEvent(p, button, down, modifiers);
    if (button == GLFW_MOUSE_BUTTON_1) {
        int index = 0;
        while (p.x() > mTabLabelExtents[index].second) {
            ++index;
            if (index == mTabLabelExtents.size())
                return false;
        }
        setActiveTab(index);
        mCallback(index);
    }
    return false;
}

void TabHeaderWidget::draw(NVGcontext* ctx) {
    Widget::draw(ctx);
    nvgFontFace(ctx, mFont.c_str());
    nvgFontSize(ctx, fontSize());
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    // Check if the button extents need to be recalculated.
    if (extentsNeedUpdate) {
        mTabLabelExtents.clear();
        int currentPosition = 0;
        for (const auto& tabLabel : mTabLabels) {
            auto nextPosition = currentPosition + nvgTextBounds(ctx, 0, 0, tabLabel.c_str(),
                                                               nullptr, nullptr) + tabMargin;
            mTabLabelExtents.push_back({ currentPosition, nextPosition });
            currentPosition = nextPosition;
        }
        extentsNeedUpdate = false;
    }
    float currentPosition = mPos.x();
    // TODO: Handle max/min width of tabs.
    // TODO: Handle max width of the header.
    for (auto& tabLabel : mTabLabels) {
        nvgText(ctx, currentPosition, mPos.y() + mSize.y() * 0.5f,
                                  tabLabel.c_str(), nullptr);
        currentPosition += (nvgTextBounds(ctx, 0, 0, tabLabel.c_str(),
                                         nullptr, nullptr) + tabMargin);
    }
}

NAMESPACE_END(nanogui)