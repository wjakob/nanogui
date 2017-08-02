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
 */
class NANOGUI_EXPORT PopupButton : public Button {
public:
    PopupButton(Widget *parent, const std::string &caption = "Untitled",
                int buttonIcon = 0);

    /// Set the chevron icon which appears on the right-hand side of the button.
    void setChevronIcon(int icon) { mChevronIcon = icon; }
    /// Returns the chevron icon which appears on the right-hand side of the button.
    int chevronIcon() const { return mChevronIcon; }

    /// Set the side of the parent window at which popup will appear.
    /// Will also set the appropriate chevron icon.
    void setSide(Popup::Side popupSide);
    /// Return the side of the parent window at which popup will appear.
    Popup::Side side() const { return mPopup->side(); }

    Popup *popup() { return mPopup; }
    const Popup *popup() const { return mPopup; }

    virtual void draw(NVGcontext* ctx) override;
    virtual Vector2i preferredSize(NVGcontext *ctx) const override;
    virtual void performLayout(NVGcontext *ctx) override;

    virtual void save(Serializer &s) const override;
    virtual bool load(Serializer &s) override;
protected:
    Popup *mPopup;
    int mChevronIcon;
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

NAMESPACE_END(nanogui)
