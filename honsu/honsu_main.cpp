/*
    honsu/honsu_main.cpp -- simle youtrack timetracker

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch> and
    Dalerank <dalerankn8@gmail.com>
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
#include <nanogui/meter.h>
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
#include <nanogui/ledmatrix.h>
#include <nanogui/tabwidget.h>
#include <nanogui/switchbox.h>
#include <nanogui/spinner.h>
#include <nanogui/dropdownbox.h>
#include <nanogui/editworkspace.h>
#include <nanogui/editproperties.h>
#include <nanogui/scrollbar.h>
#include <nanogui/windowmenu.h>
#include <nanogui/perfchart.h>
#include <nanogui/common.h>
#include <nanogui/listbox.h>
#include <nanogui/themebuilder.h>
#include <nanogui/tolerancebar.h>
#include <nanogui/treeview.h>
#include <nanogui/treeviewitem.h>
#include <nanogui/picflow.h>
#include <nanogui/textarea.h>
#include <nanogui/searchbox.h>
#include <nanogui/editproperties.h>
#include <iostream>
#include <sstream>
#include <string>
#include <numeric>
#include <tuple>

// Includes for the GLTexture class.
#include <cstdint>
#include <memory>
#include <utility>
#include <string>

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

GPUtimer gpuTimer;

void showWaitingWindow(Screen* screen) 
{
  auto& w = screen->window(Position{ 0, 0 },
                           FixedSize{ screen->size() },
                           WindowMovable{ Theme::WindowDraggable::dgFixed },
                           WindowHaveHeader{ false },
                           WidgetId{ "#waiting_window" },
                           WindowBoxLayout{Orientation::Horizontal, Alignment::Middle, 0, 6});

  w.spinner( SpinnerRadius{ 0.5f },
             BackgroundColor{ 1 } );
  screen->performLayout();
  //w.waitForAction(10, "#autoremove", [](Widget* w) {w->remove(); });
}

void createStartupScreen(Screen* screen)
{
  auto& w = screen->window(Position{ 0, 0 },
                           FixedSize{ screen->size() },
                           WindowMovable{ Theme::WindowDraggable::dgFixed },
                           WindowHaveHeader{ false },
                           WidgetId{ "#login_window" },
                           WindowGroupLayout{});

  //w.withTheme<WhiteTheme>(screen->nvgContext());

  /* No need to store a pointer, the data structure will be automatically
  freed when the parent window is deleted */
  w.textbox(FontSize{ 24 },
            IsEditable{ true },
            TextAlignment::Left,
            TextPlaceholder{ "account name" },
            WidgetId{ "#account_name" },
            TextBoxUpdateCallback{ [](TextBox* tb) {}});
  w.textbox(FontSize{ 24 },
            IsEditable{ true },
            TextAlignment::Left,
            TextPlaceholder{ "youtrack url" },
            WidgetId{ "#youtrack_url" },
            TextBoxUpdateCallback{ [](TextBox* tb) {} });
  w.textbox(FontSize{ 24 },
            IsEditable{ true },
            TextAlignment::Left,
            TextPlaceholder{ "youtrack token" },
            WidgetId{ "#youtrack_token" },
            TextBoxUpdateCallback{ [](TextBox* tb) {} });

  /* Alternative construction notation using variadic template */
  w.button(Caption{ "Login" },
           Icon{ ENTYPO_ICON_ROCKET },
           BackgroundColor{ 0, 0, 255, 25 },
           ButtonCallback{ [=] { showWaitingWindow(screen); } });
}

class HonsuScreen : public Screen {
public:
    HonsuScreen(const Vector2i& size, const std::string& caption) : Screen(size, caption, false) {
      initGPUTimer(&gpuTimer);

      createStartupScreen(this);

      fpsGraph = &wdg<PerfGraph>(GRAPH_RENDER_FPS, "Frame Time", Vector2i(5, height() - 40));
      cpuGraph = &wdg<PerfGraph>(GRAPH_RENDER_MS, "CPU Time", Vector2i(5, height() - 40 * 2));
      gpuGraph = &wdg<PerfGraph>(GRAPH_RENDER_MS, "GPU Time", Vector2i(5, height() - 40 * 3));

      previousFrameTime = getTimeFromStart();

      performLayout();
    }

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
      using namespace nanogui;
      float value = std::fmod((float)getTimeFromStart() / 10, 1.0f);
        if (auto progress = findWidget<ProgressBar>("#lineprogressbar"))
          progress->setValue(value);
        if (auto progress = findWidget<CircleProgressBar>("#circleprogressbar"))
          progress->setValue( std::fmod(value * 2, 1.0f));

