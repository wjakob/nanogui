/*
    src/theme.cpp -- Storage class for basic theme-related properties

    The text box widget was contributed by Christian Schueller.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/theme.h>
#include <nanogui/opengl.h>
#include <nanogui/entypo.h>
#include <nanogui_resources.h> // provides nanogui::createFontMem

NAMESPACE_BEGIN(nanogui)

Theme::Theme(NVGcontext *ctx) {
    mStandardFontSize                 = 16;
    mButtonFontSize                   = 20;
    mTextBoxFontSize                  = 20;
    mIconScale                        = 0.77f;

    mWindowCornerRadius               = 2;
    mWindowHeaderHeight               = 30;
    mWindowDropShadowSize             = 10;
    mButtonCornerRadius               = 2;
    mTabBorderWidth                   = 0.75f;
    mTabInnerMargin                   = 5;
    mTabMinButtonWidth                = 20;
    mTabMaxButtonWidth                = 160;
    mTabControlWidth                  = 20;
    mTabButtonHorizontalPadding       = 10;
    mTabButtonVerticalPadding         = 2;

    mDropShadow                       = Color(0, 128);
    mTransparent                      = Color(0, 0);
    mBorderDark                       = Color(29, 255);
    mBorderLight                      = Color(92, 255);
    mBorderMedium                     = Color(35, 255);
    mTextColor                        = Color(255, 160);
    mDisabledTextColor                = Color(255, 80);
    mTextColorShadow                  = Color(0, 160);
    mIconColor                        = mTextColor;

    mButtonGradientTopFocused         = Color(64, 255);
    mButtonGradientBotFocused         = Color(48, 255);
    mButtonGradientTopUnfocused       = Color(74, 255);
    mButtonGradientBotUnfocused       = Color(58, 255);
    mButtonGradientTopPushed          = Color(41, 255);
    mButtonGradientBotPushed          = Color(29, 255);

    /* Window-related */
    mWindowFillUnfocused              = Color(43, 230);
    mWindowFillFocused                = Color(45, 230);
    mWindowTitleUnfocused             = Color(220, 160);
    mWindowTitleFocused               = Color(255, 190);

    mWindowHeaderGradientTop          = mButtonGradientTopUnfocused;
    mWindowHeaderGradientBot          = mButtonGradientBotUnfocused;
    mWindowHeaderSepTop               = mBorderLight;
    mWindowHeaderSepBot               = mBorderDark;

    mWindowPopup                      = Color(50, 255);
    mWindowPopupTransparent           = Color(50, 0);

    mCheckBoxIcon                     = ENTYPO_ICON_CHECK;
    mMessageInformationIcon           = ENTYPO_ICON_INFO_WITH_CIRCLE;
    mMessageQuestionIcon              = ENTYPO_ICON_HELP_WITH_CIRCLE;
    mMessageWarningIcon               = ENTYPO_ICON_WARNING;
    mMessageAltButtonIcon             = ENTYPO_ICON_CIRCLE_WITH_CROSS;
    mMessagePrimaryButtonIcon         = ENTYPO_ICON_CHECK;
    mPopupChevronRightIcon            = ENTYPO_ICON_CHEVRON_RIGHT;
    mPopupChevronLeftIcon             = ENTYPO_ICON_CHEVRON_LEFT;
    mTabHeaderLeftIcon                = ENTYPO_ICON_ARROW_BOLD_LEFT;
    mTabHeaderRightIcon               = ENTYPO_ICON_ARROW_BOLD_RIGHT;
    mTextBoxUpIcon                    = ENTYPO_ICON_CHEVRON_UP;
    mTextBoxDownIcon                  = ENTYPO_ICON_CHEVRON_DOWN;

    mFontNormal     = createFontMem(ctx, "sans", "Roboto-Regular.ttf");
    mFontBold       = createFontMem(ctx, "sans-bold", "Roboto-Bold.ttf");
    mFontMonoNormal = createFontMem(ctx, "mono", "RobotoMono-Regular.ttf");
    mFontMonoBold   = createFontMem(ctx, "mono-bold", "RobotoMono-Bold.ttf");
    mFontIcons      = createFontMem(ctx, "icons", "entypo.ttf");

    if (mFontNormal == -1 || mFontBold == -1 ||
        mFontMonoNormal == -1 || mFontMonoBold == -1 || mFontIcons == -1)
        throw std::runtime_error("Could not load fonts!");
}

NAMESPACE_END(nanogui)
