/*
    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/**
 * \file nanogui/combobox.h
 *
 * \brief Simple combo box widget based on a popup button.
 */

#pragma once

#include <nanogui/popupbutton.h>

NAMESPACE_BEGIN(nanogui)

/**
 * \class ComboBox combobox.h nanogui/combobox.h
 *
 * \brief Simple combo box widget based on a popup button.
 */
DECLSETTERILIST(ComboBoxItems, std::vector<std::string>)
DECLSETTERILIST(ComboBoxShortItems, std::vector<std::string>)
DECLSETTER(ComboBoxStrCallback, std::function<void(std::string)>)

class NANOGUI_EXPORT ComboBox : public PopupButton {
public:
    RTTI_CLASS_UID("CMBX")
    RTTI_DECLARE_INFO(ComboBox)

    using Items = std::vector<std::string>;
    using ShortItems = std::vector<std::string>;

    /// Create an empty combo box
    explicit ComboBox(Widget *parent);

    /// Create a new combo box with the given items
    explicit ComboBox(Widget *parent, const Items &items);

    /**
     * \brief Create a new combo box with the given items, providing both short and
     * long descriptive labels for each item
     */
    explicit ComboBox(Widget *parent, const Items &items, const ShortItems &itemsShort);

    using PopupButton::set;
    template<typename... Args>
    ComboBox(Widget* parent, const Args&... args)
      : ComboBox(parent) { set<ComboBox, Args...>(args...); }

    /// The callback to execute for this ComboBox.
    std::function<void(int)> callback() const { return mCallback; }

    /// Sets the callback to execute for this ComboBox.
    void setCallback(const std::function<void(int)> &callback) { mCallback = callback; }
    void setCallback(const std::function<void(std::string)> &callback) { mStrCallback = callback; }

    /// The current index this ComboBox has selected.
    int selectedIndex() const { return mSelectedIndex; }

    /// Sets the current index this ComboBox has selected.
    void setSelectedIndex(int idx);

    /// Sets the items for this ComboBox, providing both short and long descriptive lables for each item.
    void setItems(const Items &items, const ShortItems &itemsShort);

    /// Sets the items for this ComboBox.
    void setItems(const Items &items) { setItems(items, items); }

    /// The items associated with this ComboBox.
    const Items &items() const { return mItems; }

    /// The short descriptions associated with this ComboBox.
    const ShortItems &itemsShort() const { return mItemsShort; }

    /// Handles mouse scrolling events for this ComboBox.
    virtual bool scrollEvent(const Vector2i &p, const Vector2f &rel) override;

    /// Saves the state of this ComboBox to the specified Serializer.
    virtual void save(Serializer &s) const override;

    /// Sets the state of this ComboBox from the specified Serializer.
    virtual bool load(Serializer &s) override;

protected:
    /// The items associated with this ComboBox.
    Items mItems;

    /// The short descriptions of items associated with this ComboBox.
    Items mItemsShort;

    /// The callback for this ComboBox.
    std::function<void(int)> mCallback;
    std::function<void(std::string)> mStrCallback;

    /// The current index this ComboBox has selected.
    int mSelectedIndex;

public:
    PROPSETTER(ComboBoxItems, setItems)
    PROPSETTER(ComboBoxStrCallback, setCallback)
};

NAMESPACE_END(nanogui)
