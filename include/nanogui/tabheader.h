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

class NANOGUI_EXPORT TabHeader : public Widget {
public:
    constexpr static int minButtonWidth = 20;
    constexpr static int maxButtonWidth = 160;
    constexpr static int topBottomMargin = 3;
    constexpr static int tabPadding = 10;
    constexpr static int controlsWidth = 20;

    using size_type = std::vector<std::string>::size_type;

    TabHeader(nanogui::Widget* parent, const std::string &font = "sans",
                    int fontSize = -1, Color fontColor = Color(1.f, 1.f, 1.f, 1.f));
    
    void setFont(const std::string& font) { mFont = font; }
    const std::string font() { return mFont; }
    void setFontColor(const Color& fontColor) { mFontColor = fontColor; }
    const Color& fontColor() { return mFontColor; }

    void setActiveTab(int tabIndex);
    int activeTab() const;
    int labelCount() const { return mTabLabels.size();  }
    /// Sets the callable objects which is invoked when a tab is pressed
    /// with the index of the tab.
    void setCallback(const std::function<void(int)>& callback);
    const std::function<void(int)>& callback() const;
    /// Insert a new tab button with the specified text.    
    void addTabLabel(const std::string& tabLabel);
    std::string& tabLabelAt(size_type index);
    int tabLabelIndex(const std::string& tabLabel);
    /// Removes the tab label with the specified name and returns the index of
    /// the label. If it does not find it returns the number of labels (labeCount).
    int removeTabLabel(const std::string& tabLabel);

    virtual void performLayout(NVGcontext* ctx) override;
    virtual Vector2i preferredSize(NVGcontext* ctx) const override;
    virtual bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override;

    virtual void draw(NVGcontext* ctx) override;

private:
    /// Helper enumeration class for determining the direction of the controls.
    enum class ControlsClicked {
        Left, Right, None
    };

    struct TabButton {
        std::string label;
        int x1;
        int x2;
    };

    std::vector<TabButton> mTabButtons;
    

    /// Old design

    void updateInternalState(NVGcontext* ctx);

    void drawHitBox(NVGcontext* ctx, int index);
    void calculateVisibleEnd();
    void drawControls(NVGcontext* ctx);
    ControlsClicked isInsideControls(const Vector2i& p);
    void leftControlsClicked();
    void rightControlsClicked();

    std::function<void(int)> mCallback;
    std::vector<std::string> mTabLabels;
    mutable std::vector<std::pair<int, int>> mTabLabelExtents;
    int mVisibleStart = 0;
    int mVisibleEnd = 0;
    
    int mActiveTab = 0;
    bool mIsOverflowing = false;

    std::string mFont;
    Color mFontColor;
    
    bool mInternalStateValid = false;
};



NAMESPACE_END(nanogui)