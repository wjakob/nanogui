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

DECLSETTER(PopupSide, int)

struct PopupBase { Popup* w = nullptr; };
template<class FF=Popup> struct PopupWidget : public PopupBase { template<typename... Args> PopupWidget(const Args&... args) { w = new FF(nullptr, nullptr, args...); } };

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
class NANOGUI_EXPORT PopupButton : public Button 
{
public:
    RTTI_CLASS_UID(PopupButton)
    RTTI_DECLARE_INFO(PopupButton)

    explicit PopupButton(Widget *parent);

    using Button::set;
    template<typename... Args>
    PopupButton(Widget* parent, const Args&... args)
      : PopupButton(parent) {  set<PopupButton, Args...>(args...);  }

    virtual ~PopupButton();

    void setChevronIcon(int icon) { mChevronIcon = icon; }
    int chevronIcon() const { return mChevronIcon; }

    void setSide(Popup::Side popupSide);
    Popup::Side side() const { return mPopup->side(); }

    Popup *popup() { return mPopup; }
    const Popup *popup() const { return mPopup; }

    void setPopup(Popup* pp);

    Popup& popupref() { return *popup(); }
    void setPopupSide(int side);

    template<typename... Args>
    Popup& popupset(const Args&... args)
    { popupref().add(args...); return *popup(); }

    void draw(NVGcontext* ctx) override;
    Vector2i preferredSize(NVGcontext *ctx) const override;
    void performLayout(NVGcontext *ctx) override;

    void save(Json::value &save) const override;
    bool load(Json::value &save) override;

    template<typename FF, typename First, typename... Args> void set(const PopupBase& h, const Args&... args)
    { this->setPopup(h.w);  ((FF*)this)->template set<FF, Args...>(args...); }

protected:
    Popup *mPopup = nullptr;
    int mChevronIcon = -1;

    virtual void updatePopup();
    void parentChanged() override;

public:
    PROPSETTER(PopupSide, setPopupSide)
};

namespace elm { using PopupButton = Element<PopupButton>; }

NAMESPACE_END(nanogui)
