/*
    src/combobox.cpp -- simple combo box widget based on a popup button

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/combobox.h>
#include <nanogui/layout.h>
#include <nanogui/saveload.h>
#include <cassert>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(ComboBox, PopupButton)

ComboBox::ComboBox(Widget *parent) 
  : PopupButton(parent), mSelectedIndex(0) 
{}

ComboBox::ComboBox(Widget *parent, const std::vector<std::string> &items)
    : PopupButton(parent), mSelectedIndex(0) 
{
    setItems(items);
}

ComboBox::ComboBox(Widget *parent, const std::vector<std::string> &items, const std::vector<std::string> &itemsShort)
    : PopupButton(parent), mSelectedIndex(0) 
{
    setItems(items, itemsShort);
}

void ComboBox::updatePopup()
{
  bool needUpdateItems = mPopup == nullptr;
  PopupButton::updatePopup();
  if (needUpdateItems)
    setItems(mItems, mItemsShort);
}

void ComboBox::setSelectedIndex(int idx) 
{
  if (mItemsShort.empty())
    return;
  mSelectedIndex = idx;
  setCaption(mItemsShort[idx]);

  if (auto p = popup())
  {
    const std::vector<Widget *> &children = p->children();
    if (auto button = Button::cast(children[mSelectedIndex]))
      button->setPushed(false);
    if (auto button = Button::cast(children[idx]))
      button->setPushed(true);
  }
}

void ComboBox::resolveClickItem(int index)
{
  mSelectedIndex = index;
  setCaption(mItemsShort[index]);
  setPushed(false);
  popup()->setVisible(false);

  if (mCallback)
    mCallback(index);

  if (mStrCallback)
    mStrCallback(mItems[index]);
}

void ComboBox::setItems(const std::vector<std::string> &items, const std::vector<std::string> &itemsShort) 
{
  assert(items.size() == itemsShort.size());
  mItems = items;
  mItemsShort = itemsShort;
  if (mSelectedIndex < 0 || mSelectedIndex >= (int) items.size())
      mSelectedIndex = 0;

  if (mPopup)
  {
    while (mPopup->childCount() != 0)
      mPopup->removeChild(mPopup->childCount() - 1);
    mPopup->withLayout<GroupLayout>(10);
    int index = 0;
    for (const auto &str : items)
    {
      auto& button = mPopup->button(Caption{ str });
      button.setFlags(Button::RadioButton);
      button.setCallback([this, index] { this->resolveClickItem(index); });
      index++;
    }
  }
  setSelectedIndex(mSelectedIndex);
}

bool ComboBox::scrollEvent(const Vector2i &p, const Vector2f &rel) 
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
  return Widget::scrollEvent(p, rel);
}

void ComboBox::save(Json::value &save) const {
    Widget::save(save);
    auto obj = save.get_obj();
    obj["items"] = json().set(mItems).name("Items");
    obj["itemsShort"] = json().set(mItemsShort).name("Items short");

    obj["selectedIndex"] = json().set(mSelectedIndex).name("Selected");

    save = Json::value(obj);
}

bool ComboBox::load(Json::value &save) {
    Widget::load(save);
    json s{ save.get_obj() };
    mItems = s.get<std::vector<std::string>>("items");
    mItemsShort = s.get<std::vector<std::string>>("shortItems");

    mSelectedIndex = s.get<int>("selectedIndex");
    return true;
}

NAMESPACE_END(nanogui)
