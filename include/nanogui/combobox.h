#pragma once

#include <nanogui/popupbutton.h>

NANOGUI_NAMESPACE_BEGIN

class NANOGUI_EXPORT ComboBox : public PopupButton {
public:
    /// Create a new combo box with the given items
    ComboBox(Widget *parent, const std::vector<std::string> &items);

    /// Create a new combo box with the given items, providing both short and long descriptive labels for each item
    ComboBox(Widget *parent, const std::vector<std::string> &items, const std::vector<std::string> &itemsShort);

    inline std::function<void(int)> callback() const { return mCallback; }
    inline void setCallback(std::function<void(int)> callback) { mCallback = callback; }

    inline int selectedIndex() const { return mSelectedIndex; }
    void setSelectedIndex(int idx);

    inline const std::vector<std::string> &items() const { return mItems; }
    inline void setItems(const std::vector<std::string> &items) { mItems = items; }
    inline const std::vector<std::string> &itemsShort() const { return mItems; }
    inline void setItemsShort(const std::vector<std::string> &items) { mItemsShort = items; }
protected:
    std::vector<std::string> mItems, mItemsShort;
    std::function<void(int)> mCallback;
    int mSelectedIndex;
};

NANOGUI_NAMESPACE_END
