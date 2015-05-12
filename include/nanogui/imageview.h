#if !defined(__NANOGUI_IMAGEVIEW_H)
#define __NANOGUI_IMAGEVIEW_H

#include <nanogui/widget.h>

NANOGUI_NAMESPACE_BEGIN

class ImageView : public Widget {
public:
    ImageView(Widget *parent);

    void setImage(int img)      { mImage = img; }
    int  image() const          { return mImage; }

    virtual Vector2i preferredSize(NVGcontext *ctx) const;
    virtual void draw(NVGcontext* ctx);

protected:
    int mImage = 0;
};

NANOGUI_NAMESPACE_END

#endif /* __NANOGUI_IMAGEVIEW_H */

