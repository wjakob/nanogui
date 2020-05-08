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
#include <nanovg.h>
#include <nanogui/screen.h>
#include <nanogui/saveload.h>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(Widget, Object)

std::vector<intptr_t> Widget::mFocusChain;

Widget::Widget(Widget *p)
    : mParent(p ? p : elm::get_active_widget()), mTheme(nullptr), mLayout(nullptr),
      mPos(Vector2i::Zero()), mSize(Vector2i::Zero()),
      mFixedSize(Vector2i::Zero()), mRelSize(Vector2f::Zero()), mEnabled(true),
      mFocused(false), mMouseFocus(false), mTooltip(""), mFontSize(-1.0f),
      mIconExtraScale(1.0f), mCursor(Cursor::Arrow) 
{
    if (parent())
      parent()->addChild(this);
    mVisible = true;
}

Widget::~Widget() {
    for (auto child : mChildren) {
        if (child)
            child->decRef();
    }
}

void Widget::setThemeFrom(const std::string& widgetId)
{
  if (auto w = findWidgetGlobal(widgetId))
    setTheme(w->theme());
}

void Widget::setTheme(Theme *theme) 
{
  if (!theme)
    return;
  
  if (mTheme.get() == theme)
      return;
  mTheme = theme;
  for (auto child : mChildren)
      child->setTheme(theme);
  if (mTooltipWdiget)
    mTooltipWdiget->setTheme(theme);
}

int Widget::fontSize() const {
    return (mFontSize < 0 && mTheme) ? mTheme->mStandardFontSize : mFontSize;
}

Vector2i Widget::preferredSize()
{
  Screen* scr = screen();
  return scr ? preferredSize(scr->nvgContext()) : Vector2i::Zero();
}

Vector2i Widget::preferredSize(NVGcontext *ctx) const {
    if (mLayout)
        return mLayout->preferredSize(ctx, this);
    else
    {
      return mSize.cwiseMax(minSize());
    }
}

void Widget::performLayoutLater()
{
  if (auto scr = screen())
    scr->needPerformLayout(parent());
}

void Widget::performLayout(NVGcontext *ctx)
{
  if (mLayout)
  {
    mLayout->performLayout(ctx, this);
  }
  else
  {
    for (int i=0; i< mChildren.size(); i++)
    {
      auto c = mChildren[i];
      Vector2f relk = c->relsize();
      Vector2i pref = c->preferredSize(ctx),
               rel = Vector2i(relk.x() * width(), relk.y() * height()),
               fix = c->fixedSize();

      pref = rel.fillZero(pref);
      c->setSize(fix.fillZero(pref));
      c->performLayout(ctx);
    }
  }
}

void Widget::sortChildren(std::function<bool(Widget*, Widget*)> f)
{
  if (!mChildren.empty())
    std::sort(mChildren.begin(), mChildren.end(), f);
}

Widget *Widget::findWidget(const Vector2i &p) 
{
  for (int i=(int)mChildren.size()-1; i >= 0; i--) {
      Widget *child = mChildren[i];
      if (child->visible() && child->contains(p - mPos))
      {
        if (child->prefferContains(p - mPos))
          return child;
        return child->findWidget(p - mPos);
      }
  }
  return contains(p) ? this : nullptr;
}

bool Widget::mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) {
  for (int i=(int)mChildren.size()-1; i >= 0; i--) {
      Widget *child = mChildren[i];
      if (child->visible() && child->contains(p - mPos) &&
          child->mouseButtonEvent(p - mPos, button, down, modifiers))
          return true;
  }
  if ( isMouseButtonLeft(button) && down && !mFocused)
      requestFocus();
  return false;
}

bool Widget::mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) {
    for (auto it = mChildren.rbegin(); it != mChildren.rend(); ++it) {
        Widget *child = *it;
        if (!child->visible())
            continue;
        bool contained = child->contains(p - mPos);
        bool prevContained = child->contains(p - mPos - rel);
        bool found = contained;
        if (contained != prevContained)
        {
          child->mouseEnterEvent(p, contained);
          found = true;
        }
        if ((contained || prevContained) &&
          child->mouseMotionEvent(p - mPos, rel, button, modifiers))
        {
          found |= true;
        }
        if (found)
          return true;
    }
    return false;
}

bool Widget::scrollEvent(const Vector2i &p, const Vector2f &rel) 
{
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
    if (enter)
    {
      mFocusChain.clear();
      const Widget *w = this;
      while (w) { mFocusChain.push_back((intptr_t)w); w = w->parent(); }
    }
    return false;
}

bool Widget::focusEvent(bool focused) {
    mFocused = focused;
    return false;
}

bool Widget::keyboardEvent(int key, int scancode, int action, int mods) {
  if (parent())
    return parent()->keyboardEvent(key, scancode, action, mods);

  return false;
}

bool Widget::keyboardCharacterEvent(unsigned int) {
    return false;
}

Vector4i Widget::getWidgetsArea()
{
  return { 0, 0,
           mFixedSize.x() ? mFixedSize.x() : width(),
           mFixedSize.y() ? mFixedSize.y() : height() };
}

