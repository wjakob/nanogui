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
#include <nanovg.h>
#include <nanogui/entypo.h>
#include <nanogui/screen.h>
#include <nanogui/windowmenu.h>
#include <nanogui/layout.h>
#include <nanogui/serializer/core.h>
#include <algorithm>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(Window, Widget)
RTTI_IMPLEMENT_INFO(Panel, Window)

Window::Window(Widget *parent, const std::string &title)
    : Widget(parent), mTitle(title), mButtonPanel(nullptr),
      mModal(false), mDrag(dragNone)
{
  setDrawFlag(DrawBorder, false);
}

Window::Window(Widget *parent, const std::string &title, Orientation orientation)
  : Window(parent, title)
{
  setSimpleLayout(orientation);
}

void Window::setSimpleLayout(Orientation orientation)
{
  mLayout = new BoxLayout(orientation);
}

ContextMenu& Window::submenu(const std::string& caption, const std::string& id)
{
  auto menus = findAll<ContextMenu>();
  auto menu = std::find_if(menus.begin(), menus.end(), [=](ContextMenu* m) { return (!id.empty() && m->id() == id) || (!caption.empty() && m->caption() == caption); });

  auto wmenus = findAll<WindowMenu>();
  WindowMenu* wmenu = wmenus.empty() ? nullptr : wmenus.front();

  if (wmenu == nullptr)
  {
    wmenu = &wdg<WindowMenu>();
    wmenu->activate({ 0, 0 });
  }

  ContextMenu* smenu = nullptr;
  if (menu == menus.end())
  {
    smenu = wmenu->addSubMenu(caption);
    if (!id.empty())
      smenu->setId(id);
  }
  else
    smenu = *menu;

  return *smenu;
}

const Vector2i& Window::size() const { return mCollapsed ? mCollapsedSize : mSize; }

