#pragma once

#include <nanogui/button.h>

NANOGUI_NAMESPACE_BEGIN

class NANOGUI_EXPORT ToolButton : public Button {
public:
    ToolButton(Widget *parent, int icon,
           const std::string &caption = "")
        : Button(parent, caption, icon) {
        setFlags(Flags::RadioButton | Flags::ToggleButton);
        setFixedSize(Vector2i(25, 25));
    }
};

NANOGUI_NAMESPACE_END
