#include <nanogui/imageview.h>
#include <nanogui/opengl.h>

NANOGUI_NAMESPACE_BEGIN

ImageView::ImageView(Widget *parent)
    : Widget(parent) {}

Vector2i ImageView::preferredSize(NVGcontext *ctx) const {
    int w,h;
    nvgImageSize(ctx, mImage, &w, &h);
    return Vector2i(w, h);
}

void ImageView::draw(NVGcontext* ctx) {
    Vector2i p = mPos;
    Vector2i s = Widget::size();

    int w = s.x();
    int h = s.y();
    NVGpaint imgPaint = nvgImagePattern(ctx, p.x(), p.y(), w, h, 0, mImage, 1.0);

    nvgBeginPath(ctx);
    nvgRect(ctx, p.x(), p.y(), w, h);
    nvgFillPaint(ctx, imgPaint);
    nvgFill(ctx);
}

NANOGUI_NAMESPACE_END
