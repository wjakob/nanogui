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

NAMESPACE_BEGIN(nanogui)

#define CHECK(a) if (name == #a) return (a);
#define CHECKSET(a) if (name == #a) { (a) = value; return; }
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
  CHECK(mCheckboxUncheckedColor)
  CHECK(mCheckboxCheckedColor)
  CHECK(mCheckboxPushedColor)
  CHECK(mSwitchboxBackgroundColor)
  CHECK(mSwitchboxCheckedColor)
  CHECK(mSwitchboxUncheckedColor)
  CHECK(mScrollBarActiveColor)
  CHECK(mScrollBarInactiveColor)
  CHECK(mSliderValueColor)
  CHECK(mContextMenuShortcutTextColor)
  CHECK(mTooltipBackgroundColor)
  CHECK(mTooltipTextColor)
  CHECK(mToleranceBarBorderColor)
  CHECK(mToleranceBarBgColor)
  CHECK(mToleranceBarLowColor)
  CHECK(mToleranceBarHighColor)
  std::runtime_error(std::string("No get prop for name") + name);
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
  CHECKSET(mCheckboxUncheckedColor)
  CHECKSET(mCheckboxCheckedColor)
  CHECKSET(mCheckboxPushedColor)
  CHECKSET(mSwitchboxBackgroundColor)
  CHECKSET(mSwitchboxCheckedColor)
  CHECKSET(mSwitchboxUncheckedColor)
  CHECKSET(mScrollBarActiveColor)
  CHECKSET(mScrollBarInactiveColor)
  CHECKSET(mSliderValueColor)
  CHECKSET(mContextMenuShortcutTextColor)
  CHECKSET(mTooltipBackgroundColor)
  CHECKSET(mTooltipTextColor)
  CHECKSET(mToleranceBarBorderColor)
  CHECKSET(mToleranceBarBgColor)
  CHECKSET(mToleranceBarLowColor)
  CHECKSET(mToleranceBarHighColor)
  std::runtime_error(std::string("No set prop for name") + name);
}

