/*
    src/widget.cpp -- Base class of all widgets

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/widget.h>
#include <nanogui/layout.h>
#include <nanogui/theme.h>
#include <nanogui/window.h>
#include <nanogui/opengl.h>
#include <nanogui/screen.h>
#include <nanogui/serializer/core.h>
#include <nanogui/serializer/json.h>

NAMESPACE_BEGIN(nanogui)

Widget::Widget(Widget *parent)
    : mParent(nullptr), mTheme(nullptr), mLayout(nullptr),
      mPos(Vector2i::Zero()), mSize(Vector2i::Zero()),
      mFixedSize(Vector2i::Zero()), mVisible(true), mEnabled(true),
      mFocused(false), mMouseFocus(false), mTooltip(""), mFontSize(-1.0f),
      mIconExtraScale(1.0f), mCursor(Cursor::Arrow) {
    if (parent)
        parent->addChild(this);
}

Widget::~Widget() {
    for (auto child : mChildren) {
        if (child)
            child->decRef();
    }
}

void Widget::setTheme(Theme *theme) {
    if (mTheme.get() == theme)
        return;
    mTheme = theme;
    for (auto child : mChildren)
        child->setTheme(theme);
}

int Widget::fontSize() const {
    return (mFontSize < 0 && mTheme) ? mTheme->mStandardFontSize : mFontSize;
}

Vector2i Widget::preferredSize(NVGcontext *ctx) const {
    if (mLayout)
        return mLayout->preferredSize(ctx, this);
    else
        return mSize;
}

void Widget::performLayout(NVGcontext *ctx) {
    if (mLayout) {
        mLayout->performLayout(ctx, this);
    } else {
        for (auto c : mChildren) {
            Vector2i pref = c->preferredSize(ctx), fix = c->fixedSize();
            c->setSize(Vector2i(
                fix[0] ? fix[0] : pref[0],
                fix[1] ? fix[1] : pref[1]
            ));
            c->performLayout(ctx);
        }
    }
}

Widget *Widget::findWidget(const Vector2i &p) {
    for (auto it = mChildren.rbegin(); it != mChildren.rend(); ++it) {
        Widget *child = *it;
        if (child->visible() && child->contains(p - mPos))
            return child->findWidget(p - mPos);
    }
    return contains(p) ? this : nullptr;
}

bool Widget::mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) {
    for (auto it = mChildren.rbegin(); it != mChildren.rend(); ++it) {
        Widget *child = *it;
        if (child->visible() && child->contains(p - mPos) &&
            child->mouseButtonEvent(p - mPos, button, down, modifiers))
            return true;
    }
    if (button == GLFW_MOUSE_BUTTON_1 && down && !mFocused)
        requestFocus();
    return false;
}

bool Widget::mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) {
    for (auto it = mChildren.rbegin(); it != mChildren.rend(); ++it) {
        Widget *child = *it;
        if (!child->visible())
            continue;
        bool contained = child->contains(p - mPos), prevContained = child->contains(p - mPos - rel);
        if (contained != prevContained)
            child->mouseEnterEvent(p, contained);
        if ((contained || prevContained) &&
            child->mouseMotionEvent(p - mPos, rel, button, modifiers))
            return true;
    }
    return false;
}

bool Widget::scrollEvent(const Vector2i &p, const Vector2f &rel) {
    for (auto it = mChildren.rbegin(); it != mChildren.rend(); ++it) {
        Widget *child = *it;
        if (!child->visible())
            continue;
        if (child->contains(p - mPos) && child->scrollEvent(p - mPos, rel))
            return true;
    }
    return false;
}

bool Widget::mouseDragEvent(const Vector2i &, const Vector2i &, int, int) {
    return false;
}

bool Widget::mouseEnterEvent(const Vector2i &, bool enter) {
    mMouseFocus = enter;
    return false;
}

bool Widget::focusEvent(bool focused) {
    mFocused = focused;
    return false;
}

bool Widget::keyboardEvent(int, int, int, int) {
    return false;
}

bool Widget::keyboardCharacterEvent(unsigned int) {
    return false;
}

Widget* Widget::findWidget(std::function<bool(Widget*)> cond, bool inchildren)
{
  if (cond(this))
    return this;

  if (inchildren)
  {
    for (auto* child : mChildren)
    {
      Widget* w = child->findWidget(cond, inchildren);
      if (w)
        return w;
    }
  }

  return nullptr;
}

Widget* Widget::findWidget(const std::string& id, bool inchildren)
{
  return findWidget([id](Widget* w) -> bool { return w->id() == id; }, inchildren);
}

Widget *Widget::findWidgetGlobal(const std::string& id)
{
  return screen()->findWidget(id, true);
}

Widget *Widget::findWidgetGlobal(std::function<bool(Widget*)> cond)
{
  return screen()->findWidget(cond);
}

void Widget::addChild(int index, Widget * widget) {
    assert(index <= childCount());
    Widget* prevparent = widget->parent();

    mChildren.insert(mChildren.begin() + index, widget);
    widget->incRef();
    widget->setParent(this);
    widget->setTheme(mTheme);

    if (prevparent && prevparent != this)
      prevparent->removeChild(widget);
}

std::string Widget::wtypename() const { return "widget"; }

bool Widget::bringToFront()
{
  if (parent())
    return parent()->bringChildToFront(this);

  return false;
}

bool Widget::sendChildToBack(Widget* child)
{
  auto it = mChildren.begin();
  if (child == (*it))	// already there
    return true;
  for (; it != mChildren.end(); ++it)
  {
    if (child == (*it))
    {
      mChildren.erase(it);
      mChildren.insert(mChildren.begin(), child);
      return true;
    }
  }

  return false;
}

bool Widget::sendToBack()
{
  if (parent())
    return parent()->sendChildToBack(this);

  return false;
}

bool Widget::bringChildToFront(Widget* element)
{
  auto it = mChildren.begin();
  for (; it != mChildren.end(); ++it)
  {
    if (element == (*it))
    {
      mChildren.erase(it);
      mChildren.push_back(element);
      return true;
    }
  }

  return false;
}


void Widget::addChild(Widget * widget) {
    addChild(childCount(), widget);
}

void Widget::removeChild(const Widget *widget) {
    mChildren.erase(std::remove(mChildren.begin(), mChildren.end(), widget), mChildren.end());
    widget->decRef();
}

void Widget::remove()
{
  if (parent())
    parent()->removeChild(this);
}

void Widget::removeChild(int index) {
    Widget *widget = mChildren[index];
    mChildren.erase(mChildren.begin() + index);
    widget->decRef();
}

int Widget::childIndex(Widget *widget) const {
    auto it = std::find(mChildren.begin(), mChildren.end(), widget);
    if (it == mChildren.end())
        return -1;
    return (int) (it - mChildren.begin());
}

Window *Widget::window() {
    Widget *widget = this;
    while (true) {
        if (!widget)
            throw std::runtime_error(
                "Widget:internal error (could not find parent window)");
        Window *window = dynamic_cast<Window *>(widget);
        if (window)
            return window;
        widget = widget->parent();
    }
}

Screen *Widget::screen() {
    Widget *widget = this;
    while (true) {
        if (!widget)
            throw std::runtime_error(
                "Widget:internal error (could not find parent screen)");
        Screen *screen = dynamic_cast<Screen *>(widget);
        if (screen)
            return screen;
        widget = widget->parent();
    }
}

void Widget::requestFocus() {
    Widget *widget = this;
    while (widget->parent())
        widget = widget->parent();
    ((Screen *) widget)->updateFocus(this);
}

void Widget::draw(NVGcontext *ctx) {
    #if NANOGUI_SHOW_WIDGET_BOUNDS
        nvgStrokeWidth(ctx, 1.0f);
        nvgBeginPath(ctx);
        nvgRect(ctx, mPos.x() - 0.5f, mPos.y() - 0.5f, mSize.x() + 1, mSize.y() + 1);
        nvgStrokeColor(ctx, nvgRGBA(255, 0, 0, 255));
        nvgStroke(ctx);
    #endif

    if (mChildren.empty())
        return;

    nvgSave(ctx);
    nvgTranslate(ctx, mPos.x(), mPos.y());
    for (auto child : mChildren) {
        if (child->visible()) {
            nvgSave(ctx);
            nvgIntersectScissor(ctx, child->mPos.x(), child->mPos.y(), child->mSize.x(), child->mSize.y());
            child->draw(ctx);
            nvgRestore(ctx);
        }
    }
    nvgRestore(ctx);
}

void Widget::save(Serializer &s) const {
  s.set("position", mPos); //s.set("$position_type", std::string("vec2")); s.set("$position_name", std::string("Position"));
    s.set("size", mSize);
    s.set("fixedSize", mFixedSize);
    s.set("visible", mVisible);
    s.set("enabled", mEnabled);
    s.set("focused", mFocused);
    s.set("tooltip", mTooltip);
    s.set("fontSize", mFontSize);
    s.set("cursor", (int) mCursor);
}

void Widget::save(Json::value &save) const {
  //s.["position"].get
  Json::hobject p; p.$("x", mPos.x()).$("y", mPos.y()).$("type", "position").$("name", "Position");
  Json::hobject s; s.$("w", mSize.x()).$("h", (int)mSize.y()).$("type", "size").$("name", "Size");
  Json::hobject fs; fs.$("w", mFixedSize.x()).$("h", mFixedSize.y()).$("type", "size").$("name", "Fixed size");
  Json::hobject v; v.$("value", mVisible).$("type", "boolean").$("name", "Visible");
  Json::hobject e; e.$("value", mEnabled).$("type", "boolean").$("name", "Enabled");
  Json::hobject fc; fc.$("value", mFocused).$("type", "boolean").$("name", "Focused");
  Json::hobject t; t.$("value", mTooltip).$("type", "string").$("name", "Tooltip");
  Json::hobject fh; fh.$("value", mFontSize).$("type", "integer").$("name", "Font size");
  Json::hobject cr; cr.$("value", (int)mCursor).$("type", "integer").$("name", "Cursor");

  Json::hobject ret; ret.$("position", p.obj)
                        .$("size", s.obj)
                        .$("fixedSize", fs.obj)
                        .$("visible", v.obj)
                        .$("enabled", e.obj)
                        .$("focused", fc.obj)
                        .$("tooltip", t.obj)
                        .$("fontSize", fh.obj)
                        .$("cursor", cr.obj);

  save = Json::value(ret.obj);
}

bool Widget::load(Json::value &save) { 
  auto p = save.get("position"); mPos = { p.get_int("x"), p.get_int("y") };
  auto s = save.get("size"); mSize = { s.get_int("w"), s.get_int("h") };
  auto fs = save.get("fixedSize"); mFixedSize = { fs.get_int("w"), fs.get_int("h") };
  auto v = save.get("visible"); mVisible = v.get_bool("value");
  auto e = save.get("enabled"); mEnabled = e.get_bool("value");
  auto fc = save.get("focused"); mFocused = fc.get_bool("value");
  auto t = save.get("tooltip"); mTooltip = t.get_str("value");
  auto fh = save.get("fontSize"); mFontSize = fh.get_int("value");
  auto cr = save.get("cursor"); mCursor = (Cursor)cr.get_int("value");
  return true; 
}

bool Widget::load(Serializer &s) {
    if (!s.get("position", mPos)) return false;
    if (!s.get("size", mSize)) return false;
    if (!s.get("fixedSize", mFixedSize)) return false;
    if (!s.get("visible", mVisible)) return false;
    if (!s.get("enabled", mEnabled)) return false;
    if (!s.get("focused", mFocused)) return false;
    if (!s.get("tooltip", mTooltip)) return false;
    if (!s.get("fontSize", mFontSize)) return false;
    if (!s.get("cursor", mCursor)) return false;
    return true;
}

NAMESPACE_END(nanogui)
