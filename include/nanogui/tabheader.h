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

class NANOGUI_EXPORT TabHeaderWidget : public Widget {
public:
    constexpr static int minButtonWidth = 20;
    constexpr static int maxButtonWidth = 100;
    constexpr static int buttonHeight = 100;
    constexpr static int tabMargin = 15;

    using size_type = std::vector<std::string>::size_type;

    TabHeaderWidget(nanogui::Widget* parent, 
                    const std::string &font = "sans", int fontSize = -1);

    void setActiveTab(int tabIndex);
    int activeTab() const;

    /// Sets the callable objects which is invoked when a tab is pressed
    /// with the index of the tab.
    void setCallback(const std::function<void(int)>& callback);
    const std::function<void(int)>& callback() const;
    /// Insert a new tab button with the specified text.    
    void addTab(const std::string& tabLabel);
    std::string& tabLabelAt(size_type index);

    virtual void performLayout(NVGcontext* ctx) override;
    virtual Vector2i preferredSize(NVGcontext* ctx) const override;
    virtual bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override;

    virtual void draw(NVGcontext* ctx) override;

private:
    std::function<void(int)> mCallback;
    std::vector<std::string> mTabLabels;
    std::vector<std::pair<int, int>> mTabLabelExtents;
    bool extentsNeedUpdate = true;
    std::string mFont;
    int mActiveTab = 0;
};



NAMESPACE_END(nanogui)