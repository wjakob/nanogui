/*
    The text box widget was contributed by Christian Schueller.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/**
 * \file nanogui/theme.h
 *
 * \brief Storage class for basic theme-related properties.
 */

#pragma once

#include <nanogui/common.h>
#include <nanogui/object.h>
#include <memory>
#include <string.h>

NAMESPACE_BEGIN(nanogui)

/**
 * \class Theme theme.h nanogui/theme.h
 *
 * \brief Storage class for basic theme-related properties.
 */
class NANOGUI_EXPORT Theme : public Object {
public:
    RTTI_CLASS_UID(Theme)
    RTTI_DECLARE_INFO(Theme)

    enum WindowDraggable { dgAuto = -1, dgFixed = 0, dgDraggable = 1 };
    enum WindowCollapse { clAuto = -1, clNonCollapse=0, clMayCollapse = 1 };

    Theme(NVGcontext *ctx);
    Theme(const Theme& o) = default;

    template<typename Type>
    Type get(const std::string& name) { return get(name, Type()); }

    template<typename Type>
    void set(const std::string& name, const Type& v) { return set(name, v); }

    Color get(const std::string& name, const Color&);
    void set(const std::string& name, const Color& v);

    int get(const std::string& name, const int&);
    void set(const std::string& name, const int& v);

    bool get(const std::string& name, const bool&);
    void set(const std::string& name, const bool& v);

    /* Fonts */
    /// The standard font face (default: ``"sans"`` from ``resources/roboto_regular.ttf``).
    int mFontNormal;
    /// The bold font face (default: ``"sans-bold"`` from ``resources/roboto_regular.ttf``).
    int mFontBold;
    /// The icon font face (default: ``"icons"`` from ``resources/entypo.ttf``).
    int mFontIcons;

    /**
     * The amount of scaling that is applied to each icon to fit the size of
     * NanoGUI widgets.  The default value is ``0.77f``, setting to e.g. higher
     * than ``1.0f`` is generally discouraged.
     */
    float mIconScale;

    /* Spacing-related parameters */
    /// The font size for all widgets other than buttons and textboxes (default: `` 16``).
    int mStandardFontSize;
    /// The font size for buttons (default: ``20``).
    int mButtonFontSize;
    
    /// Rounding radius for Button (and derived types) widgets (default: ``2``).
    int mButtonCornerRadius;
    /// The border width for TabHeader widgets (default: ``0.75f``).
    float mTabBorderWidth;
    /// The inner margin on a TabHeader widget (default: ``5``).
    int mTabInnerMargin;
    /// The minimum size for buttons on a TabHeader widget (default: ``20``).
    int mTabMinButtonWidth;
    /// The maximum size for buttons on a TabHeader widget (default: ``160``).
    int mTabMaxButtonWidth;
    /// Used to help specify what lies "in bound" for a TabHeader widget (default: ``20``).
    int mTabControlWidth;
    /// The amount of horizontal padding for a TabHeader widget (default: ``10``).
    int mTabButtonHorizontalPadding;
    /// The amount of vertical padding for a TabHeader widget (default: ``2``).
    int mTabButtonVerticalPadding;

    int mTextBoxCornerRadius;
    int mTextBoxUpIcon;      /// Icon to use when a TextBox has an up toggle (e.g. IntBox) (default: ``ENTYPO_ICON_CHEVRON_UP``).
    int mTextBoxDownIcon;    /// Icon to use when a TextBox has a down toggle (e.g. IntBox) (default: ``ENTYPO_ICON_CHEVRON_DOWN``).                            
    int mTextBoxFontSize;    /// The font size for text boxes (default: ``20``).

    int mPanelCornerRadius;
    int mPanelHeaderHeight;

    Color mPanelHeaderGradientTopFocus;
    Color mPanelHeaderGradientTopNormal;
    Color mPanelHeaderGradientBotFocus;
    Color mPanelHeaderGradientBotNormal;
    Color mPanelDropShadow;
    Color mPanelTitleFocused;
    Color mPanelTitleUnfocused;
    int mPanelCollapsedIcon;
    int mPanelExpandedIcon;
    int mPanelFontSize;

    int toolButtonSide = 25;

    int mContextMenuShortcutOffset;
    Color mContextMenuShortcutTextColor;
    int mContextMenuMinWidth;

    Color mSliderValueColor;

    Color mCheckboxUncheckedColor;
    Color mCheckboxCheckedColor;
    Color mCheckboxPushedColor;

    Color mSwitchboxBackgroundColor;
    Color mSwitchboxCheckedColor;
    Color mSwitchboxUncheckedColor;

