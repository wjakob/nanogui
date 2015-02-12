#if !defined(__NANOGUI_PROGRESSBAR_H)
#define __NANOGUI_PROGRESSBAR_H

#include <nanogui/widget.h>

NANOGUI_NAMESPACE_BEGIN

class ProgressBar : public Widget {
public:
    ProgressBar(Widget *parent);

    inline float value() { return mValue; }
    inline void setValue(float value) { mValue = value; }

    virtual Vector2i preferredSize(NVGcontext *ctx) const;
    virtual void draw(NVGcontext* ctx);
protected:
    float mValue;
};


NANOGUI_NAMESPACE_END

#endif /* __NANOGUI_PROGRESSBAR_H */
