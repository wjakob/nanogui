/*
    nanogui/window.h -- Top-level window widget

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/widget.h>
#include <nanogui/layout.h>

NAMESPACE_BEGIN(nanogui)

class ContextMenu;

/**
 * \class Window window.h nanogui/window.h
 *
 * \brief Top-level window widget.
 */

DECLSETTER(WindowSimpleLayout, Orientation)
DECLSETTER(WindowMovable, Theme::WindowDraggable)

class NANOGUI_EXPORT Window : public Widget {
    friend class Popup;
public:
    RTTI_CLASS_UID("WNDW")
    RTTI_DECLARE_INFO(Window)

    explicit Window(Widget *parent, const std::string &title = "Untitled");
    explicit Window(Widget *parent, const std::string &title, Orientation orientation);

    explicit Window(Widget *parent, const char* title)
      : Window(parent, std::string(title)) {}

    using Widget::set;
    template<typename... Args>
    Window(Widget* parent, const Args&... args)
      : Window(parent, "") { set<Window, Args...>(args...); }

    /// Return the window title
    const std::string &title() const { return mTitle; }
    /// Set the window title
    void setTitle(const std::string &title) { mTitle = title; }

    /// Is this a model dialog?
    bool modal() const { return mModal; }
    /// Set whether or not this is a modal dialog
    void setModal(bool modal) { mModal = modal; }

    bool isDraggable() const;
    void setDraggable(Theme::WindowDraggable flag) { mDraggable = flag; }

    bool mayCollapse() const;
    void setMayCollapse(Theme::WindowCollapse c) { mMayCollapse = c; }

    bool isCollapsed() const { return mCollapsed; }
    void collapse();

    /// Return the panel used to house window buttons
    Widget *buttonPanel();

    void setSimpleLayout(Orientation orientation);

    /// Dispose the window
    void dispose();

    /// Center the window in the current \ref Screen
    void center();

    /// Draw the window
    void draw(NVGcontext *ctx) override;
    void afterDraw(NVGcontext *ctx) override;
    /// Handle window drag events
    bool mouseDragEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) override;
    /// Handle mouse events recursively and bring the current window to the top
    bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override;
    /// Accept scroll events and propagate them to the widget under the mouse cursor
    bool scrollEvent(const Vector2i &p, const Vector2f &rel) override;
    /// Compute the preferred size of the widget
    Vector2i preferredSize(NVGcontext *ctx) const override;

    /// Invoke the associated layout generator to properly place child widgets, if any
    bool mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) override;
    virtual void performLayout(NVGcontext *ctx) override;
    virtual void save(Serializer &s) const override;
    virtual bool load(Serializer &s) override;

    ContextMenu& submenu(const std::string& caption, const std::string& id = "");

    bool prefferContains(const Vector2i& p) const override;

protected:
    /// Internal helper function to maintain nested window position values; overridden in \ref Popup
    virtual void refreshRelativePlacement();
protected:
    std::string mTitle;
    Widget *mButtonPanel;
    bool mModal;
    bool mDrag;
    bool mDragCorner;
    Vector2i mMousePos;
    int mFontSize = 0;
    bool mNeedPerformUpdate = false;
    Theme::WindowDraggable mDraggable = Theme::WindowDraggable::dgAuto;
    Theme::WindowCollapse mMayCollapse = Theme::WindowCollapse::clAuto;
    bool mCollapsed = false;
    float mCollapseIconScale = 1.5f;
    Vector2f mCollapseIconPos;
    Vector2f mCollapseIconSize;
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    PROPSETTER(WindowMovable,setDraggable)
    PROPSETTER(Caption, setTitle)
    PROPSETTER(WindowSimpleLayout,setSimpleLayout)
};

NAMESPACE_END(nanogui)
