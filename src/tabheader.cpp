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
#include <nanogui/entypo.h>
#include <numeric>

NAMESPACE_BEGIN(nanogui)

static const Color debugColor = Color(1.0f, 0.0f, 0.0f, 1.0f);

TabHeader::TabButton::TabButton(TabHeader& header, const std::string& label)
    : mHeader(&header),  mLabel(label)
{}

Vector2i TabHeader::TabButton::preferredSize(NVGcontext* ctx) const {
    float bounds[4];
    /* nvgFontFace/etc. called at this point */
    int labelWidth = nvgTextBounds(ctx, 0, 0, mLabel.c_str(), nullptr, bounds);
    int buttonWidth = labelWidth + 2 * horizontalPadding;
    int buttonHeight = bounds[3] - bounds[1] + 2 * verticalPadding;
    return Vector2i(buttonWidth, buttonHeight);
}

void TabHeader::TabButton::calculateVisibleString(NVGcontext* ctx) {
    // The size must have been set in by the enclosing tab header.
    NVGtextRow displayedText;
    nvgTextBreakLines(ctx, mLabel.c_str(), nullptr, mSize.x(), &displayedText, 1);
    // Check to see if the text need to be truncated.
    if (displayedText.next[0]) {
        auto truncatedWidth = nvgTextBounds(ctx, 0.0f, 0.0f,
                                            displayedText.start, displayedText.end, nullptr);
        auto dotsWidth = nvgTextBounds(ctx, 0.0f, 0.0f, dots, nullptr, nullptr);
        while ((truncatedWidth + dotsWidth + horizontalPadding) > mSize.x() && displayedText.end != displayedText.start) {
            --displayedText.end;
            truncatedWidth = nvgTextBounds(ctx, 0.0f, 0.0f,
                                           displayedText.start, displayedText.end, nullptr);
        }
        // Remember the truncated to know where to display the dots.
        mVisibleWidth = truncatedWidth;
        mVisibleText.last = displayedText.end;
    } else {
        mVisibleText.last = nullptr;
        mVisibleWidth = 0;
    }
    mVisibleText.first = displayedText.start;
}

void TabHeader::TabButton::drawAtPosition(NVGcontext* ctx, const Vector2i& position, bool active) {
    int xPos = position.x();
    int yPos = position.y();
    int width = mSize.x();
    int height = mSize.y();

    // Background gradients
    NVGcolor gradTop = mHeader->theme()->mButtonGradientTopPushed;
    NVGcolor gradBot = mHeader->theme()->mButtonGradientBotPushed;
    // Check if this is the active tab.
    if (active) {
        gradTop = mHeader->theme()->mWindowFillFocused;
        gradBot = mHeader->theme()->mWindowFillFocused;
    }
    // Draw the background.
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, xPos + 1, yPos + 1, width - 1, height - 1,
                   mHeader->theme()->mButtonCornerRadius);
    NVGpaint backgroundColor = nvgLinearGradient(ctx, xPos, yPos, xPos, yPos + height,
                                                 gradTop, gradBot);
    nvgFillPaint(ctx, backgroundColor);
    nvgFill(ctx);
    // Draw borders
    if (!active) {
        nvgStrokeColor(ctx, mHeader->theme()->mBorderMedium);
        nvgStroke(ctx);
        nvgBeginPath(ctx);
        nvgRoundedRect(ctx, xPos, yPos, width, height,
                       mHeader->theme()->mButtonCornerRadius);
        nvgStrokeColor(ctx, mHeader->theme()->mBorderDark);
        nvgStroke(ctx);
    } else {
        nvgBeginPath(ctx);
        nvgMoveTo(ctx, xPos, yPos + height);
        nvgLineTo(ctx, xPos, yPos);
        nvgLineTo(ctx, xPos + width, yPos);
        nvgLineTo(ctx, xPos + width, yPos + height);
        nvgStrokeColor(ctx, mHeader->theme()->mBorderDark);
        nvgStroke(ctx);
    }

    // Draw the text.
    nvgBeginPath(ctx);
    // Add initial text padding
    int textX = xPos + horizontalPadding;
    int textY = yPos + verticalPadding;
    NVGcolor textColor = mHeader->theme()->mTextColor;
    nvgFillColor(ctx, textColor);
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
    assert(tabIndex < tabCount());
    mActiveTab = tabIndex;
    mCallback(tabIndex);
}

int TabHeader::activeTab() const {
    return mActiveTab;
}

void TabHeader::appendTab(const std::string & tabLabel) {
    mTabButtons.push_back(TabButton(*this, tabLabel));
}

void TabHeader::prependTab(const std::string & tabLabel) {
    mTabButtons.insert(mTabButtons.begin(), TabButton(*this, tabLabel));
}

void TabHeader::insertTab(int index, const std::string & tabLabel)
{
    assert(index <= tabCount());
    mTabButtons.insert(std::next(mTabButtons.begin(), index), TabButton(*this, tabLabel));
}

