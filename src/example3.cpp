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
#include <string>

using namespace nanogui;


class TabApplication : public nanogui::Screen {
public:
    TabApplication() : nanogui::Screen(Eigen::Vector2i(1024, 768), "Tab Simulation Test") {
        using namespace nanogui;
        Window* window = new Window(this, "Tabs Buttons");
        window->setPosition({ 100, 100 });
        window->setLayout(new GroupLayout());
        

        Tab* tab = new Tab(window);
        
        // Create the second tab window.
        for (int j = 0; j < 8; j++) {
            auto layer = new Widget(nullptr);
            layer->setLayout(new BoxLayout(Orientation::Horizontal, Alignment::Middle, 0, 10));
            new Label(layer, "Function graph", "sans-bold");
            Graph *graph = new Graph(layer, "Some function");
            graph->setHeader("Function Tab " + std::to_string(j));
            graph->setForegroundColor(Color(0.7f, 0.2f, 0.2f, 1.0f));
            VectorXf &func = graph->values();
            func.resize(100);
            for (int i = 0; i < 100; ++i)
                func[i] = 0.5f * ((0.5f) * std::sin(i / 10.f + j) +
                                  (0.5f) * std::cos(i / 23.f + j) + 1);
            auto layerName = "Tab " + std::to_string(j);
            tab->addTab(layer, layerName);

        }
        tab->setActiveTab(0);
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
