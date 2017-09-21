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
 *     This class overrides \ref nanogui::Widget::mIconExtraScale to be ``0.8f``,
 *     which affects all subclasses of this Widget.  Subclasses must explicitly
 *     set a different value if needed (e.g., in their constructor).
 */
class NANOGUI_EXPORT PopupButton : public Button {
public:
    PopupButton(Widget *parent, const std::string &caption = "Untitled",
                int buttonIcon = 0);

    void setChevronIcon(int icon) { mChevronIcon = icon; }
    int chevronIcon() const { return mChevronIcon; }

    void setSide(Popup::Side popupSide);
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
