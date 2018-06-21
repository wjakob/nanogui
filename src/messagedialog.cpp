/*
    src/messagedialog.cpp -- Simple "OK" or "Yes/No"-style modal dialogs

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/messagedialog.h>
#include <nanogui/layout.h>
#include <nanogui/button.h>
#include <nanogui/label.h>

NAMESPACE_BEGIN(nanogui)

MessageDialog::MessageDialog(Widget *parent, Type type, const std::string &title,
              const std::string &message,
              const std::string &buttonText,
              const std::string &altButtonText, bool altButton)
    : Window(parent, title)
    , mType(type)
    , mIconLabel(nullptr)
    , mMessageLabel(nullptr)
    , mPrimaryButton(nullptr)
    , mAlternateButton(nullptr) {

    setLayout(new BoxLayout(Orientation::Vertical,
                            Alignment::Middle, 10, 10));
    setModal(true);

    Widget *panel1 = new Widget(this);
    panel1->setLayout(new BoxLayout(Orientation::Horizontal,
                                    Alignment::Middle, 10, 15));
    int icon = 0;
    switch (type) {
        case Type::Information: icon = mTheme->mMessageInformationIcon; break;
        case Type::Question: icon = mTheme->mMessageQuestionIcon; break;
        case Type::Warning: icon = mTheme->mMessageWarningIcon; break;
    }
    mIconLabel = new Label(panel1, std::string(utf8(icon).data()), mTheme->mDefaultIconFont);
    mIconLabel->setFontSize(50);
    mMessageLabel = new Label(panel1, message);
    mMessageLabel->setFixedWidth(200);
    Widget *panel2 = new Widget(this);
    panel2->setLayout(new BoxLayout(Orientation::Horizontal,
                                    Alignment::Middle, 0, 15));

    if (altButton) {
        mAlternateButton = new Button(panel2, altButtonText, mTheme->mMessageAltButtonIcon);
        mAlternateButton->setCallback([&] { if (mCallback) mCallback(1); dispose(); });
    }
    mPrimaryButton = new Button(panel2, buttonText, mTheme->mMessagePrimaryButtonIcon);
    mPrimaryButton->setCallback([&] { if (mCallback) mCallback(0); dispose(); });
    center();
    requestFocus();
}

void MessageDialog::setIcon(int icon) {
    mIconLabel->setCaption(std::string(utf8(icon).data()));
}

void MessageDialog::setPrimaryIcon(int icon) {
    mPrimaryButton->setIcon(icon);
}

void MessageDialog::setAlternateIcon(int icon) {
    if (mAlternateButton)
        mAlternateButton->setIcon(icon);
}

void MessageDialog::setTheme(Theme *theme) {
    Window::setTheme(theme);

    // set the icon label
    mIconLabel->setFont(mTheme->mDefaultIconFont);
    int icon = 0;
    switch (mType) {
        case Type::Information: icon = mTheme->mMessageInformationIcon; break;
        case Type::Question: icon = mTheme->mMessageQuestionIcon; break;
        case Type::Warning: icon = mTheme->mMessageWarningIcon; break;
    }
    setIcon(icon);

    // set the button icons
    setPrimaryIcon(mTheme->mMessagePrimaryButtonIcon);
    setAlternateIcon(mTheme->mMessageAltButtonIcon);
}

NAMESPACE_END(nanogui)
