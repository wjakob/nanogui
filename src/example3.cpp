/*
    src/example3.cpp

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/nanogui.h>
#include <iostream>

using namespace nanogui;


class TabApplication : public nanogui::Screen {
public:
    TabApplication() : nanogui::Screen(Eigen::Vector2i(1024, 768), "Tab Simulation Test") {
        using namespace nanogui;
        Window* window = new Window(this, "Tabs Buttons");
        window->center();
        window->setLayout(new GroupLayout());

        new Label(window, "Tab Buttons");
        Button* button1 = new Button(window, "Tab 1");
        button1->setFlags(Button::RadioButton);
        Button* button2 = new Button(window, "Tab 2");
        button2->setFlags(Button::RadioButton);
        
        StackedWidget* widgetLayers = new StackedWidget(window);

        new Label(widgetLayers, "Widget Layer 1");
        new Label(widgetLayers, "Another Widget Layer");
        
        button1->setCallback([widgetLayers]() { widgetLayers->setActiveTab(0); std::cout << "Tab " << 1 << " active\n"; });
        button2->setCallback([widgetLayers]() { widgetLayers->setActiveTab(1); std::cout << "Tab " << 2 << " active\n"; });

        performLayout();
        
    }

private:
    unsigned int mTabNumbers;
    unsigned int mCurrentTab;
};



int main(int /* argc */, char ** /* argv */) {
    nanogui::init();

    {
        using namespace nanogui;

        ref<TabApplication> app = new TabApplication();
        app->setVisible(true);

        nanogui::mainloop();
    }

    nanogui::shutdown();
    return 0;
}