    Color mScrollBarActiveColor;
    Color mScrollBarInactiveColor;

    Color mToleranceBarBorderColor; ///< Color of border
    Color mToleranceBarBgColor; ///< Color of background
    Color mToleranceBarLowColor; ///< Color of minimum value
    Color mToleranceBarHighColor; ///< Color of maximum value

    /* Generic colors */
    /**
     * The color of the drop shadow drawn behind widgets
     * (default: intensity=``0``, alpha=``128``; see \ref nanogui::Color::Color(int,int)).
     */
    Color mDropShadow;
    /**
     * The transparency color
     * (default: intensity=``0``, alpha=``0``; see \ref nanogui::Color::Color(int,int)).
     */
    Color mTransparent;
    /**
     * The dark border color
     * (default: intensity=``29``, alpha=``255``; see \ref nanogui::Color::Color(int,int)).
     */
    Color mBorderDark;
    /**
     * The light border color
     * (default: intensity=``92``, alpha=``255``; see \ref nanogui::Color::Color(int,int)).
     */
    Color mBorderLight;
    /**
     * The medium border color
     * (default: intensity=``35``, alpha=``255``; see \ref nanogui::Color::Color(int,int)).
     */
    Color mBorderMedium;
    /**
     * The text color
     * (default: intensity=``255``, alpha=``160``; see \ref nanogui::Color::Color(int,int)).
     */
    Color mTextColor;

    Color mButtonHoverTextColor;

    Color mButtonPressedTextColor;

    Color mToggleButtonPressedTextColor;

    Color mLinkPressedTextColor;

    Color mLinkTextColor;

    Color mLinkHoverTextColor;

    Color mLabelTextDisabledColor;
    /**
     * The disable dtext color
     * (default: intensity=``255``, alpha=``80``; see \ref nanogui::Color::Color(int,int)).
     */
    Color mDisabledTextColor;
    /**
     * The text shadow color
     * (default: intensity=``0``, alpha=``160``; see \ref nanogui::Color::Color(int,int)).
     */
    Color mTextColorShadow;
    /// The icon color (default: \ref nanogui::Theme::mTextColor).
    Color mIconColor;

    /* Button colors */
    /**
     * The top gradient color for buttons in focus
     * (default: intensity=``64``, alpha=``255``; see \ref nanogui::Color::Color(int,int)).
     */
    Color mButtonGradientTopFocused;
    /**
     * The bottom gradient color for buttons in focus
     * (default: intensity=``48``, alpha=``255``; see \ref nanogui::Color::Color(int,int)).
     */
    Color mButtonGradientBotFocused;
    /**
     * The top gradient color for buttons not in focus
     * (default: intensity=``74``, alpha=``255``; see \ref nanogui::Color::Color(int,int)).
     */
    Color mButtonGradientTopUnfocused;
    /**
     * The bottom gradient color for buttons not in focus
     * (default: intensity=``58``, alpha=``255``; see \ref nanogui::Color::Color(int,int)).
     */
    Color mButtonGradientBotUnfocused;
    /**
     * The top gradient color for buttons currently pushed
     * (default: intensity=``41``, alpha=``255``; see \ref nanogui::Color::Color(int,int)).
     */
    Color mButtonGradientTopPushed;
    /**
     * The bottom gradient color for buttons currently pushed
     * (default: intensity=``29``, alpha=``255``; see \ref nanogui::Color::Color(int,int)).
     */
    Color mButtonGradientBotPushed;

    Color mToggleButtonActiveColor;
    Color mToggleButtonInactiveColor;

    /* Window colors */
    /**
     * The fill color for a Window that is not in focus
     * (default: intensity=``43``, alpha=``230``; see \ref nanogui::Color::Color(int,int)).
     */
    Color mWindowFillUnfocused;
    /**
     * The fill color for a Window that is in focus
     * (default: intensity=``45``, alpha=``230``; see \ref nanogui::Color::Color(int,int)).
     */
    Color mWindowFillFocused;
    /**
     * The title color for a Window that is not in focus
     * (default: intensity=``220``, alpha=``160``; see \ref nanogui::Color::Color(int,int)).
     */
    Color mWindowTitleUnfocused;
    /**
     * The title color for a Window that is in focus
     * (default: intensity=``255``, alpha=``190``; see \ref nanogui::Color::Color(int,int)).
     */
    Color mWindowTitleFocused;

