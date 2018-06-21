/*
    nanogui/popupbutton.h -- Button which launches a popup widget

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/button.h>
#include <nanogui/popup.h>
#include <nanogui/entypo.h>

NAMESPACE_BEGIN(nanogui)

/**
 * \class PopupButton popupbutton.h nanogui/popupbutton.h
 *
 * \brief Button which launches a popup widget.
 *
 * \remark
 *     This class overrides \ref Widget::mIconExtraScale to what is specified by
 *     \ref Theme::mPopupIconExtraScale, which affects all subclasses of this
 *     Widget.  Subclasses must explicitly set a different value if needed
 *     (e.g., in their constructor).  Note that \ref setTheme also overwrites
 *     this value to that specified by the new Theme, so subclasses must also
 *     account for this if setting to a non-theme value.
 */
class NANOGUI_EXPORT PopupButton : public Button {
public:
    /**
     * \brief Creates a PopupButton attached to the specified parent.
     *
     * \param parent
     *     The \ref nanogui::Widget this PopupButton will be attached to.
     *
     * \param caption
     *     The name of the button (default ``"Untitled"``).
     *
     * \param buttonIcon
     *     The icon to display with this Button.  See \ref nanogui::Button::mIcon.
     *
     * \param font
     *     The font face to use (default ``""`` implies
     *     \ref Theme::defaultBoldFont, which will typically be ``"sans-bold"``).
     */
    PopupButton(Widget *parent, const std::string &caption = "Untitled",
                int buttonIcon = 0, const std::string &font = "");
    virtual ~PopupButton();

    void setChevronIcon(int icon) { mChevronIcon = icon; }
    int chevronIcon() const { return mChevronIcon; }

    void setSide(Popup::Side popupSide);
    Popup::Side side() const { return mPopup->side(); }

    Popup *popup() { return mPopup; }
    const Popup *popup() const { return mPopup; }

    virtual void draw(NVGcontext* ctx) override;
    virtual Vector2i preferredSize(NVGcontext *ctx) const override;
    virtual void performLayout(NVGcontext *ctx) override;
    /// Ensures that \ref Widget::mIconExtraScale is updated.
    virtual void setTheme(Theme *theme) override;

    virtual void save(Serializer &s) const override;
    virtual bool load(Serializer &s) override;
protected:
    Popup *mPopup;
    int mChevronIcon;
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

NAMESPACE_END(nanogui)
