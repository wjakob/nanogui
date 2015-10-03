#pragma once

#include <nanogui/popupbutton.h>

NANOGUI_NAMESPACE_BEGIN

class NANOGUI_EXPORT ColorPicker : public PopupButton {
public:
    ColorPicker(Widget *parent, const Color& color = { 1.f, 0.f, 0.f, 1.f });

    /// Set the change callback
    std::function<void(const Color &)> callback() const           { return mCallback; }
    void setCallback(std::function<void(const Color &)> callback) { mCallback = callback; }

    /// Get the current color
    Color color() const;
    /// Set the current color
    void setColor(const Color& color);

protected:
    std::function<void(const Color &)> mCallback;
    ColorWheel *mColorWheel;
    Button *mPickButton;
};

NANOGUI_NAMESPACE_END