Vector2i Window::preferredSize(NVGcontext *ctx) const 
{
  if (mButtonPanel)
    mButtonPanel->setVisible(false);
  
  Vector2i result = Widget::preferredSize(ctx);
  if (mButtonPanel)
    mButtonPanel->setVisible(true);

  nvgFontSize(ctx, 18.0f);
  nvgFontFace(ctx, "sans-bold");
  float bounds[4];
  nvgTextBounds(ctx, 0, 0, mTitle.c_str(), nullptr, bounds);

  if (mCollapsed)
    result.y() = getHeaderHeight();

  return result.cwiseMax(bounds[2] - bounds[0] + 20, bounds[3] - bounds[1]);
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

bool isTriangleContainsPoint(const Vector2i& a, const Vector2i& b, const Vector2i& c, const Vector2i& p)
{
  bool b1 = ((p.x() - b.x()) * (a.y() - b.y()) - (p.y() - b.y()) * (a.x() - b.x())) < 0.0f;
  bool b2 = ((p.x() - c.x()) * (b.y() - c.y()) - (p.y() - c.y()) * (b.x() - c.x())) < 0.0f;
  bool b3 = ((p.x() - a.x()) * (c.y() - a.y()) - (p.y() - a.y()) * (c.x() - a.x())) < 0.0f;
  return ((b1 == b2) && (b2 == b3));
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

void Window::afterDraw(NVGcontext *ctx)
{
  if (mNeedPerformUpdate)
  {
    mNeedPerformUpdate = false;
    performLayout(ctx);
  }

  Widget::afterDraw(ctx);
}

void Window::draw(NVGcontext *ctx) 
{
  int cr = theme()->mWindowCornerRadius;
  int hh = getHeaderHeight();

  /* Draw window */
  int realH = isCollapsed() ? hh : mSize.y();

  nvgSave(ctx);

  if (haveDrawFlag(DrawBody))
  {
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos, { width(), realH }, cr);

    nvgFillColor(ctx, mMouseFocus ? mTheme->mWindowFillFocused
                                  : mTheme->mWindowFillUnfocused);
    nvgFill(ctx);
  }

  if (haveDrawFlag(DrawBorder) || *theme()->windowDrawBorder)
  {
    nvgSave(ctx);
    nvgResetScissor(ctx);
    nvgBeginPath(ctx);
    nvgRoundedRect(ctx, mPos, { width(), realH }, cr);

    nvgStrokeWidth(ctx, *theme()->windowBorderSize);
    nvgStrokeColor(ctx, mMouseFocus ? mTheme->windowBorderColorFocused  
                                    : mTheme->windowBorderColorUnfocused);
    nvgStroke(ctx);
    nvgRestore(ctx);
  }

  /* Draw a drop shadow */
  int ds = mTheme->mWindowDropShadowSize;
  if (ds > 0 && haveDrawFlag(DrawShadow))
  {
    NVGpaint shadowPaint = nvgBoxGradient(
      ctx, mPos.x(), mPos.y(), mSize.x(), realH, cr * 2, ds * 2,
      mTheme->mDropShadow, mTheme->mTransparent);

    nvgSave(ctx);
    nvgResetScissor(ctx);
    nvgBeginPath(ctx);
    nvgRect(ctx, mPos.x() - ds, mPos.y() - ds, mSize.x() + 2 * ds, realH + 2 * ds);
    nvgRoundedRect(ctx, mPos.x(), mPos.y(), mSize.x(), realH, cr);
    nvgPathWinding(ctx, NVG_HOLE);
    nvgFillPaint(ctx, shadowPaint);
    nvgFill(ctx);
    nvgRestore(ctx);
  }

  if (!mTitle.empty()) 
  {
    if (haveDrawFlag(DrawHeader))
    {
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
    }

    if (haveDrawFlag(DrawTitle))
    {
      nvgFontSize(ctx, mFontSize ? mFontSize : theme()->mWindowFontSize);
      nvgFontFace(ctx, "sans-bold");
      nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);

      nvgFontBlur(ctx, 2);
      nvgFillColor(ctx, mTheme->mDropShadow);
      nvgText(ctx, mPos.x() + mSize.x() / 2, mPos.y() + hh / 2, mTitle.c_str(), nullptr);

      nvgFontBlur(ctx, 0);
      nvgFillColor(ctx, mFocused ? mTheme->mWindowTitleFocused
                                  : mTheme->mWindowTitleUnfocused);
      nvgText(ctx, mPos.x() + mSize.x() / 2, mPos.y() + hh / 2 - 1,
              mTitle.c_str(), nullptr);
    }
  }

  if (mayCollapse())
  {
    auto icon = utf8(mCollapsed ? mTheme->mWindowCollapsedIcon : mTheme->mWindowExpandedIcon);

    mCollapseIconSize.y() = fontSize() * mCollapseIconScale;
    nvgFontFaceSize(ctx, "icons", mCollapseIconSize.y());
    mCollapseIconSize.x() = nvgTextBounds(ctx, 0, 0, icon.data(), nullptr, nullptr);

    nvgFillColor(ctx, mFocused ? mTheme->mWindowTitleFocused : mTheme->mWindowTitleUnfocused);
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    mCollapseIconPos = mPos + Vector2f(5, (hh - mCollapseIconSize.y())/2);
    nvgText(ctx, mCollapseIconPos.x(), mCollapseIconPos.y(), icon.data(), nullptr);
  }

  nvgRestore(ctx);

  if (!isCollapsed())
  {
    Widget::draw(ctx);

    if (mFixedSize == Vector2i::Zero())
    {
      bool inCorner = mMouseFocus && isTriangleContainsPoint(mSize, mSize - Vector2i(15, ds), 
                                                             mSize - Vector2i(ds, 15), mMousePos - mPos);
      nvgBeginPath(ctx);
      nvgMoveTo(ctx, mPos.x() + mSize.x() - 15, mPos.y() + mSize.y() - 2);
      nvgLineTo(ctx, mPos.x() + mSize.x() - 2, mPos.y() + mSize.y() - 15);
      nvgLineTo(ctx, mPos.x() + mSize.x() - 2, mPos.y() + mSize.y() - 5);
      nvgLineTo(ctx, mPos.x() + mSize.x() - 5, mPos.y() + mSize.y() - 2);
      nvgClosePath(ctx);
      nvgFillColor(ctx, inCorner ? mTheme->mBorderDark : mTheme->mBorderLight);
      nvgFill(ctx);
    }
  }
}

