#pragma once

#include <nanogui/widget.h>

NANOGUI_NAMESPACE_BEGIN

class NANOGUI_EXPORT ImageView : public Widget {
public:
    ImageView(Widget *parent, int image = 0);

    void setImage(int img)      { mImage = img; }
    int  image() const          { return mImage; }

    virtual Vector2i preferredSize(NVGcontext *ctx) const;
    virtual void draw(NVGcontext* ctx);

protected:
    int mImage;
};

NANOGUI_NAMESPACE_END
