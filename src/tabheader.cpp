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
#include <nanogui/theme.h>
#include <nanogui/opengl.h>

NAMESPACE_BEGIN(nanogui)

TabButton::TabButton(Widget* header, const std::string& label, int index)
    : Widget(header), mLabel(label), mIndex(index)
{}

Vector2i TabButton::preferredSize(NVGcontext* ctx) const
{
    float bounds[4];
    int labelWidth = nvgTextBounds(ctx, 0, 0, mLabel.c_str(), nullptr, bounds);
    // TODO: Try using bounds array for width as well.
    int buttonWidth = labelWidth + 2 * horizontalPadding;
    int buttonHeight = bounds[3] - bounds[1] + 2 * verticalPadding;
    return Vector2i(buttonWidth, buttonHeight);
}

void TabButton::performLayout(NVGcontext* ctx)
{
    NVGtextRow displayedText;
    nvgTextBreakLines(ctx, mLabel.c_str(), nullptr, mSize.x(), &displayedText, 1);
    // Check to see if the text need to be truncated.
    if (displayedText.next[0]) {
        auto truncatedWidth = nvgTextBounds(ctx, 0.0f, 0.0f,
                                            displayedText.start, displayedText.end, nullptr);
        auto dotsWidth = nvgTextBounds(ctx, 0.0f, 0.0f, dots, nullptr, nullptr);
        while ((truncatedWidth + dotsWidth) > mSize.x()) {
            --displayedText.end;
            truncatedWidth = nvgTextBounds(ctx, 0.0f, 0.0f,
                                           displayedText.start, displayedText.end, nullptr);
        }
        // Remember the truncated width because the return value of nvgText is not what we need.
        mVisibleWidth = truncatedWidth;
        // Set the tool tip to be the full name.
        setTooltip(mLabel);
    }
    mVisibleText.first = displayedText.start;
    mVisibleText.last = displayedText.end; 

    Widget::performLayout(ctx);
}


bool TabButton::mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) {
    Widget::mouseButtonEvent(p, button, down, modifiers);
    
    if (visible() && button == GLFW_MOUSE_BUTTON_1 && down) {
        return contains(p);
    }
    return false;
}


void TabButton::draw(NVGcontext* ctx)
{
    Widget::draw(ctx); 
    int xPos = mPos.x();
    int yPos = mPos.y();
    int width = mSize.x();
    int height = mSize.y();

    // Background gradients
    NVGcolor gradTop = mTheme->mButtonGradientTopPushed;
    NVGcolor gradBot = mTheme->mButtonGradientBotPushed;
    // Check if this is the active tab.
    if (mActive) {
        gradTop = mTheme->mWindowFillFocused;
        gradBot = mTheme->mWindowFillFocused;
    }
    // Draw the background.
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, xPos+1, yPos+1, width-1, height-1, 1.0f);
    NVGpaint backgroundColor = nvgLinearGradient(ctx, xPos, yPos, xPos, yPos + height,
                                                 gradTop, gradBot);
    nvgFillPaint(ctx, backgroundColor);
    nvgFill(ctx);
    nvgStrokeColor(ctx, mTheme->mBorderMedium);
    nvgStroke(ctx);
    // White top border.
    //if (mActive) {
    //    nvgBeginPath(ctx);
    //    nvgMoveTo(ctx, xPos + 2, yPos + 2);
    //    nvgLineTo(ctx, xPos + width - 2, yPos + 2);
    //    nvgStrokeColor(ctx, mTheme->mBorderLight);
    //    nvgStroke(ctx);
    //}

    // Add initial text padding
    int textX = xPos + horizontalPadding;
    int textY = yPos + verticalPadding;
    NVGcolor textColor = mTheme->mTextColor;
    nvgFillColor(ctx, textColor);
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    nvgText(ctx, textX, textY, mVisibleText.first, mVisibleText.last);
    if (mVisibleText.last != nullptr)
        nvgText(ctx, textX + mVisibleWidth, textY, dots, nullptr);
}

TabHeader::TabHeader(Widget* parent, const std::string& font,
                                 int fontSize, Color fontColor)
    : Widget(parent), mFont(font), mFontColor(fontColor) {
    if (fontSize >= 0)
        mFontSize = fontSize;
}

