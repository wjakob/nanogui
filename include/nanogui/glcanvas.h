/*
    nanogui/glcanvas.h -- Canvas widget for rendering GL

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <iostream>

#include <nanogui/widget.h>
#include <nanogui/opengl.h>
#include <nanogui/glutil.h>

NAMESPACE_BEGIN(nanogui)

/**
 * \class GLCanvas glcanvas.h nanogui/glcanvas.h
 *
 * \brief Canvas widget for rendering GL
 */
class NANOGUI_EXPORT GLCanvas : public Widget {
public:
    GLCanvas(Widget *parent);
    ~GLCanvas();

    const Color &backgroundColor() const { return mBackgroundColor; }
    void setBackgroundColor(const Color &backgroundColor) { mBackgroundColor = backgroundColor; }

    void setDrawBorder(const bool bDrawBorder) { mDrawBorder = bDrawBorder; }
    const bool &drawBorder() const { return mDrawBorder; }

    virtual Vector2i preferredSize(NVGcontext *ctx) const override;
    void drawWidgetBorder(NVGcontext* ctx) const;
    virtual void draw(NVGcontext *ctx) override;

    void setGLDrawingCallback(std::function<void()> fncDraw);

    virtual void save(Serializer &s) const override;
    virtual bool load(Serializer &s) override;

    GLShader& imageShader() { return mShader; }

protected:
    Color mBackgroundColor;
    std::function<void()> mDrawingCallback;
    GLShader mShader;
    bool mDrawBorder;
};

NAMESPACE_END(nanogui)
