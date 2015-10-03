#include <nanogui/nanogui.h>
#include <nanogui/formhelper.h>
#include <iostream>

using namespace nanogui;

enum test_enum {
    Item1 = 0,
    Item2,
    Item3
};

bool bvar = true;
int ivar = 12345678;
float fvar = (float) M_PI;
double dvar = M_PI;
std::string strval = "A string";
test_enum enumval = Item2;
Color colval(0.5f, 0.5f, 0.7f, 1.f);

int main(int /* argc */, char ** /* argv */) {
    nanogui::init();

    {
        Screen *screen = new Screen(Vector2i(500, 700), "NanoGUI test");

        bool enabled = true;
        FormHelper *gui = new FormHelper(screen);
        ref<Window> window = gui->addWindow(Eigen::Vector2i(10, 10), "Form helper example");
        gui->addGroup("Basic types");
        gui->addVariable("bool", bvar);
        gui->addVariable("string", strval);

        gui->addGroup("Validating fields");
        gui->addVariable("int", ivar);
        gui->addVariable("float", fvar);
        gui->addVariable("double", dvar);

        gui->addGroup("Complex types");
        gui->addVariable("Enumeration", enumval, enabled)
           ->setItems({"Item 1", "Item 2", "Item 3"});
        gui->addVariable("Color", colval);

        gui->addGroup("Other widgets");
        gui->addButton("A button", [](){ std::cout << "Button pressed." << std::endl; });

        screen->setVisible(true);
        screen->performLayout();
        window->center();

        nanogui::mainloop();
    }

    nanogui::shutdown();
    return 0;
}
