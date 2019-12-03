/*
    src/vscrollbar.cpp -- Adds a vertical scrollbar around a widget
    that is too big to fit into a certain area

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/scrollbar.h>
#include <nanogui/theme.h>
#include <nanovg.h>
#include <nanogui/serializer/core.h>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(ScrollBar, Widget)

ScrollBar::ScrollBar(Widget *parent, Alignment align)
    : Widget(parent),
      mSliderPreferredSide(1),
      mScroll(0.0f),
      mAlign(align)
{}

void ScrollBar::performLayout(NVGcontext *ctx) {
    Widget::performLayout(ctx);

    Widget* p = parent();
    if (mAlign == Alignment::VerticalLeft || mAlign == Alignment::VerticalRight)
    {
      setSize(12, p->height());
      if (mAlign == Alignment::VerticalRight)
        setPosition(p->width() - 6, 0);
      else
        setPosition(0, 0);

      mSliderPreferredSide = height() * 3;
    }
    else
    {
      setSize(p->width(), 12);
      if (mAlign == Alignment::HorizontalBottom)
        setPosition(0, p->height() - 6);
      else
        setPosition(0, 0);

      mSliderPreferredSide = width() * 3;
    }
}

Vector2i ScrollBar::preferredSize(NVGcontext * /* ctx */ ) const {
  if (mAlign == Alignment::VerticalLeft || mAlign == Alignment::VerticalRight)
  {
    return Vector2i(12, parent()->height());
  }
  else
    return Vector2i(parent()->width(), 12);
}

bool ScrollBar::mouseDragEvent(const Vector2i &p, const Vector2i &rel,
                                int button, int modifiers) {
  if (mAlign == Alignment::VerticalLeft || mAlign == Alignment::VerticalRight)
  {
    if (mSliderPreferredSide > mSize.y()) {
      float scrollh = height() * std::min(1.0f, height() / (float)mSliderPreferredSide);

      mScroll = std::max((float) 0.0f, std::min((float) 1.0f, mScroll + rel.y() / (float)(mSize.y() - 8 - scrollh)));
      if (mCallback)
        mCallback(mScroll);
      return true;
    }
  }
  else if (mAlign == Alignment::HorizontalBottom || mAlign == Alignment::HorizontalTop)
  {
    if (mSliderPreferredSide > mSize.x()) {
      float scrollw = width() * std::min(1.0f, width() / (float)mSliderPreferredSide);

      mScroll = std::max((float) 0.0f, std::min((float) 1.0f, mScroll + rel.x() / (float)(mSize.x() - 8 - scrollw)));
      if (mCallback)
        mCallback(mScroll);
      return true;
    }
  }
 
  return Widget::mouseDragEvent(p, rel, button, modifiers);
}

bool ScrollBar::scrollEvent(const Vector2i &p, const Vector2f &rel) {
  if (mAlign == Alignment::VerticalLeft || mAlign == Alignment::VerticalRight)
  {
    if (mSliderPreferredSide > mSize.y()) {
      float scrollAmount = rel.y() * (mSize.y() / 20.0f);
      float scrollh = height() * std::min(1.0f, height() / (float)mSliderPreferredSide);

      mScroll = std::max((float) 0.0f, std::min((float) 1.0f,
        mScroll - scrollAmount / (float)(mSize.y() - 8 - scrollh)));
      if (mCallback)
        mCallback(mScroll);
      return true;
    }
  }
  else if (mAlign == Alignment::HorizontalBottom || mAlign == Alignment::HorizontalTop)
  {
    if (mSliderPreferredSide > mSize.x()) {
      float scrollAmount = rel.y() * (mSize.x() / 20.0f);
      float scrollw = width() * std::min(1.0f, width() / (float)mSliderPreferredSide);

      mScroll = std::max((float) 0.0f, std::min((float) 1.0f,  mScroll - scrollAmount / (float)(mSize.x() - 8 - scrollw)));
      if (mCallback)
        mCallback(mScroll);
      return true;
    }
    return true;
  }

  return Widget::scrollEvent(p, rel);
}

void ScrollBar::draw(NVGcontext *ctx) {
  if (mAlign == Alignment::VerticalLeft || mAlign == Alignment::VerticalRight)
  {
    float scrollh = height() * std::min(1.0f, height() / (float)mSliderPreferredSide);

    if (mSliderPreferredSide <= mSize.y())
      return;

    int ww = mAlign == Alignment::VerticalLeft ? 0 : mSize.x();
    int wx = mAlign == Alignment::VerticalLeft ? 0 : 12;
    int dx = mAlign == Alignment::VerticalLeft ? -2 : 0;
    NVGpaint paint = nvgBoxGradient( ctx, mPos.x() + ww - wx + 1, mPos.y() + 4 + 1, 8, mSize.y() - 8,
                                     3, 4, Color(0, 32), Color(0, 92));
    //body
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x() + ww - wx + dx, mPos.y() + 4, 8, mSize.y() - 8, 3);
    nvgFillPaint(ctx, paint);
    nvgFill(ctx);

    paint = nvgBoxGradient(ctx, mPos.x() + ww - wx - 1,
                           mPos.y() + 4 + (mSize.y() - 8 - scrollh) * mScroll - 1, 8, scrollh,
                           3, 4, Color(220, 100), Color(128, 100));

    //slider
    int sx = mAlign == Alignment::VerticalLeft ? -1 : 1;
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x() + ww - wx + sx,
                   mPos.y() + 4 + 1 + (mSize.y() - 8 - scrollh) * mScroll, 8 - 2,
                   scrollh - 2, 2);
    nvgFillPaint(ctx, paint);
    nvgFill(ctx);
  }
  else if (mAlign == Alignment::HorizontalBottom || mAlign == Alignment::HorizontalTop)
  {
    float scrollw = width() * std::min(1.0f, width() / (float)mSliderPreferredSide);

    if (mSliderPreferredSide <= mSize.x())
      return;

    NVGpaint paint = nvgBoxGradient( ctx, mPos.x() + 4 + 1, mPos.y() + mSize.y() - 12 + 1, mSize.x() - 8, 8,
                                     3, 4, Color(0, 32), Color(0, 92));
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x() + 4, mPos.y() + mSize.y() - 12, mSize.x() - 8, 8, 3);
    nvgFillPaint(ctx, paint);
    nvgFill(ctx);

    paint = nvgBoxGradient(ctx, mPos.x() + 4 + (mSize.x() - 8 - scrollw) * mScroll - 1,
                                mPos.y() + mSize.y() - 12 - 1, scrollw, 9,
                           3, 4, Color(220, 100), Color(128, 100));

    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x() + 4 + 1 + (mSize.x() - 8 - scrollw) * mScroll,
                        mPos.y() + mSize.y() - 12 + 1, scrollw - 2, 8 - 2, 2);
    nvgFillPaint(ctx, paint);
    nvgFill(ctx);
  }
}

void ScrollBar::save(Serializer &s) const {
    Widget::save(s);
    s.set("sliderPreferredSide", mSliderPreferredSide);
    s.set("alignment", mAlign);
    s.set("scroll", mScroll);
}

bool ScrollBar::load(Serializer &s) {
    if (!Widget::load(s)) return false;
    if (!s.get("sliderPreferredSide", mSliderPreferredSide)) return false;
    if (!s.get("alignment", mAlign)) return false;
    if (!s.get("scroll", mScroll)) return false;
    return true;
}

NAMESPACE_END(nanogui)
