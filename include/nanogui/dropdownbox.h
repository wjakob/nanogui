/*
    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/**
 * \file nanogui/dropdownbox.h
 *
 * \brief Simple dropdown box widget based on a popup button.
 */

#pragma once

#include <nanogui/popupbutton.h>

NAMESPACE_BEGIN(nanogui)

/**
 * \class DropdownBox dropdownbox.h nanogui/dropdownbox.h
 *
 * \brief Simple dropdownbox box widget based on a popup button.
 */
DECLSETTERILIST(DropdownBoxItems, std::vector<std::string>)
DECLSETTER(DropdownBoxStrCallback, std::function<void(std::string)>)
DECLSETTER(DropdownBoxFill, std::function<bool(std::string&)>)

class NANOGUI_EXPORT DropdownBox : public PopupButton {
public:
    RTTI_CLASS_UID(DropdownBox)
    RTTI_DECLARE_INFO(DropdownBox)

    using Items = std::vector<std::string>;
    using ShortItems = std::vector<std::string>;

    /// Create an empty combo box
    explicit DropdownBox(Widget *parent);

    /// Create a new combo box with the given items
    explicit DropdownBox(Widget *parent, const Items &items);

    /**
     * \brief Create a new dropdownbox with the given items, providing both short and
     * long descriptive labels for each item
     */
    explicit DropdownBox(Widget *parent, const Items &items, const ShortItems &itemsShort);

    using PopupButton::set;
    template<typename... Args>
    DropdownBox(Widget* parent, const Args&... args)
      : DropdownBox(parent) { set<DropdownBox, Args...>(args...); }

    /// The callback to execute for this widget.
    std::function<void(int)> callback() const { return mCallback; }

    /// Sets the callback to execute for this widget.
    void setCallback(const std::function<void(int)> &callback) { mCallback = callback; }
    void setCallback(const std::function<void(std::string)> &callback) { mStrCallback = callback; }


    /// The current index this dropdownbox has selected.
    int selectedIndex() const { return mSelectedIndex; }

    void performLayout(NVGcontext *ctx) override;

    /// Sets the current index this dropdownbox has selected.
    void setSelectedIndex(int idx);

    /// Sets the items for this dropdownbox, providing both short and long descriptive lables for each item.
    void setItems(const Items &items, const ShortItems &itemsShort);

    /// Sets the items for this dropdownbox.
    void setItems(const Items &items) { setItems(items, items); }

    /// The items associated with this dropdownbox.
    const Items &items() const { return mItems; }

    /// The short descriptions associated with this dropdownbox.
    const ShortItems &itemsShort() const { return mItemsShort; }

    void setFillFunction(std::function<bool(std::string&)> f);

    /// Handles mouse scrolling events for this dropdownbox.
    virtual bool scrollEvent(const Vector2i &p, const Vector2f &rel) override;

    /// Saves the state of this dropdownbox to the specified Serializer.
    virtual void save(Json::value &s) const override;

    /// Sets the state of this dropdownbox from the specified Serializer.
    virtual bool load(Json::value &s) override;

    virtual void draw(NVGcontext* ctx) override;
    virtual bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override;

protected:
    /// The items associated with this dropdownbox.
    Items mItems;

    /// The short descriptions of items associated with this dropdownbox.
    ShortItems mItemsShort;

    /// The callback for this dropdownbox.
    std::function<void(int)> mCallback;
    std::function<void(std::string)> mStrCallback;

    /// The current index this dropdownbox has selected.
    int mSelectedIndex;

    void updatePopup() override;

public:
    PROPSETTER(DropdownBoxItems, setItems)
    PROPSETTER(DropdownBoxStrCallback, setCallback)
    PROPSETTER(DropdownBoxFill, setFillFunction)
};

namespace elm { using DropdownBox = Element<DropdownBox>; }

NAMESPACE_END(nanogui)
