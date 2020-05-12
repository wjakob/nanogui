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

#include <nanogui/separator.h>

NAMESPACE_BEGIN(nanogui)

/**
 * \class Label label.h nanogui/label.h
 *
 * \brief Text label widget.
 *
 * The font and color can be customized. When \ref Widget::setFixedWidth()
 * is used, the text is wrapped when it surpasses the specified width.
 */

DECLSETTERARGS(TextOffset, Vector2i)
DECLSETTER(TextWidthBreak, int)
DECLSETTER(TextWrapped, bool)

class NANOGUI_EXPORT Label : public Widget {
public:
    RTTI_CLASS_UID(Label)
    RTTI_DECLARE_INFO(Label)
    WIDGET_COMMON_FUNCTIONS(Label)

    enum TextState { tEnabled=0, tDisabled };
    explicit Label(Widget* parent);

    using Widget::set;
    template<typename... Args>
    Label(Widget* parent, const Args&... args)
       : Label(parent) { set<Label, Args...>(args...); }

    explicit Label(Widget* parent, const char* caption)
      : Label(parent) { mCaption = caption; }

    explicit Label(Widget* parent, const char* caption, const char* font)
      : Label(parent) { mCaption = caption; mFont = font; }

    /// Get the label's text caption
    const std::string &caption() const { return mCaption; }
    /// Set the label's text caption
    void setCaption(const std::string &caption);

    /// Set the currently active font (2 are available by default: 'sans' and 'sans-bold')
    void setFont(const std::string &font) { mFont = font; }
    /// Get the currently active font
    const std::string &font() const { return mFont; }

    /// Get the label color
    Color color() const { return mColor; }
    /// Set the label color
    void setColor(const Color& color) { mColor = color; }
    void setDisabledColor(const Color& color) { mDisabledColor = color; }
    void setBackgroundColor(const Color& color) { mBackgroundColor = color; }

    void setTextOffset(const Vector2i& offset) { mTextOffset = offset; }

    void setTextHAlign(TextHAlign align) { mTextAlign.h = align; }
    void setTextVAlign(TextVAlign align) { mTextAlign.v = align; }

    bool tabstop(CanTabStop) const override { return false; }
    void setTextAlign(TextAlign talign) { mTextAlign = talign; }

    void setTextWidthBreak(int w) { mTextWidthBreak = w; }
    void setTextWrapped(bool w) { mTextWrapped = w; }

    /// Set the \ref Theme used to draw this widget
    void setTheme(Theme *theme) override;
    int fontSize() const override;

    /// Compute the size needed to fully display the label
    Vector2i preferredSize(NVGcontext *ctx) const override;

    /// Draw the label
    void draw(NVGcontext *ctx) override;

    void save(Json::value &s) const override;
    bool load(Json::value &s) override;
protected:
    std::string mCaption;
    std::string mFont;
    bool mTextWrapped = false;
    Vector2i mTextOffset;
    Color mBackgroundColor;
    Color mColor, mDisabledColor;
    TextAlign mTextAlign = { hLeft, vMiddle };
    Vector2i mTextRealSize;
    int mTextWidthBreak = 0;

public:
    PROPSETTER(Caption,setCaption)
    PROPSETTER(CaptionFont,setFont)
    PROPSETTER(TextColor, setColor)
    PROPSETTER(FontSize,setFontSize)
    PROPSETTER(TextOffset, setTextOffset)
    PROPSETTER(BackgroundColor, setBackgroundColor)
    PROPSETTER(CaptionHAlign,setTextHAlign)
    PROPSETTER(CaptionAlign, setTextAlign)
    PROPSETTER(OnUpdate, addChild)
    PROPSETTER(TextWidthBreak, setTextWidthBreak)
    PROPSETTER(TextWrapped, setTextWrapped)
};
namespace elm { using Label = Element<Label>; }

NAMESPACE_END(nanogui)