int TabHeader::removeTab(const std::string& tabLabel) {
    auto element = std::find_if(mTabButtons.begin(), mTabButtons.end(),
                                [&](const auto& tb) { return tabLabel == tb.label(); });
    int index = std::distance(mTabButtons.begin(), element);
    if(element == mTabButtons.end())
        mTabButtons.erase(element);
    return index;
}

void TabHeader::removeTab(int index)
{
    assert(index < tabCount());
    mTabButtons.erase(std::next(mTabButtons.begin(), index));
}

const std::string& TabHeader::tabLabelAt(int index) const {
    assert(index < tabCount());
    return mTabButtons[index].label();
}

int TabHeader::tabIndex(const std::string& tabLabel) {
    auto element = std::find_if(mTabButtons.begin(), mTabButtons.end(), 
                                [&](const auto& tb) { return tabLabel == tb.label(); });
    int index = std::distance(mTabButtons.begin(), element);
    return index;
}

std::pair<Vector2i, Vector2i> TabHeader::visibleButtonArea() const {
    if (mVisibleStart == mVisibleEnd)
        return { Vector2i::Zero(), Vector2i::Zero() };
    auto topLeft = mPos + Vector2i(controlWidth, 0);
    auto width = std::accumulate(visibleBegin(), visibleEnd(), controlWidth,
                                 [](auto acc, auto& tb) {
                                    return acc + tb.size().x();
                                 });
    auto bottomRigth = mPos + Vector2i(width, mSize.y());
    return { topLeft, bottomRigth };
}

void TabHeader::performLayout(NVGcontext* ctx) {
    Widget::performLayout(ctx);

    Vector2i currentPosition = Vector2i::Zero();
    // Place the tab buttons relative to the beginning of the tab header.
    for (auto& tab : mTabButtons) {
        auto tabPreferred = tab.preferredSize(ctx);
        if (tabPreferred.x() < minButtonWidth)
            tabPreferred.x() = minButtonWidth;
        else if (tabPreferred.x() > maxButtonWidth)
            tabPreferred.x() = maxButtonWidth;
        tab.setSize(tabPreferred);
        tab.calculateVisibleString(ctx);
        currentPosition.x() += tabPreferred.x();
    }
    calculateVisibleEnd();
    if (mVisibleStart != 0 || mVisibleEnd != tabCount())
        mOverflowing = true;
}

Vector2i TabHeader::preferredSize(NVGcontext* ctx) const {
    // Set up the nvg context for measuring the text inside the tab buttons.
    nvgFontFace(ctx, mFont.c_str());
    nvgFontSize(ctx, fontSize());
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    Vector2i size = Vector2i(2*controlWidth, 0);
    for (auto& tab : mTabButtons) {
        auto tabPreferred = tab.preferredSize(ctx);
        if (tabPreferred.x() < minButtonWidth)
            tabPreferred.x() = minButtonWidth;
        else if (tabPreferred.x() > maxButtonWidth)
            tabPreferred.x() = maxButtonWidth;
        size.x() += tabPreferred.x();
        size.y() = std::max(size.y(), tabPreferred.y());
    }
    return size;
}

bool TabHeader::mouseButtonEvent(const Vector2i& p, int button, bool down, int modifiers) {
    Widget::mouseButtonEvent(p, button, down, modifiers);

    if (button == GLFW_MOUSE_BUTTON_1 && down) {
        switch (locateClick(p)) {
        case ClickLocation::LeftControls:
            onArrowLeft();
            return true;
        case ClickLocation::RightControls:
            onArrowRight();
            return true;
        case ClickLocation::TabButtons:
            auto first = visibleBegin();
            auto last = visibleEnd();
            int currentPosition = controlWidth;
            int endPosition = p.x();
            auto firstInvisible = std::find_if(first, last,
                                               [&currentPosition, endPosition](const auto& tb)
            {
                currentPosition += tb.size().x();
                return currentPosition > endPosition;
            });
            // Clicked outside of all of the tab buttons.
            if (firstInvisible == last)
                return true;
            int index = std::distance(mTabButtons.begin(), firstInvisible);
            // Update the active tab and invoke the callback.
            setActiveTab(index);
            return true;
        }
    }
    return false;
}

// TODO: add buttons dynamically

void TabHeader::draw(NVGcontext* ctx) {

    // Draw controls
    // Draw inactive visible buttons
    // Draw active visible button
    // Draw border
    // Consider changing colors when the window is focused 
    // (most possible use callback or let the tab widget act 
    // as a mediator which changes some color).

    Widget::draw(ctx);
    if (mOverflowing)
        drawControls(ctx);

    // TODO: border around tab widget seems too large (maybe 1 or 2 instead of 3 px)
    // TODO: try combination of 2 shading colors for raised border

    // Set up common text drawing settings.
    nvgFontFace(ctx, mFont.c_str());
    nvgFontSize(ctx, fontSize());
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);

    auto current = visibleBegin();
    auto last = visibleEnd();
    auto active = std::next(mTabButtons.begin(), mActiveTab);
    Vector2i currentPosition = mPos + Vector2i(controlWidth, 0);
    while (current != last) {
        current->drawAtPosition(ctx, currentPosition, current == active);
        currentPosition.x() += current->size().x();
        ++current;
    }
}

