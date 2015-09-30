#pragma once

#include <nanogui/button.h>
#include <nanogui/popup.h>
#include <nanogui/entypo.h>

NANOGUI_NAMESPACE_BEGIN

class NANOGUI_EXPORT PopupButton : public Button {
public:
    PopupButton(Widget *parent, const std::string &caption = "Untitled",
                int buttonIcon = 0,
                int chevronIcon = ENTYPO_ICON_CHEVRON_SMALL_RIGHT);

    Popup *popup() { return mPopup; }
    const Popup *popup() const { return mPopup; }

    virtual void draw(NVGcontext* ctx);
    virtual Vector2i preferredSize(NVGcontext *ctx) const;
    virtual void performLayout(NVGcontext *ctx);

protected:
    Popup *mPopup;
    int mChevronIcon;
};

NANOGUI_NAMESPACE_END
