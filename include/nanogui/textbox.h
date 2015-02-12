#if !defined(__NANOGUI_TEXTBOX_H)
#define __NANOGUI_TEXTBOX_H

#include <nanogui/widget.h>

NANOGUI_NAMESPACE_BEGIN

class TextBox : public Widget {
public:
    TextBox(Widget *parent, const std::string &value = "Untitled");

    inline const std::string &value() const { return mValue; }
    inline void setValue(const std::string &value) { mValue = value; }

    inline const std::string &units() const { return mUnits; }
    inline void setUnits(const std::string &units) { mUnits = units; }

    inline int unitsImage() const { return mUnitsImage; }
    inline void setUnitsImage(int image) { mUnitsImage = image; }

    void draw(NVGcontext* ctx);
protected:
    std::string mValue;
    std::string mUnits;
    int mUnitsImage;
};

NANOGUI_NAMESPACE_END

#endif /* __NANOGUI_TEXTBOX_H */
