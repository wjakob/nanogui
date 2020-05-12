/*
    nanogui/vscrollpanel.h -- Adds a vertical scrollbar around a widget
    that is too big to fit into a certain area

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/widget.h>

NAMESPACE_BEGIN(nanogui)

/**
 * \class VScrollPanel vscrollpanel.h nanogui/vscrollpanel.h
 *
 * \brief Adds a vertical scrollbar around a widget that is too big to fit into
 *        a certain area.
 */
class NANOGUI_EXPORT VScrollPanel : public Widget {
public:
    RTTI_CLASS_UID(VScrollPanel)
    RTTI_DECLARE_INFO(VScrollPanel)
    WIDGET_COMMON_FUNCTIONS(VScrollPanel)

    VScrollPanel(Widget *parent);

    using Widget::set;
    template<typename... Args>
    VScrollPanel(Widget* parent, const Args&... args)
      : VScrollPanel(parent) { set<VScrollPanel, Args...>(args...); }

    /// Return the current scroll amount as a value between 0 and 1. 0 means scrolled to the top and 1 to the bottom.
    float scroll() const { return mScroll; }
    /// Set the scroll amount to a value between 0 and 1. 0 means scrolled to the top and 1 to the bottom.
    void setScroll(float scroll) { mScroll = scroll; }

    void performLayout(NVGcontext *ctx) override;
    Vector2i preferredSize(NVGcontext *ctx) const override;
    bool mouseDragEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) override;
    bool mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) override;
    bool mouseEnterEvent(const Vector2i &p, bool enter) override;
    bool scrollEvent(const Vector2i &p, const Vector2f &rel) override;
    void draw(NVGcontext *ctx) override;
    void save(Json::value &s) const override;
    bool load(Json::value &s) override;
    int getSliderWidth() const { return mSliderWidth; }
    int getSliderAreaWidth() const { return mSliderWidth + mSliderMargin * 2; }
    bool isSliderVisible() const;
protected:
    int mChildPreferredHeight;
    int mLastChildCount = -1;
    float mScroll;
    bool mUpdateLayout;
    Vector2i mLastMousePos;
    int mSliderWidth;
    int mSliderMargin;
    Color mSliderInactiveColor, mSliderActiveColor;
};

namespace elm { using VScrollPanel = Element<VScrollPanel>; }

NAMESPACE_END(nanogui)
