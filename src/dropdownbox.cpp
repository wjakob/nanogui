/*
    src/dropdownbox.cpp -- simple dropdown box widget based on a popup button

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/dropdownbox.h>
#include <nanogui/layout.h>
#include <nanovg.h>
#include <nanogui/saveload.h>
#include <algorithm>
#include <cassert>

NAMESPACE_BEGIN(nanogui)

class DropdownListItem : public Button
{
public:
  RTTI_CLASS_UID(DropdownListItem)
  RTTI_DECLARE_INFO(DropdownListItem)

  bool mInlist = true;

  DropdownListItem(Widget* parent, const std::string& str, bool inlist=true)
    : Button(parent), mInlist(inlist) 
  {
    setCaption(str);
  }

  void draw(NVGcontext *ctx) override
  {
    if (!mInlist)
    {
      NVGcolor gradTop = mTheme->mButtonGradientTopPushed;
      NVGcolor gradBot = mTheme->mButtonGradientBotPushed;

      nvgBeginPath(ctx);

      nvgRoundedRect(ctx, mPos.x() + 1, mPos.y() + 1, mSize.x() - 2,
        mSize.y() - 2, mTheme->mButtonCornerRadius - 1);

      if (mBackgroundColor.w() != 0) {
        nvgFillColor(ctx, Color(mBackgroundColor.rgb(), 1.f));
        nvgFill(ctx);
        gradTop.a = gradBot.a = 0.8f;
      }

      NVGpaint bg = nvgLinearGradient(ctx, mPos.x(), mPos.y(), mPos.x(),
        mPos.y() + mSize.y(), gradTop, gradBot);

      nvgFillPaint(ctx, bg);
      nvgFill(ctx);

      nvgBeginPath(ctx);
      nvgStrokeWidth(ctx, 1.0f);
      nvgRoundedRect(ctx, mPos.x() + 0.5f, mPos.y() + 0.5f, mSize.x() - 1, mSize.y(), mTheme->mButtonCornerRadius);
      nvgStrokeColor(ctx, mTheme->mBorderLight);
      nvgStroke(ctx);

      nvgBeginPath(ctx);
      nvgRoundedRect(ctx, mPos.x() + 0.5f, mPos.y() + 0.5f, mSize.x() - 1, mSize.y(), mTheme->mButtonCornerRadius);
      nvgStrokeColor(ctx, mTheme->mBorderDark);
      nvgStroke(ctx);
    }
    else
    {
      if (mMouseFocus && mEnabled) {
        NVGcolor gradTop = mTheme->mButtonGradientTopFocused;
        NVGcolor gradBot = mTheme->mButtonGradientBotFocused;

        nvgBeginPath(ctx);

        nvgRoundedRect(ctx, mPos.x() + 1, mPos.y() + 1.0f, mSize.x() - 2,
          mSize.y() - 2, mTheme->mButtonCornerRadius - 1);

        if (mBackgroundColor.w() != 0) {
          nvgFillColor(ctx, Color(mBackgroundColor.rgb(), 1.f));
          nvgFill(ctx);
          if (mPushed) {
            gradTop.a = gradBot.a = 0.8f;
          }
          else {
            double v = 1 - mBackgroundColor.w();
            gradTop.a = gradBot.a = mEnabled ? v : v * .5f + .5f;
          }
        }

        NVGpaint bg = nvgLinearGradient(ctx, mPos.x(), mPos.y(), mPos.x(),
          mPos.y() + mSize.y(), gradTop, gradBot);

        nvgFillPaint(ctx, bg);
        nvgFill(ctx);
      }
    }

    NVGcolor textColor = mTextColor.w() == 0 ? mTheme->mTextColor : mTextColor;
    if (mPushed && mInlist)
    {
      Vector2f center = mPos.cast<float>() + mSize.cast<float>() * 0.5f;
      nvgBeginPath(ctx);
      nvgCircle(ctx, width() * 0.05f, center.y(), 2);
      nvgFillColor(ctx, textColor);
      nvgFill(ctx);
    }

    int fontSize = mFontSize == -1 ? mTheme->mButtonFontSize : mFontSize;
    nvgFontSize(ctx, fontSize);
    nvgFontFace(ctx, "sans-bold");
    float tw = nvgTextBounds(ctx, 0, 0, mCaption.c_str(), nullptr, nullptr);

    Vector2f center = mPos.cast<float>() + mSize.cast<float>() * 0.5f;
    Vector2f textPos(center.x() - tw * 0.5f, center.y() - 1);
    if (!mEnabled)
      textColor = mTheme->mDisabledTextColor;

    if (mIcon) {
      auto icon = utf8(mIcon);

      float iw, ih = fontSize;
      if (nvgIsFontIcon(mIcon)) {
        ih *= icon_scale();
        nvgFontSize(ctx, ih);
        nvgFontFace(ctx, "icons");
        iw = nvgTextBounds(ctx, 0, 0, icon.data(), nullptr, nullptr);
      }
      else {
        int w, h;
        ih *= 0.9f;
        nvgImageSize(ctx, mIcon, &w, &h);
        iw = w * ih / h;
      }
      if (mCaption != "")
        iw += mSize.y() * 0.15f;
      nvgFillColor(ctx, textColor);
      nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
      Vector2f iconPos = center;
      iconPos.y() -= 1;

      if (mIconAlign == IconAlign::LeftCentered) {
        iconPos.x() -= (tw + iw) * 0.5f;
        textPos.x() += iw * 0.5f;
      }
      else if (mIconAlign == IconAlign::RightCentered) {
        textPos.x() -= iw * 0.5f;
        iconPos.x() += tw * 0.5f;
      }
      else if (mIconAlign == IconAlign::Left) {
        iconPos.x() = mPos.x() + 8;
      }
      else if (mIconAlign == IconAlign::Right) {
        iconPos.x() = mPos.x() + mSize.x() - iw - 8;
      }

      if (nvgIsFontIcon(mIcon)) {
        nvgText(ctx, iconPos.x(), iconPos.y() + 1, icon.data(), nullptr);
      }
      else {
        NVGpaint imgPaint = nvgImagePattern(ctx,
          iconPos.x(), iconPos.y() - ih / 2, iw, ih, 0, mIcon, mEnabled ? 0.5f : 0.25f);

        nvgFillPaint(ctx, imgPaint);
        nvgFill(ctx);
      }
    }

    nvgFontSize(ctx, fontSize);
    nvgFontFace(ctx, "sans-bold");
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgFillColor(ctx, mTheme->mTextColorShadow);
    nvgText(ctx, textPos.x(), textPos.y(), mCaption.c_str(), nullptr);
    nvgFillColor(ctx, textColor);
    nvgText(ctx, textPos.x(), textPos.y() + 1, mCaption.c_str(), nullptr);

    //Widget::draw(ctx);
  }
};

class DropdownPopup : public Popup
{
public:
  RTTI_CLASS_UID(DropdownPopup)
  RTTI_DECLARE_INFO(DropdownPopup)

  int preferredWidth = 0;

  DropdownPopup(Widget *parent, Window *parentWindow)
    : Popup(parent, parentWindow)
  {}

  float targetPath = 0;
  void hide() { targetPath = 0; }

  Vector2i preferredSize(NVGcontext *ctx) const override
  {
    Vector2i result = Popup::preferredSize(ctx);
    result.x() = preferredWidth;
    return result;
  }

  void refreshRelativePlacement() override
  {
    Popup::refreshRelativePlacement();
    mVisible &= mParentWindow->visibleRecursive();
    mPos = mParentWindow->position() + mAnchorPos;
  }

  void updateCaption(const std::string& caption)
  {
    if (mChildren.size() > 0)
    {
      auto* btn = Button::cast(mChildren[0]);
      btn->setCaption(caption);
    }
  }

  void updateVisible(bool visible)
  {
    if (!visible)
    {
      if (path > 0) path -= 0.15f;
      if (path <= 0) path = 0.f;
    }
    else
    {
      if (path < 1.f) path += 0.15f;
      if (path > 1.f) path = 1.f;
    }

    mVisible = path > 0;
  }

  float path = 0.f;

  void draw(NVGcontext* ctx) override
  {
    refreshRelativePlacement();

    if (!mVisible || mChildren.empty())
      return;

    int ds = 1, cr = mTheme->mWindowCornerRadius;
    int ww = mFixedSize.x() > 0 ? mFixedSize.x() : mSize.x();

    int headerH = mChildren[0]->height();
    int realH = clamp<int>(mSize.y() * path, headerH, mSize.y());

    nvgSave(ctx);
    nvgResetScissor(ctx);

    nvgIntersectScissor(ctx, mPos.x()-2, mPos.y() - 2, mSize.x() + 4, realH + 4);

    /* Draw a drop shadow */
    NVGpaint shadowPaint = nvgBoxGradient(
      ctx, mPos.x(), mPos.y(), ww, mSize.y(), cr * 2, ds * 2,
      mTheme->mDropShadow, mTheme->mTransparent);

    nvgBeginPath(ctx);
    nvgRect(ctx, mPos.x() - ds, mPos.y() - ds, ww + 2 * ds, mSize.y() + 2 * ds);
    nvgRoundedRect(ctx, mPos.x(), mPos.y(), ww, mSize.y(), cr);
    nvgPathWinding(ctx, NVG_HOLE);
    nvgFillPaint(ctx, shadowPaint);
    nvgFill(ctx);

    /* Draw window */
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x(), mPos.y(), ww, mSize.y(), cr);

    nvgFillColor(ctx, mTheme->mWindowPopup);
    nvgFill(ctx);

    if (mChildren.size() > 1)
    {
      nvgBeginPath(ctx);

      Vector2i fp = mPos + mChildren[1]->position();
      NVGpaint bg = nvgLinearGradient(ctx, fp.x(), fp.y(), fp.x(), fp.y() + 12 ,
                                      mTheme->mBorderMedium, mTheme->mTransparent);
      nvgRect(ctx, fp.x(), fp.y(), ww, 12);
      nvgFillPaint(ctx, bg);
      nvgFill(ctx);
    }

    Widget::draw(ctx);
    nvgRestore(ctx);
  }
};

