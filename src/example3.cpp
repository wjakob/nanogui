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
        window->setPosition({ 100, 100 });
        window->setLayout(new GroupLayout());
        //window->setFixedSize({ 500, 300 });

        // Poor man's tab header.
        new Label(window, "Tab Buttons");
        Button* button1 = new Button(window, "Tab 1");
        button1->setFlags(Button::RadioButton);
        Button* button2 = new Button(window, "Tab 2");
        button2->setFlags(Button::RadioButton);
        
        // Proper tab header.
        auto header = new TabHeaderWidget(window, "sans", 30);
        header->addTab("Tab 1 with a very long name");
        header->addTab("Tab 2 with a long name");

        // The tab contents holder.
        StackedWidget* stackedWidget = new StackedWidget(window);
        // Create the first tab window.
        auto layer1 = new Widget(stackedWidget);
        layer1->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 10));
        new Label(layer1, "Color wheel", "sans-bold");
        new ColorWheel(layer1);
        // Create the second tab window.        
        auto layer2 = new Widget(stackedWidget);
        layer2->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 10));
        new Label(layer2, "Function graph", "sans-bold");
        Graph *graph = new Graph(layer2, "Some function");
        graph->setHeader("E = 2.35e-3");
        graph->setFooter("Iteration 89");
        VectorXf &func = graph->values();
        func.resize(100);
        for (int i = 0; i < 100; ++i)
            func[i] = 0.5f * (0.5f * std::sin(i / 10.f) +
                              0.5f * std::cos(i / 23.f) + 1);
        // Connect the stacked widget with the tab header.
        header->setCallback([stackedWidget](int i) { stackedWidget->setActiveTab(i); });
        
        
        button1->setCallback([stackedWidget]() { stackedWidget->setActiveTab(0); std::cout << "Tab " << 1 << " active\n"; });
        button2->setCallback([stackedWidget]() { stackedWidget->setActiveTab(1); std::cout << "Tab " << 2 << " active\n"; });
        stackedWidget->setActiveTab(0);

        new Label(window, "A test for the tab size", "sans", 20);

        performLayout();
        
    }
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
