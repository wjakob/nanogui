#include <nanogui/theme.h>
#include <nanogui/opengl.h>
#include <resources.h>

NANOGUI_NAMESPACE_BEGIN

Theme::Theme(NVGcontext *ctx) {
	mStandardFontSize                 = 16;
	mButtonFontSize                   = 20;
	mWindowCornerRadius               = 2;
	mWindowHeaderHeight               = 30;
	mWindowDropShadowSize             = 10;
	mButtonCornerRadius               = 2;

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

	mFontNormal = nvgCreateFontMem(ctx, "sans", roboto_regular_ttf,
								   roboto_regular_ttf_size, 0);
	mFontBold = nvgCreateFontMem(ctx, "sans-bold", roboto_bold_ttf,
								 roboto_bold_ttf_size, 0);
	mFontIcons = nvgCreateFontMem(ctx, "icons", entypo_ttf,
								  entypo_ttf_size, 0);
	if (mFontNormal == -1 || mFontBold == -1 || mFontIcons == -1)
		throw std::runtime_error("Could not load fonts!");
}

NANOGUI_NAMESPACE_END
