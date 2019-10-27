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
#include <nanovg.h>
#include <nanogui/entypo.h>
#include <nanogui_resources.h>

NAMESPACE_BEGIN(nanogui)

#define CHECK(a) if(name == #a) return a;
#define CHECKSET(a) if (name == #a) { a = value; return; }
int Theme::get(const std::string& name, const int&)
{
  CHECK(mStandardFontSize)
  CHECK(mButtonFontSize)
  CHECK(mTextBoxFontSize)
  CHECK(mIconScale)
  CHECK(mWindowCornerRadius)
  CHECK(mWindowHeaderHeight)
  CHECK(mWindowDropShadowSize)
  CHECK(mButtonCornerRadius)
  CHECK(mTabBorderWidth)
  CHECK(mTabInnerMargin)
  CHECK(mTabMinButtonWidth)
  CHECK(mTabMaxButtonWidth)
  CHECK(mTabControlWidth)
  CHECK(mTabButtonHorizontalPadding)
  CHECK(mTabButtonVerticalPadding)
  CHECK(mWindowMenuHeaderOffset)
  CHECK(mWindowFontSize)
  CHECK(mTooltipOpacity)
  std::runtime_error(std::string("No get prop for name") + name);
  return 0;
}

void Theme::set(const std::string& name, const int& value)
{
  CHECKSET(mStandardFontSize)
  CHECKSET(mButtonFontSize)
  CHECKSET(mTextBoxFontSize)
  CHECKSET(mIconScale)
  CHECKSET(mWindowCornerRadius)
  CHECKSET(mWindowHeaderHeight)
  CHECKSET(mWindowDropShadowSize)
  CHECKSET(mButtonCornerRadius)
  CHECKSET(mTabBorderWidth)
  CHECKSET(mTabInnerMargin)
  CHECKSET(mTabMinButtonWidth)
  CHECKSET(mTabMaxButtonWidth)
  CHECKSET(mTabControlWidth)
  CHECKSET(mTabButtonHorizontalPadding)
  CHECKSET(mTabButtonVerticalPadding)
  CHECKSET(mWindowMenuHeaderOffset)
  CHECKSET(mWindowFontSize)
  CHECKSET(mTooltipOpacity)
  std::runtime_error(std::string("No set prop for name") + name);
}

Color Theme::get(const std::string& name, const Color&)
{
  CHECK(mDropShadow)
  CHECK(mTransparent)
  CHECK(mBorderDark)
  CHECK(mBorderLight)
  CHECK(mBorderMedium)
  CHECK(mTextColor)
  CHECK(mLabelTextDisabledColor)
  CHECK(mDisabledTextColor)
  CHECK(mTextColorShadow)
  CHECK(mIconColor)
  CHECK(mButtonGradientTopFocused)
  CHECK(mButtonGradientBotFocused)
  CHECK(mButtonGradientTopUnfocused)
  CHECK(mButtonGradientBotUnfocused)
  CHECK(mButtonGradientTopPushed)
  CHECK(mButtonGradientBotPushed)
  CHECK(mWindowFillUnfocused)
  CHECK(mWindowFillFocused)
  CHECK(mWindowTitleUnfocused)
  CHECK(mWindowTitleFocused)
  CHECK(mWindowHeaderGradientTop)
  CHECK(mWindowHeaderGradientBot)
  CHECK(mWindowHeaderSepTop)
  CHECK(mWindowHeaderSepBot)
  CHECK(mWindowPopup)
  CHECK(mWindowPopupTransparent)
  CHECK(mCheckboxUncheckedColor);
  CHECK(mCheckboxCheckedColor);
  CHECK(mCheckboxPushedColor);
  CHECK(mSwitchboxBackgroundColor);
  CHECK(mSwitchboxCheckedColor);
  CHECK(mSwitchboxUncheckedColor);
  CHECK(mScrollBarActiveColor);
  CHECK(mScrollBarInactiveColor);
  CHECK(mSliderValueColor);
  CHECK(mContextMenuShortcutTextColor);
  CHECK(mTooltipBackgroundColor)
  CHECK(mTooltipTextColor)
  std::runtime_error(std::string("No get prop for name") + name);
  return Color(0xff, 0, 0, 0xff);
}

