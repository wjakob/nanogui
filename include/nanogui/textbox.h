#pragma once

#include <nanogui/widget.h>

NANOGUI_NAMESPACE_BEGIN

class NANOGUI_EXPORT TextBox : public Widget {
public:
    TextBox(Widget *parent, const std::string &value = "Untitled");

    const std::string &value() const { return mValue; }
    void setValue(const std::string &value) { mValue = value; }

    const std::string &units() const { return mUnits; }
    void setUnits(const std::string &units) { mUnits = units; }

    int unitsImage() const { return mUnitsImage; }
    void setUnitsImage(int image) { mUnitsImage = image; }

    void draw(NVGcontext* ctx);
protected:
    std::string mValue;
    std::string mUnits;
    int mUnitsImage;
};

NANOGUI_NAMESPACE_END
