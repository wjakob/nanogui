/*
    src/vscrollpanel.cpp -- Adds a vertical scrollbar around a widget
    that is too big to fit into a certain area

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/vscrollpanel.h>
#include <nanogui/theme.h>
#include <nanovg.h>
#include <nanogui/serializer/core.h>

NAMESPACE_BEGIN(nanogui)

VScrollPanel::VScrollPanel(Widget *parent)
    : Widget(parent), mChildPreferredHeight(0), mScroll(0.0f),
      mUpdateLayout(false), mSliderWidth(8), mSliderMargin(2)
{ }

void VScrollPanel::performLayout(NVGcontext *ctx) {
    Widget::performLayout(ctx);

    if (mChildren.empty())
        return;
    if (mChildren.size() > 1)
        throw std::runtime_error("VScrollPanel should have one child.");

    Widget *child = mChildren[0];
    mChildPreferredHeight = child->preferredSize(ctx).y();

    if (mChildPreferredHeight > mSize.y()) {
        child->setPosition(Vector2i(0, -mScroll*(mChildPreferredHeight - mSize.y())));
        child->setSize(Vector2i(mSize.x()-12, mChildPreferredHeight));
    } else {
        child->setPosition(Vector2i::Zero());
        child->setSize(mSize);
        mScroll = 0;
    }
    child->performLayout(ctx);
}

bool VScrollPanel::mouseEnterEvent(const Vector2i &p, bool enter)
{
  if (!enter)
    mLastMousePos = { -1, -1 };

  return Widget::mouseEnterEvent(p, enter);
}

Vector2i VScrollPanel::preferredSize(NVGcontext *ctx) const {
    if (mChildren.empty())
        return Vector2i::Zero();
    return mChildren[0]->preferredSize(ctx) + Vector2i(12, 0);
}

bool VScrollPanel::mouseDragEvent(const Vector2i &p, const Vector2i &rel,
                            int button, int modifiers)
{
    mLastMousePos = p - mPos;
    if (!mChildren.empty() && mChildPreferredHeight > mSize.y()) {
        float scrollh = height() * std::min(1.0f, height() / (float)mChildPreferredHeight);

        mScroll = std::max((float) 0.0f, std::min((float) 1.0f,
                     mScroll + rel.y() / (float)(mSize.y() - 8 - scrollh)));
        mUpdateLayout = true;
        return true;
    } else {
        return Widget::mouseDragEvent(p, rel, button, modifiers);
    }
}

bool VScrollPanel::mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers)
{
  mLastMousePos = p - mPos;
  return Widget::mouseMotionEvent(p, rel, button, modifiers);
}

bool VScrollPanel::isSliderVisible() const
{
  return mChildPreferredHeight > height();
}

bool VScrollPanel::scrollEvent(const Vector2i &p, const Vector2f &rel) {
    if (!mChildren.empty() && mChildPreferredHeight > mSize.y()) {
        float scrollAmount = rel.y() * (mSize.y() / 20.0f);
        float scrollh = height() * std::min(1.0f, height() / (float)mChildPreferredHeight);

        mScroll = std::max((float) 0.0f, std::min((float) 1.0f,
                mScroll - scrollAmount / (float)(mSize.y() - 8 - scrollh)));
        mUpdateLayout = true;
        return true;
    } else {
        return Widget::scrollEvent(p, rel);
    }
}

void VScrollPanel::draw(NVGcontext *ctx) {
    if (mChildren.empty())
        return;
    Widget *child = mChildren[0];
    child->setPosition(Vector2i(0, -mScroll*(mChildPreferredHeight - mSize.y())));
    mChildPreferredHeight = child->preferredSize(ctx).y();
    float scrollh = height() * std::min(1.0f, height() / (float) mChildPreferredHeight);

    if (mUpdateLayout)
        child->performLayout(ctx);

    nvgSave(ctx);
    nvgTranslate(ctx, mPos.x(), mPos.y());
    nvgIntersectScissor(ctx, 0, 0, mSize.x(), mSize.y());
    if (child->visible())
        child->draw(ctx);
    nvgRestore(ctx);

    if (mChildPreferredHeight <= mSize.y())
        return;

    NVGpaint paint = nvgBoxGradient(
        ctx, mPos.x() + mSize.x() - 12 + 1, mPos.y() + 4 + 1, 8,
        mSize.y() - 8, 3, 4, Color(0, 32), Color(0, 92));
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x() + mSize.x() - 12, mPos.y() + 4, 8,
                   mSize.y() - 8, 3);
    nvgFillPaint(ctx, paint);
    nvgFill(ctx);

    Vector4i rectSlider(mPos.x() + mSize.x() - getSliderAreaWidth() + 1,   //x:x
                        mPos.y() + mSliderMargin * 2 + 1 + (mSize.y() - getSliderWidth() - scrollh) * mScroll, //y:y
                        getSliderWidth() - mSliderMargin,   //z:width
                        scrollh - 2); //w:height
    bool isSliderSelected = (mLastMousePos.x() >= rectSlider.x()
                             && mLastMousePos.y() >= rectSlider.y()
                             && mLastMousePos.x() < rectSlider.x() + rectSlider.z()
                             && mLastMousePos.y() < rectSlider.y() + rectSlider.w());
    Color sliderColor = isSliderSelected
                              ? (mSliderActiveColor.w() > 0 ? mSliderActiveColor : mTheme->mScrollBarActiveColor)
                              : (mSliderInactiveColor.w() > 0 ? mSliderInactiveColor : mTheme->mScrollBarInactiveColor);
    Color sliderSupColor = sliderColor;
    sliderSupColor /= 2;
    sliderSupColor.w() = sliderColor.w();

    paint = nvgBoxGradient( ctx,
                            mPos.x() + mSize.x() - getSliderAreaWidth() - 1, mPos.y() + mSliderMargin * 2 + (mSize.y() - getSliderWidth() - scrollh) * mScroll - 1,
                            getSliderWidth(), scrollh,
                            3, 4, sliderColor, sliderSupColor);

    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, rectSlider.x(), rectSlider.y(), rectSlider.z(), rectSlider.w(), 2);
    nvgFillPaint(ctx, paint);
    nvgFill(ctx);
}

void VScrollPanel::save(Serializer &s) const {
    Widget::save(s);
    s.set("childPreferredHeight", mChildPreferredHeight);
    s.set("scroll", mScroll);
}

bool VScrollPanel::load(Serializer &s) {
    if (!Widget::load(s)) return false;
    if (!s.get("childPreferredHeight", mChildPreferredHeight)) return false;
    if (!s.get("scroll", mScroll)) return false;
    return true;
}

NAMESPACE_END(nanogui)
