/*
    nanogui/tabheader.h -- Widget used to control tabs.

    The tab header widget was contributed by Stefan Ivanov.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/widget.h>
#include <vector>
#include <string>
#include <functional>
#include <utility>
#include <iterator>

NAMESPACE_BEGIN(nanogui)

/**
 * \class TabHeader tabheader.h nanogui/tabheader.h
 *
 * \brief A Tab navigable widget.
 */
class NANOGUI_EXPORT TabHeader : public Widget {
public:
    TabHeader(Widget *parent, const std::string &font = "sans-bold");

    void setFont(const std::string& font) { mFont = font; }
    const std::string& font() const { return mFont; }
    bool overflowing() const { return mOverflowing; }

    /**
     * Sets the callable objects which is invoked when a tab button is pressed.
     * The argument provided to the callback is the index of the tab.
     */
    void setCallback(const std::function<void(int)>& callback) { mCallback = callback; };
    const std::function<void(int)>& callback() const { return mCallback; }

    void setActiveTab(int tabIndex);
    int activeTab() const;
    bool isTabVisible(int index) const;
    int tabCount() const { return (int) mTabButtons.size();  }

    /// Inserts a tab at the end of the tabs collection.
    void addTab(const std::string& label);

    /// Inserts a tab into the tabs collection at the specified index.
    void addTab(int index, const std::string& label);

    /**
     * Removes the tab with the specified label and returns the index of the label.
     * Returns -1 if there was no such tab
     */
    int removeTab(const std::string& label);

    /// Removes the tab with the specified index.
    void removeTab(int index);

    /// Retrieves the label of the tab at a specific index.
    const std::string& tabLabelAt(int index) const;

    /**
     * Retrieves the index of a specific tab label.
     * Returns the number of tabs (tabsCount) if there is no such tab.
     */
    int tabIndex(const std::string& label);

    /**
     * Recalculate the visible range of tabs so that the tab with the specified
     * index is visible. The tab with the specified index will either be the
     * first or last visible one depending on the position relative to the
     * old visible range.
     */
    void ensureTabVisible(int index);

    /**
     * Returns a pair of Vectors describing the top left (pair.first) and the
     * bottom right (pair.second) positions of the rectangle containing the visible tab buttons.
     */
    std::pair<Vector2i, Vector2i> visibleButtonArea() const;

    /**
     * Returns a pair of Vectors describing the top left (pair.first) and the
     * bottom right (pair.second) positions of the rectangle containing the active tab button.
     * Returns two zero vectors if the active button is not visible.
     */
    std::pair<Vector2i, Vector2i> activeButtonArea() const;

    virtual void performLayout(NVGcontext* ctx) override;
    virtual Vector2i preferredSize(NVGcontext* ctx) const override;
    virtual bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override;

    virtual void draw(NVGcontext* ctx) override;

private:
    /**
     * \class TabButton tabheader.h
     *
     * \brief Implementation class of the actual tab buttons.
     */
    class TabButton {
    public:
        constexpr static const char* dots = "...";

        TabButton(TabHeader& header, const std::string& label);

        void setLabel(const std::string& label) { mLabel = label; }
        const std::string& label() const { return mLabel; }
        void setSize(const Vector2i& size) { mSize = size; }
        const Vector2i& size() const { return mSize; }

        Vector2i preferredSize(NVGcontext* ctx) const;
        void calculateVisibleString(NVGcontext* ctx);
        void drawAtPosition(NVGcontext* ctx, const Vector2i& position, bool active);
        void drawActiveBorderAt(NVGcontext * ctx, const Vector2i& position, float offset, const Color& color);
        void drawInactiveBorderAt(NVGcontext * ctx, const Vector2i& position, float offset, const Color& color);

    private:
        TabHeader* mHeader;
        std::string mLabel;
        Vector2i mSize;

        /**
         * \struct StringView tabheader.h nanogui/tabheader.h
         *
         * \brief Helper struct to represent the TabButton.
         */
        struct StringView {
            const char* first = nullptr;
            const char* last = nullptr;
        };
        StringView mVisibleText;
        int mVisibleWidth = 0;
    };

    using TabIterator = std::vector<TabButton>::iterator;
    using ConstTabIterator = std::vector<TabButton>::const_iterator;

    /// The location in which the Widget will be facing.
    enum class ClickLocation {
        LeftControls, RightControls, TabButtons
    };

    TabIterator visibleBegin() { return std::next(mTabButtons.begin(), mVisibleStart); }
    TabIterator visibleEnd() { return std::next(mTabButtons.begin(), mVisibleEnd); }
    TabIterator activeIterator() { return std::next(mTabButtons.begin(), mActiveTab); }
    TabIterator tabIterator(int index) { return std::next(mTabButtons.begin(), index); }

    ConstTabIterator visibleBegin() const { return std::next(mTabButtons.begin(), mVisibleStart); }
    ConstTabIterator visibleEnd() const { return std::next(mTabButtons.begin(), mVisibleEnd); }
    ConstTabIterator activeIterator() const { return std::next(mTabButtons.begin(), mActiveTab); }
    ConstTabIterator tabIterator(int index) const { return std::next(mTabButtons.begin(), index); }

    /// Given the beginning of the visible tabs, calculate the end.
    void calculateVisibleEnd();

    void drawControls(NVGcontext* ctx);
    ClickLocation locateClick(const Vector2i& p);
    void onArrowLeft();
    void onArrowRight();

    std::function<void(int)> mCallback;
    std::vector<TabButton> mTabButtons;
    int mVisibleStart = 0;
    int mVisibleEnd = 0;
    int mActiveTab = 0;
    bool mOverflowing = false;

    std::string mFont;
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

NAMESPACE_END(nanogui)
