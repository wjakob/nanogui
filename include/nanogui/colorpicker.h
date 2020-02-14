/*
    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/**
 * \file nanogui/colorpicker.h
 *
 * \brief Push button with a popup to tweak a color value.  This widget was
 *        contributed by Christian Schueller.
 */

#pragma once

#include <nanogui/popupbutton.h>

NAMESPACE_BEGIN(nanogui)

/**
 * \class ColorPicker colorpicker.h nanogui/colorpicker.h
 *
 * \brief Push button with a popup to tweak a color value.  This widget was
 *        contributed by Christian Schueller.
 */
class NANOGUI_EXPORT ColorPicker : public PopupButton {
public:
    RTTI_CLASS_UID(ColorPicker)
    RTTI_DECLARE_INFO(ColorPicker)

    /**
     * Attaches a ColorPicker to the specified parent.
     *
     * \param parent
     *     The Widget to add this ColorPicker to.
     *
     * \param color
     *     The color initially selected by this ColorPicker (default: Red).
     */
    ColorPicker(Widget *parent, const Color& color = Color(1.0f, 0.0f, 0.0f, 1.0f));

    /// The callback executed when the ColorWheel changes.
    std::function<void(const Color &)> callback() const { return mCallback; }

    /**
     * Sets the callback is executed as the ColorWheel itself is changed.  Set
     * this callback if you need to receive updates for the ColorWheel changing
     * before the user clicks \ref nanogui::ColorPicker::mPickButton or
     * \ref nanogui::ColorPicker::mPickButton.
     */
    void setCallback(const std::function<void(const Color &)> &callback) {
        mCallback = callback;
        mCallback(backgroundColor());
    }

    /**
     * The callback to execute when a new Color is selected on the ColorWheel
     * **and** the user clicks the \ref nanogui::ColorPicker::mPickButton or
     * \ref nanogui::ColorPicker::mResetButton.
     */
    std::function<void(const Color &)> finalCallback() const { return mFinalCallback; }

    /**
     * The callback to execute when a new Color is selected on the ColorWheel
     * **and** the user clicks the \ref nanogui::ColorPicker::mPickButton or
     * \ref nanogui::ColorPicker::mResetButton.
     */
    void setFinalCallback(const std::function<void(const Color &)> &callback) { mFinalCallback = callback; }

    /// Get the current Color selected for this ColorPicker.
    Color color() const;

    /// Set the current Color selected for this ColorPicker.
    void setColor(const Color& color);

    /// The current caption of the \ref nanogui::ColorPicker::mPickButton.
    const std::string &pickButtonCaption() { return mPickButton->caption(); }

    /// Sets the current caption of the \ref nanogui::ColorPicker::mPickButton.
    void setPickButtonCaption(const std::string &caption) { mPickButton->setCaption(caption); }

    /// The current caption of the \ref nanogui::ColorPicker::mResetButton.
    const std::string &resetButtonCaption() { return mResetButton->caption(); }

    /// Sets the current caption of the \ref nanogui::ColorPicker::mResetButton.
    void setResetButtonCaption(const std::string &caption) { mResetButton->setCaption(caption); }

protected:
    /// The "fast" callback executed when the ColorWheel has changed.
    std::function<void(const Color &)> mCallback;

    /**
     * The callback to execute when a new Color is selected on the ColorWheel
     * **and** the user clicks the \ref nanogui::ColorPicker::mPickButton or
     * \ref nanogui::ColorPicker::mResetButton.
     */
    std::function<void(const Color &)> mFinalCallback;

    /// The ColorWheel for this ColorPicker (the actual widget allowing selection).
    ColorWheel *mColorWheel;

    /**
     * The Button used to signal that the current value on the ColorWheel is the
     * desired color to be chosen.  The default value for the caption of this
     * Button is ``"Pick"``.  You can change it using
     * \ref nanogui::ColorPicker::setPickButtonCaption if you need.
     *
     * The color of this Button will not affect \ref nanogui::ColorPicker::color
     * until the user has actively selected by clicking this pick button.
     * Similarly, the \ref nanogui::ColorPicker::mCallback function is only
     * called when a user selects a new Color using by clicking this Button.
     */
    Button *mPickButton;

    /**
     * Remains the Color of the active color selection, until the user picks a
     * new Color on the ColorWheel **and** selects the
     * \ref nanogui::ColorPicker::mPickButton.  The default value for the
     * caption of this Button is ``"Reset"``.  You can change it using
     * \ref nanogui::ColorPicker::setResetButtonCaption if you need.
     */
    Button *mResetButton;

public:
};

NAMESPACE_END(nanogui)
