#include <nanogui/combobox.h>
#include <nanogui/layout.h>

NANOGUI_NAMESPACE_BEGIN

ComboBox::ComboBox(Widget *parent, const std::vector<std::string> &items)
	: PopupButton(parent), mItems(items), mItemsShort(mItems), mSelectedIndex(0) {
	mPopup->setLayout(new GroupLayout(10));
	int index = 0;
	for (const auto &str: items) {
		Button *button = new Button(mPopup, str);
		button->setButtonFlags(Button::RadioButton);
		button->setCallback([&, index] {
			mSelectedIndex = index;
			setCaption(mItemsShort[index]);
			setPushed(false);
			popup()->setVisible(false);
			if (mCallback)
				mCallback(index);
		});
		index++;
	}
	setSelectedIndex(0);
}

ComboBox::ComboBox(Widget *parent, const std::vector<std::string> &items, const std::vector<std::string> &itemsShort)
	: PopupButton(parent), mItems(items), mItemsShort(itemsShort), mSelectedIndex(0) {
	mPopup->setLayout(new GroupLayout(10));
	int index = 0;
	for (const auto &str: items) {
		Button *button = new Button(mPopup, str);
		button->setButtonFlags(Button::RadioButton);
		button->setCallback([&, index] {
			mSelectedIndex = index;
			setCaption(mItemsShort[index]);
			setPushed(false);
			popup()->setVisible(false);
			if (mCallback)
				mCallback(index);
		});
		index++;
	}
	setSelectedIndex(0);
}

void ComboBox::setSelectedIndex(int idx) {
	const std::vector<Widget *> &children = popup()->children();
	((Button *) children[mSelectedIndex])->setPushed(false);
	((Button *) children[idx])->setPushed(true);
	mSelectedIndex = idx;
	setCaption(mItemsShort[idx]);
}

NANOGUI_NAMESPACE_END
