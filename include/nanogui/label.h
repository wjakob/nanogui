/*
    nanogui/label.h -- Text label with an arbitrary font, color, and size

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/fontwidget.h>

NAMESPACE_BEGIN(nanogui)

/**
 * \class Label label.h nanogui/label.h
 *
 * \brief Text label widget.
 *
 * The font and color can be customized. When \ref Widget::setFixedWidth()
 * is used, the text is wrapped when it surpasses the specified width.
 */
class NANOGUI_EXPORT Label : public FontWidget {
public:
    Label(Widget *parent, const std::string &caption,
          const std::string &font = "", int fontSize = -1);

    /// Get the label's text caption
    const std::string &caption() const { return mCaption; }
    /// Set the label's text caption
    void setCaption(const std::string &caption) { mCaption = caption; }

    /// Get the label color
    Color color() const { return mColor; }
    /// Set the label color
    void setColor(const Color& color) { mColor = color; }

    /// Set the \ref Theme used to draw this widget
    virtual void setTheme(Theme *theme) override;

    /// Compute the size needed to fully display the label
    virtual Vector2i preferredSize(NVGcontext *ctx) const override;

    /// Draw the label
    virtual void draw(NVGcontext *ctx) override;

    virtual void save(Serializer &s) const override;
    virtual bool load(Serializer &s) override;
protected:
    std::string mCaption;
    Color mColor;
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

NAMESPACE_END(nanogui)