bool Window::prefferContains(const Vector2i& p) const
{
  int ds = theme()->mWindowDropShadowSize;
  return mMouseFocus
          && isTriangleContainsPoint(mSize, mSize - Vector2i(15, ds), mSize - Vector2i(ds, 15), p - mPos);
}

void Window::dispose() { screen()->disposeWindow(this); }
void Window::center() { screen()->centerWindow(this); }
bool Window::canEdgeResize() const { return theme()->windowResizeFromEdge; }

bool Window::mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers)
{
  mMousePos = p;
  Vector2i innerPos = p - position();
  int dragw = mDragLine > 0 ? mDragLine : theme()->mWindowDragLine;
  mCursor = Cursor::Arrow;
  if (canEdgeResize())
  {
    if (innerPos.x() < dragw || innerPos.x() > width() - dragw)
      mCursor = Cursor::HResize;
    else if (innerPos.y() < dragw || innerPos.y() > height() - dragw)
      mCursor = Cursor::VResize;
  } 

  return Widget::mouseMotionEvent(p, rel, button, modifiers);
}

bool Window::mouseDragEvent(const Vector2i &, const Vector2i &rel,
                            int buttons, int /* modifiers */) 
{
  if (!isDraggable())
    return false;
  bool edgeResize = canEdgeResize();

  if ((mDrag == dragHeader || mDrag == dragBody) && isMouseButtonLeftMod(buttons)) 
  {
    mPos += rel;
    if (theme()->windowMoveInParent)
    {
      mPos = mPos.cwiseMax(Vector2i::Zero());
      mPos = mPos.cwiseMin(parent()->size() - size());
    }
    return true;
  } 
  else if (edgeResize && mDrag == dragTop)
  {
    mPos.y() += rel.y();
    mSize.y() -= rel.y();
    mNeedPerformUpdate = true;
    return true;
  }
  else if (edgeResize && mDrag == dragLeft)
  {
    mPos.x() += rel.x();
    mSize.x() -= rel.x();
    mNeedPerformUpdate = true;
    return true;
  }
  else if (edgeResize && mDrag == dragRight)
  {
    mSize.x() += rel.x();
    mNeedPerformUpdate = true;
    return true;
  }
  else if (edgeResize && mDrag == dragBottom)
  {
    mSize.y() += rel.y();
    mNeedPerformUpdate = true;
    return true;
  }
  else if (mDrag == dragRbCorner && isMouseButtonLeftMod(buttons)) 
  {
    mSize += rel;
    mMousePos += rel;
    mSize = mSize.cwiseMax(Vector2i(15, getHeaderHeight()));
    //mSize = mSize.cwiseMin(parent()->size() - mSize);

    mNeedPerformUpdate = true;
    return true;
  }
  return false;
}

void Window::requestPerformLayout()
{
  screen()->needPerformLayout(mParent);
}

bool Window::isClickInsideCollapseArea(const Vector2i& clkPnt)
{
  return clkPnt.positive() && clkPnt.lessOrEq(mCollapseIconSize.cast<int>());
}

