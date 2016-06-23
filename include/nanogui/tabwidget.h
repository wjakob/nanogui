/*
    nanogui/tabwidget.h -- A wrapper around the widgets TabHeader and StackedWidget
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

class NANOGUI_EXPORT TabWidget : public Widget {
public:
    TabWidget(Widget* parent);

    void setActiveTab(int tabIndex);
    int activeTab() const;
    int tabCount() const;

    /// TODO: combine with new construction add operation (added by Wenzel in a bit ;))
    
    /// Forward functions for tabLabelAt() etc.

    /// Inserts a tab at the end of the tabs collection and associates it with the provided widget.
    void addTab(Widget* tab, const std::string& name);
    /// Inserts a tab into the tabs collection at the specified index and associates it with the provided widget.
    void addTab(int index, Widget* tab, const std::string& tablabel);
    /// Removes the tab with the specified label and returns the index of the label. 
    /// Returns whether the removal was successful.
    bool removeTab(const std::string& tabLabel);
    /// Removes the tab with the specified index.
    void removeTab(int index);
    /// Retrieves the label of the tab at a specific index.
    const std::string& tabLabelAt(int index) const;
    /// Retrieves the index of a specific tab using its tab label. 
    /// Returns the number of tabs (tabsCount) if there is no such tab.
    int tabLabelIndex(const std::string& tabLabel);

    const Widget* getTab(const std::string& tabName) const;
    Widget* getTab(const std::string& tabName);

    
    virtual void performLayout(NVGcontext* ctx) override;
    virtual Vector2i preferredSize(NVGcontext* ctx) const override;
    
    virtual void draw(NVGcontext* ctx) override;

    /// TODO: add callback for tab activation
private:
    /// Draws a border around the content that is indented by the offset provided.
    /// A larger offset means a "tighter" border.
    void drawBorder(NVGcontext* ctx, float offset, const Color& borderColor);

    TabHeader* mHeader;
    StackedWidget* mContent;
};



NAMESPACE_END(nanogui)