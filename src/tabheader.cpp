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

TabHeader::TabHeader(nanogui::Widget* parent, const std::string& font,
                                 int fontSize, Color fontColor)
    : Widget(parent), mFont(font), mFontColor(fontColor) {
    if (fontSize >= 0)
        mFontSize = fontSize;
}

void TabHeader::setActiveTab(int tabIndex) {
    assert(tabIndex < (int)mTabLabels.size());
    mActiveTab = tabIndex;
}

int TabHeader::activeTab() const {
    return mActiveTab;
}

void TabHeader::setCallback(const std::function<void(int)>& callback) {
    mCallback = callback;
}

const std::function<void(int)>& TabHeader::callback() const {
    return mCallback;
}

void TabHeader::addTabLabel(const std::string& tabLable) {
    mTabLabels.push_back(tabLable);
}

std::string& TabHeader::tabLabelAt(size_type index) {
    assert(index < mTabLabels.size());
    return mTabLabels[index];
}

int TabHeader::tabLabelIndex(const std::string & tabLabel)
{
    int i = 0;
    for (auto currentLabel : mTabLabels) {
        if (currentLabel == tabLabel)
            break;
        ++i;
    }
    return i;
}

int TabHeader::removeTabLabel(const std::string & tabLabel)
{
    int i = 0;
    int size = mTabLabels.size();
    while(i != size) {
        if (tabLabel == mTabLabels[i]) {
            mTabLabels.erase(std::next(mTabLabels.begin(), i));
            // Set the internal state to invalid. That will take care of the extents.
            mInternalStateValid = false;
            return i;
        }
        ++i;
    }
    return size;
}

void TabHeader::performLayout(NVGcontext* ctx) {
    Widget::performLayout(ctx);
    if (mIsOverflowing) {
        // Update the extents to take the added controls into account.
        for (auto& extents : mTabLabelExtents) {
            extents = { extents.first + controlsWidth, extents.second + controlsWidth };
        }
    }
    calculateVisibleEnd();
}

Vector2i TabHeader::preferredSize(NVGcontext* ctx) const {
    if (!mInternalStateValid)
        const_cast<TabHeader&>(*this).updateInternalState(ctx);
    if (mTabLabels.empty())
        return Vector2i::Zero();

    // Set up the nvg context for measuring the text.
    nvgFontFace(ctx, mFont.c_str());
    nvgFontSize(ctx, fontSize());
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    
    // Calculate the height of one of the tab buttons.
    float bounds[4];
    nvgTextBounds(ctx, 0, 0, mTabLabels[0].c_str(), nullptr, bounds);
    int height = bounds[3] - bounds[1] + 2*topBottomMargin;

    return Vector2i(mTabLabelExtents.back().second, height);
}

bool TabHeader::mouseButtonEvent(const Vector2i& p, int button, bool down, int modifiers) {
    Widget::mouseButtonEvent(p, button, down, modifiers);
    if (button == GLFW_MOUSE_BUTTON_1 && down) {
        // Check whether there are controls and whether they have been pressed. 
        if (mIsOverflowing) {
            switch (isInsideControls(p)) {
            case ControlsClicked::Left:
                leftControlsClicked();
                return true;
            case ControlsClicked::Right:
                rightControlsClicked();
                return true;
            case ControlsClicked::None:
                break;
            }
        }
        int index = mVisibleStart;
        int absoluteHeaderPosition = p.x() + mTabLabelExtents[mVisibleStart].first - mPos.x() - controlsWidth;
        while (absoluteHeaderPosition > mTabLabelExtents[index].second) {
            ++index;
            if (index == mVisibleEnd)
                return false;
        }
        setActiveTab(index);
        mCallback(index);
    }
    return false;
}

void TabHeader::updateInternalState(NVGcontext* ctx) {
    // Set up the nvg context for measuring the text.
    nvgFontFace(ctx, mFont.c_str());
    nvgFontSize(ctx, fontSize());
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);

    // Calculate the extent of each tab header button.
    mTabLabelExtents.clear();
    int currentPosition = 0;
    int nextPosition = 0;
    for (const auto& tabLabel : mTabLabels) {
        int labelWidth = nvgTextBounds(ctx, 0, 0, tabLabel.c_str(), nullptr, nullptr);
        int buttonWidth = labelWidth + 2 * tabPadding;
        // Check if the tab is too wide or narrow.
        if (buttonWidth > maxButtonWidth)
            nextPosition = maxButtonWidth + currentPosition;
        else if (buttonWidth < minButtonWidth)
            nextPosition = minButtonWidth + currentPosition;
        else
            nextPosition = buttonWidth + currentPosition;
        mTabLabelExtents.push_back({ currentPosition, nextPosition });
        currentPosition = nextPosition;
    }
    //calculateVisibleEnd();
    mInternalStateValid = true;
}

