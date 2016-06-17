/*
    nanogui/tab.h -- A wrapper around the widgets TabHeader and Stacked
    which hooks the two classes together.

    The tab widget was contributed by Stefan Ivanov.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#pragma once

#include <nanogui/widget.h>

NAMESPACE_BEGIN(nanogui)

class NANOGUI_EXPORT Tab : public Widget {
public:
    constexpr static int contentBorder = 20;

    Tab(Widget* parent);

    void setActiveTab(int tabIndex);
    int activeTab() const;

    /// Insert a new tab button with the specified text.    
    void addTab(Widget* tab, const std::string& name);
    const Widget* getTab(const std::string& tabName) const;
    Widget* getTab(const std::string& tabName);
    bool removeTab(const std::string& tabName);
    
    virtual void performLayout(NVGcontext* ctx) override;
    virtual Vector2i preferredSize(NVGcontext* ctx) const override;
    
    virtual void draw(NVGcontext* ctx) override;

private:
    TabHeader* mHeader;
    Stacked* mContent;
};



NAMESPACE_END(nanogui)