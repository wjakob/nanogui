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
        drawInactiveBorderAt(ctx, position, 1.0f, mHeader->theme()->mBorderDark);
    } else {
        // Draw a dark and light borders for a elevated look.
        float darkOffset = 1.2f;
        float lightOffset = 0.0f;
        drawActiveBorderAt(ctx, position, darkOffset, mHeader->theme()->mBorderDark);
        drawActiveBorderAt(ctx, position, lightOffset, mHeader->theme()->mBorderLight);
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

void TabHeader::TabButton::drawActiveBorderAt(NVGcontext * ctx, const Vector2i& position,
                                              float offset, const Color& color)
{
    int xPos = position.x();
    int yPos = position.y();
    int width = mSize.x();
    int height = mSize.y();
    nvgBeginPath(ctx);
    nvgLineJoin(ctx, NVG_ROUND);
    nvgMoveTo(ctx, xPos + offset, yPos + height + offset);
    nvgLineTo(ctx, xPos + offset, yPos + offset);
    nvgLineTo(ctx, xPos + width - offset, yPos + offset);
    nvgLineTo(ctx, xPos + width - offset, yPos + height + offset);
    nvgStrokeColor(ctx, color);
    nvgStroke(ctx);
}

void TabHeader::TabButton::drawInactiveBorderAt(NVGcontext * ctx, const Vector2i& position, 
                                                float offset, const Color& color)
{
    int xPos = position.x();
    int yPos = position.y();
    int width = mSize.x();
    int height = mSize.y();
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, xPos + offset, yPos + offset, width - offset, height - offset,
                   mHeader->theme()->mButtonCornerRadius);
    nvgStrokeColor(ctx, color);
    nvgStroke(ctx);
}


TabHeader::TabHeader(Widget* parent, const std::string& font,
                     int fontSize)
    : Widget(parent), mFont(font) {
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

void TabHeader::addTab(const std::string & tabLabel) {
    mTabButtons.push_back(TabButton(*this, tabLabel));
}

void TabHeader::addTab(int index, const std::string & tabLabel)
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

std::pair<Vector2i, Vector2i> TabHeader::activeButtonArea() const
{
    if (mVisibleStart == mVisibleEnd || mActiveTab < mVisibleStart || mActiveTab >= mVisibleEnd)
        return { Vector2i::Zero(), Vector2i::Zero() };
    auto width = std::accumulate(visibleBegin(), activePosition(), controlWidth,
                                 [](auto acc, auto& tb) {
        return acc + tb.size().x();
    });
    auto topLeft = mPos + Vector2i(width, 0);
    auto bottomRigth = mPos + Vector2i(width + activePosition()->size().x(), mSize.y());
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
    // TODO: Consider changing colors when the window is focused 
    // (most possible use callback or let the tab widget act 
    // as a mediator which changes some color).
    
    // Draw controls
    Widget::draw(ctx);
    if (mOverflowing)
        drawControls(ctx);

    // Set up common text drawing settings.
    nvgFontFace(ctx, mFont.c_str());
    nvgFontSize(ctx, fontSize());
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);

    auto current = visibleBegin();
    auto last = visibleEnd();
    auto active = std::next(mTabButtons.begin(), mActiveTab);
    Vector2i currentPosition = mPos + Vector2i(controlWidth, 0);
    // Flag to draw the active tab last. Looks a little bit better.
    bool drawActive = false;
    Vector2i activePosition = Vector2i::Zero();
    // Draw inactive visible buttons
    while (current != last) {
        if (current == active) {
            drawActive = true;
            activePosition = currentPosition;
        }
        current->drawAtPosition(ctx, currentPosition, false);
        currentPosition.x() += current->size().x();
        ++current;
    }
    // Draw active visible button
    if (drawActive)
        active->drawAtPosition(ctx, activePosition, true);
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
    // Left button.
    bool active = mVisibleStart != 0;
    // Draw the arrow.
    nvgBeginPath(ctx);
    auto iconLeft = utf8(ENTYPO_ICON_LEFT_BOLD);
    int fontSize = mFontSize == -1 ? mTheme->mButtonFontSize : mFontSize;
    float ih = fontSize;
    ih *= 1.5f;
    nvgFontSize(ctx, ih);
    nvgFontFace(ctx, "icons");
    //float leftWidth = nvgTextBounds(ctx, 0, 0, iconLeft.data(), nullptr, nullptr);
    NVGcolor arrowColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
    if (!active)
        arrowColor = mTheme->mButtonGradientBotPushed;
    nvgFillColor(ctx, arrowColor);
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    float yScaleLeft = 0.5f;
    float xScaleLeft = 0.2f;
    Vector2f leftIconPos = mPos.cast<float>() + Vector2f(xScaleLeft*controlWidth, yScaleLeft*mSize.cast<float>().y());
    nvgText(ctx, leftIconPos.x(), leftIconPos.y() + 1, iconLeft.data(), nullptr);

    // Right button.
    active = mVisibleEnd != tabCount();
    // Draw the arrow.
    nvgBeginPath(ctx);
    auto iconRight = utf8(ENTYPO_ICON_RIGHT_BOLD);
    fontSize = mFontSize == -1 ? mTheme->mButtonFontSize : mFontSize;
    ih = fontSize;
    ih *= 1.5f;
    nvgFontSize(ctx, ih);
    nvgFontFace(ctx, "icons");
    float rightWidth = nvgTextBounds(ctx, 0, 0, iconRight.data(), nullptr, nullptr);
    arrowColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
    if (!active)
        arrowColor = mTheme->mButtonGradientBotPushed;
    nvgFillColor(ctx, arrowColor);
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    float yScaleRight = 0.5f;
    float xScaleRight = 1.0f - xScaleLeft - rightWidth / controlWidth;
    auto leftControlsPos = mPos.cast<float>() + Vector2f(mSize.cast<float>().x() - controlWidth, 0);
    Vector2f rightIconPos = leftControlsPos + Vector2f(xScaleRight*controlWidth, yScaleRight*mSize.cast<float>().y());
    nvgText(ctx, rightIconPos.x(), rightIconPos.y() + 1, iconRight.data(), nullptr);
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
