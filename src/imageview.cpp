#include <nanogui/imageview.h>
#include <nanogui/opengl.h>

NANOGUI_NAMESPACE_BEGIN

ImageView::ImageView(Widget *parent, int img)
    : Widget(parent), mImage(img) {}

Vector2i ImageView::preferredSize(NVGcontext *ctx) const {
    if (!mImage)
        return Vector2i(0, 0);
    int w,h;
    nvgImageSize(ctx, mImage, &w, &h);
    return Vector2i(w, h);
}

void ImageView::draw(NVGcontext* ctx) {
    if (!mImage)
        return;
    Vector2i p = mPos;
    Vector2i s = Widget::size();

    int w, h;
    nvgImageSize(ctx, mImage, &w, &h);

    if (s.x() < w) {
        h = (int) std::round(h * (float) s.x() / w);
        w = s.x();
    }

    if (s.y() < h) {
        w = (int) std::round(w * (float) s.y() / h);
        h = s.y();
    }

    NVGpaint imgPaint = nvgImagePattern(ctx, p.x(), p.y(), w, h, 0, mImage, 1.0);

    nvgBeginPath(ctx);
    nvgRect(ctx, p.x(), p.y(), w, h);
    nvgFillPaint(ctx, imgPaint);
    nvgFill(ctx);
}

NANOGUI_NAMESPACE_END
