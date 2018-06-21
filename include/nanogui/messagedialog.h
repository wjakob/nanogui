/*
    nanogui/messagedialog.h -- Simple "OK" or "Yes/No"-style modal dialogs

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
 * \class MessageDialog messagedialog.h nanogui/messagedialog.h
 *
 * \brief Simple "OK" or "Yes/No"-style modal dialogs.
 */
class NANOGUI_EXPORT MessageDialog : public Window {
public:
    /// Classification of the type of message this MessageDialog represents.
    enum class Type {
        Information,///< An information dialog.  Uses \ref Theme::mMessageInformationIcon.
        Question,   ///< An interogative dialog.  Uses \ref Theme::mMessageQuestionIcon.
        Warning     ///< A warning dialog.  Uses \ref Theme::mMessageWarningIcon.
    };

    /**
     * \brief Constructs a MessageDialog confined to the specified parent.
     *
     * \param parent
     *     The parent, typically a \ref Screen instance.  It can also be e.g., a
     *     \ref Window, but make sure that the parent is at least 250 pixels
     *     wide.  If it is not, the positioning may be odd and moving the dialog
     *     will produce "snapping".
     *
     * \param title
     *     The title of the window to use (default: ``"Untitled"``).
     *
     * \param type
     *     The type of message dialog (determines the icon displayed, see \ref Type).
     *
     * \param message
     *     The dialog text you wish to display to the user (default:
     *     ``"Message"``).  This is the text that \ref mMessageLabel will get.
     *     It has a fixed width set to ``200``, meaning longer messages will
     *     automatically wrap to new lines.
     *
     * \param buttonText
     *     The button text for the confirmation button (default: ``"Ok"``).
     *     This button's icon is defined by \ref Theme::mMessagePrimaryButtonIcon.
     *
     * \param altButtonText
     *     The button text for the alternate button (default: ``"Cancel"``).
     *     This button's icon is defined by \ref Theme::mMessageAltButtonIcon.
     *
     * \param altButton
     *     Whether or not to include the alternate button (default: ``false``).
     */
    MessageDialog(Widget *parent, Type type, const std::string &title = "Untitled",
                  const std::string &message = "Message",
                  const std::string &buttonText = "OK",
                  const std::string &altButtonText = "Cancel", bool altButton = false);

    /// Returns \ref mIconLabel.
    Label *iconLabel() { return mIconLabel; }
    /// Returns \ref mIconLabel.
    const Label *iconLabel() const { return mIconLabel; }
    /**
     * Convenience method for setting \ref mIconLabel.  Must be a valid icon for
     * the font used in \ref mIconLabel.  The default font face is ``"icons"``,
     * specified by \ref Theme::defaultIconFont.
     *
     * \rst
     * The available icons for NanoGUI's default icon font can be found in
     * :ref:`file_nanogui_entypo.h`.
     * \endrst
     */
    void setIcon(int icon);

    /// The Label that contains the ``message`` parameter to the constructor.
    Label *messageLabel() { return mMessageLabel; }
    /// The Label that contains the ``message`` parameter to the constructor.
    const Label *messageLabel() const { return mMessageLabel; }

    /// The primary button.  See \ref mPrimaryButton.
    Button *primaryButton() { return mPrimaryButton; }
    /// The primary button.  See \ref mPrimaryButton.
    const Button *primaryButton() const { return mPrimaryButton; }
    /// Convenience method, calls ``mPrimaryButton->setIcon``.
    void setPrimaryIcon(int icon);

    /// The alternate button.  See \ref mAlternateButton.  **May be** ``nullptr``.
    Button *alternateButton() { return mAlternateButton; }
    /// The alternate button.  See \ref mAlternateButton.  **May be** ``nulltpr``.
    const Button *alternateButton() const { return mAlternateButton; }
    /// Convenience method, calls ``mAlternateButton->setIcon``.
    void setAlternateIcon(int icon);

    /// The callback used for this MessageDialog.  See \ref mCallback.
    std::function<void(int)> callback() const { return mCallback; }
    /// Sets the callback for this MessageDialog.  See \ref mCallback.
    void setCallback(const std::function<void(int)> &callback) { mCallback = callback; }

    /**
     * \brief Changes the theme for this MessageDialog.
     *
     * Typically it is desirable to specify the ``parent`` in the constructor as
     * a \ref Screen instance.  This will make the MessageDialog appear in the
     * center of the screen.  If you choose to customize the theme of say a
     * specific window and want this MessageDialog to have this custom theme,
     * make sure to call this method to update any colors / icons defined by
     * this custom theme.
     *
     * \rst
     * .. code-block:: cpp
     *
     *    auto dlg = MessageDialog(screen, MessageDialog::Type::Information);
     *    dlg->setTheme(mCustomTheme);// will update icons / colors accordingly
     *
     * \endrst
     */
    virtual void setTheme(Theme *theme) override;

protected:
    /// Stored only to allow \ref setTheme to correctly override \ref mIconLabel.
    Type mType;

    /**
     * An label with an icon as text, it's font size is set to ``50``.  The
     * initial value is determined by the \ref MessageDialog::Type specified to
     * the constructor.  Call \ref setIcon to change the icon.
     */
    Label *mIconLabel;

    /// A Label that contains the ``message`` supplied to the constructor, with a fixed width of ``200``.
    Label *mMessageLabel;

    /**
     * The primary button (caption: ``buttonText`` in constructor).  A getter
     * method \ref primaryButton exists for you to change what you desire, such
     * as the background color etc.  **However**, be careful **not** to set the
     * callback of this Button.  Its callback is configured in the constructor
     * to properly \ref Window::dispose after a response.  See documentation for
     * \ref MessageDialog::mCallback for how to know which button was clicked.
     */
    Button *mPrimaryButton;

    /**
     * The alternate button (caption: ``altButtonText`` in constructor).
     * **Only created when** ``altButton=true`` **in the constructor**.  A
     * getter method \ref alternateButton exists for you to change what you
     * desire, such as the background color etc.  **However**, be careful
     * **not** to set the callback of this Button. Its callback is configured
     * in the constructor to properly \ref Window::dispose after a response.
     * See documentation for \ref MessageDialog::mCallback for how to know which
     * button was clicked.
     */
    Button *mAlternateButton;

    /**
     * The callback to execute when either the primary or alternate button are
     * pressed.  When constructed with ``altButton=false``, only one button will
     * be added (defined by ``buttonText``).  In this case, the callback will
     * always be called with ``0``.
     *
     * When ``altButton=true``, two buttons are added.  If the user presses the
     * primary button (``buttonText`` in the constructor), the callback will
     * still be called with ``0``.  If the user presses the alternate button
     * (``altButtonText`` in the constructor), the callback will be called with ``1``.
     */
    std::function<void(int)> mCallback;

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

NAMESPACE_END(nanogui)