void fillThemeDefaultValues(Theme& theme)
{
  theme.mStandardFontSize = 16;
  theme.mButtonFontSize = 20;
  theme.mTextBoxFontSize = 20;
  theme.mIconScale = 0.77f;

  theme.mWindowCornerRadius = 2;
  theme.mWindowHeaderHeight = 30;
  theme.mWindowDropShadowSize = 10;
  theme.mButtonCornerRadius = 2;
  theme.mTabBorderWidth = 0.75f;
  theme.mTabInnerMargin = 5;
  theme.mTabMinButtonWidth = 20;
  theme.mTabMaxButtonWidth = 160;
  theme.mTabControlWidth = 20;
  theme.mTabButtonHorizontalPadding = 10;
  theme.mTabButtonVerticalPadding = 2;
  theme.mWindowMenuHeaderOffset = -3;
  theme.mWindowFontSize = 18;

  theme.mDropShadow = Color(0, 128);
  theme.mTransparent = Color(0, 0);
  theme.mBorderDark = Color(29, 255);
  theme.mBorderLight = Color(92, 255);
  theme.mBorderMedium = Color(35, 255);
  theme.mTextColor = Color(255, 160);
  theme.mLabelTextDisabledColor = Color(29, 160);
  theme.mDisabledTextColor = Color(255, 80);
  theme.mTextColorShadow = Color(0, 160);
  theme.mIconColor = theme.mTextColor;

  theme.mButtonGradientTopFocused = Color(64, 255);
  theme.mButtonGradientBotFocused = Color(48, 255);
  theme.mButtonGradientTopUnfocused = Color(74, 255);
  theme.mButtonGradientBotUnfocused = Color(58, 255);
  theme.mButtonGradientTopPushed = Color(41, 255);
  theme.mButtonGradientBotPushed = Color(29, 255);
  theme.mToggleButtonActiveColor = Color(48, 255);
  theme.mToggleButtonInactiveColor = Color(180, 255);

  theme.mTooltipBackgroundColor = Color(0, 255);
  theme.mTooltipTextColor = Color(255, 255);

  /* Window-related */
  theme.mWindowFillUnfocused = Color(43, 230);
  theme.mWindowFillFocused = Color(45, 230);
  theme.mWindowTitleUnfocused = Color(220, 160);
  theme.mWindowTitleFocused = Color(255, 190);

  theme.mWindowHeaderGradientTop = theme.mButtonGradientTopUnfocused;
  theme.mWindowHeaderGradientBot = theme.mButtonGradientBotUnfocused;
  theme.mWindowHeaderSepTop = theme.mBorderLight;
  theme.mWindowHeaderSepBot = theme.mBorderDark;

  theme.mWindowPopup = Color(50, 255);
  theme.mWindowPopupTransparent = Color(50, 0);

  theme.mCheckboxUncheckedColor = Color(0, 32);
  theme.mCheckboxCheckedColor = Color(0, 32);
  theme.mCheckboxPushedColor = Color(0, 100);

  theme.mSwitchboxBackgroundColor = Color(0, 255);
  theme.mSwitchboxCheckedColor = Color(0, 0);
  theme.mSwitchboxUncheckedColor = Color(0, 0);

  theme.mScrollBarActiveColor = Color(220, 180);
  theme.mScrollBarInactiveColor = Color(220, 100);

  theme.mToleranceBarBorderColor = Color(220, 255);
  theme.mToleranceBarBgColor = Color(70, 100);
  theme.mToleranceBarLowColor = Color(70, 255);
  theme.mToleranceBarHighColor = Color(200, 255);

  theme.mSliderValueColor = Color(0, 129, 255, 128);

  theme.mContextMenuShortcutTextColor = Color(180, 255);
  theme.mContextMenuShortcutOffset = 40;
  theme.mContextMenuMinWidth = 140;
  theme.mTooltipOpacity = 255;
  theme.mWindowMenuHeight = 18;

  theme.mCheckBoxIcon = ENTYPO_ICON_OK;
  theme.mContextSubmenu = ENTYPO_ICON_RIGHT_OPEN;
  theme.mMessageInformationIcon = ENTYPO_ICON_INFO_CIRCLED;
  theme.mMessageQuestionIcon = ENTYPO_ICON_HELP_CIRCLED;
  theme.mMessageWarningIcon = ENTYPO_ICON_WARNING;
  theme.mMessageAltButtonIcon = ENTYPO_ICON_CANCEL_2;
  theme.mMessagePrimaryButtonIcon = ENTYPO_ICON_CHECK;
  theme.mPopupChevronRightIcon = ENTYPO_ICON_RIGHT_OPEN;
  theme.mPopupChevronLeftIcon = ENTYPO_ICON_LEFT_OPEN;
  theme.mTabHeaderLeftIcon = ENTYPO_ICON_LEFT_1;
  theme.mTabHeaderRightIcon = ENTYPO_ICON_RIGHT_1;
  theme.mTextBoxUpIcon = ENTYPO_ICON_UP_1;
  theme.mTextBoxDownIcon = ENTYPO_ICON_DOWN_1;
  theme.mWindowExpandedIcon = ENTYPO_ICON_DOWN_OPEN_1;
  theme.mWindowCollapsedIcon = ENTYPO_ICON_RIGHT_OPEN_1;
}

void __nanogui_get_fontdata(const char* name, void*& data, uint32_t &datasize);

Theme::Theme(NVGcontext *ctx) {
    fillThemeDefaultValues(*this);

    void* data = nullptr; uint32_t datasize;
    __nanogui_get_fontdata("sans", data, datasize);
    mFontNormal = nvgCreateFontMem(ctx, "sans", (unsigned char*)data, datasize, 0);

    __nanogui_get_fontdata("sans-bold", data, datasize);
    mFontBold = nvgCreateFontMem(ctx, "sans-bold", (unsigned char*)data, datasize, 0);

    __nanogui_get_fontdata("icons", data, datasize);
    mFontIcons = nvgCreateFontMem(ctx, "icons", (unsigned char*)data, datasize, 0);

    if (mFontNormal == -1 || mFontBold == -1 || mFontIcons == -1)
        throw std::runtime_error("Could not load fonts!");
}

void Theme::update(const Theme& newtheme)
{
  int oldFontNormal = mFontNormal;
  int oldFontBold = mFontBold;
  int oldFontIcons = mFontIcons;

  memcpy(this, &newtheme, sizeof(Theme));
  if (mFontNormal == -1) mFontNormal = oldFontNormal;
  if (mFontBold == -1) mFontBold = oldFontBold;
  if (mFontIcons == -1) mFontIcons = oldFontIcons;
}

DefaultTheme::DefaultTheme(NVGcontext* ctx)
  : Theme()
{
  fillThemeDefaultValues(*this);
}

