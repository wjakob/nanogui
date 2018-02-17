#include <nanogui/fontwidget.h>
#include <nanogui/serializer/core.h>

NAMESPACE_BEGIN(nanogui)

FontWidget::FontWidget(Widget *parent, const std::string &font, bool fontDefaultIsBold)
    : Widget(parent)
    , mFont(font)
    , mFontSize(-1)
    , mFontExplicit(font != "")
    , mFontDefaultIsBold(fontDefaultIsBold) {

    setDefaultFont();
}

void FontWidget::setFont(const std::string &font) {
    mFont = font;
    mFontExplicit = true;
}

void FontWidget::setTheme(Theme *theme) {
    Widget::setTheme(theme);
    setDefaultFont();
}

int FontWidget::fontSize() const {
    return (mFontSize < 0 && mTheme) ? mTheme->mStandardFontSize : mFontSize;
}

void FontWidget::save(Serializer &s) const {
    Widget::save(s);
    s.set("font", mFont);
    s.set("fontSize", mFontSize);
    s.set("fontExplicit", mFontExplicit);
    s.set("fontDefaultIsBold", mFontDefaultIsBold);
}

bool FontWidget::load(Serializer &s) {
    if (!Widget::load(s)) return false;
    if (!s.get("font", mFont)) return false;
    if (!s.get("fontSize", mFontSize)) return false;
    if (!s.get("fontExplicit", mFontExplicit)) return false;
    if (!s.get("fontDefaultIsBold", mFontDefaultIsBold)) return false;
    return true;
}

void FontWidget::setDefaultFont() {
    if (!mFontExplicit) {
        if (mFontDefaultIsBold) {
            if (mTheme) mFont = mTheme->defaultBoldFont();
            else        mFont = Theme::globalDefaultBoldFont();
        }
        else {
            if (mTheme) mFont = mTheme->defaultFont();
            else        mFont = Theme::globalDefaultFont();
        }
    }
}

NAMESPACE_END(nanogui)
