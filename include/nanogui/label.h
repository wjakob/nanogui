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

#include <nanogui/widget.h>

NAMESPACE_BEGIN(nanogui)

/**
 * \class Label label.h nanogui/label.h
 *
 * \brief Text label widget.
 *
 * The font and color can be customized. When \ref Widget::setFixedWidth()
 * is used, the text is wrapped when it surpasses the specified width.
 */

DECLSETTER(Caption,std::string)
DECLSETTER(CaptionFont,std::string)
DECLSETTER(FontSize,int)
DECLSETTER(CaptionHAlign,TextHAlign)

class NANOGUI_EXPORT Label : public Widget {
public:
    enum TextState { tEnabled=0, tDisabled };
    Label(Widget* parent);
    
    using Widget::set;
    template<typename... Args>
    Label(Widget* parent, const Args&... args)
       : Label(parent) { set<Args...>(args...); }

    explicit Label(Widget* parent, const char* caption)
      : Label(parent) { mCaption = caption; }

    explicit Label(Widget* parent, const char* caption, const char* font)
      : Label(parent) { mCaption = caption; mFont = font; }

    PROPSETTER(Caption,setCaption)
    PROPSETTER(CaptionFont,setFont)
    PROPSETTER(FontSize,setFontSize)
    PROPSETTER(CaptionHAlign,setTextHAlign)

    /// Get the label's text caption
    const std::string &caption() const { return mCaption; }
    /// Set the label's text caption
    void setCaption(const std::string &caption) { mCaption = caption; }

    /// Set the currently active font (2 are available by default: 'sans' and 'sans-bold')
    void setFont(const std::string &font) { mFont = font; }
    /// Get the currently active font
    const std::string &font() const { return mFont; }

    /// Get the label color
    Color color() const { return mColor; }
    /// Set the label color
    void setColor(const Color& color) { mColor = color; }
    void setDisabledColor(const Color& color) { mDisabledColor = color; }

    void setTextHAlign(TextHAlign align) { mTextHAlign = align; }
    void setTextVAlign(TextVAlign align) { mTextVAlign = align; }

    void setTextAlign(TextHAlign halign, TextVAlign valign) {
      mTextHAlign = halign; mTextVAlign = valign;
    }

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
    std::string mFont;
    Color mColor, mDisabledColor;
    TextHAlign mTextHAlign = hLeft;
    TextVAlign mTextVAlign = vMiddle;
    Vector2i mTextRealSize;

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

NAMESPACE_END(nanogui)