bool Window::mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) 
{
    if (Widget::mouseButtonEvent(p, button, down, modifiers))
        return true;

    if (isMouseButtonLeft(button) && mEnabled) 
    {
      Vector2i clkPnt = p - mPos - Vector2i(5,5);
      if (down && isClickInsideCollapseArea(clkPnt))
      {
        mCollapsed = !mCollapsed;
        requestPerformLayout();
        if (mCollapsed)
        {
          mSaveFixedHeight = mFixedSize.y();
          mFixedSize.y() = getHeaderHeight();
        }
        else
        {
          mFixedSize.y() = mSaveFixedHeight;
          mSaveFixedHeight = 0;
        }
        mCollapsedSize = { width(), getHeaderHeight() };
        return true;
      }
    }
    
    if (isMouseButtonLeft(button) && mEnabled) 
    {
      bool moveByHeader = mMoveByHeaderOnly < 0 ? theme()->windowMoveFromTitlebarOnly : mMoveByHeaderOnly;
      int hh = moveByHeader ? getHeaderHeight() : height();
      mDrag = (down && (p - mPos).y() < hh) ? dragHeader : dragNone;
      bool edgeResize = canEdgeResize();

      if (mDrag == dragNone)
      {
        int ds = mTheme->mWindowDropShadowSize;
        int dragw = mDragLine > 0 ? mDragLine : theme()->mWindowDragLine;

        Vector2i innerPos = p - mPos;

        if (edgeResize && innerPos.x() < dragw)                mDrag = dragLeft;
        else if (edgeResize && innerPos.x() > width() - dragw) mDrag = dragRight;
        else if (edgeResize && innerPos.y() > height() - dragw)mDrag = dragBottom;
        else
        {
          bool inCorner = isTriangleContainsPoint(mSize, mSize - Vector2i(ds, 16), mSize - Vector2i(16, ds), innerPos);
          if (down && inCorner)
            mDrag = dragRbCorner;
        }
      }
      else if (edgeResize)
      {
        int dragw = mDragLine > 0 ? mDragLine : theme()->mWindowDragLine;
        Vector2i innerPos = p - mPos;

        if (innerPos.y() < dragw) mDrag = dragTop;
      }

      return true;
    }
    return false;
}

bool Window::scrollEvent(const Vector2i &p, const Vector2f &rel) {
    Widget::scrollEvent(p, rel);
    return true;
}

void Window::setHeaderHeight(int h) { mHeaderHeight = h; }

void Window::refreshRelativePlacement() {
    /* Overridden in \ref Popup */
}

int Window::getHeaderHeight() const 
{ 
  return mHeaderHeight > 0 ? mHeaderHeight : theme()->mWindowHeaderHeight; 
}

void Window::save(Serializer &s) const 
{
  Widget::save(s);
  s.set("title", mTitle);
  s.set("modal", mModal);
}

bool Window::load(Serializer &s) 
{
  if (!Widget::load(s)) return false;
  if (!s.get("title", mTitle)) return false;
  if (!s.get("modal", mModal)) return false;
  mDrag = dragNone;
  return true;
}

Vector4i Window::getWidgetsArea()
{
  Vector4i area = Widget::getWidgetsArea();
  Widget* w = findWidget([](Widget* w) -> bool { return WindowMenu::cast(w) != nullptr; }, false);
  if (auto wm = WindowMenu::cast(w)) area.y() = wm->rect().w();
  else area.y() = getHeaderHeight();

  area.x() += *theme()->windowPaddingLeft;
  area.y() += *theme()->windowPaddingTop;
  area.z() -= *theme()->windowPaddingLeft;
  return area;
}

Panel::Panel(Widget *parent, const std::string &title)
  : Window(parent, title)
{
  mDrawFlags = DrawTitle | DrawHeader | DrawHeaderUnselect;
  mDraggable = Theme::WindowDraggable::dgFixed;
  mCollapseIconScale = 1.0f;
  withLayout<BoxLayout>(Orientation::Vertical, Alignment::Fill, 2, 2);
}

void Panel::requestPerformLayout()
{
  Widget *wp = mParent;
  while (wp) {
    if (Window::cast(wp) && !Panel::cast(wp))
      break;
    wp = wp->parent();
  }
  screen()->needPerformLayout(wp ? wp : mParent);
}

