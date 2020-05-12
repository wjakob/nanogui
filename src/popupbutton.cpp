/*
    src/popupbutton.cpp -- Button which launches a popup widget

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/popupbutton.h>
#include <nanogui/theme.h>
#include <nanovg.h>
#include <nanogui/saveload.h>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(PopupButton, Button)

PopupButton::PopupButton(Widget *parent)
    : Button(parent) 
{
  mChevronIcon = mTheme ? mTheme->mPopupChevronRightIcon : -1;

  setFlags(Flag::ToggleButton | Flag::PopupButton);

  updatePopup();

  mIconExtraScale = 0.8f;// widget override
}

PopupButton::~PopupButton() { if (mPopup) mPopup->setVisible(false); }
void PopupButton::setPopupSide(int side) { if(mPopup) mPopup->setSide(Popup::Side(side)); }

Vector2i PopupButton::preferredSize(NVGcontext *ctx) const {
    return Button::preferredSize(ctx) + Vector2i(15, 0);
}

void PopupButton::updatePopup()
{
  Window *parentWindow = window();
  if (parentWindow && !mPopup)
  {
    mPopup = parentWindow->parent()->add<Popup>(window());
    mPopup->setSize(320, 250);
    mPopup->setVisible(false);
  }
}

void PopupButton::parentChanged()
{
  Window *parentWindow = window();
  if (parentWindow && mPopup)
  {
    parentWindow->parent()->addChild(mPopup);
    mPopup->setParentWindow(parentWindow);
  }
}

void PopupButton::setPopup(Popup* pp)
{
  Window *parentWindow = window();
  if (parentWindow)
  {
    Popup::Side side = Popup::Side::Right;
    if (mPopup)
    {
      side = mPopup->side();
      mPopup->removeLater();
      mPopup = nullptr;
    }

    parentWindow->parent()->addChild(pp);
    pp->setParentWindow(parentWindow);
    pp->setSide(side);
    mPopup = pp;
  }
}

void PopupButton::draw(NVGcontext* ctx) 
{
    if (!mEnabled && mPushed)
        mPushed = false;

    mPopup->setVisible(mPushed);
    Button::draw(ctx);

    if (mChevronIcon) {
        auto icon = utf8(mChevronIcon);
        NVGcolor textColor = mTextColor.notW(mTheme->mTextColor);

        nvgFontSize(ctx, (mFontSize < 0 ? mTheme->mButtonFontSize : mFontSize) * icon_scale());
        nvgFontFace(ctx, "icons");
        nvgFillColor(ctx, mEnabled ? textColor : mTheme->mDisabledTextColor);
        nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);

        float iw = nvgTextBounds(ctx, 0, 0, icon.data(), nullptr, nullptr);
        Vector2f iconPos(0, mPos.y() + mSize.y() * 0.5f - 1);

        if (mPopup->side() == Popup::Right)
            iconPos[0] = mPos.x() + mSize.x() - iw - 8;
        else
            iconPos[0] = mPos.x() + 8;

        nvgText(ctx, iconPos.x(), iconPos.y(), icon.data(), nullptr);
    }
}

void PopupButton::performLayout(NVGcontext *ctx) {
    Widget::performLayout(ctx);

    if (mChevronIcon < 0)
      mChevronIcon = mTheme ? mTheme->mPopupChevronRightIcon : -1;

    updatePopup();

    const Window *parentWindow = window();

    int posY = absolutePosition().y() - parentWindow->position().y() + mSize.y() /2;
    if (mPopup->side() == Popup::Right)
        mPopup->setAnchorPos(Vector2i(parentWindow->width() + 15, posY));
    else
        mPopup->setAnchorPos(Vector2i(0 - 15, posY));
}

void PopupButton::setSide(Popup::Side side) {
    if (mPopup->side() == Popup::Right &&
        mChevronIcon == mTheme->mPopupChevronRightIcon)
        setChevronIcon(mTheme->mPopupChevronLeftIcon);
    else if (mPopup->side() == Popup::Left &&
             mChevronIcon == mTheme->mPopupChevronLeftIcon)
        setChevronIcon(mTheme->mPopupChevronRightIcon);
    mPopup->setSide(side);
}

void PopupButton::save(Json::value &save) const 
{
  Button::save(save);
  Json::object obj = save.get_obj();
  obj["chevronIcon"] = json().set(mChevronIcon).name("Chevron");

  save = Json::value(obj);
}

bool PopupButton::load(Json::value &save) 
{
  Button::load(save);
  json s{ save.get_obj() };

  mChevronIcon = s.get<int>("chevronIcon");
  return true;
}

NAMESPACE_END(nanogui)
