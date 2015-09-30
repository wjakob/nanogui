#pragma once

#include <nanogui/widget.h>

NANOGUI_NAMESPACE_BEGIN

class NANOGUI_EXPORT ProgressBar : public Widget {
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