int Panel::getHeaderHeight() const 
{ 
  return mHeaderHeight ? mHeaderHeight : mTheme->mPanelHeaderHeight; 
}

bool Panel::inFocusChain() const
{
  return std::find(mFocusChain.begin(), mFocusChain.end(), (intptr_t)this) 
                != mFocusChain.end();
}

bool Panel::isClickInsideCollapseArea(const Vector2i& clkPnt)
{
  return clkPnt.positive() && clkPnt.lessOrEq(width(), getHeaderHeight());
}

void Panel::draw(NVGcontext *ctx)
{
  int cr = mTheme->mPanelCornerRadius;
  int hh = getHeaderHeight();

  /* Draw window */
  int realH = isCollapsed() ? hh : mSize.y();

  nvgSave(ctx);

  if (!mTitle.empty())
  {
    if (haveDrawFlag(DrawHeader))
    {
      bool underMouse = inFocusChain();
      const Color& cltop = underMouse ? theme()->mPanelHeaderGradientTopFocus : theme()->mPanelHeaderGradientTopNormal;
      const Color& clbot = underMouse ? theme()->mPanelHeaderGradientBotFocus : theme()->mPanelHeaderGradientBotNormal;

      bool visheader = true;
      if (!haveDrawFlag(DrawHeaderUnselect))
        visheader = underMouse;

      if (visheader)
      {
        NVGpaint headerPaint = nvgLinearGradient(ctx, mPos.x(), mPos.y(), mPos.x(), mPos.y() + hh, cltop, clbot);

        nvgBeginPath(ctx);
        nvgRoundedRect(ctx, mPos, { mSize.x(), hh }, cr);

        nvgFillPaint(ctx, headerPaint);
        nvgFill(ctx);
      }
    }

    if (haveDrawFlag(DrawTitle))
    {
      nvgFontSize(ctx, mFontSize > 0 ? mFontSize : theme()->mPanelFontSize);
      nvgFontFace(ctx, "sans-bold");
      nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);

      nvgFontBlur(ctx, 2);
      nvgFillColor(ctx, mTheme->mPanelDropShadow);
      nvgText(ctx, mPos + Vector2i(24, hh / 2), mTitle);

      nvgFontBlur(ctx, 0);
      nvgFillColor(ctx, mFocused ? mTheme->mPanelTitleFocused : mTheme->mPanelTitleUnfocused);
      nvgText(ctx, mPos + Vector2i(24, hh / 2 - 1), mTitle);
    }
  }

  if (mayCollapse())
  {
    auto icon = utf8(mCollapsed ? mTheme->mPanelCollapsedIcon : mTheme->mPanelExpandedIcon);

    mCollapseIconSize.y() = mFontSize > 0 ? mFontSize : theme()->mPanelFontSize;
    mCollapseIconSize.y() *= mCollapseIconScale;
    nvgFontSize(ctx, mCollapseIconSize.y());
    nvgFontFace(ctx, "icons");
    mCollapseIconSize.x() = nvgTextBounds(ctx, 0, 0, icon.data(), nullptr, nullptr);

    nvgFillColor(ctx, mFocused ? mTheme->mPanelTitleFocused : mTheme->mPanelTitleUnfocused);
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    mCollapseIconPos = Vector2f(mPos.x() + 5, mPos.y() + hh / 2);
    nvgText(ctx, mCollapseIconPos.x(), mCollapseIconPos.y(), icon.data(), nullptr);
  }

  nvgRestore(ctx);

  if (!isCollapsed())
    Widget::draw(ctx);
}

Vector4i Panel::getWidgetsArea()
{
  Vector4i area = Window::getWidgetsArea();
  area.y() = getHeaderHeight();
  area.w() = height();

  return area;
}

void Panel::performLayout(NVGcontext *ctx) 
{
  Window::performLayout(ctx);
}

Vector2i Panel::preferredSize(NVGcontext *ctx) const 
{
  return Window::preferredSize(ctx);
}

NAMESPACE_END(nanogui)
