#pragma once

#include <nanogui/widget.h>

NANOGUI_NAMESPACE_BEGIN

/**
 * \brief Text label widget
 *
 * The font and color can be customized. When \ref Widget::setFixedWidth()
 * is used, the text is wrapped when it surpasses the specified width
 */
class NANOGUI_EXPORT Label : public Widget {
public:
    Label(Widget *parent, const std::string &caption, const std::string &font = "sans");

    /// Get the label's text caption
    inline const std::string &caption() const { return mCaption; }
    /// Set the label's text caption
    inline void setCaption(const std::string &caption) { mCaption = caption; }

    /// Set the currently active font (2 are available by default: 'sans' and 'sans-bold')
    inline void setFont(const std::string &font) { mFont = font; }
    /// Get the currently active font
    inline const std::string &font() const { return mFont; }

    /// Get the font size in pixels
    inline int fontSize() const { return mFontSize; }
    /// Set the font size in pixels
    inline void setFontSize(int fontSize) { mFontSize = fontSize; }

    /// Get the label color
    inline Color color() const { return mColor; }
    /// Set the label color
    inline void setColor(Color color) { mColor = color; }

    /// Compute the size needed to fully display the label
    virtual Vector2i preferredSize(NVGcontext *ctx) const;
    /// Draw the label
    virtual void draw(NVGcontext *ctx);
protected:
    std::string mCaption;
    std::string mFont;
    Color mColor;
    int mFontSize;
};

NANOGUI_NAMESPACE_END
