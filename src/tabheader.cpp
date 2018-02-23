/*
    nanogui/tabheader.cpp -- Widget used to control tabs.

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
#include <numeric>

NAMESPACE_BEGIN(nanogui)

TabHeader::TabButton::TabButton(TabHeader &header, const std::string &label)
    : mHeader(&header), mLabel(label) { }

Vector2i TabHeader::TabButton::preferredSize(NVGcontext *ctx) const {
    // No need to call nvg font related functions since this is done by the tab header implementation
    float bounds[4];
    int labelWidth = nvgTextBounds(ctx, 0, 0, mLabel.c_str(), nullptr, bounds);
    int buttonWidth = labelWidth + 2 * mHeader->theme()->mTabButtonHorizontalPadding;
    int buttonHeight = bounds[3] - bounds[1] + 2 * mHeader->theme()->mTabButtonVerticalPadding;
    return Vector2i(buttonWidth, buttonHeight);
}

void TabHeader::TabButton::calculateVisibleString(NVGcontext *ctx) {
    // The size must have been set in by the enclosing tab header.
    NVGtextRow displayedText;
    nvgTextBreakLines(ctx, mLabel.c_str(), nullptr, mSize.x(), &displayedText, 1);

    // Check to see if the text need to be truncated.
    if (displayedText.next[0]) {
        auto truncatedWidth = nvgTextBounds(ctx, 0.0f, 0.0f,
                                            displayedText.start, displayedText.end, nullptr);
        auto dotsWidth = nvgTextBounds(ctx, 0.0f, 0.0f, dots, nullptr, nullptr);
        while ((truncatedWidth + dotsWidth + mHeader->theme()->mTabButtonHorizontalPadding) > mSize.x()
                && displayedText.end != displayedText.start) {
            --displayedText.end;
            truncatedWidth = nvgTextBounds(ctx, 0.0f, 0.0f,
                                           displayedText.start, displayedText.end, nullptr);
        }

        // Remember the truncated width to know where to display the dots.
        mVisibleWidth = truncatedWidth;
        mVisibleText.last = displayedText.end;
    } else {
        mVisibleText.last = nullptr;
        mVisibleWidth = 0;
    }
    mVisibleText.first = displayedText.start;
}

void TabHeader::TabButton::drawAtPosition(NVGcontext *ctx, const Vector2i& position, bool active) {
    int xPos = position.x();
    int yPos = position.y();
    int width = mSize.x();
    int height = mSize.y();
    auto theme = mHeader->theme();

    nvgSave(ctx);
    nvgIntersectScissor(ctx, xPos, yPos, width+1, height);
    if (!active) {
        // Background gradients
        NVGcolor gradTop = theme->mButtonGradientTopPushed;
        NVGcolor gradBot = theme->mButtonGradientBotPushed;

        // Draw the background.
        nvgBeginPath(ctx);
        nvgRoundedRect(ctx, xPos + 1, yPos + 1, width - 1, height + 1,
                       theme->mButtonCornerRadius);
        NVGpaint backgroundColor = nvgLinearGradient(ctx, xPos, yPos, xPos, yPos + height,
                                                     gradTop, gradBot);
        nvgFillPaint(ctx, backgroundColor);
        nvgFill(ctx);
    }

    if (active) {
        nvgBeginPath(ctx);
        nvgStrokeWidth(ctx, 1.0f);
        nvgRoundedRect(ctx, xPos + 0.5f, yPos + 1.5f, width,
                       height + 1, theme->mButtonCornerRadius);
        nvgStrokeColor(ctx, theme->mBorderLight);
        nvgStroke(ctx);

        nvgBeginPath(ctx);
        nvgRoundedRect(ctx, xPos + 0.5f, yPos + 0.5f, width,
                       height + 1, theme->mButtonCornerRadius);
        nvgStrokeColor(ctx, theme->mBorderDark);
        nvgStroke(ctx);
    } else {
        nvgBeginPath(ctx);
        nvgRoundedRect(ctx, xPos + 0.5f, yPos + 1.5f, width,
                       height, theme->mButtonCornerRadius);
        nvgStrokeColor(ctx, theme->mBorderDark);
        nvgStroke(ctx);
    }
    nvgResetScissor(ctx);
    nvgRestore(ctx);

    // Draw the text with some padding
    int textX = xPos + mHeader->theme()->mTabButtonHorizontalPadding;
    int textY = yPos + mHeader->theme()->mTabButtonVerticalPadding;
    NVGcolor textColor = mHeader->theme()->mTextColor;
    nvgBeginPath(ctx);
    nvgFillColor(ctx, textColor);
    nvgText(ctx, textX, textY, mVisibleText.first, mVisibleText.last);
    if (mVisibleText.last != nullptr)
        nvgText(ctx, textX + mVisibleWidth, textY, dots, nullptr);
}

void TabHeader::TabButton::drawActiveBorderAt(NVGcontext *ctx, const Vector2i &position,
                                              float offset, const Color &color) {
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
    nvgStrokeWidth(ctx, mHeader->theme()->mTabBorderWidth);
    nvgStroke(ctx);
}

void TabHeader::TabButton::drawInactiveBorderAt(NVGcontext *ctx, const Vector2i &position,
                                                float offset, const Color& color) {
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


TabHeader::TabHeader(Widget* parent, const std::string& font)
    : FontWidget(parent, font, true) { }

void TabHeader::setActiveTab(int tabIndex) {
    assert(tabIndex < tabCount());
    mActiveTab = tabIndex;
    if (mCallback)
        mCallback(tabIndex);
}

int TabHeader::activeTab() const {
    return mActiveTab;
}

bool TabHeader::isTabVisible(int index) const {
    return index >= mVisibleStart && index < mVisibleEnd;
}

void TabHeader::addTab(const std::string & label) {
    addTab(tabCount(), label);
}

void TabHeader::addTab(int index, const std::string &label) {
    assert(index <= tabCount());
    mTabButtons.insert(std::next(mTabButtons.begin(), index), TabButton(*this, label));
    setActiveTab(index);
}

int TabHeader::removeTab(const std::string &label) {
    auto element = std::find_if(mTabButtons.begin(), mTabButtons.end(),
                                [&](const TabButton& tb) { return label == tb.label(); });
    int index = (int) std::distance(mTabButtons.begin(), element);
    if (element == mTabButtons.end())
        return -1;
    mTabButtons.erase(element);
    if (index == mActiveTab && index != 0)
        setActiveTab(index - 1);
    return index;
}

void TabHeader::removeTab(int index) {
    assert(index < tabCount());
    mTabButtons.erase(std::next(mTabButtons.begin(), index));
    if (index == mActiveTab && index != 0)
        setActiveTab(index - 1);
}

const std::string& TabHeader::tabLabelAt(int index) const {
    assert(index < tabCount());
    return mTabButtons[index].label();
}

int TabHeader::tabIndex(const std::string &label) {
    auto it = std::find_if(mTabButtons.begin(), mTabButtons.end(),
                           [&](const TabButton& tb) { return label == tb.label(); });
    if (it == mTabButtons.end())
        return -1;
    return (int) (it - mTabButtons.begin());
}

void TabHeader::ensureTabVisible(int index) {
    auto visibleArea = visibleButtonArea();
    auto visibleWidth = visibleArea.second.x() - visibleArea.first.x();
    int allowedVisibleWidth = mSize.x() - 2 * theme()->mTabControlWidth;
    assert(allowedVisibleWidth >= visibleWidth);
    assert(index >= 0 && index < (int) mTabButtons.size());

    auto first = visibleBegin();
    auto last = visibleEnd();
    auto goal = tabIterator(index);

    // Reach the goal tab with the visible range.
    if (goal < first) {
        do {
            --first;
            visibleWidth += first->size().x();
        } while (goal < first);
        while (allowedVisibleWidth < visibleWidth) {
            --last;
            visibleWidth -= last->size().x();
        }
    }
    else if (goal >= last) {
        do {
            visibleWidth += last->size().x();
            ++last;
        } while (goal >= last);
        while (allowedVisibleWidth < visibleWidth) {
            visibleWidth -= first->size().x();
            ++first;
        }
    }

    // Check if it is possible to expand the visible range on either side.
    while (first != mTabButtons.begin()
           && std::next(first, -1)->size().x() < allowedVisibleWidth - visibleWidth) {
        --first;
        visibleWidth += first->size().x();
    }
    while (last != mTabButtons.end()
           && last->size().x() < allowedVisibleWidth - visibleWidth) {
        visibleWidth += last->size().x();
        ++last;
    }

    mVisibleStart = (int) std::distance(mTabButtons.begin(), first);
    mVisibleEnd = (int) std::distance(mTabButtons.begin(), last);
}

std::pair<Vector2i, Vector2i> TabHeader::visibleButtonArea() const {
    if (mVisibleStart == mVisibleEnd)
        return { Vector2i::Zero(), Vector2i::Zero() };
    auto topLeft = mPos + Vector2i(theme()->mTabControlWidth, 0);
    auto width = std::accumulate(visibleBegin(), visibleEnd(), theme()->mTabControlWidth,
                                 [](int acc, const TabButton& tb) {
        return acc + tb.size().x();
    });
    auto bottomRight = mPos + Vector2i(width, mSize.y());
    return { topLeft, bottomRight };
}

std::pair<Vector2i, Vector2i> TabHeader::activeButtonArea() const {
    if (mVisibleStart == mVisibleEnd || mActiveTab < mVisibleStart || mActiveTab >= mVisibleEnd)
        return { Vector2i::Zero(), Vector2i::Zero() };
    auto width = std::accumulate(visibleBegin(), activeIterator(), theme()->mTabControlWidth,
                                 [](int acc, const TabButton& tb) {
        return acc + tb.size().x();
    });
    auto topLeft = mPos + Vector2i(width, 0);
    auto bottomRight = mPos + Vector2i(width + activeIterator()->size().x(), mSize.y());
    return { topLeft, bottomRight };
}

void TabHeader::performLayout(NVGcontext* ctx) {
    Widget::performLayout(ctx);

    Vector2i currentPosition = Vector2i::Zero();
    // Place the tab buttons relative to the beginning of the tab header.
    for (auto& tab : mTabButtons) {
        auto tabPreferred = tab.preferredSize(ctx);
        if (tabPreferred.x() < theme()->mTabMinButtonWidth)
            tabPreferred.x() = theme()->mTabMinButtonWidth;
        else if (tabPreferred.x() > theme()->mTabMaxButtonWidth)
            tabPreferred.x() = theme()->mTabMaxButtonWidth;
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
    Vector2i size = Vector2i(2*theme()->mTabControlWidth, 0);
    for (auto& tab : mTabButtons) {
        auto tabPreferred = tab.preferredSize(ctx);
        if (tabPreferred.x() < theme()->mTabMinButtonWidth)
            tabPreferred.x() = theme()->mTabMinButtonWidth;
        else if (tabPreferred.x() > theme()->mTabMaxButtonWidth)
            tabPreferred.x() = theme()->mTabMaxButtonWidth;
        size.x() += tabPreferred.x();
        size.y() = std::max(size.y(), tabPreferred.y());
    }
    return size;
}

bool TabHeader::mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) {
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
            int currentPosition = theme()->mTabControlWidth;
            int endPosition = p.x();
            auto firstInvisible = std::find_if(first, last,
                                               [&currentPosition, endPosition](const TabButton& tb) {
                currentPosition += tb.size().x();
                return currentPosition > endPosition;
            });

            // Did not click on any of the tab buttons
            if (firstInvisible == last)
                return true;

            // Update the active tab and invoke the callback.
            setActiveTab((int) std::distance(mTabButtons.begin(), firstInvisible));
            return true;
        }
    }
    return false;
}

void TabHeader::draw(NVGcontext* ctx) {
    // Draw controls.
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
    Vector2i currentPosition = mPos + Vector2i(theme()->mTabControlWidth, 0);

    // Flag to draw the active tab last. Looks a little bit better.
    bool drawActive = false;
    Vector2i activePosition = Vector2i::Zero();

    // Draw inactive visible buttons.
    while (current != last) {
        if (current == active) {
            drawActive = true;
            activePosition = currentPosition;
        } else {
            current->drawAtPosition(ctx, currentPosition, false);
        }
        currentPosition.x() += current->size().x();
        ++current;
    }

    // Draw active visible button.
    if (drawActive)
        active->drawAtPosition(ctx, activePosition, true);
}

void TabHeader::calculateVisibleEnd() {
    auto first = visibleBegin();
    auto last = mTabButtons.end();
    int currentPosition = theme()->mTabControlWidth;
    int lastPosition = mSize.x() - theme()->mTabControlWidth;
    auto firstInvisible = std::find_if(first, last,
                                       [&currentPosition, lastPosition](const TabButton& tb) {
        currentPosition += tb.size().x();
        return currentPosition > lastPosition;
    });
    mVisibleEnd = (int) std::distance(mTabButtons.begin(), firstInvisible);
}

void TabHeader::drawControls(NVGcontext* ctx) {
    // Left button.
    bool active = mVisibleStart != 0;

    // Draw the arrow.
    nvgBeginPath(ctx);
    auto iconLeft = utf8(mTheme->mTabHeaderLeftIcon);
    int fontSize = mFontSize == -1 ? mTheme->mButtonFontSize : mFontSize;
    float ih = fontSize;
    ih *= icon_scale();
    nvgFontSize(ctx, ih);
    nvgFontFace(ctx, mIconFont.c_str());
    NVGcolor arrowColor;
    if (active)
        arrowColor = mTheme->mTextColor;
    else
        arrowColor = mTheme->mButtonGradientBotPushed;
    nvgFillColor(ctx, arrowColor);
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    float yScaleLeft = 0.5f;
    float xScaleLeft = 0.2f;
    Vector2f leftIconPos = mPos.cast<float>() + Vector2f(xScaleLeft*theme()->mTabControlWidth, yScaleLeft*mSize.cast<float>().y());
    nvgText(ctx, leftIconPos.x(), leftIconPos.y() + 1, iconLeft.data(), nullptr);

    // Right button.
    active = mVisibleEnd != tabCount();
    // Draw the arrow.
    nvgBeginPath(ctx);
    auto iconRight = utf8(mTheme->mTabHeaderRightIcon);
    fontSize = mFontSize == -1 ? mTheme->mButtonFontSize : mFontSize;
    ih = fontSize;
    ih *= icon_scale();
    nvgFontSize(ctx, ih);
    nvgFontFace(ctx, mIconFont.c_str());
    float rightWidth = nvgTextBounds(ctx, 0, 0, iconRight.data(), nullptr, nullptr);
    if (active)
        arrowColor = mTheme->mTextColor;
    else
        arrowColor = mTheme->mButtonGradientBotPushed;
    nvgFillColor(ctx, arrowColor);
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    float yScaleRight = 0.5f;
    float xScaleRight = 1.0f - xScaleLeft - rightWidth / theme()->mTabControlWidth;
    auto leftControlsPos = mPos.cast<float>() + Vector2f(mSize.cast<float>().x() - theme()->mTabControlWidth, 0);
    Vector2f rightIconPos = leftControlsPos + Vector2f(xScaleRight*theme()->mTabControlWidth, yScaleRight*mSize.cast<float>().y());
    nvgText(ctx, rightIconPos.x(), rightIconPos.y() + 1, iconRight.data(), nullptr);
}

TabHeader::ClickLocation TabHeader::locateClick(const Vector2i& p) {
    auto leftDistance = (p - mPos).array();
    bool hitLeft = (leftDistance >= 0).all() && (leftDistance < Vector2i(theme()->mTabControlWidth, mSize.y()).array()).all();
    if (hitLeft)
        return ClickLocation::LeftControls;
    auto rightDistance = (p - (mPos + Vector2i(mSize.x() - theme()->mTabControlWidth, 0))).array();
    bool hitRight = (rightDistance >= 0).all() && (rightDistance < Vector2i(theme()->mTabControlWidth, mSize.y()).array()).all();
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
    calculateVisibleEnd();
}

NAMESPACE_END(nanogui)