void TabHeader::setActiveTab(int tabIndex) {
    assert(tabIndex < (int)mChildren.size());
    auto oldActive = dynamic_cast<TabButton*>(mChildren[mActiveTab]);
    auto newActive = dynamic_cast<TabButton*>(mChildren[tabIndex]);
    if (!(oldActive && newActive))
        throw std::runtime_error("Tab header widget contains a direct child that is not a tab button!");
    oldActive->setActive(false);
    newActive->setActive(true);
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

void TabHeader::addTabLabel(const std::string& tabLabel) {
    int newTabIndex = mChildren.size();
    new TabButton(this, tabLabel, newTabIndex);
}

const std::string& TabHeader::tabLabelAt(int index) {
    assert(index < (int)mChildren.size());
    auto tabButton = dynamic_cast<TabButton*>(mChildren[index]);
    if (!tabButton)
        throw std::runtime_error("Tab header widget contains a direct child that is not a tab button!");
    return tabButton->label();
}

int TabHeader::tabLabelIndex(const std::string& tabLabel)
{
    int i = 0;
    for (auto child : mChildren) {
        auto tabButton = dynamic_cast<TabButton*>(child);
        if (!tabButton)
                throw std::runtime_error("Tab header widget contains a direct child that is not a tab button!");
        if (tabButton->label() == tabLabel)
            break;
        ++i;
    }
    return i;
}

int TabHeader::removeTabLabel(const std::string& tabLabel)
{
    int i = 0;
    int size = mChildren.size();
    while(i != size) {
        auto tabButton = dynamic_cast<TabButton*>(mChildren[i]);
        if (!tabButton)
            throw std::runtime_error("Tab header widget contains a direct child that is not a tab button!");
        if (tabButton->label() == tabLabel) {
            mChildren.erase(std::next(mChildren.begin(), i));
            return i;
        }
        ++i;
    }
    return size;
}

void TabHeader::performLayout(NVGcontext* ctx) {
    Vector2i currentPosition = Vector2i::Zero();
    // Place the tab buttons relative to the beginning of the tab header.
    for (auto i = 0u; i != mChildren.size(); ++i) {
        auto child = mChildren[i];
        auto childPreferred = child->preferredSize(ctx);
        int width = childPreferred.x();
        if (childPreferred.x() < minButtonWidth)
            width = maxButtonWidth;
        else if (childPreferred.x() > maxButtonWidth)
            width = maxButtonWidth;
        child->setFixedWidth(width);
        child->setPosition(currentPosition);
        currentPosition.x() += width;
    }

    Widget::performLayout(ctx);

    calculateVisibleEnd();
    if (mOverflowing)
        updateVisibility();

}

Vector2i TabHeader::preferredSize(NVGcontext* ctx) const {
    // Set up the nvg context for measuring the text inside the tab buttons.
    nvgFontFace(ctx, mFont.c_str());
    nvgFontSize(ctx, fontSize());
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    
    Vector2i size = Vector2i::Zero();
    for (auto child : mChildren) {
        auto childPreferred = child->preferredSize(ctx);
        if (childPreferred.x() < minButtonWidth)
            childPreferred.x() = minButtonWidth;
        else if (childPreferred.x() > maxButtonWidth)
            childPreferred.x() = maxButtonWidth;
        size.x() += childPreferred.x();
        size.y() = std::max(size.y(), childPreferred.y());
    }
    return size;
}

bool TabHeader::mouseButtonEvent(const Vector2i& p, int button, bool down, int /*modifiers*/) {
    
    if (button == GLFW_MOUSE_BUTTON_1 && down) {
        auto pAbsolute = p;
        // Check whether there are controls and whether they have been pressed. 
        if (mOverflowing) {
            switch (isInsideControls(p)) {
            case ControlsClicked::Left:
                leftControlsClicked();
                return true;
            case ControlsClicked::Right:
                rightControlsClicked();
                return true;
            case ControlsClicked::None:
                pAbsolute = p - Vector2i(controlsContentWidth, 0);
                break;
            }
        }
        auto first = std::next(mChildren.begin(), mVisibleStart);
        auto last = std::next(mChildren.begin(), mVisibleEnd);
        // Translate the mouse press to tab header coordinates.
        pAbsolute += Vector2i((*first)->position().x(), 0);
        // TODO: Use a callback on the tab button.
        while (first != last) {
            auto child = *first;
            if (child->contains(pAbsolute)) {
                int index = std::distance(mChildren.begin(), first);
                mCallback(index);
                setActiveTab(index);
                return true;
            }
            ++first;
        }
    }
    return false;
}

void TabHeader::draw(NVGcontext* ctx) {
    // Draw the tab header border.
    int xPos = mPos.x();
    int yPos = mPos.y();
    int width = mSize.x();
    int height = mSize.y();
    NVGcolor gradTop = mTheme->mButtonGradientTopPushed;
    NVGcolor gradBot = mTheme->mButtonGradientBotPushed;
    NVGpaint backgroundColor = nvgLinearGradient(ctx, xPos, yPos, xPos, yPos + height,
                                                 gradTop, gradBot);

    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, xPos + 1.0f, yPos + 1.0f, width - 1.0f, height - 2.0f, mTheme->mButtonCornerRadius);
    nvgFillPaint(ctx, backgroundColor);
    nvgFill(ctx);

    // White bottom border.
    //nvgBeginPath(ctx);
    //nvgRoundedRect(ctx, xPos + 1.0f, yPos + 1.0f, width - 1.0f, height, mTheme->mButtonCornerRadius);
    ////nvgStrokeWidth(ctx, 3.0f);
    //nvgStrokeColor(ctx, mTheme->mBorderLight);
    //nvgStroke(ctx);

    // Dark border
    //nvgBeginPath(ctx);
    //nvgRoundedRect(ctx, xPos, yPos, width, height, mTheme->mButtonCornerRadius);
    ////nvgStrokeWidth(ctx, 3.0f);
    //nvgStrokeColor(ctx, mTheme->mBorderDark);
    //nvgStroke(ctx);

    if (mOverflowing) {
        drawControls(ctx);
        nvgTranslate(ctx, controlsWidth, 0);
    }
   

    // Translate in order to place the first button in the correct position.
    nvgTranslate(ctx, -mChildren[mVisibleStart]->position().x(), 0);
    // Set up the text for the tab buttons
    nvgFillColor(ctx, mFontColor);
    nvgFontFace(ctx, mFont.c_str());
    nvgFontSize(ctx, fontSize());
    Widget::draw(ctx);
    // Reverse translations
    
    // Fix up the space after the last button.
    nvgTranslate(ctx, mChildren[mVisibleStart]->position().x(), 0);

    nvgBeginPath(ctx);
    if (mVisibleEnd != 0) {
        int lastVisibleXPos = mChildren[mVisibleEnd - 1]->position().x() - mChildren[mVisibleStart]->position().x();
        int lastVisibleWidth = mChildren[mVisibleEnd - 1]->size().x();
        nvgMoveTo(ctx, lastVisibleXPos + lastVisibleWidth, yPos + 1);
        nvgLineTo(ctx, width - (mOverflowing ? 2*controlsWidth : 0), yPos + 1);
        nvgStrokeColor(ctx, gradTop);
        nvgStroke(ctx);
    }
   
    if (mOverflowing) {
        nvgTranslate(ctx, -controlsWidth, 0);
    }
}


