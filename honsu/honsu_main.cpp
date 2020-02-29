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
#include <nanogui/saveload.h>

#define CPPHTTPLIB_OPENSSL_SUPPORT 1
#include <httplib.h>
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

#define CA_CERT_FILE "./ca-bundle.crt"

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

struct AgileInfo 
{
  std::string name;
  std::string id;
};

struct IssueInfo
{
  std::string id;
  std::string entityId;
  std::string state;
  std::string sprint;
  std::string type;
  std::string summary;
  Json::value js;

  void readField()
  {
    int i = 0;
    Json::value field = js.get("field");
    Json::value info = field.get(i++);
    while (!info.is<Json::null>())
    {
      const std::string name = info.get_str("name");
      if (name == "State")
        state = info.get("value").get(0).to_str();
      else if (name == "Type")
        type = info.get("value").get(0).to_str();
      else if (name == "Sprints")
        sprint = info.get("value").get(0).to_str();
      else if (name == "summary")
        summary = info.get("value").to_str();

      info = field.get(i++);
    }

  }
};

struct AccountData
{
  std::string title;
  std::string url;
  std::string token;

  const std::string filename = "account.txt";

  void load() {
    json data;
    data.load(filename);
    title = data.obj["title"].get_str("value");
    url = data.obj["url"].get_str("value");
    token = data.obj["token"].get_str("value");
  }

  void save() {
    json data;
    data.load(filename);
    data.obj["title"] = json().set(title);
    data.obj["url"] = json().set(url);
    data.obj["token"] = json().set(token);
    data.save(filename);
  }

  std::vector<AgileInfo> agiles;
  std::string activeAgile;
  std::vector<IssueInfo> issues;
} account;

