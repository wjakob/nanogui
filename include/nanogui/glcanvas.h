/*
    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/**
 * \file nanogui/glcanvas.h
 *
 * \brief Canvas widget for rendering OpenGL content.  This widget was
 *        contributed by Jan Winkler.
 */

#pragma once

#include <iostream>

#include <nanogui/widget.h>
#include <nanogui/common.h>
#include <nanovg.h>
//#include <nanogui/glutil.h>

NAMESPACE_BEGIN(nanogui)

/**
 * \class GLCanvas glcanvas.h nanogui/glcanvas.h
 *
 * \brief Canvas widget for rendering OpenGL content.  This widget was
 *        contributed by Jan Winkler.
 *
 * Canvas widget that can be used to display arbitrary OpenGL content. This is
 * useful to display and manipulate 3D objects as part of an interactive
 * application. The implementation uses scissoring to ensure that rendered
 * objects don't spill into neighboring widgets.
 *
 * \rst
 * **Usage**
 *     Override :func:`nanogui::GLCanvas::drawGL` in subclasses to provide
 *     custom drawing code.  See :ref:`nanogui_example_4`.
 *
 * \endrst
 */
class NANOGUI_EXPORT GLCanvas : public Widget {
public:
    /**
     * Creates a GLCanvas attached to the specified parent.
     *
     * \param parent
     *     The Widget to attach this GLCanvas to.
     */
    GLCanvas(Widget *parent);

    /// Returns the background color.
    const Color &backgroundColor() const { return mBackgroundColor; }

    /// Sets the background color.
    void setBackgroundColor(const Color &backgroundColor) { mBackgroundColor = backgroundColor; }

    /// Set whether to draw the widget border or not.
    void setDrawBorder(const bool bDrawBorder) { mDrawBorder = bDrawBorder; }

    /// Return whether the widget border gets drawn or not.
    const bool &drawBorder() const { return mDrawBorder; }

    /// Draw the canvas.
    virtual void draw(NVGcontext *ctx) override;

    /// Draw the GL scene. Override this method to draw the actual GL content.
    virtual void drawGL() {}

    /// Save the state of this GLCanvas to the specified Serializer.
    virtual void save(Serializer &s) const override;

    /// Set the state of this GLCanvas from the specified Serializer.
    virtual bool load(Serializer &s) override;

protected:
    /// Internal helper function for drawing the widget border
    void drawWidgetBorder(NVGcontext* ctx) const;

protected:
    /// The background color (what is used with ``glClearColor``).
    Color mBackgroundColor;

    /// Whether to draw the widget border or not.
    bool mDrawBorder;

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

NAMESPACE_END(nanogui)
