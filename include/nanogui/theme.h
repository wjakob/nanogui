/*
    nanogui/theme.h -- Storage class for basic theme-related properties

    The text box widget was contributed by Christian Schueller.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/common.h>
#include <nanogui/object.h>

NAMESPACE_BEGIN(nanogui)

/**
 * \class Theme theme.h nanogui/theme.h
 *
 * \brief Storage class for basic theme-related properties.
 */
class NANOGUI_EXPORT Theme : public Object {
public:
    Theme(NVGcontext *ctx);

    /* Fonts */
    int mFontNormal;
    int mFontBold;
    int mFontIcons;

    /* Spacing-related parameters */
    int mStandardFontSize;
    int mButtonFontSize;
    int mTextBoxFontSize;
    int mWindowCornerRadius;
    int mWindowHeaderHeight;
    int mWindowDropShadowSize;
    int mButtonCornerRadius;
    float mTabBorderWidth;
    int mTabInnerMargin;
    int mTabMinButtonWidth;
    int mTabMaxButtonWidth;
    int mTabControlWidth;
    int mTabButtonHorizontalPadding;
    int mTabButtonVerticalPadding;

    /* Generic colors */
    Color mDropShadow;
    Color mTransparent;
    Color mBorderDark;
    Color mBorderLight;
    Color mBorderMedium;
    Color mTextColor;
    Color mDisabledTextColor;
    Color mTextColorShadow;
    Color mIconColor;

    /* Button colors */
    Color mButtonGradientTopFocused;
    Color mButtonGradientBotFocused;
    Color mButtonGradientTopUnfocused;
    Color mButtonGradientBotUnfocused;
    Color mButtonGradientTopPushed;
    Color mButtonGradientBotPushed;

    /* VScrollPanel colors */
    Color mScrollerGradientTop;
    Color mScrollerGradientBot;
    Color mTrackGradientTop;
    Color mTrackGradientBot;

    /* Window colors */
    Color mWindowFillUnfocused;
    Color mWindowFillFocused;
    Color mWindowTitleUnfocused;
    Color mWindowTitleFocused;

    Color mWindowHeaderGradientTop;
    Color mWindowHeaderGradientBot;
    Color mWindowHeaderSepTop;
    Color mWindowHeaderSepBot;

    Color mWindowPopup;
    Color mWindowPopupTransparent;
protected:
    virtual ~Theme() { };
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

NAMESPACE_END(nanogui)
