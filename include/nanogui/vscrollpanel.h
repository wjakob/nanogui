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
    VScrollPanel(Widget *parent);

    virtual void performLayout(NVGcontext *ctx) override;
    virtual Vector2i preferredSize(NVGcontext *ctx) const override;
    virtual bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override;
    virtual bool mouseDragEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) override;
    virtual bool scrollEvent(const Vector2i &p, const Vector2f &rel) override;
    virtual void draw(NVGcontext *ctx) override;

    virtual void save(Serializer &s) const override;
    virtual bool load(Serializer &s) override;
protected:
    int scrollerHeight() const;

    float normalizedScroll() const { return mScroll; };
    void setNormalizedScroll(float scroll);

    int mChildPreferredHeight;
    float mScroll;
    bool mUpdateLayout;
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

NAMESPACE_END(nanogui)
