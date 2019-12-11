/*
    src/colorwheel.cpp -- fancy analog widget to select a color value

    This widget was contributed by Dmitriy Morozov.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/colorwheel.h>
#include <nanogui/theme.h>
#include <nanovg.h>
#include <nanogui/serializer/core.h>
#include <algorithm>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(ColorWheel, Widget)

ColorWheel::ColorWheel(Widget *parent, const Color& rgb)
    : Widget(parent), mDragRegion(None) {
    setColor(rgb);
}

Vector2i ColorWheel::preferredSize(NVGcontext *) const {
    return { 100, 100 };
}

void ColorWheel::draw(NVGcontext *ctx) {
    Widget::draw(ctx);

    if (!mVisible)
        return;

    float x = mPos.x(),
          y = mPos.y(),
          w = mSize.x(),
          h = mSize.y();

    NVGcontext* vg = ctx;

    int i;
    float r0, r1, ax,ay, bx,by, cx,cy, aeps, r;
    float hue = mHue;
    NVGpaint paint;

    nvgSave(vg); // save 1

    cx = x + w*0.5f;
    cy = y + h*0.5f;
    r1 = (w < h ? w : h) * 0.5f - 5.0f;
    r0 = r1 * .75f;

    aeps = 0.5f / r1;   // half a pixel arc length in radians (2pi cancels out).

    for (i = 0; i < 6; i++) {
        float a0 = (float)i / 6.0f * NVG_PI * 2.0f - aeps;
        float a1 = (float)(i+1.0f) / 6.0f * NVG_PI * 2.0f + aeps;
        nvgBeginPath(vg);
        nvgArc(vg, cx,cy, r0, a0, a1, NVG_CW);
        nvgArc(vg, cx,cy, r1, a1, a0, NVG_CCW);
        nvgClosePath(vg);
        ax = cx + cosf(a0) * (r0+r1)*0.5f;
        ay = cy + sinf(a0) * (r0+r1)*0.5f;
        bx = cx + cosf(a1) * (r0+r1)*0.5f;
        by = cy + sinf(a1) * (r0+r1)*0.5f;
        paint = nvgLinearGradient(vg, ax, ay, bx, by,
                                  nvgHSLA(a0 / (NVG_PI * 2), 1.0f, 0.55f, 255),
                                  nvgHSLA(a1 / (NVG_PI * 2), 1.0f, 0.55f, 255));
        nvgFillPaint(vg, paint);
        nvgFill(vg);
    }

    nvgBeginPath(vg);
    nvgCircle(vg, cx,cy, r0-0.5f);
    nvgCircle(vg, cx,cy, r1+0.5f);
    nvgStrokeColor(vg, nvgRGBA(0,0,0,64));
    nvgStrokeWidth(vg, 1.0f);
    nvgStroke(vg);

    // Selector
    nvgSave(vg); //save 2
    nvgTranslate(vg, cx,cy);
    nvgRotate(vg, hue*NVG_PI*2);

    // Marker on
    float u = std::max(r1/50, 1.5f);
          u = std::min(u, 4.f);
    nvgStrokeWidth(vg, u);
    nvgBeginPath(vg);
    nvgRect(vg, r0-1,-2*u,r1-r0+2,4*u);
    nvgStrokeColor(vg, nvgRGBA(255,255,255,192));
    nvgStroke(vg);

    paint = nvgBoxGradient(vg, r0-3,-5,r1-r0+6,10, 2,4, nvgRGBA(0,0,0,128), nvgRGBA(0,0,0,0));
    nvgBeginPath(vg);
    nvgRect(vg, r0-2-10,-4-10,r1-r0+4+20,8+20);
    nvgRect(vg, r0-2,-4,r1-r0+4,8);
    nvgPathWinding(vg, NVG_HOLE);
    nvgFillPaint(vg, paint);
    nvgFill(vg);

    // Center triangle
    r = r0 - 6;
    ax = cosf(120.0f/180.0f*NVG_PI) * r;
    ay = sinf(120.0f/180.0f*NVG_PI) * r;
    bx = cosf(-120.0f/180.0f*NVG_PI) * r;
    by = sinf(-120.0f/180.0f*NVG_PI) * r;
    nvgBeginPath(vg);
    nvgMoveTo(vg, r,0);
    nvgLineTo(vg, ax, ay);
    nvgLineTo(vg, bx, by);
    nvgClosePath(vg);
    paint = nvgLinearGradient(vg, r, 0, ax, ay, nvgHSLA(hue, 1.0f, 0.5f, 255),
                              nvgRGBA(255, 255, 255, 255));
    nvgFillPaint(vg, paint);
    nvgFill(vg);
    paint = nvgLinearGradient(vg, (r + ax) * 0.5f, (0 + ay) * 0.5f, bx, by,
                              nvgRGBA(0, 0, 0, 0), nvgRGBA(0, 0, 0, 255));
    nvgFillPaint(vg, paint);
    nvgFill(vg);
    nvgStrokeColor(vg, nvgRGBA(0, 0, 0, 64));
    nvgStroke(vg);

    // Select circle on triangle
    float sx = r*(1 - mWhite - mBlack) + ax*mWhite + bx*mBlack;
    float sy =                           ay*mWhite + by*mBlack;

    nvgStrokeWidth(vg, u);
    nvgBeginPath(vg);
    nvgCircle(vg, sx,sy,2*u);
    nvgStrokeColor(vg, nvgRGBA(255,255,255,192));
    nvgStroke(vg);

    nvgRestore(vg); // restore save 2

    nvgRestore(vg); // restore save 1
}

bool ColorWheel::mouseButtonEvent(const Vector2i &p, int button, bool down,
                                  int modifiers) {
    Widget::mouseButtonEvent(p, button, down, modifiers);
    if (!mEnabled || !isMouseButtonLeft(button))
        return false;

    if (down) {
        mDragRegion = adjustPosition(p);
        return mDragRegion != None;
    } else {
        mDragRegion = None;
        return true;
    }
}

bool ColorWheel::mouseDragEvent(const Vector2i &p, const Vector2i &,
                                int, int) {
    return adjustPosition(p, mDragRegion) != None;
}

ColorWheel::Region ColorWheel::adjustPosition(const Vector2i &p, Region consideredRegions) {
  float x = p.x() - mPos.x(),
    y = p.y() - mPos.y(),
    w = mSize.x(),
    h = mSize.y();

  float cx = w * 0.5f;
  float cy = h * 0.5f;
  float r1 = (w < h ? w : h) * 0.5f - 5.0f;
  float r0 = r1 * .75f;

  x -= cx;
  y -= cy;

  float mr = std::sqrt(x*x + y*y);

  if ((consideredRegions & OuterCircle) &&
    ((mr >= r0 && mr <= r1) || (consideredRegions == OuterCircle))) {
    if (!(consideredRegions & OuterCircle))
      return None;
    mHue = std::atan(y / x);
    if (x < 0)
      mHue += NVG_PI;
    mHue /= 2 * NVG_PI;

    if (mCallback)
      mCallback(color());

    return OuterCircle;
  }

  float sine = sin(-mHue * 2 * NVG_PI);
  float cose = cos(-mHue * 2 * NVG_PI);

  Vector2f xy(cose * x - sine * y, sine * x + cose * y);

  float r = r0 - 6;
  float l0 = (r - xy.x() + std::sqrt(3) * xy.y()) / (3 * r);
  float l1 = (r - xy.x() - std::sqrt(3) * xy.y()) / (3 * r);
  float l2 = 1 - l0 - l1;
  bool triangle_test = l0 >= 0 && l0 <= 1.f && l1 >= 0.f && l1 <= 1.f &&
    l2 >= 0.f && l2 <= 1.f;

  if ((consideredRegions & InnerTriangle) &&
    (triangle_test || consideredRegions == InnerTriangle)) {
    if (!(consideredRegions & InnerTriangle))
      return None;
    l0 = std::min(std::max(0.f, l0), 1.f);
    l1 = std::min(std::max(0.f, l1), 1.f);
    l2 = std::min(std::max(0.f, l2), 1.f);
    float sum = l0 + l1 + l2;
    l0 /= sum;
    l1 /= sum;
    mWhite = l0;
    mBlack = l1;
    if (mCallback)
      mCallback(color());
    return InnerTriangle;
  }

  return None;
}

Color ColorWheel::hue2rgb(float h) const {
    float s = 1., v = 1.;

    if (h < 0) h += 1;

    int i = int(h * 6);
    float f = h * 6 - i;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);

    float r = 0, g = 0, b = 0;
    switch (i % 6) {
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
    }

    return { r, g, b, 1.f };
}

Color ColorWheel::color() const {
    Color rgb    = hue2rgb(mHue);
    Color black  { 0.f, 0.f, 0.f, 1.f };
    Color white  { 1.f, 1.f, 1.f, 1.f };
    return rgb * (1 - mWhite - mBlack) + black * mBlack + white * mWhite;
}

void ColorWheel::setColor(const Color &rgb) {
    float r = rgb.r(), g = rgb.g(), b = rgb.b();

    float M = std::max({ r, g, b });
    float m = std::min({ r, g, b });

    if (M == m) {
      float l = 0.5f * (M + m);
      mHue = 0.f;
      mBlack = 1.f - l;
      mWhite = l;
    }
    else {
      float d = M - m, h;

      if (M == r)
        h = (g - b) / d + (g < b ? 6 : 0);
      else if (M == g)
        h = (b - r) / d + 2;
      else
        h = (r - g) / d + 4;
      h /= 6;

      Color ch = hue2rgb(mHue);
      float M2 = std::max({ ch.r(), ch.g(), ch.b() });
      float m2 = std::min({ ch.r(), ch.g(), ch.b() });

      mWhite = (M*m2 - m*M2) / (m2 - M2);
      mBlack = (M + m2 + m*M2 - m - M*m2 - M2) / (m2 - M2);
      mHue = h;
    }
}

void ColorWheel::save(Serializer &s) const {
    Widget::save(s);
    s.set("hue", mHue);
    s.set("white", mWhite);
    s.set("black", mBlack);
}

bool ColorWheel::load(Serializer &s) {
    if (!Widget::load(s)) return false;
    if (!s.get("hue", mHue)) return false;
    if (!s.get("white", mWhite)) return false;
    if (!s.get("black", mBlack)) return false;
    mDragRegion = Region::None;
    return true;
}

NAMESPACE_END(nanogui)

