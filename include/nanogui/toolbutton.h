#if !defined(__NANOGUI_TOOLBUTTON_H)
#define __NANOGUI_TOOLBUTTON_H

#include <nanogui/button.h>

NANOGUI_NAMESPACE_BEGIN

class ToolButton : public Button {
public:
	ToolButton(Widget *parent, int icon,
		   const std::string &caption = "")
		: Button(parent, caption, icon) {
		setButtonFlags(RadioButton | ToggleButton);
		setFixedSize(Vector2i(25, 25));
	}
};

NANOGUI_NAMESPACE_END

#endif /* __NANOGUI_TOOLBUTTON_H */
