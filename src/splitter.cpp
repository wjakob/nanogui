#include <nanogui/splitter.h>
#include <nanogui/screen.h>
#include <nanogui/window.h>
#include <nanovg.h>
#include <functional>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(Splitter, Widget)

Splitter::Splitter(Widget* parent, Orientation dir)
  : Widget(parent)
{
  mOrientation = dir;
  setCursor(dir == Orientation::Horizontal ? Cursor::VResize : Cursor::HResize);
}

bool Splitter::mouseDragEvent(const Vector2i &, const Vector2i &rel,
  int buttons, int /* modifiers */)
{
  if (isMouseButtonLeftMod(buttons))
  {
    auto& widgets = parent()->children();
    auto it = std::find(widgets.begin(), widgets.end(), this);
    if (it != widgets.end() && it != widgets.begin())
    {
      Widget* prevElem = *(it - 1);
      Vector2i ws = prevElem->size();
      ws += (mOrientation == Orientation::Horizontal) 
                             ? Vector2i{0, rel.y()}
                             : Vector2i{rel.x(), 0};

      prevElem->setFixedSize(ws);
      screen()->needPerformLayout(parent());
    }
    return true;
  }

  return false;
}

void Splitter::draw(NVGcontext* ctx)
{
  Widget::draw(ctx);

  nvgSave(ctx);
  nvgResetScissor(ctx);
  nvgBeginPath(ctx);

  Vector2i tpos = (mOrientation == Orientation::Horizontal) ? Vector2i{ 0, mPos.y() }
                                                            : Vector2i{ mPos.x(), 0 };
  Vector2i tsize = (mOrientation == Orientation::Horizontal) ? Vector2i{ parent()->width(), height() }
                                                             : Vector2i{ width(), parent()->height() };
  Vector2i tc = tpos + tsize / 2;
  Vector2f offs = (mOrientation == Orientation::Horizontal) ? Vector2f{ 6, 0 } : Vector2f{ 0, 6 };
  nvgRoundedRect(ctx, tpos, tsize, 2);
  nvgFillColor(ctx, Color(0, mEnabled ? 32 : 10));
  nvgFill(ctx);

  nvgBeginPath(ctx);
  nvgFillColor(ctx, Color(150, mEnabled ? 255 : 100));
  nvgCircle(ctx, tc.cast<float>(), 2.3f);
  nvgCircle(ctx, tc.cast<float>() + offs, 2.3f);
  nvgCircle(ctx, tc.cast<float>() - offs, 2.3f);
  nvgFill(ctx);

  nvgRestore(ctx);
}

Vector2i Splitter::preferredSize(NVGcontext * /*ctx*/) const
{
  if (mOrientation == Orientation::Horizontal)
    return {1, 5};
  else
    return {5, 1};
}

NAMESPACE_END(nanogui)