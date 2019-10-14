/*
    src/example1.cpp -- C++ version of an example application that shows
    how to use the various widget classes. For a Python implementation, see
    '../python/example1.py'.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

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
#include <nanogui/windowmenu.h>
#include <nanogui/common.h>
#include <iostream>
#include <sstream>
#include <string>

// Includes for the GLTexture class.
#include <cstdint>
#include <memory>
#include <utility>

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

class ExampleApplication : public nanogui::Screen {
public:
    ExampleApplication() : nanogui::Screen(Eigen::Vector2i(1280, 800), "NanoGUI Test") {
        using namespace nanogui;

        auto& mmenu = wdg<WindowMenu>();
        mmenu.activate({ 0, 0 });
        mmenu.submenu("File")
                .item("New", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "New", "New Clicked!"); })
                .item("Open", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Open", "New Clicked!"); })
                .item("Save", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Save", "New Clicked!"); });

        mmenu.submenu("Edit")
                .item("Undo", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Undo", "New Clicked!"); })
                .item("Redo", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Redo", "New Clicked!"); })
                .item("Cut", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Cut", "New Clicked!"); })
                .item("Copy", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Copy", "New Clicked!"); })
                .item("Paste", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Paste", "New Clicked!"); })
                .item("Delete", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Delete", "New Clicked!"); });

        mmenu.submenu("View")
                .item("Code", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Undo", "New Clicked!"); })
                .item("Solution", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Undo", "New Clicked!"); })
                .item("Widgets", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Undo", "New Clicked!"); })
                .item("Output", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Undo", "New Clicked!"); })
                .item("Toolbox", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Undo", "New Clicked!"); })
                .item("Notifications", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Undo", "New Clicked!"); })
                .item("Full screen", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Undo", "New Clicked!"); })
                .item("Option", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Undo", "New Clicked!"); });

        mmenu.submenu("Build")
                .item("Build solution", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Undo", "New Clicked!"); });

        mmenu.submenu("Samples")
                .item("Sample 1", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Undo", "New Clicked!"); })
                .item("Sample 2", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Undo", "New Clicked!"); })
                .item("Sample 3", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Undo", "New Clicked!"); })
                .item("Sample 4", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Undo", "New Clicked!"); });

        mmenu.submenu("Widgets")
                .item("w1", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Undo", "New Clicked!"); })
                .item("w2", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Undo", "New Clicked!"); })
                .item("w3", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Undo", "New Clicked!"); })
                .item("w4", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Undo", "New Clicked!"); });

        mmenu.submenu("Help")
                .item("View help", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Undo", "New Clicked!"); })
                .item("Send feedback", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Undo", "New Clicked!"); });

        auto area = new Widget(this);
        area->setPosition(0, mmenu.preferredSize(nvgContext()).y());
        area->setSize(width(), height() - area->position().y());

        auto fo = new Foldout(area, Vector4i(0, 0, width()/4, area->height()), "foldout_ed");
        fo->addPage("page1",  "Page1", new Widget(this));
        fo->addPage("page2", "Page2", new Widget(this));
        fo->addPage("page3", "Page3", new Widget(this));
        fo->addPage("page4", "Page4", new Widget(this));

        auto editor = new EditorWorkspace(area, "workspace");
        editor->setSize(area->width()/2, area->height());
        editor->setPosition(area->width()/4, 0);

        auto propeditor = new PropertiesEditor(area, "propeditor");
        propeditor->setSize(area->width() / 4, area->height());
        propeditor->setPosition(area->width() * 0.75, 0);
        editor->setSelectedCallback([=](Widget* w) { propeditor->parse(w); });

        auto* eb = new Button(editor, "Editor button");
        eb->setCallback([] { cout << "pushed!" << endl; });
        eb->setTooltip("short tooltip");

        auto* ew = new Window(editor, "Editor window");
        ew->setSize(100, 200);
        ew->setPosition(0, 50);

        performLayout();
    }

    ~ExampleApplication() {}

    virtual bool keyboardEvent(int key, int scancode, int action, int modifiers) {
        if (Screen::keyboardEvent(key, scancode, action, modifiers))
            return true;
        if (nanogui::isKeyboardKeyEscape(key) && nanogui::isKeyboardActionPress(action)) {
            setVisible(false);
            return true;
        }
        return false;
    }

    virtual void draw(NVGcontext *ctx) {
        /* Animate the scrollbar */
        if (mProgress)
          mProgress->setValue(std::fmod((float) nanogui::getTimeFromStart() / 10, 1.0f));

        /* Draw the user interface */
        Screen::draw(ctx);
    }

    virtual void drawContents() {
        using namespace nanogui;
    }

    bool mouseButtonEvent(const Eigen::Vector2i &p, int button, bool down, int modifiers) override {
        if (Widget::mouseButtonEvent(p, button, down, modifiers))
            return true;
        if(down && nanogui::isMouseButtonRight(button) && findWidget(p)==this) {
            auto menu = new nanogui::ContextMenu(this, "", true);
            menu->addItem("Item 1", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Item 1", "Item 1 Clicked!"); }, ENTYPO_ICON_PLUS);

            auto submenu = menu->addSubMenu("Submenu");
            submenu->addItem("Subitem 1", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Subitem 1", "Subitem 1 Clicked!"); });
            auto subsubmenu = submenu->addSubMenu("Subsubmenu", ENTYPO_ICON_LOOP);
            submenu->addItem("Subitem 2", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Subitem 2", "Subitem 2 Clicked!"); });

            subsubmenu->addItem("Subsubitem 1", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Subsubitem 1", "Subsubitem 1 Clicked!"); });
            subsubmenu->addItem("Subsubitem 2", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Subsubitem 2", "Subsubitem 2 Clicked!"); });

            menu->activate(p-mPos);
            performLayout();
        }
        return true;
    }

private:
    nanogui::ProgressBar *mProgress = nullptr;
    int mCurrentImage;
};

int main(int /* argc */, char ** /* argv */) {
    try {
        nanogui::init();

        /* scoped variables */ {
            nanogui::ref<ExampleApplication> app = new ExampleApplication();
            app->drawAll();
            app->setVisible(true);
            nanogui::mainloop();
        }

        nanogui::shutdown();
    } catch (const std::runtime_error &e) {
        std::string error_msg = std::string("Caught a fatal error: ") + std::string(e.what());
        #if defined(_WIN32)
            MessageBoxA(nullptr, error_msg.c_str(), NULL, MB_ICONERROR | MB_OK);
        #else
            std::cerr << error_msg << endl;
        #endif
        return -1;
    }

    return 0;
}
