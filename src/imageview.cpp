/*
    src/imageview.cpp -- Simple widget which shows an image that was
    previously uploaded to the graphics card

    NanoGUI was developed by Wenzel Jakob <wenzel@inf.ethz.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/imageview.h>
#include <nanogui/opengl.h>

NAMESPACE_BEGIN(nanogui)

ImageView::ImageView(Widget *parent, int img, SizePolicy policy)
    : Widget(parent), mImage(img), mPolicy(policy) {}

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

    if (mPolicy == SizePolicy::Fixed) {
        if (s.x() < w) {
            h = (int) std::round(h * (float) s.x() / w);
            w = s.x();
        }

        if (s.y() < h) {
            w = (int) std::round(w * (float) s.y() / h);
            h = s.y();
        }
    } else {    // mPolicy == Expand
        // expand to width
        h = (int) std::round(h * (float) s.x() / w);
        w = s.x();

        // shrink to height, if necessary
        if (s.y() < h) {
            w = (int) std::round(w * (float) s.y() / h);
            h = s.y();
        }
    }

    NVGpaint imgPaint = nvgImagePattern(ctx, p.x(), p.y(), w, h, 0, mImage, 1.0);

    nvgBeginPath(ctx);
    nvgRect(ctx, p.x(), p.y(), w, h);
    nvgFillPaint(ctx, imgPaint);
    nvgFill(ctx);
}

NAMESPACE_END(nanogui)
