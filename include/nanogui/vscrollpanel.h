/*
    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/**
 * \file nanogui/vscrollpanel.h
 *
 * \brief Adds a vertical scrollbar around a widget that is too big to fit into
 *        a certain area.
 */

#pragma once

#include <nanogui/widget.h>

NAMESPACE_BEGIN(nanogui)

/**
 * \class VScrollPanel vscrollpanel.h nanogui/vscrollpanel.h
 *
 * \brief Adds a vertical scrollbar around a widget that is too big to fit into
 *        a certain area.
 *
 * \rst
 * .. tip::
 *
 *    See :ref:`nanogui_example_icons` for example usage of this class.  That
 *    example uses fixed sizes, which is not strictly necessary, but it
 *    demonstrates the expected workflow for this widget --- there is exactly
 *    **one** child of ``VScrollPanel``, and all children being controlled are
 *    added to said single child.
 *
 * \endrst
 */
class NANOGUI_EXPORT VScrollPanel : public Widget {
public:
    /// Adds a VScrollPanel attached to the specified parent.
    VScrollPanel(Widget *parent);

    /// Positions the expected single child, and all children of the single child.
    virtual void performLayout(NVGcontext *ctx) override;

    /// The preferred size of this VScrollPanel.
    virtual Vector2i preferredSize(NVGcontext *ctx) const override;

    /// Handles mouse drag events.
    virtual bool mouseDragEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) override;

    /// Handles mouse scroll events (to display / hide widgets).
    virtual bool scrollEvent(const Vector2i &p, const Vector2f &rel) override;

    /// Draws the currently visible children of the single child of this VScrollPanel.
    virtual void draw(NVGcontext *ctx) override;

    /// Saves the state of this VScrollPanel to the specified Serializer.
    virtual void save(Serializer &s) const override;

    /// Sets the state of this VScrollPanel from the specified Serializer.
    virtual bool load(Serializer &s) override;

protected:
    /// The preferred height of the single child of this VScrollPanel.
    int mChildPreferredHeight;

    /// The current scroll position (used to determine which widgets to display).
    float mScroll;

    /// Whether or not \ref performLayout needs to be called.
    bool mUpdateLayout;

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

NAMESPACE_END(nanogui)
