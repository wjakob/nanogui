/*
    nanogui/contextmenu.h -- Standard context menu with support for submenus

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/widget.h>
#include <unordered_map>

NAMESPACE_BEGIN(nanogui)

/**
 * \class ContextMenu contextmenu.h nanogui/contextmenu.h
 *
 * \brief Nestable context menu.
 *
 * A context menu may be used in either 'disposable' or 'non-disposable' mode.
 * In disposable mode, it may be created and activated on-the-fly in an event
 * handler, and the menu will automatically delete itself after use.
 * In non-disposable mode, it may be created and stored before it is actually
 * needed, and then activated inside an event handler. In this mode the menu
 * instance is reusable as long as its parent is alive.
 *
 * \rst
 * .. code-block:: cpp
 *
 *    auto menu = new ContextMenu(this, true);
 *    menu->addItem("Item 1", [](){
 *      std::cout << "Item 1 clicked!" << std::endl;
 *    });
 *    auto submenu = menu->addSubMenu("Submenu 1");
 *    submenu->addItem("Subitem 1", [](){
 *      std::cout << "Subitem 1 clicked!"
 *    });
 *    menu->activate();
 *
 * \endrst
 */
class NANOGUI_EXPORT ContextMenu : public Widget {
public:
    /**
     * \brief Construct a new ContextMenu.
     * \param parent Parent widget.
     * \param disposable When true, the context menu and all submenus will be
     *                   destroyed upon deactivation.
     */
    ContextMenu(Widget* parent, bool disposable);

    /**
     * \brief Activate the context menu at the given position.
     *
     * Make the context menu visible. When an item is clicked, the context menu
     * will be deactivated.
     *
     * \param pos The desired position of the top left corner of the context
     *            menu, relative to the parent.
     */
    void activate(const Vector2i& pos);

    /**
     * \brief Deactivate the context menu.
     *
     * This method is called automatically after an item is clicked. If the
     * context menu is disposable, it will be removed from its parent and
     * destroyed.
     */
    void deactivate();

    /**
     * \brief Add an item to the menu. The callback is called when the item is clicked.
     * \param name Name of the item. The name is displayed in the context menu.
     * \param cb Callback to be executed when the item is clicked.
     * \param icon Optional icon to display to the left of the label.
     */
    void addItem(const std::string& name, const std::function<void()>& cb, int icon=0);

    /**
     * Add a submenu to the menu.
     *
     * \param name The display text of the item.
     * \returns nullptr if a submenu or item already exists under the given name.
     * \param icon Optional icon to display to the left of the label.
     */
    ContextMenu* addSubMenu(const std::string& name, int icon = 0);

    Vector2i preferredSize(NVGcontext* ctx) const override;
    bool mouseEnterEvent(const Vector2i& p, bool enter) override;
    bool mouseMotionEvent(const Vector2i& p, const Vector2i& rel, int button, int modifiers) override;
    bool mouseButtonEvent(const Vector2i& p, int button, bool down, int modifiers) override;
    void draw(NVGcontext* ctx) override;
    bool focusEvent(bool focused) override;

protected:
    /**
     * \brief Determine if an item opens a new context menu.
     * \param name Name of the item.
     */
    bool isSubMenu_(const std::string& name);

    /**
     * \brief Determine if a row contains the point `p`.
     * \param name Name of the item that is on the desired row.
     * \param p Point relative to self.
     */
    bool isRowSelected_(const std::string& name, const Vector2i& p) const;

private:
    /// Activate a submenu.
    void activateSubmenu(const std::string& name);
    /// Deactivate the currently active submenu if there is one.
    void deactivateSubmenu();
    /// Remove the context menu and all submenus from their parent widget.
    void dispose();
    /// Calculate a submenus position.
    Vector2i submenuPosition(const std::string& name) const;

private:
    Widget *mItemContainer;
    AdvancedGridLayout *mItemLayout;
    std::unordered_map<std::string, std::function<void()>> mItems;
    std::unordered_map<std::string, ContextMenu*> mSubmenus;
    std::unordered_map<std::string, Label*> mLabels;
    Label *mHighlightedItem;
    ContextMenu *mActiveSubmenu;
    ContextMenu *mRootMenu;
    bool mDisposable;
    bool mActivated;
    bool mUpdateLayout;

    Color mBackgroundColor, mMarginColor, mHighlightColor;
};

NAMESPACE_END(nanogui)