void showWaitingScreen(Screen* screen, bool show) 
{
  if (Window* waitingScr = screen->findWidgetGlobal<Window>("#waiting_window"))
  {
    waitingScr->setVisible(show);
    waitingScr->bringToFront();
    return;
  }

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

void resolveAdminData(std::shared_ptr<httplib::Response> response)
{

}

bool checkAccountUrl(TextBox* url)
{
  if (!url)
    return false;

  if (url->value().empty()) 
  {
    //url->animate<TextColorAnimator>();
    return false;
  }

  account.url = url->value();
  account.save();
  return true;
}

bool checkAccountToken(TextBox* token)
{
  if (!token)
    return false;

  if (token->value().empty())
  {
    //url->animate<TextColorAnimator>();
    return false;
  }

  account.token = token->value();
  account.save();
  return true;
}

void requestAgiles()
{
  httplib::SSLClient cli(account.url);
  cli.set_ca_cert_path(CA_CERT_FILE);
  cli.enable_server_certificate_verification(true);

  httplib::Headers headers{
    { "Accept", "application/json" },
    { "Authorization", std::string("Bearer ") + account.token },
    { "Cache-Control", "no-cache" },
    { "Content-Type", "application/json" }
  };

  auto res = cli.Get("/youtrack/api/agiles?fields=name,id,projects(id,shortName,name),columnSettings(columns(presentation))&$top=100", headers);
  if (res && res->status == 200)
  {
    account.agiles.clear();

    Json::value response;
    Json::parse(response, res->body);

    int i = 0;
    Json::value info = response.get(i++);
    while (!info.is<Json::null>())
    {
      if (info.get("$type").get_str() == "Agile")
      {
        AgileInfo agInfo;
        agInfo.id = info.get("id").get_str();
        agInfo.name = info.get("name").get_str();
        account.agiles.push_back(agInfo);
      }
      info = response.get(i++);
    }
  }
}

void showTasksWindow(Screen* screen, bool show)
{
  if (Window* tasksScr = screen->findWidgetGlobal<Window>("#tasks_window"))
  {
    tasksScr->setVisible(show);
    tasksScr->bringToFront();
    return;
  }

  auto& w = screen->window(Position{ 0, 0 },
                           FixedSize{ screen->size() },
                           WindowMovable{ Theme::WindowDraggable::dgFixed },
                           WindowHaveHeader{ false },
                           WidgetId{ "#tasks_window" },
                           WidgetStretchLayout{ Orientation::Vertical });

  auto& vstack = w.vscrollpanel(RelativeSize{ 1.f, 0.f }).vstack();

  for (auto& issue : account.issues)
  {
    auto& f = vstack.frame(FixedHeight{ 200 }, 
                           WidgetBoxLayout{ Orientation::Vertical, Alignment::Fill, 2, 2 });
    auto& header = f.hlayer(FixedHeight{ 30 });
    header.link(Caption{ issue.entityId });
    header.link(Caption{ issue.state });
    header.button(Caption{ "REC" }, Icon{ ENTYPO_ICON_RECORD });

    f.label(Caption{ issue.summary }, FixedHeight{ 150 });
  }
  screen->performLayout();
}

std::string encodeQueryData(std::string data)
{
  std::string r;
  for (char c : data)
    if (c == ' ') r.append("%20");
    else r.push_back(c);
  return r;
}

void requestTasks(std::string board)
{
  httplib::SSLClient cli(account.url);
  cli.set_ca_cert_path(CA_CERT_FILE);
  cli.enable_server_certificate_verification(true);

  httplib::Headers headers{
      { "Accept", "application/json" },
      { "Authorization", std::string("Bearer ") + account.token },
      { "Cache-Control", "no-cache" },
      { "Content-Type", "application/json" }
  };

  std::string request = "/youtrack/rest/issue?filter=for:me%20Board%20";
  request += encodeQueryData(board);
  request += ":%7BCurrent%20sprint%7D%20%23Unresolved%20";
  auto res = cli.Get(request.c_str(), headers);
  if (res && res->status == 200)
  {
    account.issues.clear();

    Json::value response;
    Json::parse(response, res->body);

    Json::value issues = response.get("issue");

    int i = 0;
    Json::value info = issues.get(i++);
    while (!info.is<Json::null>())
    {
      IssueInfo issue;
      issue.js = info;
      issue.id = info.get("id").get_str();
      issue.entityId = info.get("entityId").get_str();
      issue.readField();
      
      account.issues.push_back(issue);
      info = issues.get(i++);
    }
  }
}

void showAgilesScreen(Screen* screen, bool show)
{
  if (Window* agilesScr = screen->findWidgetGlobal<Window>("#agiles_window"))
  {
    agilesScr->setVisible(show);
    agilesScr->bringToFront();
    return;
  }

  auto& w = screen->window(Position{ 0, 0 },
                           FixedSize{ screen->size() },
                           WindowMovable{ Theme::WindowDraggable::dgFixed },
                           WindowHaveHeader{ false },
                           WidgetId{ "#agiles_window" },
                           WidgetStretchLayout{ Orientation::Vertical });

  auto& vstack = w.vscrollpanel(RelativeSize{ 1.f, 0.f }).vstack();

  for (auto& a : account.agiles)
  {
    vstack.button(Caption{ a.name }, 
                  ButtonFlags{ Button::RadioButton },
                  ButtonCallback{ [=] { account.activeAgile = a.name;} });
  }

  w.button(Caption{ "Save" }, 
           FixedHeight{30},
           ButtonCallback{ [=] { 
              requestTasks(account.activeAgile); 
              showTasksWindow(screen, true);
           }});
  screen->performLayout();
}

void requestAdminData(Screen* screen) 
{
  if (!checkAccountUrl(screen->findWidget<TextBox>("#youtrack_url")))
    return;

  if (!checkAccountToken(screen->findWidget<TextBox>("#youtrack_token")))
    return;

  showWaitingScreen(screen, true);

  account.save();

  httplib::SSLClient cli(account.url);
  cli.set_ca_cert_path(CA_CERT_FILE);
  cli.enable_server_certificate_verification(true);

 /* std::thread refresh_thread = std::thread([screen]() {
        std::chrono::milliseconds time(refresh);
        while (is_main_loop_active()) {
          std::this_thread::sleep_for(time);
          post_empty_event();
        }
      }); 
    }*/

  httplib::Headers headers{
    {"Accept", "application/json"},
    {"Authorization", std::string("Bearer ") + account.token},
    {"Cache-Control", "no-cache"},
    {"Content-Type", "application/json"}
  };

  auto res = cli.Get("/youtrack/api/admin/users/me?fields=id,login,name,email", headers);
  showWaitingScreen(screen, false);

  if (res && res->status == 200) 
  {
    requestAgiles();
    showAgilesScreen(screen, true);
  }
}

void showStartupScreen(Screen* screen, bool show)
{
  if (Window* startupScr = screen->findWidgetGlobal<Window>("#login_window"))
  {
    startupScr->setVisible(show);
    startupScr->bringToFront();
    return;
  }

  auto& w = screen->window(Position{ 0, 0 },
                           FixedSize{ screen->size() },
                           WindowMovable{ Theme::WindowDraggable::dgFixed },
                           WindowHaveHeader{ false },
                           WidgetId{ "#login_window" },
                           WindowGroupLayout{});

  //w.withTheme<WhiteTheme>(screen->nvgContext());
  w.label(FixedHeight{ 100 }, 
          Caption{ "Add new account" },
          CaptionHAlign{ hCenter },
          FontSize{ 26 })
   .toolbutton(Icon{ ENTYPO_ICON_OFF }, 
               IsSubElement{ true },
               Position{ 15, 15 },
               FontSize{ 32 },
               ButtonDrawFlags{ Button::DrawBody|Button::DrawIcon },
               BackgroundColor{ Color::transparent },
               BackgroundHoverColor{ Color::red },
               ButtonCallback{ [] { nanogui::sample::stop_frame_loop(); } });

  /* No need to store a pointer, the data structure will be automatically
  freed when the parent window is deleted */
  w.textbox(FontSize{ 24 },
            IsEditable{ true },
            TextAlignment::Left,
            TextPlaceholder{ "account name" },
            TextValue{ account.title },
            WidgetId{ "#account_name" });
  w.textbox(FontSize{ 24 },
            IsEditable{ true },
            TextAlignment::Left,
            TextPlaceholder{ "youtrack url" },
            WidgetId{ "#youtrack_url" },
            TextValue{ account.url });
  w.textbox(FontSize{ 24 },
            IsEditable{ true },
            TextAlignment::Left,
            TextPlaceholder{ "youtrack token" },
            TextValue{ account.token },
            WidgetId{ "#youtrack_token" });

  /* Alternative construction notation using variadic template */
  w.button(Caption{ "Login" },
           Icon{ ENTYPO_ICON_ROCKET },
           BackgroundColor{ 0, 0, 255, 25 },
           ButtonCallback{ [=] { requestAdminData(screen); } });
}

class HonsuScreen : public Screen {
public:
    HonsuScreen(const Vector2i& size, const std::string& caption) : Screen(size, caption, false) 
    {
      initGPUTimer(&gpuTimer);

      account.load();
      showStartupScreen(this, true);

      fpsGraph = &wdg<PerfGraph>(GRAPH_RENDER_FPS, "Frame Time", Vector2i(5, height() - 40));
      cpuGraph = &wdg<PerfGraph>(GRAPH_RENDER_MS, "CPU Time", Vector2i(5, height() - 40 * 2));
      gpuGraph = &wdg<PerfGraph>(GRAPH_RENDER_MS, "GPU Time", Vector2i(5, height() - 40 * 3));

      previousFrameTime = getTimeFromStart();

      performLayout();

      theme()->keyboardNavigation = false;
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
  Vector2i size{ 400, 600 };
  auto window = nanogui::sample::create_window(size.x(), size.y(), "Honsu", true, false);
  nanogui::sample::remove_window_border(window);
  nanogui::sample::create_context();

  {
    HonsuScreen screen(size + Vector2i{13, 36}, "");
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