void TabHeader::updateVisibility() {
    auto current = mChildren.begin();
    auto firstVisible = std::next(mChildren.begin(), mVisibleStart);
    while (current != firstVisible) {
        (*current)->setVisible(false);
        ++current;
    }
    auto firstInvisible = std::next(mChildren.begin(), mVisibleEnd);
    while (current != firstInvisible) {
        (*current)->setVisible(true);
        ++current;
    }
    auto last = mChildren.end();
    while (current != last) {
        (*current)->setVisible(false);
        ++current;
    }
}

void TabHeader::calculateVisibleEnd() {
    int lastChildEnd = mChildren.back()->position().x() + mChildren.back()->size().x();
    if (mSize.x() < lastChildEnd) {
        auto first = std::next(mChildren.begin(), mVisibleStart);
        auto last = mChildren.end();
        // The offset must take into account both of the controls on either end.
        int offset = (*first)->position().x() + controlsWidth;
        int relativeEnd = (*first)->position().x() + mSize.x() - controlsWidth;
        while (first != last) {
            Widget* child = *first;
            offset += child->size().x();
            if (offset > relativeEnd) {
                break;
            }
            ++first;
        }
        mVisibleEnd = std::distance(mChildren.begin(), first);
        mOverflowing = true;
    }
    else {
        mVisibleEnd = mChildren.size();
        mOverflowing = false;
    }
}