        if (auto editor = findWidget<PropertiesEditor>("#prop_editor"))
        {
          if (lastSelected != mSelectedWidget 
              && !editor->isMyChildRecursive(mSelectedWidget))
          {
            lastSelected = mSelectedWidget;
            editor->parse(mSelectedWidget);
          }
        }

        startGPUTimer(&gpuTimer);

        double t = getTimeFromStart();
        double dt = t - previousFrameTime;
        previousFrameTime = t;

        /* Draw the user interface */
        Screen::draw(ctx);

        cpuTime = getTimeFromStart() - t;

        if (auto meter = findWidget<Meter>("#meter"))
        {
          static int i = 0;
          static int v[30] = { 0 };
          v[(++i)%30] = (int)(1 / dt);
          meter->setValue(std::accumulate(std::begin(v), std::end(v), 0) / 30);
        }

        if (fpsGraph) fpsGraph->update(dt);
        if (cpuGraph) cpuGraph->update(cpuTime);
        if (gpuGraph && gpuTimer.supported)
        {
          float gpuTimes[3];
          int n = stopGPUTimer(&gpuTimer, gpuTimes, 3);
          for (int i = 0; i < n; i++)
            gpuGraph->update(gpuTimes[i]);
        }

        if (auto led = findWidget<LedMatrix>("#led"))
        {
          static std::list<int> ledvalues;
          int value = 1.f / dt;

          ledvalues.push_back(value);
          if (ledvalues.size() > led->columnCount())
            ledvalues.pop_front();
          int k = 0;
          for (auto& c : ledvalues)
          {
            led->clearColumn(k);
            int t = led->rowCount() * c / 50;
            for (int i = 0; i <= t; i++)
            {
              int rk = 0xff * (i / (float)led->rowCount());
              led->setColorAt(led->rowCount() - i, k, Color(0xff, 0, 0, rk));
            }
            k++;
          }
        }

    }

    virtual void drawContents() {
        using namespace nanogui;
    }

    bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override {
        if (Widget::mouseButtonEvent(p, button, down, modifiers))
            return true;
        if (down && isMouseButtonRight(button) && findWidget(p)==this) {
            auto menu = new ContextMenu(this, "", true);
            menu->addItem("Item 1", [this]() { new MessageDialog(this, MessageDialog::Type::Information, "Item 1", "Item 1 Clicked!"); }, ENTYPO_ICON_PLUS);

            auto submenu = menu->addSubMenu("Submenu");
            submenu->addItem("Subitem 1", [this]() { new MessageDialog(this, MessageDialog::Type::Information, "Subitem 1", "Subitem 1 Clicked!"); });
            auto subsubmenu = submenu->addSubMenu("Subsubmenu", ENTYPO_ICON_LOOP);
            submenu->addItem("Subitem 2", [this]() { new MessageDialog(this, MessageDialog::Type::Information, "Subitem 2", "Subitem 2 Clicked!"); });

            subsubmenu->addItem("Subsubitem 1", [this]() { new MessageDialog(this, MessageDialog::Type::Information, "Subsubitem 1", "Subsubitem 1 Clicked!"); });
            subsubmenu->addItem("Subsubitem 2", [this]() { new MessageDialog(this, MessageDialog::Type::Information, "Subsubitem 2", "Subsubitem 2 Clicked!"); });

            menu->activate(p-mPos);
            performLayout();
        }
        return true;
    }

private:
    nanogui::PerfGraph *fpsGraph = nullptr;
    nanogui::PerfGraph *cpuGraph = nullptr;
    nanogui::PerfGraph *gpuGraph = nullptr;
    double previousFrameTime = 0, cpuTime = 0;
    Widget* lastSelected = nullptr;
};

int main(int /* argc */, char ** /* argv */) 
{
  nanogui::init();
  auto window = nanogui::sample::create_window(300, 900, "Honsu", true, false);
  nanogui::sample::create_context();

  {
    HonsuScreen screen({ 300, 900 }, "");
    nanogui::sample::setup_window_params(window, &screen);
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