void Theme::set(const std::string& name, const Color& value)
{
  CHECKSET(mDropShadow)
  CHECKSET(mTransparent)
  CHECKSET(mBorderDark)
  CHECKSET(mBorderLight)
  CHECKSET(mBorderMedium)
  CHECKSET(mTextColor)
  CHECKSET(mLabelTextDisabledColor)
  CHECKSET(mDisabledTextColor)
  CHECKSET(mTextColorShadow)
  CHECKSET(mIconColor)
  CHECKSET(mButtonGradientTopFocused)
  CHECKSET(mButtonGradientBotFocused)
  CHECKSET(mButtonGradientTopUnfocused)
  CHECKSET(mButtonGradientBotUnfocused)
  CHECKSET(mButtonGradientTopPushed)
  CHECKSET(mButtonGradientBotPushed)
  CHECKSET(mWindowFillUnfocused)
  CHECKSET(mWindowFillFocused)
  CHECKSET(mWindowTitleUnfocused)
  CHECKSET(mWindowTitleFocused)
  CHECKSET(mWindowHeaderGradientTop)
  CHECKSET(mWindowHeaderGradientBot)
  CHECKSET(mWindowHeaderSepTop)
  CHECKSET(mWindowHeaderSepBot)
  CHECKSET(mWindowPopup)
  CHECKSET(mWindowPopupTransparent)
  CHECKSET(mCheckboxUncheckedColor);
  CHECKSET(mCheckboxCheckedColor);
  CHECKSET(mCheckboxPushedColor);
  CHECKSET(mSwitchboxBackgroundColor);
  CHECKSET(mSwitchboxCheckedColor);
  CHECKSET(mSwitchboxUncheckedColor);
  CHECKSET(mScrollBarActiveColor);
  CHECKSET(mScrollBarInactiveColor);
  CHECKSET(mSliderValueColor);
  CHECKSET(mContextMenuShortcutTextColor);
  CHECKSET(mTooltipBackgroundColor)
  CHECKSET(mTooltipTextColor)
  std::runtime_error(std::string("No set prop for name") + name);
}

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
    mWindowMenuHeaderOffset           = -3;
    mWindowFontSize                   = 18;

    mDropShadow                       = Color(0, 128);
    mTransparent                      = Color(0, 0);
    mBorderDark                       = Color(29, 255);
    mBorderLight                      = Color(92, 255);
    mBorderMedium                     = Color(35, 255);
    mTextColor                        = Color(255, 160);
    mLabelTextDisabledColor           = Color(29, 160);
    mDisabledTextColor                = Color(255, 80);
    mTextColorShadow                  = Color(0, 160);
    mIconColor                        = mTextColor;

    mButtonGradientTopFocused         = Color(64, 255);
    mButtonGradientBotFocused         = Color(48, 255);
    mButtonGradientTopUnfocused       = Color(74, 255);
    mButtonGradientBotUnfocused       = Color(58, 255);
    mButtonGradientTopPushed          = Color(41, 255);
    mButtonGradientBotPushed          = Color(29, 255);
    mTooltipBackgroundColor           = Color(0, 255);
    mTooltipTextColor                 = Color(255, 255);

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

    mCheckboxUncheckedColor           = Color(0, 32);
    mCheckboxCheckedColor             = Color(0, 32);
    mCheckboxPushedColor              = Color(0, 100);

    mSwitchboxBackgroundColor         = Color(0, 255);
    mSwitchboxCheckedColor            = Color(0, 0);
    mSwitchboxUncheckedColor          = Color(0, 0);

    mScrollBarActiveColor             = Color(220, 180);
    mScrollBarInactiveColor           = Color(220, 100);

    mSliderValueColor                 = Color(0, 129, 255, 128);

    mContextMenuShortcutTextColor     = Color(180, 255);
    mContextMenuShortcutOffset        = 40;
    mContextMenuMinWidth              = 140;
    mTooltipOpacity                   = 255;

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

    mFontNormal = nvgCreateFontMem(ctx, "sans", roboto_regular_ttf,
                                   roboto_regular_ttf_size, 0);
    mFontBold = nvgCreateFontMem(ctx, "sans-bold", roboto_bold_ttf,
                                 roboto_bold_ttf_size, 0);
    mFontIcons = nvgCreateFontMem(ctx, "icons", entypo_ttf,
                                  entypo_ttf_size, 0);
    if (mFontNormal == -1 || mFontBold == -1 || mFontIcons == -1)
        throw std::runtime_error("Could not load fonts!");
}

NAMESPACE_END(nanogui)
