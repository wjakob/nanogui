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


class NANOGUI_EXPORT TabButton : public Widget {
public:
    constexpr static int horizontalPadding = 10;
    constexpr static int verticalPadding = 2;
    constexpr static char* dots = "...";


    TabButton(Widget* header, const std::string& label)
        : Widget(header), mLabel(label)
    {}

    void setLabel(const std::string& label) { mLabel = label; }
    const std::string& label() const { return mLabel; }
    
    virtual Vector2i preferredSize(NVGcontext* ctx) const override;
    virtual void performLayout(NVGcontext* ctx) override;
    virtual bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override;
    virtual void draw(NVGcontext* ctx) override;

private:
    std::string mLabel;

    struct StringView {
        const char* first = nullptr;
        const char* last = nullptr;
    };
    
    StringView mVisibleText;
    int mVisibleWidth;
};


class NANOGUI_EXPORT TabHeader : public Widget {
public:
    constexpr static int minButtonWidth = 20;
    constexpr static int maxButtonWidth = 160;
    constexpr static int topBottomMargin = 3;
    constexpr static int tabPadding = 10;
    constexpr static int controlsWidth = 20;

    TabHeader(Widget* parent, const std::string &font = "sans",
                    int fontSize = -1, Color fontColor = Color(1.f, 1.f, 1.f, 1.f));
    
    void setFont(const std::string& font) { mFont = font; }
    const std::string& font() { return mFont; }
    void setFontColor(const Color& fontColor) { mFontColor = fontColor; }
    const Color& fontColor() { return mFontColor; }

    void setActiveTab(int tabIndex);
    int activeTab() const;
    int labelCount() const { return mChildren.size();  }
    /// Sets the callable objects which is invoked when a tab is pressed
    /// with the index of the tab.
    void setCallback(const std::function<void(int)>& callback);
    const std::function<void(int)>& callback() const;
    /// Insert a new tab button with the specified text.    
    void addTabLabel(const std::string& tabLabel);
    const std::string& tabLabelAt(int index);
    int tabLabelIndex(const std::string& tabLabel);
    /// Removes the tab label with the specified name and returns the index of
    /// the label. If it does not find it returns the number of labels (labelCount).
    int removeTabLabel(const std::string& tabLabel);

    virtual void performLayout(NVGcontext* ctx) override;
    virtual Vector2i preferredSize(NVGcontext* ctx) const override;
    virtual bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override;

    virtual void draw(NVGcontext* ctx) override;

private:

    enum class ControlsClicked {
        Left, Right, None
    };

    void calculateVisibleEnd();
    void updateVisibility();


    void drawControls(NVGcontext* ctx);
    ControlsClicked isInsideControls(const Vector2i& p);
    void leftControlsClicked();
    void rightControlsClicked();

    std::function<void(int)> mCallback;
    
    int mVisibleStart = 0;
    int mVisibleEnd = 0;
    
    int mActiveTab = 0;
    bool mOverflowing = false;

    std::string mFont;
    Color mFontColor;
};



NAMESPACE_END(nanogui)