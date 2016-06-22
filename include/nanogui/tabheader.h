/*
    nanogui/tabheader.h -- Widget used to control tabs.

    The tab header widget was contributed by Stefan Ivanov.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#pragma once

#include <nanogui/widget.h>
#include <vector>
#include <string>
#include <functional>
#include <utility>

NAMESPACE_BEGIN(nanogui)

class TabButton {
public:
    constexpr static int horizontalPadding = 10;
    constexpr static int verticalPadding = 2;
    constexpr static char* dots = "...";

    TabButton(TabHeader& header, const std::string& label);

    void setLabel(const std::string& label) { mLabel = label; }
    const std::string& label() const { return mLabel; }
    void setSize(const Vector2i& size) { mSize = size; }
    const Vector2i& size() const { return mSize; }

    Vector2i preferredSize(NVGcontext* ctx) const;
    void calculateVisibleString(NVGcontext* ctx);
    void TabButton::drawAtPosition(NVGcontext* ctx, const Vector2i& position, bool active);
private:
    TabHeader* mHeader;
    std::string mLabel;
    Vector2i mSize;
    struct StringView {
        const char* first = nullptr;
        const char* last = nullptr;
    };
    StringView mVisibleText;
    int mVisibleWidth = 0;
};

class NANOGUI_EXPORT TabHeader : public Widget {
public:
    constexpr static int minButtonWidth = 20;
    constexpr static int maxButtonWidth = 160;
    constexpr static int topBottomMargin = 3;
    constexpr static int tabPadding = 10;
    constexpr static int controlsWidth = 20;
    constexpr static int controlsContentWidth = 15;

    TabHeader(Widget* parent, const std::string &font = "sans-bold",
                    int fontSize = -1, Color fontColor = Color(1.f, 1.f, 1.f, 1.f));
    
    void setFont(const std::string& font) { mFont = font; }
    const std::string& font() const { return mFont; }
    void setFontColor(const Color& fontColor) { mFontColor = fontColor; }
    const Color& fontColor() const { return mFontColor; }
    bool overflowing() const { return mOverflowing; }
    /// Sets the callable objects which is invoked when a tab button is pressed.
    /// The argument provided to the call back is the index of the tab.
    void setCallback(const std::function<void(int)>& callback) { mCallback = callback; };
    const std::function<void(int)>& callback() const { return mCallback; }

    void setActiveTab(int tabIndex);
    int activeTab() const;
    int labelCount() const { return mTabButtons.size();  }

    /// Insert a new tab button with the specified text.    
    void addTabButton(const std::string& tabLabel);
    /// Removes the tab label with the specified name and returns the index of
    /// the label. If it does not find it returns the number of labels (labelCount).
    int removeTabButton(const std::string& tabLabel);
    const std::string& tabLabelAt(int index) const;
    int tabLabelIndex(const std::string& tabLabel);
    std::pair<Vector2i, Vector2i> visibleButtonsArea() const;

    virtual void performLayout(NVGcontext* ctx) override;
    virtual Vector2i preferredSize(NVGcontext* ctx) const override;
    virtual bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override;

    virtual void draw(NVGcontext* ctx) override;

private:
    using TabIterator = std::vector<TabButton>::iterator;
    using ConstTabIterator = std::vector<TabButton>::const_iterator;
    enum class ClickLocation {
        LeftControls, RightControls, TabButtons
    };

    TabIterator visibleBegin() { return std::next(mTabButtons.begin(), mVisibleStart); }
    TabIterator visibleEnd() { return std::next(mTabButtons.begin(), mVisibleEnd); }
    ConstTabIterator visibleBegin() const { return std::next(mTabButtons.begin(), mVisibleStart); }
    ConstTabIterator visibleEnd() const { return std::next(mTabButtons.begin(), mVisibleEnd); }
    void calculateVisibleEnd();
    void drawControls(NVGcontext* ctx);
    ClickLocation locateClick(const Vector2i& p);
    void leftControlsClicked();
    void rightControlsClicked();

    std::function<void(int)> mCallback;
    std::vector<TabButton> mTabButtons;
    int mVisibleStart = 0;
    int mVisibleEnd = 0;
    int mActiveTab = 0;
    bool mOverflowing = false;

    std::string mFont;
    Color mFontColor;
};

NAMESPACE_END(nanogui)