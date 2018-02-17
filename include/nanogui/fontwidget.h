/*
    nanogui/fontwidget.h -- Base class of all widgets that draw text.

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

class NANOGUI_EXPORT FontWidget : public Widget {
public:
    FontWidget(Widget *parent, const std::string &font, bool fontDefaultIsBold);

    /**
     * \brief Set the font used to draw the text for this FontWidget.
     *
     * There are four font faces available by default:
     *
     * 1. ``"sans"`` (\ref Theme::mFontNormal).
     * 2. ``"sans-bold"`` (\ref Theme::mFontBold).
     * 3. ``"mono"`` (\ref Theme::mFontMono).
     * 4. ``"mono-bold"`` (\ref Theme::mFontMonoBold).
     */
    void setFont(const std::string &font);

    /// Get the font used to draw the text for this FontWidget.
    const std::string &font() const { return mFont; }

    /// Return current font size. If not set the default of the current theme will be returned
    int fontSize() const;

    /// Set the font size of this widget
    void setFontSize(int fontSize) { mFontSize = fontSize; }

    /// Return whether the font size is explicitly specified for this widget
    bool hasFontSize() const { return mFontSize > 0; }

    /**
     * Set the \ref Theme used to draw this widget.  If \ref mFontExplicit is ``false``
     * then the \ref Theme::defaultWindowFont will overwrite \ref mFont.
     */
    virtual void setTheme(Theme *theme) override;

    virtual void save(Serializer &s) const override;

    virtual bool load(Serializer &s) override;

protected:
    virtual void setDefaultFont();

    /// The current font face being used to draw text.
    std::string mFont;

    /**
     * Used to set the font size of a widget explicitly.  The initial value is
     * ``-1``, and a negative number indicates that the theme's font size should
     * be used instead.  Specifically, \ref fontSize will return
     * \ref Theme::mStandardFontSize when ``mFontSize < 0``.
     */
    int mFontSize;

    /**
     * When the font is specified either via the constructor or \ref setFont,
     * this is set to ``true`` to indicate that \ref setTheme should not
     * overwrite the user-specified default.  Otherwise, when \ref setTheme is
     * called, the theme's default window font will be used.
     */
    bool mFontExplicit = false;

private:
    /**
     * There are two default fonts in NanoGUI: ``"sans"`` and ``"sans-bold"``.
     * When this variable is ``true``, the bold font will be used.  When
     * ``false``, the regular font face will be used.  Subclasses are
     * responsible for specifying this behavior when calling the parent class
     * constructor (\ref FontWidget::FontWidget).  For example,
     * \ref nanogui::Label will always set this to ``false``, whereas
     * \ref nanogui::Button will always set this to ``true``.
     */
    bool mFontDefaultIsBold;
};

NAMESPACE_END(nanogui)
