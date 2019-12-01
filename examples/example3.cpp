/*
    src/example3.cpp -- C++ version of an example application that shows
    how to use nanogui in an application with an already created and managed
    glfw context.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

// GLFW
//
#if defined(NANOGUI_GLAD)
    #if defined(NANOGUI_SHARED) && !defined(GLAD_GLAPI_EXPORT)
        #define GLAD_GLAPI_EXPORT
    #endif

    #include <glad/glad.h>
#else
    #if defined(__APPLE__)
        #define GLFW_INCLUDE_GLCOREARB
    #else
        #define GL_GLEXT_PROTOTYPES
    #endif
#endif

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

Screen *screen = nullptr;

int main(int /* argc */, char ** /* argv */) {

  nanogui::init();

  {
    // Create a nanogui screen and pass the glfw pointer to initialize
    screen = new Screen({ 1600, 900 }, "NanoGUI Test");
    // Create nanogui gui
    bool enabled = true;
    FormHelper *gui = new FormHelper(screen);
    ref<Window> nanoguiWindow = gui->addWindow({ 10, 10 }, "Form helper example");
    gui->addGroup("Basic types");
    gui->addVariable("bool", bvar)->setTooltip("Test tooltip.");
    gui->addVariable("string", strval);

    gui->addGroup("Validating fields");
    gui->addVariable("int", ivar)->setSpinnable(true);
    gui->addVariable("float", fvar)->setTooltip("Test.");
    gui->addVariable("double", dvar)->setSpinnable(true);

    gui->addGroup("Complex types");
    gui->addVariable("Enumeration", enumval, enabled)->setItems({ "Item 1", "Item 2", "Item 3" });
    gui->addVariable("Color", colval)
      ->setFinalCallback([](const Color &c) {
      std::cout << "ColorPicker Final Callback: ["
        << c.r() << ", "
        << c.g() << ", "
        << c.b() << ", "
        << c.w() << "]" << std::endl;
    });

    gui->addGroup("Other widgets");
    gui->addButton("A button", []() { std::cout << "Button pressed." << std::endl; })->setTooltip("Testing a much longer tooltip, that will wrap around to new lines multiple times.");;

    screen->setVisible(true);
    screen->performLayout();

    screen->drawAll();
    nanogui::mainloop();
  }

  nanogui::shutdown();


  return 0;
}
