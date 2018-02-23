#include "python.h"

class PyTheme : public Theme {
public:
    using Theme::Theme;
    std::string defaultFont() const {
        PYBIND11_OVERLOAD(std::string, Theme, defaultFont);
    }
    std::string defaultBoldFont() const {
        PYBIND11_OVERLOAD(std::string, Theme, defaultBoldFont);
    }
    std::string defaultIconFont() const {
        PYBIND11_OVERLOAD(std::string, Theme, defaultIconFont);
    }
};

void register_theme(py::module &m) {
    py::class_<Theme, ref<Theme>, PyTheme>(m, "Theme", D(Theme))
        .def(py::init<NVGcontext *>(), D(Theme, Theme))
        .def("globalDefaultFont", &Theme::globalDefaultFont, D(Theme, globalDefaultFont))
        .def("globalDefaultBoldFont", &Theme::globalDefaultBoldFont, D(Theme, globalDefaultBoldFont))
        .def("defaultCheckBoxIconExtraScale", &Theme::defaultCheckBoxIconExtraScale, D(Theme, defaultCheckBoxIconExtraScale))
        .def("defaultPopupIconExtraScale", &Theme::defaultPopupIconExtraScale, D(Theme, defaultPopupIconExtraScale))
        .def("defaultTextBoxIconExtraScale", &Theme::defaultTextBoxIconExtraScale, D(Theme, defaultTextBoxIconExtraScale))
        .def("defaultFont", &Theme::defaultFont, D(Theme, defaultFont))
        .def("defaultBoldFont", &Theme::defaultBoldFont, D(Theme, defaultBoldFont))
        .def("defaultIconFont", &Theme::defaultIconFont, D(Theme, defaultIconFont))
        .def_readwrite("mIconScale", &Theme::mIconScale, D(Theme, mIconScale))
        .def_readwrite("mStandardFontSize", &Theme::mStandardFontSize, D(Theme, mStandardFontSize))
        .def_readwrite("mButtonFontSize", &Theme::mButtonFontSize, D(Theme, mButtonFontSize))
        .def_readwrite("mTextBoxFontSize", &Theme::mTextBoxFontSize, D(Theme, mTextBoxFontSize))
        .def_readwrite("mWindowCornerRadius", &Theme::mWindowCornerRadius, D(Theme, mWindowCornerRadius))
        .def_readwrite("mWindowHeaderHeight", &Theme::mWindowHeaderHeight, D(Theme, mWindowHeaderHeight))
        .def_readwrite("mWindowDropShadowSize", &Theme::mWindowDropShadowSize, D(Theme, mWindowDropShadowSize))
        .def_readwrite("mButtonCornerRadius", &Theme::mButtonCornerRadius, D(Theme, mButtonCornerRadius))
        .def_readwrite("mTabBorderWidth", &Theme::mTabBorderWidth, D(Theme, mTabBorderWidth))
        .def_readwrite("mTabInnerMargin", &Theme::mTabInnerMargin, D(Theme, mTabInnerMargin))
        .def_readwrite("mTabMinButtonWidth", &Theme::mTabMinButtonWidth, D(Theme, mTabMinButtonWidth))
        .def_readwrite("mTabMaxButtonWidth", &Theme::mTabMaxButtonWidth, D(Theme, mTabMaxButtonWidth))
        .def_readwrite("mTabControlWidth", &Theme::mTabControlWidth, D(Theme, mTabControlWidth))
        .def_readwrite("mTabButtonHorizontalPadding", &Theme::mTabButtonHorizontalPadding, D(Theme, mTabButtonHorizontalPadding))
        .def_readwrite("mTabButtonVerticalPadding", &Theme::mTabButtonVerticalPadding, D(Theme, mTabButtonVerticalPadding))
        .def_readwrite("mDropShadow", &Theme::mDropShadow, D(Theme, mDropShadow))
        .def_readwrite("mTransparent", &Theme::mTransparent, D(Theme, mTransparent))
        .def_readwrite("mBorderDark", &Theme::mBorderDark, D(Theme, mBorderDark))
        .def_readwrite("mBorderLight", &Theme::mBorderLight, D(Theme, mBorderLight))
        .def_readwrite("mBorderMedium", &Theme::mBorderMedium, D(Theme, mBorderMedium))
        .def_readwrite("mTextColor", &Theme::mTextColor, D(Theme, mTextColor))
        .def_readwrite("mDisabledTextColor", &Theme::mDisabledTextColor, D(Theme, mDisabledTextColor))
        .def_readwrite("mTextColorShadow", &Theme::mTextColorShadow, D(Theme, mTextColorShadow))
        .def_readwrite("mIconColor", &Theme::mIconColor, D(Theme, mIconColor))
        .def_readwrite("mButtonGradientTopFocused", &Theme::mButtonGradientTopFocused, D(Theme, mButtonGradientTopFocused))
        .def_readwrite("mButtonGradientBotFocused", &Theme::mButtonGradientBotFocused, D(Theme, mButtonGradientBotFocused))
        .def_readwrite("mButtonGradientTopUnfocused", &Theme::mButtonGradientTopUnfocused, D(Theme, mButtonGradientTopUnfocused))
        .def_readwrite("mButtonGradientBotUnfocused", &Theme::mButtonGradientBotUnfocused, D(Theme, mButtonGradientBotUnfocused))
        .def_readwrite("mButtonGradientTopPushed", &Theme::mButtonGradientTopPushed, D(Theme, mButtonGradientTopPushed))
        .def_readwrite("mButtonGradientBotPushed", &Theme::mButtonGradientBotPushed, D(Theme, mButtonGradientBotPushed))
        .def_readwrite("mWindowFillUnfocused", &Theme::mWindowFillUnfocused, D(Theme, mWindowFillUnfocused))
        .def_readwrite("mWindowFillFocused", &Theme::mWindowFillFocused, D(Theme, mWindowFillFocused))
        .def_readwrite("mWindowTitleUnfocused", &Theme::mWindowTitleUnfocused, D(Theme, mWindowTitleUnfocused))
        .def_readwrite("mWindowTitleFocused", &Theme::mWindowTitleFocused, D(Theme, mWindowTitleFocused))
        .def_readwrite("mWindowHeaderGradientTop", &Theme::mWindowHeaderGradientTop, D(Theme, mWindowHeaderGradientTop))
        .def_readwrite("mWindowHeaderGradientBot", &Theme::mWindowHeaderGradientBot, D(Theme, mWindowHeaderGradientBot))
        .def_readwrite("mWindowHeaderSepTop", &Theme::mWindowHeaderSepTop, D(Theme, mWindowHeaderSepTop))
        .def_readwrite("mWindowHeaderSepBot", &Theme::mWindowHeaderSepBot, D(Theme, mWindowHeaderSepBot))
        .def_readwrite("mWindowPopup", &Theme::mWindowPopup, D(Theme, mWindowPopup))
        .def_readwrite("mWindowPopupTransparent", &Theme::mWindowPopupTransparent, D(Theme, mWindowPopupTransparent))
        .def_readwrite("mCheckBoxIcon", &Theme::mCheckBoxIcon, D(Theme, mCheckBoxIcon))
        .def_readwrite("mCheckBoxIconExtraScale", &Theme::mCheckBoxIconExtraScale, D(Theme, mCheckBoxIconExtraScale))
        .def_readwrite("mMessageInformationIcon", &Theme::mMessageInformationIcon, D(Theme, mMessageInformationIcon))
        .def_readwrite("mMessageQuestionIcon", &Theme::mMessageQuestionIcon, D(Theme, mMessageQuestionIcon))
        .def_readwrite("mMessageWarningIcon", &Theme::mMessageWarningIcon, D(Theme, mMessageWarningIcon))
        .def_readwrite("mMessageAltButtonIcon", &Theme::mMessageAltButtonIcon, D(Theme, mMessageAltButtonIcon))
        .def_readwrite("mMessagePrimaryButtonIcon", &Theme::mMessagePrimaryButtonIcon, D(Theme, mMessagePrimaryButtonIcon))
        .def_readwrite("mPopupChevronRightIcon", &Theme::mPopupChevronRightIcon, D(Theme, mPopupChevronRightIcon))
        .def_readwrite("mPopupChevronLeftIcon", &Theme::mPopupChevronLeftIcon, D(Theme, mPopupChevronLeftIcon))
        .def_readwrite("mPopupIconExtraScale", &Theme::mPopupIconExtraScale, D(Theme, mPopupIconExtraScale))
        .def_readwrite("mTabHeaderLeftIcon", &Theme::mTabHeaderLeftIcon, D(Theme, mTabHeaderLeftIcon))
        .def_readwrite("mTabHeaderRightIcon", &Theme::mTabHeaderRightIcon, D(Theme, mTabHeaderRightIcon))
        .def_readwrite("mTextBoxUpIcon", &Theme::mTextBoxUpIcon, D(Theme, mTextBoxUpIcon))
        .def_readwrite("mTextBoxDownIcon", &Theme::mTextBoxDownIcon, D(Theme, mTextBoxDownIcon))
        .def_readwrite("mTextBoxIconExtraScale", &Theme::mTextBoxIconExtraScale, D(Theme, mTextBoxIconExtraScale));
}
