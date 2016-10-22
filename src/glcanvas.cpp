/*
    nanogui/glcanvas.h -- OpenGL canvas widget for showing a rendering
    framebuffer

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/window.h>
#include <nanogui/screen.h>
#include <nanogui/glcanvas.h>
#include <nanogui/theme.h>
#include <nanogui/opengl.h>
#include <nanogui/serializer/core.h>

NAMESPACE_BEGIN(nanogui)

namespace {
    constexpr char const *const defaultGLCanvasVertexShader =
        R"(#version 330
        in vec2 vertex;
        out vec2 uv;
        void main() {
            uv = vertex;
            gl_Position = vec4(2.0 * vertex - 1.0, 0.0, 1.0);
        })";

    constexpr char const *const defaultGLCanvasFragmentShader =
        R"(#version 330
        uniform sampler2D image;
        out vec4 color;
        in vec2 uv;
        void main() {
            color = texture(image, uv);
        })";
}

GLCanvas::GLCanvas(Widget *parent)
  : Widget(parent), mBackgroundColor(Vector4i(128, 128, 128, 255)), mFrameBuffer(0), mTexture(0), mDepthRenderBuffer(0), mDrawBorder(true) {
    mSize = Vector2i(250, 250);

    glGenFramebuffers(1, &mFrameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);

    this->resizeTexture();

    glDrawBuffers(1, mDrawBuffers);

    mShader.init("GLCanvasShader", defaultGLCanvasVertexShader, defaultGLCanvasFragmentShader);

    MatrixXu indices(3, 2);
    indices.col(0) << 0, 1, 2;
    indices.col(1) << 2, 3, 1;

    MatrixXf vertices(2, 4);
    vertices.col(0) << 0, 0;
    vertices.col(1) << 1, 0;
    vertices.col(2) << 0, 1;
    vertices.col(3) << 1, 1;

    mShader.bind();
    mShader.uploadIndices(indices);
    mShader.uploadAttrib("vertex", vertices);
}

GLCanvas::~GLCanvas() {
    mShader.free();
    
    glDeleteBuffers(1, &mDepthRenderBuffer);
    glDeleteTextures(1, &mTexture);
    glDeleteBuffers(1, &mFrameBuffer);
}

void GLCanvas::resizeTexture() {
    glDeleteTextures(1, &mTexture);
    glDeleteBuffers(1, &mDepthRenderBuffer);

    glGenTextures(1, &mTexture);
    glBindTexture(GL_TEXTURE_2D, mTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mSize[0], mSize[1], 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, mTexture, 0);

    glGenRenderbuffers(1, &mDepthRenderBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, mDepthRenderBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, mSize[0], mSize[1]);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, mDepthRenderBuffer);
}

Vector2i GLCanvas::preferredSize(NVGcontext *) const {
    return mSize;
}

void GLCanvas::drawWidgetBorder(NVGcontext* ctx) const {
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

    if(mDrawBorder)
      drawWidgetBorder(ctx);

    const Screen* screen = dynamic_cast<const Screen*>(this->window()->parent());
    assert(screen);

    Vector2f screenSize = screen->size().cast<float>();
    Vector2i positionInScreen = absolutePosition();
    Vector2i imagePosition = Vector2i(positionInScreen[0], screenSize[1] - positionInScreen[1] - mSize[1]);

    GLint arrnViewport[4];
    glGetIntegerv(GL_VIEWPORT, arrnViewport);

    glBindFramebuffer(GL_FRAMEBUFFER, mFrameBuffer);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE) {
      glViewport(0, 0, mSize[0], mSize[1]);

      glClearColor(mBackgroundColor[0], mBackgroundColor[1], mBackgroundColor[2], mBackgroundColor[3]);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

      mDrawingCallback();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(imagePosition[0], imagePosition[1], mSize[0], mSize[1]);

    mShader.bind();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mTexture);

    mShader.setUniform("image", 0);
    mShader.drawIndexed(GL_TRIANGLES, 0, 2);

    glViewport(arrnViewport[0], arrnViewport[1], arrnViewport[2], arrnViewport[3]);
}

void GLCanvas::setGLDrawingCallback(std::function<void()> fncDraw) {
  mDrawingCallback = fncDraw;
}

void GLCanvas::setSize(const Vector2i &size) {
  this->Widget::setSize(size);
  this->resizeTexture();
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