void DropdownBox::setFillFunction(std::function<bool(std::string&)> f)
{
  std::vector<std::string> items;
  std::string result;
  bool mayContinue = f(result);
  do
  {
    if (mayContinue)
      items.push_back(result);
    mayContinue = f(result);
  } while (mayContinue);

  setItems(items);
}

DropdownBox::DropdownBox(Widget *parent)
  : PopupButton(parent)
{
  mSelectedIndex = 0;
}

DropdownBox::DropdownBox(Widget *parent, const std::vector<std::string> &items)
    : DropdownBox(parent) 
{
  setItems(items);
}

void DropdownBox::updatePopup()
{
  Window *parentWindow = window();
  if (parentWindow && !mPopup)
  {
    parentWindow->parent()->removeChild(mPopup);

    mPopup = new DropdownPopup(parentWindow->parent(), window());
    mPopup->setSize(Vector2i(320, 250));
    mPopup->setVisible(false);
    mPopup->setAnchorPos(Vector2i(0, 0));
    setItems(mItems, mItemsShort);
  }
}

DropdownBox::DropdownBox(Widget *parent, const std::vector<std::string> &items, const std::vector<std::string> &itemsShort)
    : DropdownBox(parent) 
{
  setItems(items, itemsShort);
}

void DropdownBox::performLayout(NVGcontext *ctx) {
  PopupButton::performLayout(ctx);

  DropdownPopup* dpopup = DropdownPopup::cast(mPopup);
  if (dpopup)
  {
    Window* ww = window();
    dpopup->setAnchorPos(absolutePosition() - ww->absolutePosition());
    dpopup->preferredWidth = width();
  }
}