WhiteTheme::WhiteTheme(NVGcontext* ctx)
  : Theme()
{
  fillThemeDefaultValues(*this);
  // overriding some default icons as demonstration
  // default: ``ENTYPO_ICON_CHECK``
  mCheckBoxIcon = ENTYPO_ICON_CHECK_1;
  //mCheckBoxIconExtraScale = 1.3f;
  // default: ``ENTYPO_ICON_CHEVRON_RIGHT``
  mPopupChevronRightIcon = ENTYPO_ICON_RIGHT_DIR;
  // default: ``ENTYPO_ICON_CHEVRON_LEFT``
  mPopupChevronLeftIcon = ENTYPO_ICON_LEFT_DIR;
  //mPopupIconExtraScale = 0.8f;
  // default: ``ENTYPO_ICON_ARROW_BOLD_LEFT``
  mTabHeaderLeftIcon = ENTYPO_ICON_LEFT_CIRCLED_1;
  // default: ``ENTYPO_ICON_ARROW_BOLD_RIGHT``
  mTabHeaderRightIcon = ENTYPO_ICON_RIGHT_CIRCLED_1;
  // default: ``ENTYPO_ICON_CHEVRON_UP``
  mTextBoxUpIcon = ENTYPO_ICON_UP_DIR;
  // default: ``ENTYPO_ICON_CHEVRON_DOWN``
  mTextBoxDownIcon = ENTYPO_ICON_DOWN_DIR;
  //mTextBoxIconExtraScale = 0.6f;

  mDropShadow = Color(0.0f, 0.17f, 0.21f, 1.0f);
  mTransparent = Color(0.0f, 0.0f, 0.0f, 0.0f);
  mBorderDark = Color(0.46f, 0.53f, 0.56f, 0.90f);
  mBorderLight = Color(0.64f, 0.69f, 0.71f, 0.90f);
  mBorderMedium = Color(0.56f, 0.62f, 0.64f, 0.901f);
  mTextColor = Color(0.34f, 0.43f, 0.45f, 1.0f);
  mDisabledTextColor = Color(0.61f, 0.66f, 0.67f, 0.90f);
  mTextColorShadow = Color(0.93f, 0.91f, 0.84f, 0.90f);
  mIconColor = Color(0.34f, 0.43f, 0.45f, 1.0f);

  mButtonGradientTopFocused = Color(0.76f, 0.78f, 0.79f, 1.0f);
  mButtonGradientBotFocused = Color(0.65f, 0.67f, 0.68f, 1.0f);
  mButtonGradientTopUnfocused = Color(0.80f, 0.82f, 0.83f, 1.0f);
  mButtonGradientBotUnfocused = Color(0.68f, 0.70f, 0.71f, 1.0f);
  mButtonGradientTopPushed = Color(0.64f, 0.67f, 0.67f, 1.0f);
  mButtonGradientBotPushed = Color(0.55f, 0.57f, 0.58f, 1.0f);

  mTooltipBackgroundColor = Color(0.03f, 0.21f, 0.25f, 0.90f);
  mTooltipTextColor = Color(0.76f, 0.79f, 0.79f, 1.0f);

  mToleranceBarBorderColor = Color(0.46f, 0.53f, 0.56f, 0.90f);
  mToleranceBarBgColor = Color(0, 50);
  mToleranceBarLowColor = Color(0.f, 1.f, 0.f, 1.f);
  mToleranceBarHighColor = Color(1.f, 0.f, 0.f, 1.f);

  /* Window-related */
  mWindowFillUnfocused = Color(0.88f, 0.86f, 0.79f, 0.90f);
  mWindowFillFocused = Color(0.93f, 0.90f, 0.83f, 0.90f);
  mWindowTitleUnfocused = Color(0.65f, 0.68f, 0.68f, 1.0f);
  mWindowTitleFocused = Color(0.76f, 0.79f, 0.79f, 1.0f);

  mWindowHeaderGradientTop = Color(0.16f, 0.32f, 0.36f, 0.90f);
  mWindowHeaderGradientBot = Color(0.02f, 0.21f, 0.25f, 0.90f);
  mWindowHeaderSepTop = Color(0.28f, 0.42f, 0.45f, 0.90f);
  mWindowHeaderSepBot = Color(0.0f, 0.16f, 0.21f, 1.0f);

  mWindowPopup = Color(0.93f, 0.90f, 0.83f, 0.90f);
  mWindowPopupTransparent = Color(0.19f, 0.19f, 0.19f, 0.0f);
}

NAMESPACE_END(nanogui)
