/*
    nanogui/textarea.h -- Multi-line read-only text widget

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/widget.h>
#include <cstdio>
#include <sstream>

NAMESPACE_BEGIN(nanogui)

/**
 * \class TextBox textarea.h nanogui/textarea.h
 *
 * \brief Multi-line read-only text widget, ideal for displaying
 * log messages etc.
 *
 * Appended text can use different colors, but the font size is
 * fixed for the entire widget.
 */
DECLSETTER(LongText, std::string)
DECLSETTER(TextHeader, std::string)
DECLSETTER(BulletLine, std::string)
DECLSETTER(SeparatorLine, std::string)
DECLSETTER(IndentWidth, int)
DECLSETTER(UnindentWidth, int)

class NANOGUI_EXPORT TextArea : public Widget 
{
public:
    RTTI_CLASS_UID(TextArea)
    RTTI_DECLARE_INFO(TextArea)

    explicit TextArea(Widget *parent);

    using Widget::set;
    template<typename... Args>
    TextArea(Widget* parent, const Args&... args)
      : TextArea(parent) { set<TextArea, Args...>(args...); }

    /// Set the used font
    void setFont(const std::string &font) { m_font = font; }

    /// Return the used font
    const std::string &font() const { return m_font; }

    /// Set the foreground color (applies to all subsequently added text)
    void setForegroundColor(const Color &color) { m_foreground_color = color; }

    /// Return the foreground color (applies to all subsequently added text)
    const Color &foregroundColor() const { return m_foreground_color; }

    /// Set the widget's background color (a global property)
    void setBackgroundColor(const Color &background_color) { m_background_color = background_color; }

    /// Return the widget's background color (a global property)
    const Color &backgroundColor() const { return m_background_color; }
    
    /// Set the widget's selection color (a global property)
    void setSelectionColor(const Color &selection_color) { m_selection_color = selection_color; }

    /// Return the widget's selection color (a global property)
    const Color &selectionColor() const { return m_selection_color; }

    /// Set the amount of padding to add around the text
    void setPadding(int padding) { m_padding = padding; }

    /// Return the amount of padding that is added around the text
    int padding() const { return m_padding; }

    /// Set whether the text can be selected using the mouse
    void setSelectable(bool selectable) { m_selectable = selectable; }

    /// Return whether the text can be selected using the mouse
    int isSelectable() const { return m_selectable; }

    /// Append text at the end of the widget
    void append(const std::string &text);
    void appendIcon(int icon);

    void setText(const std::string &text);

    /// Append a line of text at the bottom
    void appendLine(const std::string &text) { append(text + "\n"); }

    TextArea& header(const std::string &text);
    TextArea& bulletLine(const std::string &text);
    TextArea& separator(const std::string &text);

    /// Clear all current contents
    void clear();

    /* Widget implementation */
    void draw(NVGcontext *ctx) override;
    Vector2i preferredSize(NVGcontext *ctx) const override;
    bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override;
    bool mouseDragEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) override;
    bool keyboardEvent(int key, int scancode, int action, int modifiers) override;

    TextArea& indent(int w = 15) { mIndentWidth += w; return *this; }
    TextArea& unindent(int w = 0) { mIndentWidth = w; return *this; }

protected:
    Vector2i positionToBlock(const Vector2i &pos) const;
    Vector2i blockToPosition(const Vector2i &pos) const;

protected:
    struct Block {
        Vector2i offset;
        int width;
        std::string text;
        Color color;
        bool isSeparator;
        bool isIcon;
    };

    std::vector<Block> m_blocks;
    Color m_foreground_color;
    Color m_background_color;
    Color m_selection_color;
    std::string m_font;
    int mIndentWidth = 0;
    Vector2i m_offset, m_max_size;
    int m_padding;
    bool m_selectable;
    Vector2i m_selection_start;
    Vector2i m_selection_end;
    bool mNeedUpdate = false;

public:
    PROPSETTER(LongText, setText)
    PROPSETTER(TextHeader, header)
    PROPSETTER(BulletLine, bulletLine)
    PROPSETTER(SeparatorLine, separator)
    PROPSETTER(IndentWidth, indent)
    PROPSETTER(UnindentWidth, unndent)
};

namespace elm { using TextArea = Element<TextArea>; }

NAMESPACE_END(nanogui)