void TabHeader::drawControls(NVGcontext* ctx) {
    // Compute bounds for the first button.
    int xPos = mPos.x();
    int yPos = mPos.y();
    int width = controlsWidth;
    int height = mSize.y();
    // Background gradients
    NVGcolor gradTop = mTheme->mButtonGradientTopUnfocused;
    NVGcolor gradBot = mTheme->mButtonGradientBotUnfocused;
    // Check if the button should be active
    if (mVisibleStart == 0) {
        // TODO: Add a separate color for marking disabled.
        gradTop = mTheme->mButtonGradientTopPushed;
        gradBot = mTheme->mButtonGradientBotPushed;
    }
    // Draw the background.
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, xPos + 1, yPos + 1, width - 1, height - 1, mTheme->mButtonCornerRadius);
    NVGpaint backgroundColor = nvgLinearGradient(ctx, xPos, yPos, xPos, yPos + height,
                                                 gradTop, gradBot);
    nvgFillPaint(ctx, backgroundColor);
    nvgFill(ctx);
    // Draw the arrow.
    nvgBeginPath(ctx);
    nvgMoveTo(ctx, xPos + 8.0f / 9.0f * width, yPos + 1.0f / 6.0f * height);
    nvgLineTo(ctx, xPos + 2.0f / 9.0f * width, yPos + 0.5f * height);
    nvgLineTo(ctx, xPos + 8.0f / 9.0f * width, yPos + 5.0f / 6.0f * height);
    nvgStrokeWidth(ctx, 4.0f);
    nvgStrokeColor(ctx, mTheme->mBorderDark);
    nvgStroke(ctx);

    nvgBeginPath(ctx);
    nvgMoveTo(ctx, xPos + 8.0f / 9.0f * width, yPos + 1.0f / 6.0f * height);
    nvgLineTo(ctx, xPos + 2.0f / 9.0f * width, yPos + 0.5f * height);
    nvgLineTo(ctx, xPos + 8.0f / 9.0f * width, yPos + 5.0f / 6.0f * height);
    nvgStrokeWidth(ctx, 1.5f);
    if (mVisibleEnd == 0)
        nvgStrokeColor(ctx, mTheme->mBorderMedium);
    else
        nvgStrokeColor(ctx, mTheme->mBorderLight);
    nvgStroke(ctx);


    // Compute bounds for the second button.
    xPos = mPos.x() + mSize.x() - controlsWidth;
    yPos = mPos.y();
    width = controlsWidth;
    height = mSize.y();
    // Background gradients
    gradTop = mTheme->mButtonGradientTopUnfocused;
    gradBot = mTheme->mButtonGradientBotUnfocused;
    // Check if the button should be active
    if (mVisibleEnd == (int)mChildren.size()) {
        // TODO: Add a separate color for marking disabled.
        gradTop = mTheme->mButtonGradientTopPushed;
        gradBot = mTheme->mButtonGradientBotPushed;
    }
    // Draw the background.
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, xPos + 1, yPos + 1, width - 1, height - 1, mTheme->mButtonCornerRadius);
    backgroundColor = nvgLinearGradient(ctx, xPos, yPos, xPos, yPos + height,
                                                 gradTop, gradBot);
    nvgFillPaint(ctx, backgroundColor);
    nvgFill(ctx);
    // Draw the arrow.
    nvgBeginPath(ctx);
    nvgMoveTo(ctx, xPos + 1.0f / 9.0f * width, yPos + 1.0f / 6.0f * height);
    nvgLineTo(ctx, xPos + 7.0f / 9.0f * width, yPos + 0.5f * height);
    nvgLineTo(ctx, xPos + 1.0f / 9.0f * width, yPos + 5.0f / 6.0f * height);
    nvgStrokeWidth(ctx, 4.0f);
    nvgStrokeColor(ctx, mTheme->mBorderDark);
    nvgStroke(ctx);

    nvgBeginPath(ctx);
    nvgMoveTo(ctx, xPos + 1.0f / 9.0f * width, yPos + 1.0f / 6.0f * height);
    nvgLineTo(ctx, xPos + 7.0f / 9.0f * width, yPos + 0.5f * height);
    nvgLineTo(ctx, xPos + 1.0f / 9.0f * width, yPos + 5.0f / 6.0f * height);
    nvgStrokeWidth(ctx, 1.5f);
    if (mVisibleEnd == (int)mChildren.size())
        nvgStrokeColor(ctx, mTheme->mBorderMedium);
    else
        nvgStrokeColor(ctx, mTheme->mBorderLight);
    nvgStroke(ctx);

}

TabHeader::ControlsClicked TabHeader::isInsideControls(const Vector2i& p) {
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
    updateVisibility();
}

void TabHeader::rightControlsClicked() {
    if (mVisibleEnd == (int)mChildren.size()) {
        return;
    }
    ++mVisibleStart;
    calculateVisibleEnd();
    updateVisibility();
}

NAMESPACE_END(nanogui)
