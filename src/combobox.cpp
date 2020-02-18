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
#include <nanogui/serializer/json.h>
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

void ComboBox::setSelectedIndex(int idx) 
{
    if (mItemsShort.empty())
        return;
    const std::vector<Widget *> &children = popup()->children();
    if (auto button = Button::cast(children[mSelectedIndex]))
      button->setPushed(false);
    if (auto button = Button::cast(children[idx]))
      button->setPushed(true);
    mSelectedIndex = idx;
    setCaption(mItemsShort[idx]);
}

void ComboBox::setItems(const std::vector<std::string> &items, const std::vector<std::string> &itemsShort) 
{
    assert(items.size() == itemsShort.size());
    mItems = items;
    mItemsShort = itemsShort;
    if (mSelectedIndex < 0 || mSelectedIndex >= (int) items.size())
        mSelectedIndex = 0;
    while (mPopup->childCount() != 0)
        mPopup->removeChild(mPopup->childCount()-1);
    mPopup->withLayout<GroupLayout>(10);
    int index = 0;
    for (const auto &str: items) 
    {
        auto& button = mPopup->button(str);
        button.setFlags(Button::RadioButton);
        button.setCallback([&, index] {
            mSelectedIndex = index;
            setCaption(mItemsShort[index]);
            setPushed(false);
            popup()->setVisible(false);
            if (mCallback)
              mCallback(index);
            if (mStrCallback)
              mStrCallback(mItems[index]);
        });
        index++;
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
    Json::object obj = save.get_obj();
    Json::value::array items;
    for (auto& e : mItems) items.push_back(Json::value(e));
    obj["items"] = Json::hobject().$("value", items).$("type", "array").$("name", "Items");
    Json::value::array itemsShort;
    for (auto& e : mItemsShort) itemsShort.push_back(Json::value(e));
    obj["itemsShort"] = Json::hobject().$("value", itemsShort).$("type", "array").$("name", "Items short");
    obj["selectedIndex"] = Json::hobject().$("value", mSelectedIndex).$("type", "integer").$("name", "Selected");

    save = Json::value(obj);
}

bool ComboBox::load(Json::value &save) {
    Widget::load(save);
    mItems.clear();
    auto items = save.get("items"); 
    { 
      int i = 0; 
      Json::value e = items.get(i);
      while(!e.is<Json::null>())
      {
        mItems.push_back(e.to_str());
        e = items.get(i++);
      }
    }

    auto shortItems = save.get("items");
    mItemsShort.clear();
    {
      int i = 0;
      Json::value e = items.get(i);
      while (!e.is<Json::null>())
      {
        mItemsShort.push_back(e.to_str());
        e = items.get(i++);
      }
    }

    auto s = save.get("selectedIndex"); mSelectedIndex = s.get_int("value");
    return true;
}

NAMESPACE_END(nanogui)
