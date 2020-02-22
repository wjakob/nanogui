/*
    src/contextmenu.cpp --  Standard context menu with support for submenus

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/contextmenu.h>
#include <nanogui/screen.h>
#include <nanovg.h>
#include <nanogui/layout.h>
#include <nanogui/saveload.h>
#include <nanogui/entypo.h>
#include "nanogui/label.h"
#include "nanogui/screen.h"
#include "nanogui/window.h"
#include "nanogui/theme.h"
#include <algorithm>

NAMESPACE_BEGIN(nanogui)

class NANOGUI_EXPORT ContextMenuDummy : public ContextMenu
{
public:
  RTTI_CLASS_UID(ContextMenuDummy)
  RTTI_DECLARE_INFO(ContextMenuDummy)

  ContextMenuDummy() : ContextMenu(nullptr, "", false) {}
};

class NANOGUI_EXPORT ContextItemAsMenu : public ContextMenu
{
public:
  RTTI_CLASS_UID("CIMN")
  RTTI_DECLARE_INFO(ContextItemAsMenu)

  ContextMenuLabel* l = nullptr;
  ContextItemAsMenu() : ContextMenu(nullptr, "", false) {}

  void setEnabled(bool en) override { if (l) l->setEnabled(en); }
  void setShortcut(const std::string& text) override { if (l) l->setShortcut(text); }
};

class NANOGUI_EXPORT ContextMenuArea : public Widget
{
public:
  RTTI_CLASS_UID(ContextMenuArea)
  RTTI_DECLARE_INFO(ContextMenuArea)

  ContextMenuArea(Widget* w) : Widget(w) {}
  Vector2i minSize() const override { return parent()->minSize(); }
};

void ContextMenuLabel::draw(NVGcontext* ctx)
{
  Label::draw(ctx);

  if (!mShortcut.empty())
  {
    nvgFontFace(ctx, mFont.c_str());
    nvgFontSize(ctx, fontSize());
    nvgFillColor(ctx, mTheme->mContextMenuShortcutTextColor);

    Vector2i opos = { mSize.x() - mTheme->mContextMenuShortcutOffset, 0 };

    switch (mTextVAlign)
    {
    case TextVAlign::vMiddle: opos.y() = (mSize.y() - mTextRealSize.y()) / 2; break;
    case TextVAlign::vBottom: opos.y() = (mSize.y() - mTextRealSize.y()); break;
    }


    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
    nvgText(ctx, mPos + opos, mShortcut);
  }

  if (mChecked) {
    nvgFontSize(ctx, mSize.y() * icon_scale());
    nvgFontFace(ctx, "icons");
    nvgFillColor(ctx, mEnabled ? mTheme->mIconColor : mTheme->mDisabledTextColor);
    nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
    nvgText(ctx, mPos.x() + mSize.x() - (mSize.y() * 0.5f + 1),
                 mPos.y() + mSize.y() * 0.5f, utf8(mTheme->mCheckBoxIcon).data(),
                 nullptr);
  }
}

Vector2i ContextMenuLabel::preferredSize(NVGcontext* ctx) const
{
  Vector2i pf = Label::preferredSize(ctx);
  nvgFontFace(ctx, mFont.c_str());
  nvgFontSize(ctx, fontSize());
  nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
  int tw = nvgTextBounds(ctx, 0, 0, mShortcut.empty() ? "Ctrl+A" : mShortcut.c_str(), nullptr, nullptr) + 2;
  pf.x() += tw + 12;

  return pf;
}


ContextMenu::ContextMenu(Widget *parent, const std::string& caption, bool disposable)
    : Widget(parent),
        mHighlightedItem(nullptr),
        mActiveSubmenu(nullptr),
        mRootMenu(nullptr),
        mDisposable(disposable),
        mActivated(false),
        mBackgroundColor(0.3f, 1.0f),
        mMarginColor(0.28f, 1.0f),
        mHighlightColor(0.15f, 1.0f),
        mCaption(caption)
{
    mItemContainer = &wdg<ContextMenuArea>();
    mItemContainer->setPosition(0,0);
    mItemLayout = new AdvancedGridLayout({10,0,0}, {}, 2);
    mItemLayout->setColStretch(0, 1.0f);
    mItemContainer->setLayout(mItemLayout);
    mSubElement = true;
    setVisible(false);
}

void ContextMenu::requestPerformLayout() { mUpdateLayout = true; }

void ContextMenu::_checkConditions()
{
  for (auto it = mLabels.begin(); it != mLabels.end(); ++it)
  {
    auto cit = mConditions.find(it->first);
    if (cit != mConditions.end())
    {
      ContextMenuLabel* item = it->second;
      bool enabled = item->enabled();
      bool checked = item->checked();
      cit->second(enabled, checked);
      item->setEnabled(enabled);
      if (item->checkable())
        item->setChecked(checked);
    }
  }
}

void ContextMenu::activate(const Vector2i& pos) {
    if (!mActivated) {
        mActivated = true;
        bringToFront();
        _checkConditions();
        setVisible(true);
        requestPerformLayout();

        if (mRootMenu) {
            mRootMenu->requestFocus();
        }
        else {
            requestFocus();
        }
    }
    mAnchorPos = pos;
}

void ContextMenu::deactivate() {
    if (mActivated) {
        mActivated = false;
        setVisible(false);
        deactivateSubmenu();
        if (mDisposable) {
            dispose();
        }
    }
}

ContextMenu& ContextMenu::item(const std::string& name)
{
  auto menuit = mSubmenus.find(name);
  if (menuit == mSubmenus.end())
  {
    auto lbit = mLabels.find(name);
    if (lbit == mLabels.end())
    {
      static ContextMenuDummy dummy;
      return dummy;
    }

    static ContextItemAsMenu labelAsMenu;
    labelAsMenu.l = lbit->second;
    return labelAsMenu;
  }

  return *mSubmenus[name];
}

ContextMenu& ContextMenu::item(const std::string& name, const std::function<void()>& cb, int icon)
{
  auto it = mItems.find(name);
  if (it == mItems.end())
    addItem(name, cb, icon);
  else
    mItems[name] = cb;

  return *this;
}

ContextMenu& ContextMenu::item(const std::string& name, const std::function<void(bool)>& cb, 
                               const std::function<void(bool&, bool&)>& condition, int icon)
{
  item(name, cb, icon);
  if (condition != nullptr)
    mConditions[name] = condition;

  return *this;
}

ContextMenu& ContextMenu::item(const std::string& name, const std::function<void(bool)>& cb, int icon)
{
  auto it = mChItems.find(name);
  if (it == mChItems.end())
  {
    addItem(name, nullptr, icon);
    if (cb != nullptr)
    {
      mChItems[name] = cb;
      mLabels[name]->setCheckable(true);
    }
  }
  else
    mChItems[name] = cb;

  return *this;
}

ContextMenu& ContextMenu::item(const std::string& name, const std::string& shortcut, const std::function<void()>& cb, int icon)
{
  auto it = mLabels.find(name);
  if (it == mLabels.end())
    addItem(name, shortcut, cb, icon);
  else
    mLabels[name]->setShortcut(shortcut);

  return *this;
}

void ContextMenu::addItem(const std::string& name, const std::string& shortcut, const std::function<void()>& value, int icon)
{
  auto& lbl = mItemContainer->wdg<ContextMenuLabel>(name);
  mLabels[name] = &lbl;
  if (value != nullptr)
    mItems[name] = value;

  lbl.setFontSize(fontSize());
  lbl.setShortcut(shortcut);
  lbl.setHeight(lbl.fontSize() * 2);
  mItemLayout->appendRow(0);
  mItemLayout->setAnchor(&lbl, AdvancedGridLayout::Anchor{ 1,mItemLayout->rowCount() - 1, 1, 1 });
  if (nvgIsFontIcon(icon)) {
    auto& iconLbl = mItemContainer->wdg<Label>(Caption{ utf8(icon).data() }, CaptionFont{ "icons" });
    iconLbl.setFontSize(fontSize() + 2);
    mItemLayout->setAnchor(&iconLbl, AdvancedGridLayout::Anchor{ 0,mItemLayout->rowCount() - 1,1,1 });
  }
}

void ContextMenu::addItem(const std::string& name, const std::function<void()>& value, int icon) {
  addItem(name, "", value, icon);
}

ContextMenu& ContextMenu::submenu(const std::string& name, int icon) { return *addSubMenu(name, icon); }

ContextMenu* ContextMenu::addSubMenu(const std::string& name, int icon) {
    if (!mParent)
        return nullptr;

    auto it = mSubmenus.find(name);
    if (it != mSubmenus.end())
      return it->second;

    auto& submenu = parent()->wdg<ContextMenu>(name, false);
    submenu.mRootMenu = mRootMenu ? mRootMenu : this;
    auto& lbl1 = mItemContainer->wdg<ContextMenuLabel>(name);
    auto& lbl2 = mItemContainer->wdg<Label>(Caption{ utf8(mTheme->mContextSubmenu).data() }, CaptionFont{ "icons" });

    lbl1.setFontSize(fontSize());
    lbl1.setHeight(lbl1.fontSize() * 2);
    lbl2.setFontSize(fontSize());
    mItemLayout->appendRow(0);
    mItemLayout->setAnchor(&lbl1, AdvancedGridLayout::Anchor{1,mItemLayout->rowCount() - 1, 1, 1});
    mItemLayout->setAnchor(&lbl2, AdvancedGridLayout::Anchor{2,mItemLayout->rowCount() - 1, 1, 1});
    if (nvgIsFontIcon(icon)) {
      auto& iconLbl = mItemContainer->wdg<Label>(Caption{ utf8(icon).data() }, CaptionFont{ "icons" });
        iconLbl.setFontSize(fontSize()+2);
        mItemLayout->setAnchor(&iconLbl, AdvancedGridLayout::Anchor{ 0,mItemLayout->rowCount() - 1,1,1 });
    }

    mLabels[name] = &lbl1;
    mSubmenus[name] = &submenu;
    return &submenu;
}

void ContextMenu::setShortcut(const std::string& /*text*/)
{

}

