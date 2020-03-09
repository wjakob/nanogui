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
#include <nanogui/separator.h>
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

void open_url(const std::string& url, const std::string& prefix);

struct Url
{
  const std::string http = "http://";
  const std::string issues = "/youtrack/issue/";
} y_url;

struct IssueInfo
{
  using Ptr = std::shared_ptr<IssueInfo>;
  std::string id;
  std::string entityId;
  std::string state;
  std::string sprint;
  std::string type;
  std::string summary;
  bool rec;
  float recordTimeSec;
  float recordTimeTodaySec;
  Json::value js;

  void updateTime(float dtSec)
  {
    if (rec)
    {
      recordTimeSec += dtSec;
      recordTimeTodaySec += dtSec;
    }
  }

  void openUrl(std::string base) { open_url(y_url.http + base + y_url.issues + entityId, ""); }

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

  IssueInfo::Ptr getActiveIssue()
  {
    for (auto& i : issues) 
      if (i->rec) return i;
    return nullptr;
  }

  void setIssueRecord(IssueInfo::Ptr issue, bool rec)
  {
    for (auto& i : issues) i->rec = false;
    issue->rec = rec;
  }

  std::vector<AgileInfo> agiles;
  std::string activeAgile;
  std::vector<IssueInfo::Ptr> issues;
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

void open_url(const std::string& url, const std::string& prefix)
{
#ifdef __UNIX__
  std::string command = prefix + "xdg-open '" + url + "'";
  Logger::info(command);
  auto result = ::system(command.c_str());
  result;
#elif defined(WIN32)
  ShellExecuteA(0, "Open", url.c_str(), 0, 0, SW_SHOW);
#elif defined(__DARWIN__)
  std::string command = "open \"" + url + "\" &";
  auto result = ::system(command.c_str());
  result;
#endif
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

void createWindowHeader(Widget& w)
{
  auto& c = w.widget(WidgetBoxLayout{ Orientation::Horizontal, Alignment::Fill, 2, 2 });
  c.toolbutton(Icon{ ENTYPO_ICON_OFF }, FontSize{ 32 },
               DrawFlags{ Button::DrawBody | Button::DrawIcon },
               BackgroundColor{ Color::transparent }, BackgroundHoverColor{ Color::red },
               ButtonCallback{ [] { nanogui::sample::stop_frame_loop(); } });
  c.label(Caption{ "H" });
  c.toolbutton(Icon{ ENTYPO_ICON_RECORD }, FixedWidth{ 15 }, DrawFlags{ Button::DrawIcon }, IconColor{ Color::red });
  c.label(Caption{ "N S U" });
}

void showTasksWindow(Screen* screen);
void showRecordsWindow(Screen* screen)
{
  auto& w = createWindow(screen, "#tasks_window", WidgetBoxLayout{ Orientation::Vertical, Alignment::Fill, 20, 10 });

  createWindowHeader(w);

  auto& buttons = w.hlayer();

  buttons
    .button(Caption{ "Boards" }, DrawFlags{ Button::DrawCaption },  HoveredTextColor{ Color::red }, 
            ButtonCallback{ [screen] { showTasksWindow(screen); } })
    .line(BackgroundColor{ Color::grey }, DrawFlags{ Line::Horizontal | Line::Bottom | Line::CenterH },
          RelativeSize{ 0.9f, 0.f }, LineWidth{ 2 });
  buttons
    .button(Caption{ "Records" }, DrawFlags{ Button::DrawCaption })
    .line(BackgroundColor{ Color::red }, DrawFlags{ Line::Horizontal | Line::Bottom | Line::CenterH },
          RelativeSize{ 0.9f, 0.f }, LineWidth{ 2 });

  screen->needPerformLayout(screen);
}

void openAgileUrl(const std::string& agile)
{

}

void createNewIssue(Screen* screen)
{

}

void changeTaskRecordStatus(IssueInfo::Ptr issue, bool rec)
{
  for (auto& i : account.issues)
    issue->rec = false;
  issue->rec = rec;
}

void stopIssueRecord(IssueInfo::Ptr issue)
{
  if (!issue)
    return;

  account.setIssueRecord(issue, false);
}

void startIssueRecord(IssueInfo::Ptr issue)
{
  std::string body = "/youtrack/rest/issue/";
  body += issue->entityId;
  body += "/timetracking/workitem";
  SSLRequest request{ body, [issue](int status, std::string body) {
    if (status != 200)
      return;

    Json::value response;
    Json::parse(response, body);

    std::time_t timet = std::time(nullptr);
    std::tm* tmt = std::localtime(&timet);
    std::tm tm_day_start = *tmt, 
            tm_day_end = *tmt;
    tm_day_start.tm_sec = 0;
    tm_day_start.tm_min = 0;
    tm_day_start.tm_hour = 0;

    tm_day_end.tm_sec = 59;
    tm_day_end.tm_min = 59;
    tm_day_end.tm_hour = 23;
    
    //mktime convert tm to secs from epoch
    std::time_t day_start = mktime(&tm_day_start) * 1000;
    std::time_t day_end = mktime(&tm_day_end) * 1000;

    int i = 0;
    int timeSpent = 0;
    uint64_t timeSpentToday = 0;
    Json::value info = response.get(i++);
    while (!info.is<Json::null>())
    {
      int duration = info.get("duration").get_int();
      uint64_t created = (uint64_t)info.get("created").get_int();
      if (created >= day_start && created <= day_end)
        timeSpentToday += duration;
      timeSpent += duration;
      info = response.get(i++);
    }
    issue->recordTimeSec = 0;
    issue->recordTimeTodaySec = timeSpentToday * 60;

    account.setIssueRecord(issue, true);
  }};
  sslrequests.push_back(request);
}

class TaskRecordButton : public Button
{
public:
  std::function<IssueInfo::Ptr()> getIssue = [] { return nullptr; };
  std::function<bool()> forceShow = [] { return false; };
  int lastIssueRec = -1;

  using Button::set;
  TaskRecordButton(Widget* parent, std::function<IssueInfo::Ptr()> _get, std::function<bool()> _watch = [] { return false; })
    : Button(parent,
      Caption{ "REC" }, FixedWidth{ 80 },
      DrawFlags{ Button::DrawBody | Button::DrawCaption | Button::DrawIcon },
      Icon{ ENTYPO_ICON_RECORD })
  {
    getIssue = _get;
    forceShow = _watch;
    setCallback( [this] { 
      if (getIssue())
      {
        if (getIssue()->rec)
        {
          stopIssueRecord(getIssue());
        }
        else
        {
          stopIssueRecord(account.getActiveIssue());
          startIssueRecord(getIssue());
        }
      }
    });
    updateState(getIssue() ? getIssue()->rec : 0);
  }

  void draw(NVGcontext* ctx) override
  {
    if (forceShow() || (getIssue() && getIssue()->rec))
      Button::draw(ctx);
  }

  void updateState(int state)
  {
    setCaption(state > 0 ? "STOP" : "REC");
    Color background = state > 0 ? Color::red : Color::transparent;
    Color text = background == Color::red ? Color::white : Color::red;
    Color hover = (text == Color::red || background == Color::red) ? Color::white : Color::red;
    setIcon(state > 0 ? ENTYPO_ICON_STOP_1 : ENTYPO_ICON_RECORD);
    setIconColor(text);
    setIconHoveredColor(hover);
    setTextColor(text);
    setTextHoverColor(hover);
    setBackgroundColor(background);
    setBackgroundHoverColor(Color::red);
  }

  void afterDraw(NVGcontext* ctx) override
  {
    Button::afterDraw(ctx);

    int state = getIssue() ? (getIssue()->rec?1:0) : 0;
    if (lastIssueRec == state)
      return;

    lastIssueRec = state;
    updateState(state);
  }
};

class TaskRecordPanel : public Frame
{
public:
  TaskRecordPanel(Widget* parent)
    : Frame(parent)
  {
    setSubElement(true);

    withLayout<BoxLayout>(Orientation::Vertical, Alignment::Fill, 10, 10);
    line(LineWidth{ 4 }, BackgroundColor{ Color::red }, DrawFlags{ Line::Horizontal | Line::Top | Line::CenterH});

    auto& header = widget().flexlayout(Orientation::ReverseHorizontal);
    header.label(WidgetId{"#txt"}, Caption{ "No task recording" }, FontSize{ 28 });
    header.wdg<TaskRecordButton>([] { return account.getActiveIssue(); });
  
    auto& timeline = hlayer();
    timeline.label(WidgetId{ "#time" }, Caption{ "00:00:00" }, CaptionHAlign{ TextHAlign::hLeft },
                   FontSize{ 28 });
    timeline.label(WidgetId{ "#dtime" }, Caption{ "00:00:00" }, CaptionHAlign{ TextHAlign::hRight });
  }

  void performLayout(NVGcontext* ctx) override
  {
    setSize(parent()->width(), preferredSize(ctx).y());
    setPosition(0, parent()->height() - height());

    Frame::performLayout(ctx);
  }

  void setCaptionSafe(std::string id, std::string txt) { if (auto lb = findWidget<Label>(id)) lb->setCaption(txt); }

  void afterDraw(NVGcontext* ctx) override
  {
    IssueInfo::Ptr issue = account.getActiveIssue();

    auto sec2str = [](int sec) {
      int minutes = sec / 60;
      char str[16] = { 0 };
      snprintf(str, 16, "%02d:%02d:%02d", (minutes / 60), minutes % 60, sec % 60);
      return std::string(str);
    };
  
    setCaptionSafe("#time", sec2str(issue ? issue->recordTimeSec : 0));
    setCaptionSafe("#dtime", "TODAY:" + sec2str(issue ? issue->recordTimeTodaySec : 0));
    setCaptionSafe("#txt", issue ? issue->summary : "No task recording");

    Frame::afterDraw(ctx);
  }
};

class TaskPanel : public Frame 
{
public:
  IssueInfo::Ptr issue;

  TaskPanel(Widget* parent, IssueInfo::Ptr _issue)
    : Frame(parent), issue(_issue)
  {
    withLayout<BoxLayout>(Orientation::Vertical, Alignment::Fill, 10, 10);

    auto& header = hlayer(FixedHeight{ 30 });
    header.link(Caption{ issue->entityId }, TextColor{ Color::white },
                ButtonCallback{ [this] { issue->openUrl(account.url); } });
    header.link(Caption{ issue->state }, TextColor{ Color::white }, WidgetCursor{ Cursor::Arrow });
    header.widget();
    header.wdg<TaskRecordButton>([this] { return issue; }, [this] { return inFocusChain(); });

    label(Caption{ issue->summary });
  }
};

void showTasksWindow(Screen* screen)
{
  auto& w = createWindow(screen, "#tasks_window", WidgetBoxLayout{ Orientation::Vertical, Alignment::Fill, 20, 10 });
  
  createWindowHeader(w);

  auto& buttons = w.hlayer();

  auto linestyle = DrawFlags{ Line::Horizontal | Line::Bottom | Line::CenterH };
  buttons
    .button(Caption{ "Boards" }, DrawFlags{ Button::DrawCaption })
    .line(BackgroundColor{ Color::red }, LineWidth{ 2 }, RelativeSize{ 0.9f, 0.f }, linestyle);

  buttons
    .button(Caption{ "Records" }, DrawFlags{ Button::DrawCaption }, HoveredTextColor{ Color::red }, 
            ButtonCallback{ [screen] { showRecordsWindow(screen); }})
    .line(BackgroundColor{ Color::grey }, LineWidth{ 2 }, RelativeSize{ 0.9f, 0.f }, linestyle);

  auto& vstack = w.vscrollpanel(RelativeSize{ 1.f, 0.f }).vstack();

  vstack
    .button(Caption{ account.activeAgile }, Icon{ ENTYPO_ICON_FORWARD_OUTLINE }, CaptionHAlign{ TextHAlign::hLeft },
            DrawFlags{ Button::DrawCaption | Button::DrawIcon }, IconAlignment{ IconAlign::Right },
            HoveredTextColor{ Color::lightGray }, ButtonCallback{ [] { openAgileUrl(account.activeAgile);} })
    .line(BackgroundColor{ Color::black }, linestyle);

  auto& actions = vstack.hstack(2, 10);
  actions.button(Caption{ "create issue" }, BackgroundColor{ Color::darkGrey }, BackgroundHoverColor{Color::heavyDarkGrey},
                 DrawFlags{ Button::DrawBody | Button::DrawCaption }, CornerRadius{ 4 },
                 ButtonCallback{ [screen] { createNewIssue(screen); }});
  actions.toolbutton(Icon{ ENTYPO_ICON_CCW }, BackgroundColor{ Color::darkGrey }, BackgroundHoverColor{ Color::heavyDarkGrey },
                     DrawFlags{ Button::DrawBody | Button::DrawIcon }, CornerRadius{ 4 },
                     ButtonCallback{ [screen] { showTasksWindow(screen); }});

  for (auto& issue : account.issues)
    vstack.wdg<TaskPanel>(issue);

  w.wdg<TaskRecordPanel>();
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
          auto issue = std::make_shared<IssueInfo>();
          issue->js = info;
          issue->id = info.get("id").get_str();
          issue->entityId = info.get("entityId").get_str();
          issue->readField();

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

  createWindowHeader(w);

  auto& header = w.vstack();
  header.label(FixedHeight{ 60 },
               Caption{ "Select agile boards" },
               FontColor{ Color::white },
               CaptionAlign{ hCenter, vBottom },
               CaptionFont{ "sans-bold" },
               FontSize{ 42 });
  header.label(Caption{ "You will be able to edit this selection later" },
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

  createWindowHeader(w);

  w.label(FixedHeight{ 100 },
    Caption{ "Add new account" },
    CaptionHAlign{ TextHAlign::hCenter },
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

  w.link(Caption{ "How to obtain a new permament token?" },
         ButtonCallback{ [] { open_url("https://www.jetbrains.com/help/youtrack/standalone/Manage-Permanent-Token.html", ""); }});

  /* Alternative construction notation using variadic template */
  w.button(Caption{ "Login" }, FontSize{ 32 },
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

      //fpsGraph = &wdg<PerfGraph>(GRAPH_RENDER_FPS, "Frame Time", Vector2i(width() - 210, 5));
      cpuGraph = &wdg<PerfGraph>(GRAPH_RENDER_MS, "CPU Time", Vector2i(width() - 205, 10));

      previousFrameTime = getTimeFromStart();

      needPerformLayout(this);

      theme()->keyboardNavigation = false;
      //theme()->debugHighlightMouseover = true;
    }

    virtual bool keyboardEvent(int key, int scancode, int action, int modifiers) {
        if (Screen::keyboardEvent(key, scancode, action, modifiers))
            return true;
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

        //if (fpsGraph) fpsGraph->update(dt);
        if (cpuGraph) cpuGraph->update(cpuTime);
    }

private:
    nanogui::PerfGraph *fpsGraph = nullptr;
    nanogui::PerfGraph *cpuGraph = nullptr;
    double previousFrameTime = 0, cpuTime = 0;
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

    auto timerec_thread = std::thread([&] {
      int lasttime = getTimeFromStart();
      while (requests_thread_active)
      {
        for (auto& issue : account.issues)
          issue->updateTime(0.5);
        std::this_thread::sleep_for(0.5s);
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
    timerec_thread.join();
    nanogui::sample::poll_events();
  }

  nanogui::sample::destroy_window(window);
  nanogui::shutdown();
  return 0;
}