void TabHeader::calculateVisibleEnd() {
    auto first = visibleBegin();
    auto last = mTabButtons.end();
    int currentPosition = controlWidth;
    int lastPosition = mSize.x() - controlWidth;
    auto firstInvisible = std::find_if(first, last,
                                       [&currentPosition, lastPosition](const auto& tb)
    {
        currentPosition += tb.size().x();
        return currentPosition > lastPosition;
    });
    mVisibleEnd = std::distance(mTabButtons.begin(), firstInvisible);
}

void TabHeader::drawControls(NVGcontext* ctx) {
    // Compute bounds for the first button.
    int xPos = mPos.x();
    int yPos = mPos.y();
    int width = controlWidth;
    int height = mSize.y();
    bool active = mVisibleStart != 0;
    // Background gradients
    NVGcolor gradTop = mTheme->mButtonGradientTopUnfocused;
    NVGcolor gradBot = mTheme->mButtonGradientBotUnfocused;
    if (mFocused) {
        gradTop = mTheme->mButtonGradientTopFocused;
        gradBot = mTheme->mButtonGradientBotFocused;
    }
    if (!active) {
        // TODO: Add a separate color for marking disabled.
        gradTop = mTheme->mButtonGradientTopPushed;
        gradBot = mTheme->mButtonGradientBotPushed;
    }
    // Draw the arrow.
    nvgBeginPath(ctx);
    auto iconLeft = utf8(ENTYPO_ICON_LEFT_BOLD);
    int fontSize = mFontSize == -1 ? mTheme->mButtonFontSize : mFontSize;
    float ih = fontSize;
    ih *= 1.5f;
    nvgFontSize(ctx, ih);
    nvgFontFace(ctx, "icons");
    nvgTextBounds(ctx, 0, 0, iconLeft.data(), nullptr, nullptr);
    
    NVGcolor arrowColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
    if (!active)
        arrowColor = mTheme->mButtonGradientBotPushed;
    nvgFillColor(ctx, arrowColor);
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    Vector2f iconPos = mPos.cast<float>() + 0.5f*Vector2f(0.5f*controlWidth, mSize.cast<float>().y()); 
    nvgText(ctx, iconPos.x(), iconPos.y() + 1, iconLeft.data(), nullptr);

    // Compute bounds for the second button.
    xPos = mPos.x() + mSize.x() - controlWidth;
    yPos = mPos.y();
    width = controlWidth;
    height = mSize.y();
    active = mVisibleEnd != tabCount();
    // Background gradients
    gradTop = mTheme->mButtonGradientTopUnfocused;
    gradBot = mTheme->mButtonGradientBotUnfocused;
    // Check if the button should be active
    if (!active) {
        // TODO: Add a separate color for marking disabled.
        gradTop = mTheme->mButtonGradientTopPushed;
        gradBot = mTheme->mButtonGradientBotPushed;
    }
    // Draw the arrow.
    nvgBeginPath(ctx);
    auto iconRight = utf8(ENTYPO_ICON_RIGHT_BOLD);
    fontSize = mFontSize == -1 ? mTheme->mButtonFontSize : mFontSize;
    ih = fontSize;
    ih *= 1.5f;
    nvgFontSize(ctx, ih);
    nvgFontFace(ctx, "icons");
    nvgTextBounds(ctx, 0, 0, iconRight.data(), nullptr, nullptr);
    arrowColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
    if (!active)
        arrowColor = mTheme->mButtonGradientBotPushed;
    nvgFillColor(ctx, arrowColor);
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);

    auto leftControlsPos = mPos.cast<float>() + Vector2f(mSize.cast<float>().x() - controlWidth, 0);
    iconPos = leftControlsPos + 0.5f*Vector2f(0.5f*controlWidth, mSize.cast<float>().y());

    nvgText(ctx, iconPos.x(), iconPos.y() + 1, iconRight.data(), nullptr);


}

TabHeader::ClickLocation TabHeader::locateClick(const Vector2i& p) {
    auto leftDistance = (p - mPos).array();                                                                             
    bool hitLeft = (leftDistance >= 0).all() && (leftDistance < Vector2i(controlWidth, mSize.y()).array()).all();
    if (hitLeft)
        return ClickLocation::LeftControls;
    auto rightDistance = (p - (mPos + Vector2i(mSize.x() - controlWidth, 0))).array();
    bool hitRight = (rightDistance >= 0).all() && (rightDistance < Vector2i(controlWidth, mSize.y()).array()).all();
    if (hitRight)
        return ClickLocation::RightControls;
    return ClickLocation::TabButtons;
}
                                                      
void TabHeader::onArrowLeft() {
    if (mVisibleStart == 0)
        return;
    --mVisibleStart;
    calculateVisibleEnd();
}

void TabHeader::onArrowRight() {
    if (mVisibleEnd == tabCount())
        return;
    ++mVisibleStart;
    calculateVisibleEnd();;
}

NAMESPACE_END(nanogui)
