#include <nanogui/fontwidget.h>
#include <nanogui/serializer/core.h>

NAMESPACE_BEGIN(nanogui)

FontWidget::FontWidget(Widget *parent, const std::string &font, bool isWindow)
    : Widget(parent)
    , mFont(font)
    , mFontExplicit(font != "")
    , mFontIsWindow(isWindow) {

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

void FontWidget::save(Serializer &s) const {
    Widget::save(s);
    s.set("font", mFont);
    s.set("fontExplicit", mFontExplicit);
    s.set("fontIsWindow", mFontIsWindow);
}

bool FontWidget::load(Serializer &s) {
    if (!Widget::load(s)) return false;
    if (!s.get("font", mFont)) return false;
    if (!s.get("fontExplicit", mFontExplicit)) return false;
    if (!s.get("fontIsWindow", mFontIsWindow)) return false;
    return true;
}

void FontWidget::setDefaultFont() {
    if (!mFontExplicit) {
        if (mFontIsWindow) {
            if (mTheme) mFont = mTheme->defaultWindowFont();
            else        mFont = Theme::globalDefaultWindowFont();
        }
        else {
            if (mTheme) mFont = mTheme->defaultFont();
            else        mFont = Theme::globalDefaultFont();
        }
    }
}

NAMESPACE_END(nanogui)
