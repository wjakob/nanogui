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

struct SSLRequest {
  std::string request;
  std::function< void(int status, std::string)> answer;
};

using namespace std::chrono_literals;
httplib::SSLClient* sslClient = nullptr;
httplib::Headers sslHeaders;

std::vector<SSLRequest> sslrequests;

void showStartupScreen(Screen*);
void requestAgilesAndResolve(Screen*);

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

WidgetId lastWindowId{ "" };

template<typename ... Args>
Window& createWindow(Screen* screen, std::string id, const Args&... args)
{
  if (!lastWindowId.value.empty())
  {
    if (auto rm = screen->findWidget(lastWindowId.value, false))
      rm->remove();
  }
  lastWindowId.value = id;
  return screen->window(Position{ 0, 0 },
                        FixedSize{ screen->size() },
                        WindowMovable{ Theme::WindowDraggable::dgFixed },
                        WindowHaveHeader{ false },
                        WidgetId{ id },
                        args... );
}

void showWaitingScreen(Screen* screen)
{
  auto& w = createWindow(screen, "#waiting_window", WindowBoxLayout{ Orientation::Horizontal, Alignment::Middle, 0, 6 });

  w.spinner(SpinnerRadius{ 0.5f }, BackgroundColor{ Color::transparent });
  screen->needPerformLayout(screen);
}

void showTasksWindow(Screen* screen)
{
  auto& w = createWindow(screen, "#tasks_window", WidgetStretchLayout{ Orientation::Vertical });
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
  screen->needPerformLayout(screen);
}

std::string encodeQueryData(std::string data)
{
  std::string r;
  for (char c : data)
    if (c == ' ') r.append("%20");
    else r.push_back(c);
  return r;
}