void DropdownBox::setSelectedIndex(int idx) 
{
  if (mItemsShort.empty())
    return;

  if (mPopup)
  {
    const std::vector<Widget *> &children = popup()->children();
    if (auto b = Button::cast(children[mSelectedIndex]))
      b->setPushed(false);
    if (auto b = Button::cast(children[idx]))
      b->setPushed(true);
    if (auto pp = DropdownPopup::cast(mPopup))
      pp->updateCaption(mItemsShort[idx]);
  }

  mSelectedIndex = idx;
  setCaption(mItemsShort[idx]);
}

void DropdownBox::setItems(const std::vector<std::string> &items, const std::vector<std::string> &itemsShort) {
    assert(items.size() == itemsShort.size());
    mItems = items;
    mItemsShort = itemsShort;
    if (mSelectedIndex < 0 || mSelectedIndex >= (int) items.size())
        mSelectedIndex = 0;

    if (mPopup)
    {
      while (mPopup->childCount() != 0)
        mPopup->removeChild(mPopup->childCount() - 1);

      mPopup->withLayout<GroupLayout>(0, 0, 0, 0);
      if (!items.empty())
      {
        auto& button = mPopup->wdg<DropdownListItem>(items[mSelectedIndex], false);
        button.setPushed(false);
        button.setCallback([&] { setPushed(false); popup()->setVisible(false); });
      }

      int index = 0;
      for (const auto &str : items) {
        auto& button = mPopup->wdg<DropdownListItem>(str);
        button.setFlags(Button::RadioButton);
        button.setCallback([&, index] {
          mSelectedIndex = index;
          setCaption(mItemsShort[index]);
          setPushed(false);
          if (mCallback)
            mCallback(index);
          if (mStrCallback)
            mStrCallback(mItems[index]);
        });
        index++;
      }
    }
    setSelectedIndex(mSelectedIndex);
}