void TabHeader::draw(NVGcontext* ctx) {
    Widget::draw(ctx);
    
    if (!mInternalStateValid)
        updateInternalState(ctx);
    nvgFillColor(ctx, mFontColor);
    nvgFontFace(ctx, mFont.c_str());
    nvgFontSize(ctx, fontSize());
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);

    int xOffSet = mPos.x();
    int yOffSet = mPos.y();
    
    int relativeOffset = mTabLabelExtents[mVisibleStart].first;
    // Draw the controls if necessary.
    if (mIsOverflowing) {
        drawControls(ctx);
        relativeOffset -= controlsWidth;
    }

    for (int i = mVisibleStart; i != mVisibleEnd; ++i) {
        auto& tabLabel = mTabLabels[i];
        // Find the last character to be displayed.
        NVGtextRow displayedText;
        nvgTextBreakLines(ctx, tabLabel.c_str(), nullptr, maxButtonWidth, &displayedText, 1);
        
        int xPosition = xOffSet + mTabLabelExtents[i].first + tabPadding - relativeOffset;
        int yPosition = yOffSet + mSize.y() * 0.5f;
        if (!displayedText.next[0]) {
            nvgText(ctx, xPosition, yPosition,
                    displayedText.start, displayedText.end);
        }
        else {
            // Revert back some symbols to have space for the three dots.
            static const char* dots = "...";
            auto truncatedWidth = nvgTextBounds(ctx, 0.0f, 0.0f,
                                                displayedText.start, displayedText.end, nullptr);
            auto dotsWidth = nvgTextBounds(ctx, 0.0f, 0.0f,dots, nullptr, nullptr);
            while ((truncatedWidth + dotsWidth) > (maxButtonWidth - tabPadding)) {
                // Revert back some symbols to have space for the three dots.
                // TODO: Add proper calculation for how much characters to revert   
                // TODO: Loop over text and determine if the characters fit.
                --displayedText.end;
                // Update the width calculation.
                truncatedWidth = nvgTextBounds(ctx, 0.0f, 0.0f,
                                               displayedText.start, displayedText.end, nullptr);
            }
            // TODO: Handle edge case of the last tab being too wide to fit.
            // Finally, draw the label
            nvgText(ctx, xPosition, yPosition, displayedText.start, displayedText.end);
            auto dotsPosition = xPosition + truncatedWidth;
            // Add the dots at the end.
            nvgText(ctx, dotsPosition, yPosition, dots, nullptr);
        }
    }
}

void TabHeader::drawHitBox(NVGcontext* ctx, int index) {
    // Compute bounds.
    int minX = mPos.x() + mTabLabelExtents[index].first;
    int minY = mPos.y();
    int maxX = mPos.x() + mTabLabelExtents[index].second;
    int maxY = mPos.y() + mSize.y();
    // Draw the boxes.
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, minX, minY, maxX - minX, maxY - minY, 0.5f);
    float modifier = 1.0f / ((float)(index) + 1);
    Color fillColor = Color(modifier, 0.1f, 0.1f, 1.0f);
    nvgFillColor(ctx, fillColor);
    nvgFill(ctx);
}

void TabHeader::calculateVisibleEnd() {
    if (mSize.x() < mTabLabelExtents.back().second) {
        // Draw controls
        // Find the index of the last visible tab given that we know the first one.
        int relativePos = mTabLabelExtents[mVisibleStart].first;  // mTabLabelExtents already takes into account the initial controls.
        int xSize = mSize.x() - 2*controlsWidth;
        auto visibleEndIter = std::find_if(std::next(mTabLabelExtents.begin(), mVisibleStart),
                                           mTabLabelExtents.end(),
                                           [relativePos, xSize](const auto& extents) {
            int relativeStart = extents.first - relativePos;
            int realtiveEnd = extents.second - relativePos;
            return (relativeStart < xSize) && (realtiveEnd > xSize);
        });
        mVisibleEnd = std::distance(mTabLabelExtents.begin(), visibleEndIter);
        mIsOverflowing = true;
    }
    else {
        mVisibleEnd = mTabLabels.size();
        mIsOverflowing = false;
    }
}

void TabHeader::drawControls(NVGcontext* ctx) {
    // Compute bounds for the first button.
    int minX = mPos.x();
    int minY = mPos.y();
    int maxX = mPos.x() + controlsWidth;
    int maxY = mPos.y() + mSize.y();
    // Draw the background.
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, minX, minY, maxX - minX, maxY - minY, 0.5f);
    Color fillColor = Color(1.0f, 0.1f, 0.1f, 1.0f);
    nvgFillColor(ctx, fillColor);
    nvgFill(ctx);
    // TODO: Draw the arrow.
    // TODO: Check if the arrow is enabled.

    // Compute bounds for the second button.
    minX = mPos.x() + mSize.x() - controlsWidth;
    minY = mPos.y();
    maxX = mPos.x() + mSize.x();
    maxY = mPos.y() + mSize.y();
    // Draw the background.
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, minX, minY, maxX - minX, maxY - minY, 0.5f);
    fillColor = Color(1.0f, 0.1f, 0.1f, 1.0f);
    nvgFillColor(ctx, fillColor);
    nvgFill(ctx);
    // TODO: Draw the arrow.
    // TODO: Check if the arrow is enabled.

}

TabHeader::ControlsClicked TabHeader::isInsideControls(const Vector2i & p) {
    auto leftDistance = (p - mPos).array();                                                                             
    bool hitLeft = (leftDistance >= 0).all() && (leftDistance < Vector2i(controlsWidth, mSize.y()).array()).all();
    if (hitLeft) {
        return ControlsClicked::Left;
    }
    auto rightDistance = (p - (mPos + Vector2i(mSize.x() - controlsWidth, 0))).array();
    bool hitRight = (rightDistance >= 0).all() && (rightDistance < Vector2i(controlsWidth, mSize.y()).array()).all();
    if (hitRight) {
        return ControlsClicked::Right;
    }
    return ControlsClicked::None;
}
                                                      
void TabHeader::leftControlsClicked() {
    if (mVisibleStart == 0) {
        return;
    }
    --mVisibleStart;
    calculateVisibleEnd();
}

void TabHeader::rightControlsClicked() {
    if (mVisibleEnd == (int)mTabLabels.size()) {
        return;
    }
    ++mVisibleStart;
    calculateVisibleEnd();
}

NAMESPACE_END(nanogui)