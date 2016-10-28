/*
    nanogui/combobox.h -- simple combo box widget based on a popup button

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/popupbutton.h>

NAMESPACE_BEGIN(nanogui)

/**
 * \class ComboBox combobox.h nanogui/combobox.h
 *
 * \brief Simple combo box widget based on a popup button.
 */
class NANOGUI_EXPORT ComboBox : public PopupButton {
public:
    /// Create an empty combo box
    ComboBox(Widget *parent);

    /// Create a new combo box with the given items
    ComboBox(Widget *parent, const std::vector<std::string> &items);

    /**
     * \brief Create a new combo box with the given items, providing both short and
     * long descriptive labels for each item
     */
    ComboBox(Widget *parent, const std::vector<std::string> &items,
             const std::vector<std::string> &itemsShort);

    std::function<void(int)> callback() const { return mCallback; }
    void setCallback(const std::function<void(int)> &callback) { mCallback = callback; }

    int selectedIndex() const { return mSelectedIndex; }
    void setSelectedIndex(int idx);

    void setItems(const std::vector<std::string> &items, const std::vector<std::string> &itemsShort);
    void setItems(const std::vector<std::string> &items) { setItems(items, items); }
    const std::vector<std::string> &items() const { return mItems; }
    const std::vector<std::string> &itemsShort() const { return mItemsShort; }

    virtual bool scrollEvent(const Vector2i &p, const Vector2f &rel) override;

    virtual void save(Serializer &s) const override;
    virtual bool load(Serializer &s) override;
protected:
    std::vector<std::string> mItems, mItemsShort;
    std::function<void(int)> mCallback;
    int mSelectedIndex;
};

NAMESPACE_END(nanogui)
