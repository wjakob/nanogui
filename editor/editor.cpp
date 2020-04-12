/*
    src/editor.cpp

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanovg.h>
#include <nanogui/screen.h>
#include <nanogui/window.h>
#include <nanogui/layout.h>
#include <nanogui/label.h>
#include <nanogui/checkbox.h>
#include <nanogui/dial.h>
#include <nanogui/button.h>
#include <nanogui/toolbutton.h>
#include <nanogui/popupbutton.h>
#include <nanogui/combobox.h>
#include <nanogui/progressbar.h>
#include <nanogui/entypo.h>
#include <nanogui/messagedialog.h>
#include <nanogui/textbox.h>
#include <nanogui/slider.h>
#include <nanogui/imagepanel.h>
#include <nanogui/imageview.h>
#include <nanogui/vscrollpanel.h>
#include <nanogui/colorwheel.h>
#include <nanogui/colorpicker.h>
#include <nanogui/table.h>
#include <nanogui/graph.h>
#include <nanogui/tabwidget.h>
#include <nanogui/switchbox.h>
#include <nanogui/dropdownbox.h>
#include <nanogui/editworkspace.h>
#include <nanogui/editproperties.h>
#include <nanogui/foldout.h>
#include <nanogui/scrollbar.h>
#include <nanogui/treeview.h>
#include <nanogui/treeviewitem.h>
#include <nanogui/windowmenu.h>
#include <nanogui/common.h>
#include <nanogui/widgetsfactory.h>
#include <iostream>
#include <sstream>
#include <string>

// Includes for the GLTexture class.
#include <cstdint>
#include <memory>
#include <utility>

#include "widgetctor.h"

#if defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#endif
#if defined(_WIN32)
#  pragma warning(push)
#  pragma warning(disable: 4457 4456 4005 4312)
#endif

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <nanogui/contextmenu.h>

#if defined(_WIN32)
#  pragma warning(pop)
#endif
#if defined(_WIN32)
#  if defined(APIENTRY)
#    undef APIENTRY
#  endif
#  include <windows.h>
#endif

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::pair;
using std::to_string;
using namespace nanogui;

struct {
  std::string assets = "#assets";
  WidgetId workspace{ "#workspace" };
  WidgetId propeditor{ "#propeditor" };
  WidgetId mainmenu{ "#mainmenu" };
  WidgetId editor{ "#editor" };
  WidgetId mainview{ "#mainview" };
  WidgetId layers{ "#layers" };
  WidgetId factoryview{ "#factoryview" };
} ID;

class EditorScreen : public Screen {
public:
    EditorScreen(const Vector2i& s) : Screen(s, "Editor", false)
    {
      auto& mmenu = createMainMenu();

      auto& area = hlayer(2, 2, ID.editor);
      area.layout()->setId(0xfa);
      area.setPosition(0, theme()->mWindowMenuHeight);
      area.setFixedSize(size() - Vector2i(0, theme()->mWindowMenuHeight));
      createControlWidgetsArea(area, 0.15f);
      createWorkspace(area, 0.7f);
      createPropertiesEditor(area, 0.15f);
      createBaseEditorWidget(area);

      performLayout();
    }

    Widget& createMainMenu()
    {
      auto& mmenu = wdg<WindowMenu>();
      mmenu.setId(ID.mainmenu.value);
      mmenu.activate({ 0, 0 });
      auto dlg = [this](const std::string& title) { msgdialog(DialogType{ (int)MessageDialog::Type::Information }, 
                                                              DialogTitle{ title },
                                                              DialogMessage{ "New Clicked!" }); };
      mmenu.submenu("File")
        .item("New", [dlg]() { dlg("New"); })
        .item("Open", [dlg]() { dlg("Open"); })
        .item("Save", [dlg]() { dlg("Save"); });

      mmenu.submenu("Edit")
        .item("Undo",  [dlg]() { dlg("Undo"); })
        .item("Redo",  [dlg]() { dlg("Redo"); })
        .item("Cut",   [dlg]() { dlg("Cut"); })
        .item("Copy",  [dlg]() { dlg("Copy"); })
        .item("Paste", [dlg]() { dlg("Paste"); })
        .item("Delete",[dlg]() { dlg("Delete"); });

      mmenu.submenu("View")
        .item("Code",     [dlg]() { dlg("Code"); })
        .item("Solution", [dlg]() { dlg("Solution"); })
        .item("Widgets",  [dlg]() { dlg("Widgets"); })
        .item("Output",   [dlg]() { dlg("Output"); })
        .item("Toolbox",  [dlg]() { dlg("Toolbox"); })
        .item("Notifications", [dlg]() { dlg("Notifications"); })
        .item("Full screen", [dlg]() { dlg("Full screen"); })
        .item("Option",   [dlg]() { dlg("Option"); });

      mmenu.submenu("Build")
        .item("Build solution", [dlg]() { dlg("Build"); });

      mmenu.submenu("Samples")
        .item("Sample 1", [dlg]() { dlg("Sample 1"); })
        .item("Sample 2", [dlg]() { dlg("Smaple 2"); })
        .item("Sample 3", [dlg]() { dlg("Smaple 3"); })
        .item("Sample 4", [dlg]() { dlg("Sample 4"); });

      mmenu.submenu("Widgets")
        .item("w1", [dlg]() { dlg("Undo"); })
        .item("w2", [dlg]() { dlg("Undo"); })
        .item("w3", [dlg]() { dlg("Undo"); })
        .item("w4", [dlg]() { dlg("Undo"); });

      mmenu.submenu("Help")
        .item("View help", [dlg]() { dlg("Undo"); })
        .item("Send feedback", [dlg]() { dlg("Undo"); });

      return mmenu;
    }

    void createPropertiesEditor(Widget& area, float relw)
    {
      auto& propeditor = area.wdg<PropertiesEditor>(ID.propeditor);
      propeditor.setRelativeSize(relw, 1.f);
      propeditor.setDraggable(Theme::WindowDraggable::dgFixed);

      if (auto editor = findWidget<EditorWorkspace>(ID.workspace))
        editor->addSelectedCallback([&](Widget* w) { propeditor.parse(w); });
    }

    void createWorkspace(Widget& area, float relw)
    {
      auto& editor = area.wdg<EditorWorkspace>(ID.workspace);
      editor.setRelativeSize(0.7f, 1.f);
      editor.setChildrenChangeCallback([this]() { fillTreeView(); });
      editor.setHoveredCallback([this](Widget* w) {
        auto treeview = findWidget<TreeView>(ID.layers);
        auto node = treeview->findNode([w](TreeViewItem* i) -> bool { return i->data() == (intptr_t)w; });
        treeview->setHovered(node);
      });
      editor.addSelectedCallback([this](Widget* w) {
        auto treeview = findWidget<TreeView>(ID.layers);
        auto node = treeview->findNode([w](TreeViewItem* i) -> bool { return i->data() == (intptr_t)w; });
        treeview->setSelected(node);
      });
    }

    void createControlWidgetsArea(Widget& area, float relw)
    {
      auto& wa = area.vlayer(2, 2, RelativeSize{ relw, 1.f });
      auto& waheader = wa.hlayer(2, 2, FixedHeight{ 30 });

      auto& wawidgets = wa.vlayer(2, 2, RelativeSize{ 1, 0 });
      auto& factoryview = wawidgets.wdg<Foldout>(ID.factoryview);
      auto& wfactory = WidgetFactory::instance();
      for (auto f : wfactory.factories())
      {
        auto& layer = this->vlayer(0, 0);
        for (auto wdgtype : f->types())
        {
          auto& ctor = layer.wdg<WidgetCtor>(Caption{ wdgtype }, FixedHeight{ 22 },
                          ButtonCallback{[wdgtype, this] () {
                            auto editor = findWidget<EditorWorkspace>(ID.workspace);
                            if (editor) editor->prepareCreateWidget(wdgtype);
                          }}
                        );
          ctor.mWtype = wdgtype;
        }
        factoryview.addPage(f->name(), f->name(), &layer);
      }
      factoryview.hide();

      auto& view = wawidgets.wdg<TreeView>(RelativeSize{ 1, 0 }, ID.layers);
      view.setActionIcon(TreeView::IconCollapsed, ENTYPO_ICON_RIGHT_DIR);
      view.setActionIcon(TreeView::IconExpanded, ENTYPO_ICON_DOWN_DIR);

      view.setRelativeSize(1, 1);
      view.show();

      view.setSelectNodeCallback([this](TreeViewItem* w) {
        if (!w) return;
        auto editor = findWidget<EditorWorkspace>(ID.workspace);
        editor->setSelectedElement((Widget*)w->data());
      });

      view.setHoverNodeCallback([this](TreeViewItem* w) {
        if (!w) return;
        auto editor = findWidget<EditorWorkspace>(ID.workspace);
        editor->setHoveredElement((Widget*)w->data());
      });

      waheader.link(Caption{ "Assets" }, ButtonFlags{ Button::ToggleButton|Button::RadioButton }, 
                    ButtonCallback{ [&] { view.hide(); factoryview.show(); } });
      waheader.link(Caption{ "Layers" }, ButtonFlags{ Button::ToggleButton|Button::RadioButton },
                    ButtonCallback{ [&] { view.show(); factoryview.hide(); } });
      waheader.label(Caption{ "" }, RelativeSize{0.3f, 1.f});
      waheader.button(Caption{ "Page" });
    }

    void addTreeViewNode(EditorWorkspace* editor, TreeViewItem* item, Widget* w)
    {
      auto node = item->addNode("Unknown widget");
      node->setData((intptr_t)w);
      node->setNodeId((intptr_t)w);
      if (auto parea = node->previewArea())
      {
        bool editable = editor->isWidgetEditable((intptr_t)w);
        parea->add<ToggleButton>(Icon{ ENTYPO_ICON_LOCK },
          ButtonPushed{ !editable },
          ButtonChangeCallback{ [this,w](Button* b) {
            if (auto workspace = findWidget<EditorWorkspace>(ID.workspace))
              workspace->setWidgetEditable((intptr_t)w, !b->pushed());
          }
        });

        parea->add<ToggleButton>(Icon{ ENTYPO_ICON_EYE },
          ButtonPushed{ !w->visible() },
          ButtonChangeCallback{ [w](Button* b) { w->setVisible(!b->pushed()); }
        });
      }

      for (auto& c : w->children())
        addTreeViewNode(editor, node, c);
    }

    void fillTreeView()
    {
      auto treeview = findWidget<TreeView>(ID.layers);
      auto editor = findWidget<EditorWorkspace>(ID.workspace);
      if (!treeview)
        return;

      treeview->removeAllNodes();

      for (auto& c : editor->children())
        addTreeViewNode(editor, treeview->rootNode(), c);
    }

    void createBaseEditorWidget(Widget& area)
    {
      auto* editor = findWidget<EditorWorkspace>(ID.workspace);
      if (!editor)
        return;

      auto& eb = editor->button("Editor button");
      eb.setCallback([] { cout << "pushed!" << endl; });
      eb.setTooltip("short tooltip");
      

      auto& ew = editor->wdg<Window>(Caption{ "Editor window" });
      ew.setSize(100, 200);
      ew.setPosition(0, 50);
      ew.button(Caption{ "Button" });

      fillTreeView();
    }

    bool keyboardEvent(int key, int scancode, int action, int modifiers) override
    {
        if (Screen::keyboardEvent(key, scancode, action, modifiers))
            return true;
        if (isKeyboardKeyEscape(key) && isKeyboardActionPress(action)) {
            setVisible(false);
            return true;
        }
        return false;
    }

    virtual void draw(NVGcontext *ctx) {
        /* Draw the user interface */
        Screen::draw(ctx);
    }

    virtual void drawContents() {
        using namespace nanogui;
    }

    bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override {
        if (Widget::mouseButtonEvent(p, button, down, modifiers))
            return true;
        if (down && nanogui::isMouseButtonRight(button) && findWidget(p)==this) {
            auto menu = new nanogui::ContextMenu(this, "", true);
            menu->addItem("Item 1", [this]() { msgdialog(DialogTitle{ "Item 1" }, DialogMessage{ "Item 1 Clicked!" }); }, ENTYPO_ICON_PLUS);

            auto submenu = menu->addSubMenu("Submenu");
            submenu->addItem("Subitem 1", [this]() { msgdialog(DialogTitle{ "Subitem 1" }, DialogMessage{ "Subitem 1 Clicked!" }); });
            auto subsubmenu = submenu->addSubMenu("Subsubmenu", ENTYPO_ICON_LOOP);
            submenu->addItem("Subitem 2", [this]() { msgdialog(DialogTitle{ "Subitem 2" }, DialogMessage{ "Subitem 2 Clicked!" }); });

            subsubmenu->addItem("Subsubitem 1", [this]() { msgdialog(DialogTitle{ "Subsubitem 1" }, DialogMessage{ "Subsubitem 1 Clicked!" }); });
            subsubmenu->addItem("Subsubitem 2", [this]() { msgdialog(DialogTitle{ "Subsubitem 2" }, DialogMessage{ "Subsubitem 2 Clicked!" }); });

            menu->activate(p-mPos);
            performLayout();
        }
        return true;
    }
};

int main(int /* argc */, char ** /* argv */) {
    nanogui::init();

    auto window = nanogui::sample::create_window(1920, 1080, "Nigma: editor", true, false);
    nanogui::sample::create_context();

    /* scoped variables */ {
      EditorScreen screen({1920, 1080});
      nanogui::sample::setup_window_params(window, &screen);

      screen.drawAll();
      screen.setVisible(true);
      screen.performLayout();
      
      nanogui::sample::run([&] {
        nanogui::sample::clear_frame(screen.background());

        screen.drawAll();

        nanogui::sample::present_frame(window);

        /* Wait for mouse/keyboard or empty refresh events */
        nanogui::sample::wait_events();
      });

      nanogui::sample::poll_events();
    }

    nanogui::sample::destroy_window(window);
    nanogui::shutdown();

    return 0;
}
