/*
    src/window.cpp -- Top-level window widget

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/window.h>
#include <nanogui/theme.h>
#include <nanogui/opengl.h>
#include <nanogui/entypo.h>
#include <nanogui/screen.h>
#include <nanogui/layout.h>
#include <nanogui/serializer/core.h>

NAMESPACE_BEGIN(nanogui)

Window::Window(Widget *parent, const std::string &title)
    : Widget(parent), mTitle(title), mButtonPanel(nullptr), mModal(false), mDrag(false) { }

Vector2i Window::preferredSize(NVGcontext *ctx) const {
    if (mButtonPanel)
        mButtonPanel->setVisible(false);
    Vector2i result = Widget::preferredSize(ctx);
    if (mButtonPanel)
        mButtonPanel->setVisible(true);

    nvgFontSize(ctx, 18.0f);
    nvgFontFace(ctx, "sans-bold");
    float bounds[4];
    nvgTextBounds(ctx, 0, 0, mTitle.c_str(), nullptr, bounds);

    return result.cwiseMax(Vector2i(
        bounds[2]-bounds[0] + 20, bounds[3]-bounds[1]
    ));
}

Widget *Window::buttonPanel() {
    if (!mButtonPanel) {
        mButtonPanel = new Widget(this);
        mButtonPanel->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 4));
    }
    return mButtonPanel;
}

bool Window::isDraggable() const
{
  if (mDraggable == Theme::WindowDraggable::dgAuto)
    return theme()->mWindowDraggable == Theme::WindowDraggable::dgDraggable;

  return mDraggable == Theme::WindowDraggable::dgDraggable;
}

bool Window::mayCollapse() const
{
  if (mMayCollapse == Theme::WindowCollapse::clAuto)
    return theme()->mWindowCollapse == Theme::WindowCollapse::clMayCollapse;

  return mMayCollapse == Theme::WindowCollapse::clMayCollapse;
}

void Window::performLayout(NVGcontext *ctx) {
    if (!mButtonPanel) {
        Widget::performLayout(ctx);
    } else {
        mButtonPanel->setVisible(false);
        Widget::performLayout(ctx);
        for (auto w : mButtonPanel->children()) {
            w->setFixedSize(Vector2i(22, 22));
            w->setFontSize(15);
        }
        mButtonPanel->setVisible(true);
        mButtonPanel->setSize(Vector2i(width(), 22));
        mButtonPanel->setPosition(Vector2i(width() - (mButtonPanel->preferredSize(ctx).x() + 5), 3));
        mButtonPanel->performLayout(ctx);
    }
}

void Window::draw(NVGcontext *ctx) {
    int ds = mTheme->mWindowDropShadowSize, cr = mTheme->mWindowCornerRadius;
    int hh = mTheme->mWindowHeaderHeight;

    /* Draw window */
    int realH = isCollapsed() ? hh : mSize.y();

    nvgSave(ctx);
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos.x(), mPos.y(), mSize.x(), realH, cr);

    nvgFillColor(ctx, mMouseFocus ? mTheme->mWindowFillFocused
                                  : mTheme->mWindowFillUnfocused);
    nvgFill(ctx);

    /* Draw a drop shadow */
    NVGpaint shadowPaint = nvgBoxGradient(
        ctx, mPos.x(), mPos.y(), mSize.x(), realH, cr*2, ds*2,
        mTheme->mDropShadow, mTheme->mTransparent);

    nvgSave(ctx);
    nvgResetScissor(ctx);
    nvgBeginPath(ctx);
    nvgRect(ctx, mPos.x()-ds,mPos.y()-ds, mSize.x()+2*ds, realH +2*ds);
    nvgRoundedRect(ctx, mPos.x(), mPos.y(), mSize.x(), realH, cr);
    nvgPathWinding(ctx, NVG_HOLE);
    nvgFillPaint(ctx, shadowPaint);
    nvgFill(ctx);
    nvgRestore(ctx);

    bool collapsable = mayCollapse(); 
    if (!mTitle.empty()) {
        /* Draw header */
        NVGpaint headerPaint = nvgLinearGradient(
            ctx, mPos.x(), mPos.y(), mPos.x(),
            mPos.y() + hh,
            mTheme->mWindowHeaderGradientTop,
            mTheme->mWindowHeaderGradientBot);

        nvgBeginPath(ctx);
        nvgRoundedRect(ctx, mPos.x(), mPos.y(), mSize.x(), hh, cr);

        nvgFillPaint(ctx, headerPaint);
        nvgFill(ctx);

        nvgBeginPath(ctx);
        nvgRoundedRect(ctx, mPos.x(), mPos.y(), mSize.x(), hh, cr);
        nvgStrokeColor(ctx, mTheme->mWindowHeaderSepTop);

        nvgSave(ctx);
        nvgIntersectScissor(ctx, mPos.x(), mPos.y(), mSize.x(), 0.5f);
        nvgStroke(ctx);
        nvgRestore(ctx);

        nvgBeginPath(ctx);
        nvgMoveTo(ctx, mPos.x() + 0.5f, mPos.y() + hh - 1.5f);
        nvgLineTo(ctx, mPos.x() + mSize.x() - 0.5f, mPos.y() + hh - 1.5);
        nvgStrokeColor(ctx, mTheme->mWindowHeaderSepBot);
        nvgStroke(ctx);

        nvgFontSize(ctx, 18.0f);
        nvgFontFace(ctx, "sans-bold");
        nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);

        nvgFontBlur(ctx, 2);
        nvgFillColor(ctx, mTheme->mDropShadow);
        nvgText(ctx, mPos.x() + mSize.x() / 2,
                mPos.y() + hh / 2, mTitle.c_str(), nullptr);

        nvgFontBlur(ctx, 0);
        nvgFillColor(ctx, mFocused ? mTheme->mWindowTitleFocused
                                   : mTheme->mWindowTitleUnfocused);
        nvgText(ctx, mPos.x() + mSize.x() / 2, mPos.y() + hh / 2 - 1,
                mTitle.c_str(), nullptr);
    }

    if (collapsable) {
      auto icon = utf8(mCollapsed ? ENTYPO_ICON_CHEVRON_SMALL_RIGHT : ENTYPO_ICON_CHEVRON_SMALL_DOWN);

      mCollapseIconSize.y() = fontSize();
      mCollapseIconSize.y() *= mCollapseIconScale;
      nvgFontSize(ctx, mCollapseIconSize.y());
      nvgFontFace(ctx, "icons");
      mCollapseIconSize.x() = nvgTextBounds(ctx, 0, 0, icon.data(), nullptr, nullptr);

      nvgFillColor(ctx, mFocused ? mTheme->mWindowTitleFocused
                                 : mTheme->mWindowTitleUnfocused);
      nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
      mCollapseIconPos = Vector2f(mPos.x() + 5, mPos.y() + (hh - mCollapseIconSize.y())/2 );
      nvgText(ctx, mCollapseIconPos.x(), mCollapseIconPos.y(), icon.data(), nullptr);
    }

    nvgRestore(ctx);

    if (!isCollapsed())
      Widget::draw(ctx);
}