void requestTasksAndResolve(Screen* screen, std::string board)
{
  std::string body = "/youtrack/rest/issue?filter=for:me%20Board%20";
  body += encodeQueryData(board);
  body += ":%7BCurrent%20sprint%7D%20%23Unresolved%20";

  showWaitingScreen(screen);
  SSLRequest request{ body,
    [screen](int status, std::string body)
    {
      account.issues.clear();

      if (status == 200)
      {
        account.issues.clear();

        Json::value response;
        Json::parse(response, body);

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

      if (account.issues.empty()) requestAgilesAndResolve(screen);
      else showTasksWindow(screen);
    }
  };
  sslrequests.push_back(request);
}

void showAgilesScreen(Screen* screen)
{
  auto& w = createWindow(screen, "#agiles_window", WidgetBoxLayout{ Orientation::Vertical, Alignment::Fill, 20, 20 });
  w.label(FixedHeight{ 60 },
          Caption{ "Select agile boards" },
          CaptionAlign{ hCenter, vBottom },
          CaptionFont{ "sans-bold" },
          FontSize{ 42 });
  w.label(Caption{ "You will be able to edit this selection later" },
          CaptionAlign{ hCenter, vTop },
          FixedHeight{ 20 });

  auto& vstack = w.vscrollpanel(RelativeSize{ 1.f, 0.f }).vstack(20, 20);

  auto agilebtn = [&vstack] (std::string name, int index) {
    vstack.button(Caption{ name },
                  FixedHeight{ 50 },
                  ButtonFlags{ Button::RadioButton },
                  BorderSize{ 2 },
                  FontSize{ 24 },
                  CaptionFont{ "sans" },
                  Icon{ ENTYPO_ICON_OK },
                  IconColor{ Color::dimGrey },
                  IconPushedColor{ Color::aquamarine },
                  BorderColor{ Color::dimGrey },
                  BackgroundColor{ Color::transparent },
                  BackgroundHoverColor{ Color::transparent },
                  ButtonCallback{ [=] { account.activeAgile = name; }});
  };

  for (int i=0; i < account.agiles.size(); i++)
    agilebtn(account.agiles[i].name, i);
    
  vstack.button(Caption{ "Save" },
                FixedHeight{ 50 },
                BorderSize{ 0 },
                BackgroundColor{ Color::indianRed },
                BackgroundHoverColor{ Color::caesarRed },
                ButtonCallback{ [screen] { requestTasksAndResolve(screen, account.activeAgile); }});
  screen->needPerformLayout(screen);
}

void requestAgilesAndResolve(Screen* screen)
{
  SSLRequest request{ "/youtrack/api/agiles?fields=name,id,projects(id,shortName,name),columnSettings(columns(presentation))&$top=100",
           [screen](int status, std::string body) {
              account.agiles.clear();

              if (status == 200)
              {
                Json::value agiles;
                Json::parse(agiles, body);

                int i = 0;
                Json::value info = agiles.get(i++);
                while (!info.is<Json::null>())
                {
                  if (info.get("$type").get_str() == "Agile")
                  {
                    AgileInfo agInfo;
                    agInfo.id = info.get("id").get_str();
                    agInfo.name = info.get("name").get_str();
                    account.agiles.push_back(agInfo);
                  }
                  info = agiles.get(i++);
                }
              }

              if (account.agiles.empty()) showStartupScreen(screen);
              else showAgilesScreen(screen);
            }
  };
  sslrequests.push_back(request);
}

void createSSLClient() 
{
  if (sslClient)
    return;

  sslClient = new httplib::SSLClient(account.url);
  //sslClient->set_ca_cert_path(CA_CERT_FILE);
  //sslClient->enable_server_certificate_verification(true);

  sslHeaders = httplib::Headers{
    { "Accept", "application/json" },
    { "Authorization", std::string("Bearer ") + account.token },
    { "Cache-Control", "no-cache" },
    { "Content-Type", "application/json" }
  };
}

void requestAdminData(Screen* screen)
{
  if (!checkAccountUrl(screen->findWidget<TextBox>("#youtrack_url")))
  {
    showStartupScreen(screen);
    return;
  }

  if (!checkAccountToken(screen->findWidget<TextBox>("#youtrack_token")))
  {
    showStartupScreen(screen);
    return;
  }

  account.save();
  showWaitingScreen(screen);
 
  SSLRequest sslr{
    "/youtrack/api/admin/users/me?fields=id,login,name,email",
    [screen](int status, std::string) {
      if (status == 200) requestAgilesAndResolve(screen);
      else showStartupScreen(screen);
    }
  };

  sslrequests.push_back(sslr);
}

void update_requests()
{
  if (!sslrequests.empty())
  {
    createSSLClient();

    SSLRequest r = sslrequests.front();
    sslrequests.erase(sslrequests.begin());

    std::shared_ptr<httplib::Response> nn = sslClient->Get(r.request.c_str(), sslHeaders);
    auto answer = r.answer;
    answer(nn ? nn->status : -1, nn ? nn->body : "");
  }
}

void showStartupScreen(Screen* screen)
{
  auto& w = createWindow(screen, "#login_window", WidgetBoxLayout{ Orientation::Vertical, Alignment::Fill, 20, 20 });
    //w.withTheme<WhiteTheme>(screen->nvgContext());

  auto& c = w.widget(WidgetBoxLayout{ Orientation::Horizontal, Alignment::Fill, 2, 2 });
  c.toolbutton(Icon{ ENTYPO_ICON_OFF },
               FontSize{ 32 },
               ButtonDrawFlags{ Button::DrawBody | Button::DrawIcon },
               BackgroundColor{ Color::transparent },
               BackgroundHoverColor{ Color::red },
               ButtonCallback{ [] { nanogui::sample::stop_frame_loop(); } });
  c.label(Caption{ "H" });
  c.toolbutton(Icon{ ENTYPO_ICON_RECORD }, FixedWidth{15}, ButtonDrawFlags{ Button::DrawIcon }, IconColor{ Color::red });
  c.label(Caption{ "N S U" });

  w.label(FixedHeight{ 100 },
    Caption{ "Add new account" },
    CaptionHAlign{ hCenter },
    FontSize{ 26 });

  auto textfield = [&](std::string placeholder, std::string value, std::string id) {
    w.textbox(FontSize{ 24 },
      IsEditable{ true },
      TextAlignment::Left,
      TextPlaceholder{ placeholder },
      TextValue{ value },
      BorderColor{ Color::dimGrey },
      BorderSize{ 2 },
      BackgroundHoverColor{ Color::transparent },
      BackgroundColor{ Color::transparent },
      WidgetId{ id });
  };

  /* No need to store a pointer, the data structure will be automatically
  freed when the parent window is deleted */
  textfield("account name", account.title, "#account_name");
  textfield("youtrack url", account.url, "#youtrack_url");
  textfield("youtrack token", account.token, "#youtrack_token");

  w.link(Caption{ "How to obtain a new permament token?" });

  /* Alternative construction notation using variadic template */
  w.button(Caption{ "Login" },
           FontSize{ 32 },
           Icon{ ENTYPO_ICON_ROCKET },
           BackgroundColor{ 0, 0, 255, 25 },
           ButtonCallback{ [screen] { requestAdminData(screen); }});
  screen->needPerformLayout(screen);
}

class HonsuScreen : public Screen {
public:
    HonsuScreen(const Vector2i& size, const std::string& caption) : Screen(size, caption, false) 
    {
      initGPUTimer(&gpuTimer);

      account.load();
      showStartupScreen(this);

      fpsGraph = &wdg<PerfGraph>(GRAPH_RENDER_FPS, "Frame Time", Vector2i(5, height() - 40));
      cpuGraph = &wdg<PerfGraph>(GRAPH_RENDER_MS, "CPU Time", Vector2i(5, height() - 40 * 2));
      gpuGraph = &wdg<PerfGraph>(GRAPH_RENDER_MS, "GPU Time", Vector2i(5, height() - 40 * 3));

      previousFrameTime = getTimeFromStart();

      needPerformLayout(this);

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
        startGPUTimer(&gpuTimer);

        double t = getTimeFromStart();
        double dt = t - previousFrameTime;
        previousFrameTime = t;

        /* Draw the user interface */
        Screen::draw(ctx);

        cpuTime = getTimeFromStart() - t;

        if (fpsGraph) fpsGraph->update(dt);
        if (cpuGraph) cpuGraph->update(cpuTime);
        if (gpuGraph && gpuTimer.supported)
        {
          float gpuTimes[3];
          int n = stopGPUTimer(&gpuTimer, gpuTimes, 3);
          for (int i = 0; i < n; i++)
            gpuGraph->update(gpuTimes[i]);
        }
    }

    virtual void drawContents() {
        using namespace nanogui;
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
    
    bool requests_thread_active = true;
    auto requests_thread = std::thread([&] {
      while (requests_thread_active)
      {
        update_requests();
        std::this_thread::sleep_for(1s);
      }
    });
  
    nanogui::sample::run([&] {
        nanogui::sample::clear_frame(screen.background());

        screen.drawAll();

        nanogui::sample::present_frame(window);

        /* Wait for mouse/keyboard or empty refresh events */
        nanogui::sample::wait_events();
    });

    requests_thread_active = false;
    requests_thread.join();
    nanogui::sample::poll_events();
  }

  nanogui::sample::destroy_window(window);
  nanogui::shutdown();
  return 0;
}