    /**
     * The top gradient color for Window headings
     * (default: \ref nanogui::Theme::mButtonGradientTopUnfocused).
     */
    Color mWindowHeaderGradientTop;
    /**
     * The bottom gradient color for Window headings
     * (default: \ref nanogui::Theme::mButtonGradientBotUnfocused).
     */
    Color mWindowHeaderGradientBot;
    /// The Window header top separation color (default: \ref nanogui::Theme::mBorderLight).
    Color mWindowHeaderSepTop;
    /// The Window header bottom separation color (default: \ref nanogui::Theme::mBorderDark).
    Color mWindowHeaderSepBot;
    Color windowBorderColorFocused;
    Color windowBorderColorUnfocused;
    FloatObservable windowBorderSize;

    /// Rounding radius for Window widget corners (default: ``2``).
    int mWindowCornerRadius;
    /// Default size of Window widget titles (default: ``30``).
    int mWindowHeaderHeight;
    /// Size of drop shadow rendered behind the Window widgets (default: ``10``).
    int mWindowDropShadowSize;
    int mWindowFontSize;
    int mWindowMenuHeaderOffset;
    int mWindowDragLine;
    bool windowResizeFromEdge = false;
    bool windowMoveFromTitlebarOnly = true;
    bool windowMoveInParent = true;
    shared_bool windowDrawBorder;
    shared_float windowPaddingLeft;
    shared_float windowPaddingTop;
    shared_float framePaddingLeft;
    shared_float framePaddingTop;

    shared_float innerSpacingCommon;

    bool frameDrawBorder = false;
    bool debugHighlightMouseover = false;
    bool keyboardNavigation = true;
    Color frameBorderColor;

    WindowDraggable mWindowDraggable = WindowDraggable::dgDraggable;
    WindowCollapse mWindowCollapse = WindowCollapse::clMayCollapse;

    /**
     * The popup window color
     * (default: intensity=``50``, alpha=``255``; see \ref nanogui::Color::Color(int,int))).
     */
    Color mWindowPopup;
    /**
     * The transparent popup window color
     * (default: intensity=``50``, alpha=``0``; see \ref nanogui::Color::Color(int,int))).
     */
    Color mWindowPopupTransparent;

    /// Icon to use for CheckBox widgets (default: ``ENTYPO_ICON_CHECK``).
    int mCheckBoxIcon;

    int mContextSubmenu;
    /// Icon to use for informational MessageDialog widgets (default: ``ENTYPO_ICON_INFO_WITH_CIRCLE``).
    int mMessageInformationIcon;
    /// Icon to use for interrogative MessageDialog widgets (default: ``ENTYPO_ICON_HELP_WITH_CIRCLE``).
    int mMessageQuestionIcon;
    /// Icon to use for warning MessageDialog widgets (default: ``ENTYPO_ICON_WARNING``).
    int mMessageWarningIcon;
    /// Icon to use on MessageDialog alt button (default: ``ENTYPO_ICON_CIRCLE_WITH_CROSS``).
    int mMessageAltButtonIcon;
    /// Icon to use on MessageDialog primary button (default: ``ENTYPO_ICON_CHECK``).
    int mMessagePrimaryButtonIcon;
    /// Icon to use for PopupButton widgets opening to the right (default: ``ENTYPO_ICON_CHEVRON_RIGHT``).
    int mPopupChevronRightIcon;
    /// Icon to use for PopupButton widgets opening to the left (default: ``ENTYPO_ICON_CHEVRON_LEFT``).
    int mPopupChevronLeftIcon;
    /// Icon to indicate hidden tabs to the left on a TabHeader (default: ``ENTYPO_ICON_ARROW_BOLD_LEFT``).
    int mTabHeaderLeftIcon;
    /// Icon to indicate hidden tabs to the right on a TabHeader (default: ``ENTYPO_ICON_ARROW_BOLD_RIGHT``).
    int mTabHeaderRightIcon;

    int mWindowExpandedIcon;
    int mWindowCollapsedIcon;

    int mWindowMenuHeight;

    int mTooltipOpacity;
    Color mTooltipBackgroundColor;
    Color mTooltipTextColor;

    struct {
      struct {
        bool enable = true;
        bool drawCursor = true;
      } mouse;
    } nav;

    bool textAreaBlinkCursor = true;

    void update(const Theme& newtheme);

protected:
    /// Default destructor does nothing; allows for inheritance.
    virtual ~Theme() { }
    Theme() {}
};

class NANOGUI_EXPORT DefaultTheme : public Theme
{
public:
  DefaultTheme(NVGcontext* ctx);
};

class NANOGUI_EXPORT WhiteTheme : public Theme
{
public:
  WhiteTheme(NVGcontext* ctx);
};

NAMESPACE_END(nanogui)
