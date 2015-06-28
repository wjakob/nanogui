#include <nanogui/popupbutton.h>
#include <nanogui/entypo.h>
#include <nanogui/theme.h>
#include <nanogui/opengl.h>

NANOGUI_NAMESPACE_BEGIN

PopupButton::PopupButton(Widget *parent,const std::string &caption,int icon,bool hideOverlay)
    : Button(parent, caption, icon) {

    setButtonFlags(ToggleButton | Button::PopupButton);

    Window *parentWindow = window();
    mPopup = new Popup(parentWindow->parent(), window());
    mPopup->setSize(Vector2i(320, 250));
    mHideOverlay = hideOverlay;
}

Vector2i PopupButton::preferredSize(NVGcontext *ctx) const {
    return Button::preferredSize(ctx) + Vector2i(15, 0);
}

void PopupButton::draw(NVGcontext* ctx) {
    if (!mEnabled && mPushed)
        mPushed = false;

    mPopup->setVisible(mPushed);
    Button::draw(ctx);

    if(!mHideOverlay) {
      auto icon = utf8(ENTYPO_ICON_CHEVRON_SMALL_RIGHT);
      NVGcolor textColor = mTextColor.w() == 0 ? mTheme->mTextColor : mTextColor;
      nvgFontSize(ctx,(mFontSize < 0 ? mTheme->mButtonFontSize : mFontSize) * 1.5f);
      nvgFontFace(ctx,"icons");
      nvgFillColor(ctx,mEnabled ? textColor : mTheme->mDisabledTextColor);
      nvgTextAlign(ctx,NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);

      float iw = nvgTextBounds(ctx,0,0,icon.data(),nullptr,nullptr);
      Vector2f iconPos(
        mPos.x() + mSize.x() - iw - 8,mPos.y() + mSize.y() * 0.5f -1);

      nvgText(ctx,iconPos.x(),iconPos.y(),icon.data(),nullptr);
    }
}

void PopupButton::performLayout(NVGcontext *ctx) {
    Widget::performLayout(ctx);

    const Window *parentWindow = window();

    mPopup->setAnchorPos(Vector2i(parentWindow->width() + 15,
        absolutePosition().y() - parentWindow->position().y() + mSize.y() /2));
}

NANOGUI_NAMESPACE_END
