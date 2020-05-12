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
#include <functional>

NAMESPACE_BEGIN(nanogui)

/**
 * \class VScrollPanel vscrollpanel.h nanogui/vscrollpanel.h
 *
 * \brief Adds a vertical scrollbar around a widget that is too big to fit into
 *        a certain area.
 */
class NANOGUI_EXPORT ScrollBar : public Widget {
public:
   RTTI_CLASS_UID(ScrollBar)
   RTTI_DECLARE_INFO(ScrollBar)

   enum Alignment { VerticalRight = 0,
                   HorizontalBottom = 1,
                   VerticalLeft = 2,
                   HorizontalTop = 3};
    ScrollBar(Widget *parent, Alignment align = VerticalRight);

    /// Return the current scroll amount as a value between 0 and 1. 0 means scrolled to the top and 1 to the bottom.
    float scroll() const { return mScroll; }
    /// Set the scroll amount to a value between 0 and 1. 0 means scrolled to the top and 1 to the bottom.
    void setScroll(float scroll) { mScroll = scroll; }

    void performLayout(NVGcontext *ctx) override;
    Vector2i preferredSize(NVGcontext *ctx) const override;
    bool mouseDragEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) override;
    bool scrollEvent(const Vector2i &p, const Vector2f &rel) override;
    void draw(NVGcontext *ctx) override;
    void save(Json::value &s) const override;
    bool load(Json::value &s) override;

    /// The current callback to execute (for any type of scrollbar).
    std::function<void(float)> callback() const { return mCallback; }

    /// Set the change callback (for any type of scrollbar).
    void setCallback(const std::function<void(float)> &callback) { mCallback = callback; }
protected:
    int mSliderPreferredSide;
    float mScroll;
    int mAlign;
    std::function<void(float)> mCallback;
};

NAMESPACE_END(nanogui)
