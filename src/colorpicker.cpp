/*
    src/colorpicker.cpp -- push button with a popup to tweak a color value

    This widget was contributed by Christian Schueller.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/colorpicker.h>
#include <nanogui/layout.h>
#include <nanogui/colorwheel.h>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(ColorPicker, PopupButton)

ColorPicker::ColorPicker(Widget *parent, const Color& color) 
  : PopupButton(parent) 
{
    setBackgroundColor(color);
    Popup *popup = this->popup();
    popup->withLayout<GroupLayout>();

    // initialize callback to do nothing; this is for users to hook into
    // receiving a new color value
    mCallback = [](const Color &) {};
    mFinalCallback = [](const Color &) {};

    // set the color wheel to the specified color
    mColorWheel = popup->add<ColorWheel>(color);

    // set the pick button to the specified color
    mPickButton = popup->add<Button>(Caption{ "Pick" },
                                     BackgroundColor{ color},
                                     TextColor{ color.contrastingColor()},
                                     FixedSize{ 100, 20 });

    // set the reset button to the specified color
    mResetButton = popup->add<Button>(Caption{ "Reset" },
                                      BackgroundColor{ color },
                                      TextColor{ color.contrastingColor() },
                                      FixedSize{ 100, 20 });

    PopupButton::setChangeCallback([this](Button*) {
        if (mPickButton->pushed()) {
            setColor(backgroundColor());
            mFinalCallback(backgroundColor());
        }
    });

    mColorWheel->setCallback([&](const Color &value) {
        mPickButton->setBackgroundColor(value);
        mPickButton->setTextColor(value.contrastingColor());
        mCallback(value);
    });

    mPickButton->setCallback([this]() {
        if (mPushed) {
            Color value = mColorWheel->color();
            setPushed(false);
            setColor(value);
            mFinalCallback(value);
        }
    });

    mResetButton->setCallback([this]() {
        Color bg = this->mResetButton->backgroundColor();
        Color fg = this->mResetButton->textColor();

        mColorWheel->setColor(bg);
        mPickButton->setBackgroundColor(bg);
        mPickButton->setTextColor(fg);

        mCallback(bg);
        mFinalCallback(bg);
    });
}

Color ColorPicker::color() const {
    return backgroundColor();
}

void ColorPicker::setColor(const Color& color) {
    /* Ignore setColor() calls when the user is currently editing */
    if (!mPushed) {
        Color fg = color.contrastingColor();
        setBackgroundColor(color);
        setTextColor(fg);
        mColorWheel->setColor(color);

        mPickButton->setBackgroundColor(color);
        mPickButton->setTextColor(fg);

        mResetButton->setBackgroundColor(color);
        mResetButton->setTextColor(fg);
    }
}

NAMESPACE_END(nanogui)
