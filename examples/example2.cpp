/*
    src/example2.cpp -- C++ version of an example application that shows
    how to use the form helper class. For a Python implementation, see
    '../python/example2.py'.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/nanogui.h>
#include <iostream>

using namespace nanogui;

enum test_enum {
    Item1 = 0,
    Item2,
    Item3
};

bool bvar = true;
int ivar = 12345678;
double dvar = 3.1415926;
float fvar = (float)dvar;
std::string strval = "A string";
test_enum enumval = Item2;
Color colval(0.5f, 0.5f, 0.7f, 1.f);

int main(int /* argc */, char ** /* argv */) {
    nanogui::init();

    auto window = nanogui::sample::create_window(1600, 900, "NanoGUI test", true, false);
    nanogui::sample::create_context();

    /* scoped variables */ {
        Screen *screen = new Screen({ 1600, 900 }, "NanoGUI test", false);
        nanogui::sample::setup_window_params(window, screen);

        bool enabled = true;
        FormHelper *gui = new FormHelper(screen);
        ref<Window> window = gui->addWindow(Vector2i(10, 10), "Form helper example");
        gui->addGroup("Basic types");
        gui->addVariable("bool", bvar);
        gui->addVariable("string", strval);

        gui->addGroup("Validating fields");
        gui->addVariable("int", ivar)->setSpinnable(true);
        gui->addVariable("float", fvar);
        gui->addVariable("double", dvar)->setSpinnable(true);

        gui->addGroup("Complex types");
        gui->addVariable("Enumeration", enumval, enabled)
           ->setItems({"Item 1", "Item 2", "Item 3"});
        gui->addVariable("Color", colval)
           ->setFinalCallback([](const Color &c) {
                 std::cout << "ColorPicker Final Callback: ["
                           << c.r() << ", "
                           << c.g() << ", "
                           << c.b() << ", "
                           << c.w() << "]" << std::endl;
             });

        gui->addGroup("Other widgets");
        gui->addButton("A button", []() { std::cout << "Button pressed." << std::endl; });

        screen->setVisible(true);
        screen->performLayout();

        nanogui::sample::run([&] {
          nanogui::sample::clear_frame(screen->background());

          screen->drawAll();

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
