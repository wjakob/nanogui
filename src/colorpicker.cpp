#include <nanogui/colorpicker.h>
#include <nanogui/layout.h>
#include <nanogui/colorwheel.h>

NANOGUI_NAMESPACE_BEGIN

ColorPicker::ColorPicker(Widget *parent, const Color& color) : PopupButton(parent, "") {
    setBackgroundColor(color);
    Popup *popup = this->popup();
    popup->setLayout(new GroupLayout());

    mColorWheel = new ColorWheel(popup);
    mPickButton = new Button(popup, "Pick");
    mPickButton->setFixedSize(Vector2i(100, 25));

    PopupButton::setChangeCallback([&](bool) {
        setColor(backgroundColor());
    });

    mColorWheel->setCallback([&](const Color &value) {
        mPickButton->setBackgroundColor(value);
        mPickButton->setTextColor(value.contrastingColor());
    });

    mPickButton->setCallback([&]() {
        Color value = mColorWheel->color();
        setColor(value);
        mCallback(value);
        setPushed(false);
    });
}

Color ColorPicker::color() const {
    return backgroundColor();
}

void ColorPicker::setColor(const Color& color) {
    Color fg = color.contrastingColor();
    setBackgroundColor(color);
    setTextColor(fg);
    mColorWheel->setColor(color);
    mPickButton->setBackgroundColor(color);
    mPickButton->setTextColor(fg);
}

NANOGUI_NAMESPACE_END
