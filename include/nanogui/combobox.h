#pragma once

#include <nanogui/popupbutton.h>

NANOGUI_NAMESPACE_BEGIN

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
    void setCallback(std::function<void(int)> callback) { mCallback = callback; }

    int selectedIndex() const { return mSelectedIndex; }
    void setSelectedIndex(int idx);

    void setItems(const std::vector<std::string> &items, const std::vector<std::string> &itemsShort);
    void setItems(const std::vector<std::string> &items) { setItems(items, items); }
    const std::vector<std::string> &items() const { return mItems; }
    const std::vector<std::string> &itemsShort() const { return mItemsShort; }
protected:
    std::vector<std::string> mItems, mItemsShort;
    std::function<void(int)> mCallback;
    int mSelectedIndex;
};

NANOGUI_NAMESPACE_END
