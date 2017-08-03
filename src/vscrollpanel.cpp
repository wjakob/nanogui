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
#include <nanogui/opengl.h>
#include <nanogui/serializer/core.h>

NAMESPACE_BEGIN(nanogui)

static const int kScrollerWidth = 12;

VScrollPanel::VScrollPanel(Widget *parent)
    : Widget(parent), mChildPreferredHeight(0), mScroll(0.0f), mUpdateLayout(false) { }

int VScrollPanel::scrollerHeight() const {
    return height() * std::min(1.0f, height() / (float)mChildPreferredHeight);
}

void VScrollPanel::setNormalizedScroll(float scroll) {
    scroll = std::max(0.0f, std::min(1.0f, scroll));
    if (scroll != mScroll) {
        mScroll = scroll;
        mUpdateLayout = true;
    }
}

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
        child->setSize(Vector2i(mSize.x() - kScrollerWidth, mChildPreferredHeight));
    } else {
        child->setPosition(Vector2i::Zero());
        child->setSize(Vector2i(mSize.x() - kScrollerWidth, mSize.y()));
        mScroll = 0;
    }
    child->performLayout(ctx);
}

Vector2i VScrollPanel::preferredSize(NVGcontext *ctx) const {
    if (mChildren.empty())
        return Vector2i::Zero();

    Vector2i size = mChildren[0]->preferredSize(ctx) + Vector2i(kScrollerWidth, 0);

    // Can never be larger than our parent - we're a scrollview!
    const Widget *parent = this->parent();
    if (parent) {
        Vector2i pos = position();
        if (pos.x() < parent->width())
            size.x() = std::min(size.x(), parent->width() - pos.x());
        if (pos.y() < parent->height())
            size.y() = std::min(size.y(), parent->height() - pos.y());
    }
    return size;
}

bool VScrollPanel::mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) {
    if (down
        && p.x() > (mPos.x() + mSize.x() - kScrollerWidth - 1)
        && !mChildren.empty() && mChildPreferredHeight > mSize.y()) {
        float scrollh = scrollerHeight();
        int scrollerPos = mScroll * (height() - scrollh);
        float delta = (height() + scrollh) / (float)mChildPreferredHeight;

        if (p.y() < scrollerPos) {
            // page-up
            setNormalizedScroll(mScroll - delta);
            return true;
        } else if (p.y() > (scrollerPos + scrollh)) {
            // page-down
            setNormalizedScroll(mScroll + delta);
            return true;
        }
    }
    return Widget::mouseButtonEvent(p, button, down, modifiers);
}

bool VScrollPanel::mouseDragEvent(const Vector2i &p, const Vector2i &rel,
                            int button, int modifiers) {
    if (!mChildren.empty() && mChildPreferredHeight > mSize.y()) {
        float scrollh = scrollerHeight();
        setNormalizedScroll(mScroll + rel.y() / (float)(mSize.y() - 8 - scrollh));
        return true;
    } else {
        return Widget::mouseDragEvent(p, rel, button, modifiers);
    }
}

bool VScrollPanel::scrollEvent(const Vector2i &p, const Vector2f &rel) {
    if (!mChildren.empty() && mChildPreferredHeight > mSize.y()) {
        float scrollAmount = rel.y() * (mSize.y() / 20.0f);
        float scrollh = scrollerHeight();
        setNormalizedScroll(mScroll - scrollAmount / (float)(mSize.y() - 8 - scrollh));
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
        ctx, mPos.x() + mSize.x() - kScrollerWidth + 1, mPos.y() + 4 + 1, 8,
        mSize.y() - 8, 3, 4, mTheme->mTrackGradientTop, mTheme->mTrackGradientBot);
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x() + mSize.x() - kScrollerWidth, mPos.y() + 4, 8,
                   mSize.y() - 8, 3);
    nvgFillPaint(ctx, paint);
    nvgFill(ctx);

    float scrollh = scrollerHeight();

    paint = nvgBoxGradient(
        ctx, mPos.x() + mSize.x() - kScrollerWidth - 1,
        mPos.y() + 4 + (mSize.y() - 8 - scrollh) * mScroll - 1, 8, scrollh,
        3, 4, mTheme->mScrollerGradientTop, mTheme->mScrollerGradientBot);

    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x() + mSize.x() - kScrollerWidth + 1,
                   mPos.y() + 4 + 1 + (mSize.y() - 8 - scrollh) * mScroll, 8 - 2,
                   scrollh - 2, 2);
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