Vector2i ContextMenu::preferredSize(NVGcontext* ctx) const {
  Vector2i pf = mItemContainer->preferredSize(ctx);
  pf.x() = std::max({ minWidth(), pf.x(), mTheme->mContextMenuMinWidth});
  return mItemContainer->position() + pf;
}

bool ContextMenu::mouseEnterEvent(const Vector2i& p, bool enter) {
    Widget::mouseEnterEvent(p, enter);
    if (!enter) {
        if (mActiveSubmenu) {
            if (mActiveSubmenu->mMouseFocus)
                mMouseFocus = true;
            else
                deactivateSubmenu();
        } else if (mHighlightedItem) {
            mHighlightedItem = nullptr;
        }
    }
    return true;
}

bool ContextMenu::mouseMotionEvent(const Vector2i& p, const Vector2i& rel, int button, int modifiers) {
    Widget::mouseMotionEvent(p, rel, button, modifiers);
    Vector2i mousePos = p - mPos;
    for (const auto& w : mLabels) {
        // Deactivate old highlighted submenu, activate new submenu
        if (isRowSelected_(w.first, mousePos)) {
            // Deactivate current submenu unless we are still hovering it.
            if (mActiveSubmenu && !(_isSubMenu(w.first) && mSubmenus[w.first] == mActiveSubmenu)) {
                deactivateSubmenu();
            }
            // Activate the item we are hovering
            if (_isSubMenu(w.first) && mSubmenus[w.first] != mActiveSubmenu) {
                activateSubmenu(w.first);
            }
            mHighlightedItem = mLabels[w.first];
            break;
        }
    }
    return true;
}