void Window::dispose() {
    Widget *widget = this;
    while (widget->parent())
        widget = widget->parent();
    ((Screen *) widget)->disposeWindow(this);
}

void Window::center() {
    Widget *widget = this;
    while (widget->parent())
        widget = widget->parent();
    ((Screen *) widget)->centerWindow(this);
}

bool Window::mouseDragEvent(const Vector2i &, const Vector2i &rel,
                            int button, int /* modifiers */) {
  if (!isDraggable())
    return false;

    if (mDrag && (button & (1 << GLFW_MOUSE_BUTTON_1)) != 0) {
        mPos += rel;
        mPos = mPos.cwiseMax(Vector2i::Zero());
        mPos = mPos.cwiseMin(parent()->size() - mSize);
        return true;
    }
    return false;
}

bool Window::mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) {
    if (Widget::mouseButtonEvent(p, button, down, modifiers))
        return true;
    if (button == GLFW_MOUSE_BUTTON_1 && mEnabled) {
      Vector2i clkPnt = p - mPos - Vector2i(5,5);
      if (down && clkPnt.x() > 0 && clkPnt.x() < mCollapseIconSize.x()
          && clkPnt.y() > 0 && clkPnt.y() < mCollapseIconSize.y())
      {
        mCollapsed = !mCollapsed;
        return true;
      }
    }
    if (button == GLFW_MOUSE_BUTTON_1 && mEnabled) {
        mDrag = down && (p.y() - mPos.y()) < mTheme->mWindowHeaderHeight;
        return true;
    }
    return false;
}

bool Window::scrollEvent(const Vector2i &p, const Vector2f &rel) {
    Widget::scrollEvent(p, rel);
    return true;
}

void Window::refreshRelativePlacement() {
    /* Overridden in \ref Popup */
}

void Window::save(Serializer &s) const {
    Widget::save(s);
    s.set("title", mTitle);
    s.set("modal", mModal);
}

bool Window::load(Serializer &s) {
    if (!Widget::load(s)) return false;
    if (!s.get("title", mTitle)) return false;
    if (!s.get("modal", mModal)) return false;
    mDrag = false;
    return true;
}

NAMESPACE_END(nanogui)
