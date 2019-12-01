/*
    nanogui/windowmenu.h -- Standard window menu with support for submenus

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    WindowMenu was developed by dalerank <dalerankn8@gmail.com>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/window.h>
#include <nanogui/contextmenu.h>

NAMESPACE_BEGIN(nanogui)

class BoxLayout;

/**
 * \class WindowMenu windowmenu.h nanogui/windowmenu.h
 *
 * \brief Nestable window menu.
 *
 *
 * \rst
 * .. code-block:: cpp
 *
 *    auto wmenu = new WindowMenu(window, true);
 *    wmenu->addItem("Item 1", []() {
 *      std::cout << "Item 1 clicked!" << std::endl;
 *    });
 *    auto submenu = wmenu->addSubMenu("Submenu 1");
 *    submenu->addItem("Subitem 1", []() {
 *      std::cout << "Subitem 1 clicked!"
 *    });
 *
 * \endrst
 */
class NANOGUI_EXPORT WindowMenu : public ContextMenu {
public:
    RTTI_CLASS_UID("WNDM")
    RTTI_DECLARE_INFO(WindowMenu)

    /**
     * \brief Construct a new WindowtMenu.
     * \param parent Parent widget.
     */
    WindowMenu(Widget* parent);

    Vector2i preferredSize(NVGcontext* ctx) const override;
    void draw(NVGcontext* ctx) override;

    void addItem(const std::string& name, const std::string& shortcut, const std::function<void()>& value, int icon) override;
    ContextMenu* addSubMenu(const std::string& name, int icon = 0) override;
    void performLayout(NVGcontext *ctx) override;
    bool mouseMotionEvent(const Vector2i& p, const Vector2i& rel, int button, int modifiers) override;

    Vector2i submenuPosition(const std::string &name) const override;
    void requestPerformLayout() override;
    void deactivate() override;
private:
    bool _isLabelSelected(const std::string& name, const Vector2i& p) const;

    int mItemMargin = 2;
    int mItemSpacing = 10;
    bool mUseCustomHeight = false;
    BoxLayout *mItemHLayout = nullptr;
};

NAMESPACE_END(nanogui)