bool ContextMenu::mouseButtonEvent(const Vector2i& p, int button, bool down, int /*modifiers*/) {
    Vector2i mousePos = p - mPos;
    if (isMouseButtonLeft(button) && !down) {
        // Preserve our existence in case the click destroys us.
        ref<ContextMenu> buoy(this);
        for (const auto& w : mLabels) {
            if (isRowSelected_(w.first, mousePos) && !_isSubMenu(w.first)) {
                std::function<void()> callback;
                if (mLabels[w.first]->checkable())
                  callback = [lbCallback = mChItems[w.first], checked = mLabels[w.first]->checked()]() {
                    if (lbCallback) lbCallback(!checked);
                  };
                else
                  callback = mItems[w.first];

                if (mRootMenu)
                    mRootMenu->deactivate();
                else
                    deactivate();

                if (callback)
                {
                  callback();
                  mLabels[w.first]->toggle();
                }
                return true;
            }
        }
    }
    return true;
}

void ContextMenu::draw(NVGcontext* ctx) 
{
  if (mUpdateLayout)
  {
    mUpdateLayout = false;
    Vector2i ps = preferredSize(ctx);
    setSize(ps);

    performLayout(ctx);
  }

  nvgSave(ctx);

  mPos = mAnchorPos;
  nvgTranslate(ctx, mPos.x(), mPos.y());

  int w = mItemContainer->position().x() + mItemContainer->width();
  int h = mItemContainer->position().y() + mItemContainer->height();

  /* Draw background */
  nvgBeginPath(ctx);
  nvgRect(ctx, 0, 0, w, h);
  nvgFillColor(ctx, mBackgroundColor);
  nvgFill(ctx);

  /* Draw margin background */
  if (!mLabels.empty()) {
      auto lbl = mLabels.begin()->second;
      nvgBeginPath(ctx);
      nvgRect(ctx, 0, 0, lbl->position().x()-1, h);
      nvgFillColor(ctx, mMarginColor);
      nvgFill(ctx);
  }

  /* Draw outline */
  nvgBeginPath(ctx);
  nvgStrokeWidth(ctx, 1.0f);
  nvgRect(ctx, 0.5f, 1.5f, w - 1, h - 2);
  nvgStrokeColor(ctx, mTheme->mBorderLight);
  nvgStroke(ctx);

  nvgBeginPath(ctx);
  nvgRect(ctx, 0.5f, 0.5f, w - 1, h - 0.5f);
  nvgStrokeColor(ctx, mTheme->mBorderDark);
  nvgStroke(ctx);

  if (mHighlightedItem && mHighlightedItem->enabled()) {
      nvgBeginPath(ctx);
      nvgRect(ctx, 1, mHighlightedItem->position().y() + 1, w - 2, mHighlightedItem->height() - 2);
      nvgFillColor(ctx, mHighlightColor);
      nvgFill(ctx);
  }
  nvgRestore(ctx);

  Widget::draw(ctx);
}