bool DropdownBox::mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers)
{
  if (isMouseButtonLeft(button) && mEnabled) 
  {
    if (!mItems.empty())
    {
      if (auto item = DropdownListItem::cast(mPopup->childAt(0)))
        item->setCaption(mItems[mSelectedIndex]);
    }
  }

  return PopupButton::mouseButtonEvent(p, button, down, modifiers);
}

bool DropdownBox::scrollEvent(const Vector2i &p, const Vector2f &rel) 
{
    if (rel.y() < 0) 
    {
        setSelectedIndex(std::min(mSelectedIndex+1, (int)(items().size()-1)));
        if (mCallback)
            mCallback(mSelectedIndex);
        if (mStrCallback)
          mStrCallback(mItems[mSelectedIndex]);
        return true;
    } 
    else if (rel.y() > 0) 
    {
        setSelectedIndex(std::max(mSelectedIndex-1, 0));
        if (mCallback)
            mCallback(mSelectedIndex);
        if (mStrCallback)
          mStrCallback(mItems[mSelectedIndex]);
        return true;
    }
    return PopupButton::scrollEvent(p, rel);
}

void DropdownBox::draw(NVGcontext* ctx) 
{
  if (!mEnabled && mPushed)
    mPushed = false;

  if (auto pp = DropdownPopup::cast(mPopup))
    pp->updateVisible(mPushed);

  Button::draw(ctx);

  if (mChevronIcon) 
  {
    auto icon = utf8(mChevronIcon);

    nvgFontSize(ctx, (mFontSize < 0 ? mTheme->mButtonFontSize : mFontSize) * icon_scale());
    nvgFontFace(ctx, "icons");
    nvgFillColor(ctx, mEnabled 
                          ? mTextColor.notW(mTheme->mTextColor)
                          : mTheme->mDisabledTextColor);
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);

    float iw = nvgTextBounds(ctx, 0, 0, icon.data(), nullptr, nullptr);
    Vector2f iconPos(0, mPos.y() + mSize.y() * 0.5f - 1);

    if (mPopup->side() == Popup::Right)
      iconPos.x() = mPos.x() + mSize.x() - iw - 8;
    else
      iconPos.x() = mPos.x() + 8;

    nvgText(ctx, iconPos, icon.data());
  }
}

void DropdownBox::save(Json::value &s) const 
{
    Widget::save(s);
    auto obj = s.get_obj();

    obj["items"] = json().set(mItems).name("Items");
    obj["itemsShort"] = json().set(mItemsShort).name("Short items");
    obj["selectedIndex"] = json().set(mSelectedIndex).name("Selected");

    s = Json::value(obj);
}

bool DropdownBox::load(Json::value &save) 
{
  Widget::load(save);
  json s{ save.get_obj() };
  mItems = s.get<decltype(mItems)>("items");
  mItemsShort = s.get<decltype(mItemsShort)>("itemsShort");
  mSelectedIndex = s.get<int>("selectedIndex");
  return true;
}

RTTI_IMPLEMENT_INFO(DropdownBox, PopupButton)
RTTI_IMPLEMENT_INFO(DropdownListItem, Button)
RTTI_IMPLEMENT_INFO(DropdownPopup, Popup)

NAMESPACE_END(nanogui)
