#include <nanogui/spinner.h>
#include <nanogui/nanogui.h>
#include <nanovg.h>

NAMESPACE_BEGIN(nanogui)

Spinner::Spinner(Widget* parent)
  : Widget(parent)
{

}

Vector2i Spinner::preferredSize(NVGcontext *ctx) const
{
  const Vector2i& ps = parent()->size();
  const Vector2i& fs = fixedSize();

  return {fs.x() > 0 ? fs.x() : ps.x(),
          fs.y() > 0 ? fs.y() : ps.y()};
}

void Spinner::draw(NVGcontext* ctx)
{
  float t = getTimeFromStart() * mSpeed;
  float a0 = 0.0f + t * 6;
  float a1 = NVG_PI + t * 6;
  float r = (std::min(width(), height()) / 2) * mRadius;
  float r0 = r;
  float r1 = r * 0.75f;
  float ax,ay, bx,by;

  Vector2i center = mPos + (mSize / 2);
  NVGpaint paint;

  nvgSave(ctx);

  Color gradTop = mTheme->mButtonGradientTopUnfocused;
  Color gradBot = mTheme->mButtonGradientBotUnfocused;

  nvgBeginPath(ctx);
  nvgRoundedRect(ctx, mPos.x() + 1, mPos.y() + 1.0f, mSize.x() - 2,
                      mSize.y() - 2, mTheme->mButtonCornerRadius - 1);

  NVGpaint bg = nvgLinearGradient(ctx, mPos.x(), mPos.y(), mPos.x(),
                                  mPos.y() + mSize.y(), gradTop.mul_a(0.3), gradBot.mul_a(0.3));

  nvgFillPaint(ctx, bg);
  nvgFill(ctx);

  nvgBeginPath(ctx);
  nvgArc(ctx, center.x(), center.y(), r0, a0, a1, NVG_CW);
  nvgArc(ctx, center.x(), center.y(), r1, a1, a0, NVG_CCW);
  nvgClosePath(ctx);
  ax = center.x() + cosf(a0) * (r0+r1)*0.5f;
  ay = center.y() + sinf(a0) * (r0+r1)*0.5f;
  bx = center.x() + cosf(a1) * (r0+r1)*0.5f;
  by = center.y() + sinf(a1) * (r0+r1)*0.5f;
  if (mSpeed < 0)
    paint = nvgLinearGradient(ctx, bx, by, ax, ay, nvgRGBA(0, 0, 0, 0), nvgRGBA(0, 0, 0, 128));
  else
    paint = nvgLinearGradient(ctx, ax,ay, bx,by, nvgRGBA(0,0,0,0), nvgRGBA(0,0,0,128));
  nvgFillPaint(ctx, paint);
  nvgFill(ctx);

  nvgRestore(ctx);
}

NAMESPACE_END(nanogui)
