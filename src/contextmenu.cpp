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
#include <nanogui/opengl.h>
#include <nanogui/layout.h>
#include <nanogui/serializer/core.h>
#include <nanogui/entypo.h>
#include "nanogui/label.h"
#include "nanogui/screen.h"
#include "nanogui/window.h"
#include "nanogui/theme.h"

NAMESPACE_BEGIN(nanogui)
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
    mItemContainer = new Widget(this);
    mItemContainer->setPosition({0,0});
    mItemLayout = new AdvancedGridLayout({10,0,0}, {}, 2);
    mItemLayout->setColStretch(0, 1.0f);
    mItemContainer->setLayout(mItemLayout);
    setVisible(false);
}

void ContextMenu::requestPerformLayout()
{
  auto wnd = window();
  if (wnd)
    wnd->performLayout(screen()->nvgContext());
}

void ContextMenu::activate(const Vector2i& pos) {
    if (!mActivated) {
        mActivated = true;
        bringToFront();
        setVisible(true);
        requestPerformLayout();

        if (mRootMenu) {
            mRootMenu->requestFocus();
        }
        else {
            requestFocus();
        }
    }
    setPosition(pos);
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

ContextMenu& ContextMenu::item(const std::string& name, const std::function<void()>& cb, int icon)
{
  auto it = mItems.find(name);
  if (it == mItems.end())
    addItem(name, cb, icon);
  else
    mItems[name] = cb;

  return *this;
}

void ContextMenu::addItem(const std::string& name, const std::function<void()>& value, int icon) {
    mItems[name] = value;
    auto lbl = new Label(mItemContainer, name);
    mLabels[name] = lbl;
    lbl->setFontSize(fontSize());
    lbl->setMinSize(mMinSize);
    lbl->setHeight(lbl->fontSize() * 2);
    mItemLayout->appendRow(0);
    mItemLayout->setAnchor(lbl, AdvancedGridLayout::Anchor{1,mItemLayout->rowCount() - 1, 1, 1});
    if (nvgIsFontIcon(icon)) {
        auto iconLbl = new Label(mItemContainer, utf8(icon).data(), "icons");
        iconLbl->setFontSize(fontSize()+2);
        mItemLayout->setAnchor(iconLbl, AdvancedGridLayout::Anchor{ 0,mItemLayout->rowCount() - 1,1,1 });
    }
}

ContextMenu* ContextMenu::addSubMenu(const std::string& name, int icon) {
    if (!mParent)
        return nullptr;
    mSubmenus[name] = new ContextMenu(mParent, name, false);
    mSubmenus[name]->mRootMenu = mRootMenu ? mRootMenu : this;
    auto lbl1 = new Label(mItemContainer, name);
    auto lbl2 = new Label(mItemContainer, utf8(ENTYPO_ICON_CHEVRON_THIN_RIGHT).data(), "icons");
    mLabels[name] = lbl1;
    lbl1->setFontSize(fontSize());
    lbl1->setHeight(lbl1->fontSize() * 2);
    lbl2->setFontSize(fontSize());
    mItemLayout->appendRow(0);
    mItemLayout->setAnchor(lbl1, AdvancedGridLayout::Anchor{1,mItemLayout->rowCount() - 1, 1, 1});
    mItemLayout->setAnchor(lbl2, AdvancedGridLayout::Anchor{2,mItemLayout->rowCount() - 1, 1, 1});
    if (nvgIsFontIcon(icon)) {
        auto iconLbl = new Label(mItemContainer, utf8(icon).data(), "icons");
        iconLbl->setFontSize(fontSize()+2);
        mItemLayout->setAnchor(iconLbl, AdvancedGridLayout::Anchor{ 0,mItemLayout->rowCount() - 1,1,1 });
    }
    return mSubmenus[name];
}

Vector2i ContextMenu::preferredSize(NVGcontext* ctx) const {
    return mItemContainer->position() + mItemContainer->preferredSize(ctx);
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
            if (mActiveSubmenu && !(isSubMenu_(w.first) && mSubmenus[w.first] == mActiveSubmenu)) {
                deactivateSubmenu();
            }
            // Activate the item we are hovering
            if (isSubMenu_(w.first) && mSubmenus[w.first] != mActiveSubmenu) {
                activateSubmenu(w.first);
            }
            mHighlightedItem = mLabels[w.first];
            break;
        }
    }
    return true;
}

bool ContextMenu::mouseButtonEvent(const Vector2i& p, int button, bool down, int modifiers) {
    Vector2i mousePos = p - mPos;
    if (button==GLFW_MOUSE_BUTTON_LEFT && !down) {
        // Preserve our existence in case the click destroys us.
        ref<ContextMenu> buoy(this);
        for (const auto& w : mLabels) {
            if (isRowSelected_(w.first, mousePos) && !isSubMenu_(w.first)) {
                std::function<void()> cb = mItems[w.first];
                if (mRootMenu)
                    mRootMenu->deactivate();
                else
                    deactivate();
                if (cb) cb();
                return true;
            }
        }
    }
    return true;
}

void ContextMenu::draw(NVGcontext* ctx) {
    nvgSave(ctx);
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

    if (mHighlightedItem) {
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

bool ContextMenu::isSubMenu_(const std::string& name) {
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
    if(mActiveSubmenu) {
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

NAMESPACE_END(nanogui)
