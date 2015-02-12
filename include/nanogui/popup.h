#if !defined(__NANOGUI_POPUP_H)
#define __NANOGUI_POPUP_H

#include <nanogui/window.h>

NANOGUI_NAMESPACE_BEGIN

/**
 * \brief Popup window for combo boxes, popup buttons, nested dialogs etc.
 *
 * Usually the Popup instance is constructed by another widget (e.g. \ref PopupButton)
 * and does not need to be created by hand.
 */
class Popup : public Window {
public:
    /// Create a new popup parented to a screen (first argument) and a parent window
    Popup(Widget *parent, Window *parentWindow);

    /// Return the anchor position in the parent window; the placement of the popup is relative to it
    inline void setAnchorPos(const Vector2i &anchorPos) { mAnchorPos = anchorPos; }
    /// Set the anchor position in the parent window; the placement of the popup is relative to it
    inline const Vector2i &anchorPos() const { return mAnchorPos; }

    /// Set the anchor height; this determines the vertical shift relative to the anchor position
    inline void setAnchorHeight(int anchorHeight) { mAnchorHeight = anchorHeight; }
    /// Return the anchor height; this determines the vertical shift relative to the anchor position
    inline int anchorHeight() const { return mAnchorHeight; }

    /// Return the parent window of the popup
    inline Window *parentWindow() { return mParentWindow; }
    /// Return the parent window of the popup
    inline const Window *parentWindow() const { return mParentWindow; }

    /// Invoke the associated layout generator to properly place child widgets, if any
    virtual void performLayout(NVGcontext *ctx);

    /// Draw the popup window
    virtual void draw(NVGcontext* ctx);
protected:
    /// Internal helper function to maintain nested window position values
    virtual void refreshRelativePlacement();

protected:
    Window *mParentWindow;
    Vector2i mAnchorPos;
    int mAnchorHeight;
};

NANOGUI_NAMESPACE_END

#endif /* __NANOGUI_POPUP_H */