Widget* Widget::findWidget(std::function<bool(Widget*)> cond, bool inchildren)
{
  for (auto child : mChildren)
  {
    if (cond(child))
      return child;

    if (inchildren)
    {
      if (auto w = child->findWidget(cond, inchildren))
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

void Widget::addChild(int index, Widget * widget) 
{
  if (!widget)
    return;

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
  if (child == (*it))  // already there
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

bool Widget::bringChildToFront(Widget* widget)
{
  auto it = std::find(mChildren.begin(), mChildren.end(), widget);
  if (it != mChildren.end())
  {
    mChildren.erase(it);
    mChildren.push_back(widget);
    return true;
  }

  return false;
}

void Widget::setParent(Widget* parent)
{
  mParent = parent; 
  parentChanged();
}

void Widget::addChild(Widget * widget) {
    addChild(childCount(), widget);
}

void Widget::removeChild(const Widget *widget) {
  if (!widget)
    return;

  mChildren.erase(std::remove(mChildren.begin(), mChildren.end(), widget), mChildren.end());
  widget->decRef();
}

void Widget::removeChild(const std::string& id)
{
  if (auto w = findWidget(id, false))
    removeChild(w);
}

void Widget::remove()
{
  if (parent())
    parent()->removeChild(this);
}

void Widget::removeLater()
{
  auto scr = screen();
  if (scr)
    scr->removeLater(this);
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

bool Widget::tabstop(CanTabStop mode) const 
{ 
  return mode == TabStopSelf ? false : true;
}

Window *Widget::window() 
{
  Widget *w = this;
  while (w) {
    Window *parentw = Window::cast(w);
    Panel *parentpn = Panel::cast(w);
    if (parentw && !parentpn)
      return parentw;
    w = w->parent();
  }
  return nullptr;
}
Screen *Widget::screen() { return findParent<Screen>(); }

void Widget::requestFocus() {
    Widget *widget = this;
    while (widget->parent())
        widget = widget->parent();
    ((Screen *) widget)->updateFocus(this);
}

void Widget::draw(NVGcontext *ctx) 
{
  if (!mChildren.empty())
  {
    nvgSave(ctx);
    nvgTranslate(ctx, mPos.x(), mPos.y());
    for (size_t i = 0; i < mChildren.size(); i++) {
      auto child = mChildren[i];
        if (child->visible()) {
            nvgSave(ctx);
            nvgIntersectScissor(ctx, child->mPos.x(), child->mPos.y(), child->mSize.x(), child->mSize.y());
            child->draw(ctx);
            nvgRestore(ctx);
        }
    }
    nvgRestore(ctx);
  }

  if (theme()->frameDrawBorder)
  {
    nvgSave(ctx);

    nvgResetScissor(ctx);
    nvgStrokeWidth(ctx, 1.0f);
    nvgBeginPath(ctx);
    nvgRect(ctx, mPos - Vector2f(1), mSize + Vector2f(2));
    nvgStrokeColor(ctx, theme()->frameBorderColor);
    nvgStroke(ctx);

    nvgRestore(ctx);
  }

  if (theme()->keyboardNavigation && tabstop(TabStopSelf))
  {
    drawTabstop(ctx);
  }

  if (theme()->debugHighlightMouseover)
  {
    if (std::find(mFocusChain.begin(), mFocusChain.end(), (intptr_t)this) != mFocusChain.end())
    {
      nvgSave(ctx);

      nvgResetScissor(ctx);

      nvgStrokeWidth(ctx, 1.0f);
      nvgBeginPath(ctx);
      nvgRect(ctx, mPos - Vector2f(1), mSize + Vector2f(2));
      nvgStrokeColor(ctx, nvgRGBA(255, 0, 0, 255));
      nvgStroke(ctx);

      nvgRestore(ctx);
    }
  }
}

void Widget::drawTabstop(NVGcontext *ctx)
{
  Widget* screen = mFocusChain.empty() ? nullptr : (Widget*)mFocusChain.back();
  if (screen && (this == screen->getCurrentSelection()))
  {
    nvgSave(ctx);

    nvgResetScissor(ctx);
    nvgBeginPath(ctx);
    nvgStrokeWidth(ctx, 2.0f);
    nvgRect(ctx, position() - Vector2f(3), size() + Vector2f(6));
    nvgStrokeColor(ctx, nvgRGBA(0, 0, 255, 255));
    nvgStroke(ctx);

    nvgRestore(ctx);
  }
}

void Widget::setVisible(bool visible)
{
  if (mVisible != visible)
  {
    auto scr = screen();
    if (scr) scr->needPerformLayout(mParent);
  }
  mVisible = visible;
}

void Widget::setTooltip(Widget* w)
{
  if (mTooltipWdiget)
    mTooltipWdiget->removeLater();

  mTooltipWdiget = w;
  mTooltipWdiget->setTheme(theme());
}

void Widget::afterDraw(NVGcontext *ctx) {
  if (mChildren.empty())
    return;

  for (auto& child : mChildren)
    child->afterDraw(ctx);
}

void Widget::save(Json::value &save) const 
{
  Json::object obj;

  obj["position"] = json().set(mPos).name("Position");
  obj["size"] = json().set(mSize).name("Size");
  obj["fixedSize"] = json().set(mFixedSize).name("Fixed size");
  obj["visible"] = json().set(mVisible).name("Visible");
  obj["enabled"] = json().set(mEnabled).name("Enabled");
  obj["focused"] = json().set(mFocused).name("Focused");
  obj["tooltip"] = json().set(mTooltip).name("Tooltip");
  obj["fontSize"] = json().set(mFontSize).name("Font size");
  obj["cursor"] = json().set((int)mCursor).name("Cursor");

  save = Json::value(obj);
}

bool Widget::load(Json::value &save) 
{
  json s{ save.get_obj() };

  mPos = s.get<Vector2i>("position"); 
  mSize = s.get<Vector2i>("size");
  mFixedSize = s.get<Vector2i>("fixedSize");
  mVisible = s.get<bool>("visible");
  mEnabled = s.get<bool>("enabled");
  mFocused = s.get<bool>("focused"); 
  mTooltip = s.get<std::string>("tooltip");  
  mFontSize = s.get<int>("fontSize"); 
  mCursor = (Cursor)s.get<int>("cursor");
  return true;
}

NAMESPACE_END(nanogui)
