/*
    nanogui/popup.h -- Simple popup widget which is attached to another given
    window (can be nested)

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/window.h>

NAMESPACE_BEGIN(nanogui)

/**
 * \class Popup popup.h nanogui/popup.h
 *
 * \brief Popup window for combo boxes, popup buttons, nested dialogs etc.
 *
 * Usually the Popup instance is constructed by another widget (e.g. \ref PopupButton)
 * and does not need to be created by hand.
 */
class NANOGUI_EXPORT Popup : public Window {
public:
    RTTI_CLASS_UID(Popup)
    RTTI_DECLARE_INFO(Popup)

    enum Side { Left = 0, Right };

    /// Create a new popup parented to a screen (first argument) and a parent window
    Popup(Widget *parent, Window *parentWindow);

    /// Return the anchor position in the parent window; the placement of the popup is relative to it
    void setAnchorPos(const Vector2i &anchorPos) { mAnchorPos = anchorPos; }
    /// Set the anchor position in the parent window; the placement of the popup is relative to it
    const Vector2i &anchorPos() const { return mAnchorPos; }

    /// Set the anchor height; this determines the vertical shift relative to the anchor position
    void setAnchorHeight(int anchorHeight) { mAnchorHeight = anchorHeight; }
    /// Return the anchor height; this determines the vertical shift relative to the anchor position
    int anchorHeight() const { return mAnchorHeight; }

    /// Set the side of the parent window at which popup will appear
    void setSide(Side popupSide) { mSide = popupSide; }
    int getHeaderHeight() const override;
    /// Return the side of the parent window at which popup will appear
    Side side() const { return mSide; }

    /// Return the parent window of the popup
    Window *parentWindow() { return mParentWindow; }
    /// Return the parent window of the popup
    const Window *parentWindow() const { return mParentWindow; }

    /// Invoke the associated layout generator to properly place child widgets, if any
    virtual void performLayout(NVGcontext *ctx) override;

    /// Draw the popup window
    void draw(NVGcontext* ctx) override;

    void save(Json::value &s) const override;
    bool load(Json::value &s) override;
protected:
    /// Internal helper function to maintain nested window position values
    virtual void refreshRelativePlacement() override;

protected:
    Window *mParentWindow;
    Vector2i mAnchorPos;
    int mAnchorHeight;
    Side mSide;
};

NAMESPACE_END(nanogui)
