#if !defined(__NANOGUI_POPUPBUTTON_H)
#define __NANOGUI_POPUPBUTTON_H

#include <nanogui/button.h>
#include <nanogui/popup.h>

NANOGUI_NAMESPACE_BEGIN

class PopupButton : public Button {
public:
    PopupButton(Widget *parent,const std::string &caption = "Untitled",int icon = 0,bool hideOverlay = false);

    inline Popup *popup() { return mPopup; }
    inline void setPopup(Popup *popup) { mPopup = popup; }

    inline void setHideOverlay(bool hideOverlay) { mHideOverlay = hideOverlay; }
    inline const bool hideOverlay() const { return mHideOverlay; }

    virtual void draw(NVGcontext* ctx);
    virtual Vector2i preferredSize(NVGcontext *ctx) const;
    virtual void performLayout(NVGcontext *ctx);

protected:
    Popup *mPopup;
    bool mHideOverlay;
};

NANOGUI_NAMESPACE_END

#endif /* __NANOGUI_POPUPBUTTON_H */