bool ContextMenu::focusEvent(bool focused) {
    Widget::focusEvent(focused);
    // Deactivate when focus is lost
    if (!focused) {
        deactivate();
    }
    return true;
}

Vector2i ContextMenu::minSize() const {
  Vector2i ret = mMinSize;
  ret.x() = std::max(mMinSize.x(), mTheme->mContextMenuMinWidth);
  return ret;
}

bool ContextMenu::_isSubMenu(const std::string& name) {
    return mSubmenus.find(name) != mSubmenus.end();
}

bool ContextMenu::isRowSelected_(const std::string& name, const Vector2i& p) const {
    if (mLabels.find(name) == mLabels.end())
        return false;
    auto w = mLabels.at(name);
    int lblLeft = mItemContainer->position().x();
    int lblRight = mItemContainer->position().x() + mItemContainer->width();
    int lblTop = mItemContainer->position().y() + w->position().y();
    int lblBot = mItemContainer->position().y() + w->position().y() + w->height();
    return p.y() >= lblTop && p.y() < lblBot && p.x() >= lblLeft && p.x() < lblRight;
}

void ContextMenu::activateSubmenu(const std::string &name) {
    if (mActiveSubmenu) {
        deactivateSubmenu();
    }
    mActiveSubmenu = mSubmenus[name];
    mActiveSubmenu->activate(submenuPosition(name));
}

void ContextMenu::deactivateSubmenu() {
    if (mActiveSubmenu) {
        mActiveSubmenu->deactivate();
        mActiveSubmenu = nullptr;
    }
}

void ContextMenu::dispose() {
    for (const auto& w : mSubmenus)
        w.second->dispose();
    if (mParent) {
        // Defer focus to parent widget before we destroy ourselves.
        if (mFocused)
            mParent->requestFocus();
        mParent->removeChild(this);
    }
}

Vector2i ContextMenu::submenuPosition(const std::string &name) const {
    return mItemContainer->position() + Vector2i{ mItemContainer->width() - 1, mLabels.at(name)->position().y() + 1 } + mPos;
}

RTTI_IMPLEMENT_INFO(ContextMenuDummy, ContextMenu)
RTTI_IMPLEMENT_INFO(ContextItemAsMenu, ContextMenu)
RTTI_IMPLEMENT_INFO(ContextMenuArea, Widget)
RTTI_IMPLEMENT_INFO(ContextMenu, Widget)
RTTI_IMPLEMENT_INFO(ContextMenuLabel, Label)

NAMESPACE_END(nanogui)
