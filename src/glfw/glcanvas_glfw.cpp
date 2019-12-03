/*
    nanogui/glcanvas.cpp -- Canvas widget for rendering full-fledged
    OpenGL content within its designated area. Very useful for
    displaying and manipulating 3D objects or scenes. Subclass it and
    overload `drawGL` for rendering.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/window.h>

#if NANOGUI_OPENGL_BACKEND

#include <nanogui/screen.h>
#include <nanogui/glcanvas.h>
#include <nanogui/theme.h>
#include <nanogui/opengl.h>
#include <nanogui/serializer/core.h>

NAMESPACE_BEGIN(nanogui)

GLCanvas::GLCanvas(Widget *parent)
  : Widget(parent), mBackgroundColor({ 128, 128, 128, 255 }),
    mDrawBorder(true) {
    mSize = Vector2i(250, 250);
}

void GLCanvas::drawWidgetBorder(NVGcontext *ctx) const {
    nvgBeginPath(ctx);
    nvgStrokeWidth(ctx, 1.0f);
    nvgRoundedRect(ctx, mPos.x() - 0.5f, mPos.y() - 0.5f,
                   mSize.x() + 1, mSize.y() + 1, mTheme->mWindowCornerRadius);
    nvgStrokeColor(ctx, mTheme->mBorderLight);
    nvgRoundedRect(ctx, mPos.x() - 1.0f, mPos.y() - 1.0f,
                   mSize.x() + 2, mSize.y() + 2, mTheme->mWindowCornerRadius);
    nvgStrokeColor(ctx, mTheme->mBorderDark);
    nvgStroke(ctx);
}

void GLCanvas::draw(NVGcontext *ctx) {
    Widget::draw(ctx);
    nvgEndFrame(ctx);

    if (mDrawBorder)
        drawWidgetBorder(ctx);

    const Screen* screen = this->screen();
    assert(screen);

    float pixelRatio = screen->pixelRatio();
    Vector2f screenSize = screen->size().cast<float>();
    Vector2i positionInScreen = absolutePosition();

    Vector2i size = (mSize.cast<float>() * pixelRatio).cast<int>(),
             imagePosition = (Vector2f(positionInScreen[0],
                                       screenSize[1] - positionInScreen[1] -
                                       (float) mSize[1]) * pixelRatio).cast<int>();

    GLint storedViewport[4];
    glGetIntegerv(GL_VIEWPORT, storedViewport);

    glViewport(imagePosition[0], imagePosition[1], size[0] , size[1]);

    glEnable(GL_SCISSOR_TEST);
    glScissor(imagePosition[0], imagePosition[1], size[0], size[1]);
    glClearColor(mBackgroundColor.r(), mBackgroundColor.g(),
                 mBackgroundColor.b(), mBackgroundColor.a());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    this->drawGL();

    glDisable(GL_SCISSOR_TEST);
    glViewport(storedViewport[0], storedViewport[1],
               storedViewport[2], storedViewport[3]);
}

void GLCanvas::save(Serializer &s) const {
    Widget::save(s);
    s.set("backgroundColor", mBackgroundColor);
    s.set("drawBorder", mDrawBorder);
}

bool GLCanvas::load(Serializer &s) {
    if (!Widget::load(s)) return false;
    if (!s.get("backgroundColor", mBackgroundColor)) return false;
    if (!s.get("drawBorder", mDrawBorder)) return false;
    return true;
}

NAMESPACE_END(nanogui)

#endif //NANOGUI_OPENGL_BACKEND