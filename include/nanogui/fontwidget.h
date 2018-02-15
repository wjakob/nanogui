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
    FontWidget(Widget *parent, const std::string &font, bool isWindow = false);

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
     * When the font is specified either via the constructor or \ref setFont,
     * this is set to ``true`` to indicate that \ref setTheme should not
     * overwrite the user-specified default.  Otherwise, when \ref setTheme is
     * called, the theme's default window font will be used.
     */
    bool mFontExplicit = false;

private:
    /**
     * There are two default fonts in NanoGUI:
     *
     * 1. If the derived type of this FontWidget is \ref nanogui::Window, then
     *    ``mFontIsWindow`` will be ``true``.  The default font face used will be
     *    ``"sans-bold"`` (or \ref Theme::defaultWindowFont if using a derived
     *    \ref Theme class).
     * 2. If the derived type is anything else, then ``mFontIsWindow`` will be
     *    ``false``.  The default font face used will be ``"sans"`` (or
     *    \ref Theme::defaultFont if using a derived \ref Theme class).
     */
    bool mFontIsWindow;
};

NAMESPACE_END(nanogui)
