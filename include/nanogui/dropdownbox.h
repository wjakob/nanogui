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
using DropdownBoxItems = std::vector<std::string>;
class NANOGUI_EXPORT DropdownBox : public PopupButton {
public:
    /// Create an empty combo box
    DropdownBox(Widget *parent);

    /// Create a new combo box with the given items
    DropdownBox(Widget *parent, const DropdownBoxItems &items);

    /**
     * \brief Create a new dropdownbox with the given items, providing both short and
     * long descriptive labels for each item
     */
    DropdownBox(Widget *parent, const DropdownBoxItems &items,
             const DropdownBoxItems &itemsShort);

    /// The callback to execute for this widget.
    std::function<void(int)> callback() const { return mCallback; }

    /// Sets the callback to execute for this widget.
    void setCallback(const std::function<void(int)> &callback) { mCallback = callback; }

    /// The current index this dropdownbox has selected.
    int selectedIndex() const { return mSelectedIndex; }

    void performLayout(NVGcontext *ctx) override;

    /// Sets the current index this dropdownbox has selected.
    void setSelectedIndex(int idx);

    /// Sets the items for this dropdownbox, providing both short and long descriptive lables for each item.
    void setItems(const DropdownBoxItems &items, const DropdownBoxItems &itemsShort);

    /// Sets the items for this dropdownbox.
    void setItems(const DropdownBoxItems &items) { setItems(items, items); }

    /// The items associated with this dropdownbox.
    const DropdownBoxItems &items() const { return mItems; }

    /// The short descriptions associated with this dropdownbox.
    const DropdownBoxItems &itemsShort() const { return mItemsShort; }

    /// Handles mouse scrolling events for this dropdownbox.
    virtual bool scrollEvent(const Vector2i &p, const Vector2f &rel) override;

    /// Saves the state of this dropdownbox to the specified Serializer.
    virtual void save(Serializer &s) const override;

    /// Sets the state of this dropdownbox from the specified Serializer.
    virtual bool load(Serializer &s) override;

    virtual void draw(NVGcontext* ctx) override;
    virtual bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override;

protected:
    /// The items associated with this dropdownbox.
    DropdownBoxItems mItems;

    /// The short descriptions of items associated with this dropdownbox.
    DropdownBoxItems mItemsShort;

    /// The callback for this dropdownbox.
    std::function<void(int)> mCallback;

    /// The current index this dropdownbox has selected.
    int mSelectedIndex;

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

NAMESPACE_END(nanogui)
