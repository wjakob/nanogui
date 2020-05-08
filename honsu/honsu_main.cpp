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

struct AgilesWindow;
struct InactiveWarning;

struct AgileInfo
{
  std::string name;
  std::string id;
  std::string shortName;

  Json::value js;

  AgileInfo(const Json::value& data)
  {
    js = data;
    id = js.get("id").get_str();
    Json::value projects = js.get("projects");
    if (projects.is<Json::array>())
    {
      Json::value proj = projects.get(0);
      if (!proj.is<Json::null>())
        shortName = proj.get("shortName").get_str();
    }

    name = js.get("name").get_str();
  }

  bool valid() const { return js.get("$type").get_str() == "Agile"; }
};

void open_url(const std::string& url, const std::string& prefix);
std::function<void(bool)> showRecPanelWait = [](bool) {};
std::function<void(bool, bool)> showAppExclusive = [](bool, bool) {};

struct Url
{
  const char* SCHEME_REGEX = "((http[s]?)://)?";  // match http or https before the ://
  const char* USER_REGEX = "(([^@/:\\s]+)@)?";  // match anything other than @ / : or whitespace before the ending @
  const char* HOST_REGEX = "([^@/:\\s]+)";      // mandatory. match anything other than @ / : or whitespace
  const char* PORT_REGEX = "(:([0-9]{1,5}))?";  // after the : match 1 to 5 digits
  const char* PATH_REGEX = "(/[^:#?\\s]*)?";    // after the / match anything other than : # ? or whitespace
  const char* QUERY_REGEX = "(\\?(([^?;&#=]+=[^?;&#=]+)([;|&]([^?;&#=]+=[^?;&#=]+))*))?"; // after the ? match any number of x=y pairs, seperated by & or ;
  const char* FRAGMENT_REGEX = "(#([^#\\s]*))?";    // after the # match anything other than # or whitespace

  std::string scheme;
  std::string user;
  std::string host;
  std::string port;
  std::string path;
  std::string query;
  std::string fragment;

  static std::string encodeQueryData(std::string data)
  {
    std::string r;
    for (char c : data)
      if (c == ' ') r.append("%20");
      else r.push_back(c);
    
    return r;
  }

  Url(const std::string &i_uri)
  {
    static const std::regex regExpr(std::string("^") + SCHEME_REGEX + USER_REGEX + HOST_REGEX + PORT_REGEX  + PATH_REGEX + QUERY_REGEX + FRAGMENT_REGEX + "$");

    std::smatch matchResults;
    if (std::regex_match(i_uri.cbegin(), i_uri.cend(), matchResults, regExpr))
    {
      scheme.assign(matchResults[2].first, matchResults[2].second);
      user.assign(matchResults[4].first, matchResults[4].second);
      host.assign(matchResults[5].first, matchResults[5].second);
      port.assign(matchResults[7].first, matchResults[7].second);
      path.assign(matchResults[8].first, matchResults[8].second);
      query.assign(matchResults[10].first, matchResults[10].second);
      fragment.assign(matchResults[15].first, matchResults[15].second);
    }
  }
};

struct Youtrack
{
  const std::string http = "http://";
  const std::string issues = "/issue/";
  const std::string rest_issues = "/rest/issue/";
  const std::string filter_issues = "/rest/issue?filter=";
  const std::string filter_my_issues = "/rest/issue?filter=for:me";
} youytrack;

std::string _s(const char* t) { return std::string(t); }
std::string _s(uint64_t t) { return std::to_string(t); }
struct AllOf {
  std::string str;
  AllOf(std::initializer_list<std::string> l) { for (auto& i : l) str += i; }
  operator std::string() const { return str; }
};

struct IssueInfo
{
  using Ptr = std::shared_ptr<IssueInfo>;
  std::string id;
  std::string entityId;
  std::string state;
  std::vector<std::string> sprints;
  std::string type;
  std::string summary;
  std::string projectShortName;
  int64_t createdEpochSec = 0;
  int64_t updatedEpochSec = 0;
  bool rec = false;
  int64_t workStartTime = 0;
  int64_t spentTimeMin = 0;
  float recordTimeSec = 0;
  float recordTimeAllSec = 0;
  float recordTimeTodaySec = 0;
  Json::value js;

  static Ptr fromJson(const Json::value& js)
  {
    auto ptr = std::make_shared<IssueInfo>();
    ptr->readField(js);
    return ptr;
  }

  std::string getWorktimeUrl() { return AllOf{ youytrack.rest_issues, entityId, "/timetracking/workitem" }; }

  void updateTime(float dtSec)
  {
    if (rec)
    {
      recordTimeSec += dtSec;
      recordTimeTodaySec += dtSec;
    }
  }

  void openUrl(std::string base) { open_url(youytrack.http + base + youytrack.issues + entityId, ""); }

  void readField(const Json::value& _js)
  {
    js = _js;
    id = js.get("id").get_str();
    entityId = js.get("entityId").get_str();
    Json::value field = js.get("field");
    field.update([&] (auto& v) {
      auto _v = [](auto& i) { return i.get("value"); };
      const std::string name = v.get_str("name");
      if      (name == "State")   state = _v(v).get(0).to_str();
      else if (name == "Type")    type = _v(v).get(0).to_str();
      else if (name == "Sprints") sprints.push_back(_v(v).get(0).to_str());
      else if (name == "sprint") {
        Json::value arr = _v(v);
        arr.update([&](auto& v) { sprints.push_back(_v(v).to_str()); return true; });
      }
      else if (name == "summary") summary = _v(v).to_str();
      else if (name == "projectShortName") projectShortName = _v(v).to_str();
      else if (name == "created") createdEpochSec = std::stoll(_v(v).get_str());
      else if (name == "updated") updatedEpochSec = std::stoll(_v(v).get_str());
      else if (name == "Spent time") spentTimeMin = std::stoll(_v(v).get(0).get_str());
      return true;
    });
  }
};

struct AccountData
{
  std::string title;
  std::string url;
  std::string path;
  std::string token;
  std::mutex issuesGuard;

  const std::string filename = "account.txt";

  void load() {
    json data;
    data.load(filename);
    title = data.get<std::string>("title");
    url = data.get<std::string>("url");
    token = data.get<std::string>("token");
  }

  void save() {
    json data;
    data.load(filename);
    data["title"] = json().set(title);
    data["url"] = json().set(url);
    data["token"] = json().set(token);
    data.save(filename);
  }

  IssueInfo::Ptr getActiveIssue()
  {
    std::lock_guard<std::mutex> guard(issuesGuard);
    for (auto& i : issues) 
      if (i->rec) return i;
    return nullptr;
  }

  void resetInactiveTime()
  {
    suspendInactiveTime = false;
    inactiveTimeSec = 0.f;
    inactiveLastTimeSec = 0.f;
  }

  void setIssueRecord(IssueInfo::Ptr issue, bool rec)
  {
    std::lock_guard<std::mutex> guard(issuesGuard);
    for (auto& i : issues) i->rec = false;
    issue->rec = rec;
  }

  std::vector<AgileInfo> agiles;
  std::string activeAgile;
  std::vector<IssueInfo::Ptr> issues;
  float inactiveTimeSec = 0;
  float inactiveLastTimeSec = 0;
  float lastCheckActivityTimeSec = 0;
  float nocheckActivityInteralSec = 60;
  bool suspendInactiveTime = false;
} account;

struct SSLGet {
  using ResponseHandler = std::function< void(int status, std::string)>;
  static std::vector<SSLGet> requests;

  std::string path;
  httplib::Headers headers;
  ResponseHandler answer;
  SSLGet& onResponse(ResponseHandler handler) { answer = handler; return *this; }
  void execute() { requests.push_back(*this); }
};

struct SSLPost {
  using ResponseHandler = std::function< void(int status, std::string)>;
  static std::vector<SSLPost> requests;
  static int invalidRequestTime;
  static int invalidRequestSent;

  std::string path;
  std::string body;
  std::string content_type;
  httplib::Headers headers;
  ResponseHandler answer;
  SSLPost& onResponse(ResponseHandler handler) { answer = handler; return *this; }
  void execute() { requests.push_back(*this); }
};

std::vector<SSLGet> SSLGet::requests;
std::vector<SSLPost> SSLPost::requests;
int SSLPost::invalidRequestTime = 0;
int SSLPost::invalidRequestSent = 0;

using namespace std::chrono_literals;
httplib::SSLClient* sslClient = nullptr;
httplib::Headers sslHeaders;

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

std::string sec2str(int sec)
{
  int minutes = sec / 60;
  char str[16] = { 0 };
  snprintf(str, 16, "%02d:%02d:%02d", (minutes / 60), minutes % 60, sec % 60);
  return std::string(str);
};

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

void makeDayInterval(std::tm& b, std::tm& e)
{
  b.tm_sec = b.tm_min = b.tm_hour = 0;
  e.tm_sec = e.tm_min = 59; e.tm_hour = 23;
}

struct UniqueWindow : public Window
{
  enum { NoHeader=false, ShowHeader=true } ;
  static WidgetId lastWindowId;

  template<typename ... Args>
  UniqueWindow(std::string id, bool addheader, const Args&... args)
    : Window(nullptr, WidgetId{ id }, Position{ 0, 0 }, WindowHaveHeader{ false },
             FixedSize{ elm::active_screen()->size() }, WindowMovable{ Theme::WindowDraggable::dgFixed },
             args...)
  {
    if (auto rm = Window::find(lastWindowId.value.c_str()))
      rm->removeLater();
    lastWindowId.value = id;

    if (addheader)
    {
      widget(WidgetBoxLayout{ Orientation::Horizontal, Alignment::Fill, 2, 2 }, FixedHeight{ 40 },
             Children{},
               elm::ToolButton{ 
                 Icon{ ENTYPO_ICON_OFF }, FontSize{ 32 }, DrawFlags{ Button::DrawBody | Button::DrawIcon },
                 BackgroundColor{ Color::transparent }, BackgroundHoverColor{ Color::red },
                 ButtonCallback{ [] { nanogui::sample::stop_frame_loop(); } }
               },
               elm::Label{ "H" },
               elm::ToolButton{ Icon{ ENTYPO_ICON_RECORD }, FixedWidth{ 15 }, DrawFlags{ Button::DrawIcon }, IconColor{ Color::red }},
               elm::Label{ "N S U" }
      );
    }
  }
};

WidgetId UniqueWindow::lastWindowId = { "" };

namespace nanogui {
  namespace elm { template<typename W, typename...Args> void create(const Args&... args) { new W(args...); } }
}

struct WaitingWindow : public UniqueWindow
{
  WaitingWindow()
    : UniqueWindow("#waiting_window", NoHeader, WindowBoxLayout{ Orientation::Horizontal, Alignment::Middle, 0, 6 })
  {
    spinner(SpinnerRadius{ 0.5f }, BackgroundColor{ Color::transparent });
    performLayoutLater();
  }

  static void create() { new WaitingWindow(); }
};

struct LoginWindow;

class IssuePanel : public Frame
{
public:
  RTTI_CLASS_UID(IssuePanel)
  RTTI_DECLARE_INFO(IssuePanel)

  IssueInfo::Ptr issue;
  int timeSpentDaySec = 0;
  int64_t day = 0;

  void addTimeSpent(int timeSec)
  {
    timeSpentDaySec += timeSec;
    if (auto lb = Label::find("#timelb", this))
    {
      lb->removeChild("#spinner");
      lb->setCaption("[" + sec2str(timeSpentDaySec) + "]");
    }
  }

  IssuePanel(Widget* parent, IssueInfo::Ptr _issue, int _day)
    : Frame(parent, WidgetBoxLayout{ Orientation::Vertical, Alignment::Fill, 10, 10 },
            BorderColor { Color::transparent }, BackgroundColor{ Color::heavyDarkGrey },
            CornerRadius{ 6 }, WidgetId{ "#issue_" + _issue->id }),
      issue(_issue), day(_day)
  {
    hstack(2, 2,
           elm::Label { 
              WidgetId{ "#timelb" }, TextColor{ Color::grey }, FontSize{ 18 }, Caption{ "[00:00:00]" },
              Element<Spinner> {
                WidgetId{ "#spinner" }, SpinnerRadius{ 0.5f }, RelativeSize{ 1.f, 1.f },
                BackgroundColor{ Color::ligthDarkGrey }, IsSubElement{ true }, 
              }
           },
           elm::Label { 
             Caption{ issue->summary }, FontSize{ 18 }, TextColor{ Color::white }, TextWrapped{ true }
           });

    label(Caption{ issue->sprints.empty() ? "Not found projects" : issue->sprints.front() }, FontSize{ 14 });
  }
};

RTTI_IMPLEMENT_INFO(IssuePanel, Frame)

void updateRecordsIssueDay(std::string wId, IssueInfo::Ptr issue, std::tm date, int duration)
{
  std::string _idDayWidget = "#day_" + std::to_string(date.tm_mon+1) +
                             "_" + std::to_string(date.tm_mday+1);

  //found records scroll panel by wId
  if (auto vstack = Widget::find(wId.c_str()))
  {
    auto dayWidget = Widget::find(_idDayWidget.c_str(), vstack);
    if (!dayWidget)
    {
      dayWidget = &vstack->vstack(5, 0, WidgetId{ _idDayWidget });

      char datestr[100] = { 0 };
      std::strftime(datestr, sizeof(datestr), "%Y-%m-%d", &date);
      dayWidget->label(Caption{ datestr });
    }

    auto issueWidget = dayWidget->findWidget([issue](Widget* w) {
      if (auto ip = IssuePanel::cast(w))
        return ip->issue == issue;
      return false;
    }, false);

    if (!issueWidget)
    {
      int day = date.tm_mon * 100 + date.tm_mday;
      issueWidget = dayWidget->add<IssuePanel>(issue, day);
    }

    if (auto w = IssuePanel::cast(issueWidget))
    {
      w->addTimeSpent(duration * 60);
      Screen* scr = w->screen();
      scr->needPerformLayout(scr);
    }

    vstack->sortChildren([](Widget* w1, Widget* w2) {
      auto _i1 = w1->findFirst<IssuePanel>();
      auto _i2 = w2->findFirst<IssuePanel>();
      return (_i1 && _i2)
        ? (_i1->day > _i2->day)
        : false;
    });
  }
}

void requestIssueWorktime(IssueInfo::Ptr issue, std::string wId)
{
  SSLGet{ issue->getWorktimeUrl(), sslHeaders }
    .onResponse([issue, wId](int status, std::string body) {
      if (status != 200)
        return;
      Json::value response(body);
      Json::parse(response, body);

      response.update([issue, wId](auto& i) {
        int duration = (int)i.get("duration").get_int();
        std::time_t created = (uint64_t)i.get("created").get_int() / 1000;

        std::tm* tmt = std::localtime(&created);
        std::tm tm_date = *tmt, _tm = *tmt;
        makeDayInterval(tm_date, _tm);

        updateRecordsIssueDay(wId, issue, tm_date, duration);
        return true;
      });
    })
    .execute();
}

struct TasksWindow;
struct RecordsWindow : public UniqueWindow
{
  RecordsWindow()
    : UniqueWindow("#records_window", ShowHeader, WidgetStretchLayout{ Orientation::Vertical, 10, 10 })
  {
    hlayer(5, 2, FixedHeight{ 40 }, WidgetId{"#boards_records_layer"});
    auto hbutton = [](auto caption, auto color, auto hover, auto func) {
      if (auto layer = Widget::find("#boards_records_layer"))
      {
        layer->add(
          elm::Button{
            Caption{ caption }, DrawFlags{ Button::DrawCaption }, HoveredTextColor{ hover }, ButtonCallback{ func },
            Children{},
              elm::Line {
                         BackgroundColor{ color }, LineWidth{ 2 }, RelativeSize{ 0.9f, 0.f },
                         DrawFlags{ Line::Horizontal | Line::Bottom | Line::CenterH }
            }
        });
      };
    };

    hbutton("Boards", Color::grey, Color::red, [=] { elm::create<TasksWindow>(); });
    hbutton("Records", Color::red, Color::grey, [] {});

    WidgetId wId{ "#rec_vstack" };
    add(elm::VScrollPanel{ RelativeSize{ 1.f, 0.f },
             elm::VStack{ 5, 0, wId,
                 elm::Spinner{
                   SpinnerRadius{ 0.5f }, BackgroundColor{ Color::transparent },
                   FixedHeight{ width() / 2 }, RemoveAfterSec{ 10.f }
                 }
             }
        });
    std::string body = AllOf{ youytrack.filter_my_issues, 
                              "%20Board%20", Url::encodeQueryData(account.activeAgile), ":%7BCurrent%20sprint%7D" };
    SSLGet{ body, sslHeaders }
      .onResponse([wId](int status, std::string body) {
        if (status != 200)
          return;

        Json::value response;
        Json::parse(response, body);

        response.get("issue")
          .update([wId](auto& js) {
          auto issue = IssueInfo::fromJson(js);
          if (issue->spentTimeMin > 0)
            requestIssueWorktime(issue, wId.value);
          return true;
        });

        auto scr = elm::active_screen();
        scr->needPerformLayout(scr);
      })
      .execute();

    performLayoutLater();
  }
};

void openAgileUrl(const std::string& agile) {}
void createNewIssue() {}

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

  if (issue->recordTimeSec < 6)
  {
    account.setIssueRecord(issue, false);
    return;
  }

  std::string path = AllOf{ youytrack.rest_issues, issue->entityId, "/timetracking/workitem" };
  std::string payload = AllOf{ "<?xml version=\"1.0\" encoding=\"UTF-8\"?>",
                               "<workItem><date>", _s(issue->workStartTime), "</date>",
                                 "<duration>", _s(std::max<uint64_t>(issue->recordTimeSec / 60, 1)), "</duration>",
                                 "<description>added by Honsu</description>",
                                 "<worktype>",
                                    "<name>Development</name>",
                                 "</worktype>",
                               "</workItem>" };
  auto headers = httplib::Headers{
    { "Accept", "application/json" },
    { "Authorization", std::string("Bearer ") + account.token },
    { "Cache-Control", "no-cache" } };

  showRecPanelWait(true);
  SSLPost{path, payload, "application/xml", headers }
    .onResponse([issue](int status, std::string body) { showRecPanelWait(false); })
    .execute();

  account.setIssueRecord(issue, false);
}

void startIssueRecord(IssueInfo::Ptr issue)
{
  showRecPanelWait(true);
    
  SSLGet{ issue->getWorktimeUrl(), sslHeaders }
    .onResponse([issue](int status, std::string body) {
      showRecPanelWait(false);

      if (status != 200)
        return;

      Json::value response;
      Json::parse(response, body);

      std::time_t timet = std::time(nullptr);
      std::tm* tmt = std::localtime(&timet);
      std::tm tm_day_start = *tmt, tm_day_end = *tmt;
      makeDayInterval(tm_day_start, tm_day_end);
    
      //mktime convert tm to secs from epoch
      std::time_t current_time = mktime(tmt) * 1000;
      std::time_t day_start = mktime(&tm_day_start) * 1000;
      std::time_t day_end = mktime(&tm_day_end) * 1000;

      int timeSpent = 0;
      uint64_t timeSpentToday = 0;
      response.update([&] (auto& i) {
        int duration = (int)i.get("duration").get_int();
        int created = (int)i.get("created").get_int();
        if (created >= day_start && created <= day_end)
          timeSpentToday += duration;
        timeSpent += duration;
        return true;
      });
      issue->recordTimeSec = 0;
      issue->workStartTime = current_time;
      issue->recordTimeTodaySec = timeSpentToday * 60;

      account.setIssueRecord(issue, true);
    })
  .execute();
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

struct ActivityWithNoTaskWarning : public Window
{
  static const std::string Id;
  ActivityWithNoTaskWarning()
    : Window(nullptr, WidgetStretchLayout{ Orientation::Vertical, 10, 10 },
      Position{ 0, 0 }, FixedSize{ elm::active_screen()->size() },
      WindowMovable{ Theme::WindowDraggable::dgFixed }, WindowHaveHeader{ false },
      WidgetId{ Id })
  {
    account.suspendInactiveTime = true;
    showAppExclusive(true, true);

    vstack(10, 10,
           elm::Label{ 
             Caption{ "Long activity without task" },
             FontSize{ 42 }, TextColor{ Color::white }, CaptionHAlign{ TextHAlign::hCenter }
           },
           elm::Label{
             Caption{ "Are you read to work?" }, 
             FontSize{ 32 }, TextColor{ Color::yellow }, CaptionHAlign{ TextHAlign::hCenter }
           }
    );
    button(Caption{ "Take me a minute to select task" }, FontSize{ 32 }, DrawFlags{ Button::DrawBody | Button::DrawCaption },
      BackgroundColor{ Color::darkSeaGreen }, BackgroundHoverColor{ Color::darkGreen },
      ButtonChangeCallback{ [](Button* b) {
        account.nocheckActivityInteralSec = 1 * 60;
        account.lastCheckActivityTimeSec = 0;
        Window::cast(b->parent())->dispose();
        showAppExclusive(true, false);
      } }
    );
    button(Caption{ "Stop annoying me next 30 min" }, FontSize{ 32 }, DrawFlags{ Button::DrawBody | Button::DrawCaption },
      BackgroundColor{ Color::indianRed }, BackgroundHoverColor{ Color::red },
      ButtonChangeCallback{ [](Button* b) {
        account.nocheckActivityInteralSec = 30 * 60;
        account.lastCheckActivityTimeSec = 0;
        Window::cast(b->parent())->dispose();
        showAppExclusive(false, false);
      } }
    );
    line(LineWidth{ 4 }, BackgroundColor{ Color::red }, DrawFlags{ Line::Horizontal | Line::Top | Line::CenterH });
    performLayoutLater();
  }

  static void update()
  {
    if (account.lastCheckActivityTimeSec < account.nocheckActivityInteralSec)
      return;
    account.lastCheckActivityTimeSec = 0;
    if (auto w = Widget::find(Id.c_str())) {
      w->bringToFront();
      return;
    }
    if (account.issues.empty())
      return;
    if (account.getActiveIssue())
      return;
    elm::create<ActivityWithNoTaskWarning>();
  }
};
const std::string ActivityWithNoTaskWarning::Id = "#active_notask_warn";

struct InactiveWarning : public Window
{
  static const std::string Id;
  InactiveWarning()
    : Window(nullptr, WidgetStretchLayout{ Orientation::Vertical, 10, 10 },
             Position{ 0, 0 }, FixedSize{ elm::active_screen()->size() },
             WindowMovable{ Theme::WindowDraggable::dgFixed }, WindowHaveHeader{ false },
             WidgetId{ Id })
  {
    account.suspendInactiveTime = true;
    showAppExclusive(true, true);

    elm::BeginChildren{ this };
      elm::VStack{ 10, 10,
        elm::Label{ Caption{ "You were inactive" }, FontSize{ 48 }, TextColor{ Color::white }, CaptionHAlign{ TextHAlign::hCenter } },
        elm::Label{ Caption{ "What should I do with 00:00:00?" }, FontSize{ 32 }, TextColor{ Color::yellow }, CaptionHAlign{ TextHAlign::hCenter },
          OnUpdate{ [](Widget* w) {
            if ((int)account.inactiveTimeSec == (int)account.inactiveLastTimeSec)
              return;
            account.inactiveLastTimeSec = account.inactiveTimeSec;
            Label::cast(w)->setCaption("What should I do with " + sec2str(account.inactiveTimeSec) + "?");
          }
        }
      }};
      elm::Button{ Caption{ "Add" }, FontSize{ 36 }, DrawFlags{ Button::DrawBody | Button::DrawCaption },
        BackgroundColor{ Color::darkSeaGreen }, BackgroundHoverColor{ Color::darkGreen },
        ButtonChangeCallback{ [](Button* b) {
          account.resetInactiveTime();
          Window::cast(b->parent())->dispose();
          showAppExclusive(false, false);
        }}
      };
      elm::Button{ Caption{ "Remove" }, FontSize{ 36 }, DrawFlags{ Button::DrawBody | Button::DrawCaption },
        BackgroundColor{ Color::indianRed }, BackgroundHoverColor{ Color::red },
        ButtonChangeCallback{ [](Button* b) {
          if (auto issue = account.getActiveIssue())
          {
            issue->recordTimeSec -= account.inactiveTimeSec;
            issue->recordTimeTodaySec -= account.inactiveTimeSec;
            account.resetInactiveTime();
          }
          Window::cast(b->parent())->dispose();
          showAppExclusive(false, false);
        }}
      };
      elm::Line{ 
        LineWidth{ 4 }, BackgroundColor{ Color::red }, DrawFlags{ Line::Horizontal | Line::Top | Line::CenterH } 
      };
    elm::EndChildren{};
    performLayoutLater();
  }

  static void update() 
  {
    if (account.inactiveTimeSec < (5 * 60))
      return;
    if (auto w = Widget::find(Id.c_str()))
      return;
    elm::create<InactiveWarning>();
  }
};
const std::string InactiveWarning::Id = "#inactive_warn";

class TaskRecordPanel : public Frame
{
public:
  Label* time = nullptr;
  Label* dtime = nullptr;
  TaskRecordPanel()
    : Frame(nullptr, IsSubElement{ true }, WidgetBoxLayout{ Orientation::Vertical, Alignment::Fill, 10, 10 })
  {
    elm::BeginChildren{ this };
      elm::Line{ 
        LineWidth{ 4 }, IsSubElement{ true }, BackgroundColor{ Color::red }, DrawFlags{ Line::Horizontal | Line::Top | Line::Left }
      };

      elm::Widget{ WidgetStretchLayout{ Orientation::ReverseHorizontal },
        elm::Label{ WidgetId{"#txt"}, Caption{ "No task recording" }, FontSize{ 28 } },
        Element<TaskRecordButton>{ [] { return account.getActiveIssue(); } }
      };
  
      elm::HLayer{ 2, 2,
        elm::Label{
          WidgetId{ "#time" }, Caption{ "00:00:00" }, TextOffset{ 5, 0 },
          CaptionAlign{ TextHAlign::hLeft, TextVAlign::vBottom }, FontSize{ 28 }
        },
        elm::Label{
          WidgetId{ "#dtime" }, Caption{ "Today 00:00:00" }, TextOffset { 5, 0 },
          CaptionAlign{ TextHAlign::hRight, TextVAlign::vBottom }
        }
      };

      elm::Label{
        Caption{ "Records less 1 minute no send" }, CaptionHAlign{ TextHAlign::hCenter },
        BackgroundColor{ Color::red }, TextColor{ Color::white }, WidgetId{ "#warn" }
      };
    elm::EndChildren{};
    showRecPanelWait = [this](bool show) { showWaitNotification(show); };
  }

  void showWaitNotification(bool show)
  {
    auto sp = findWidget<Spinner>("#wait", true);
    if (show)
    {
      if (!sp)
        spinner(SpinnerRadius{ 0.5f }, BackgroundColor{ Color::ligthDarkGrey },
                WidgetId{ "#wait" }, IsSubElement{ true }, WidgetSize{ size() });
    }
    else if (sp)
      sp->removeLater();
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

    if (issue) mPos.y() = parent()->height() - height();
    else mPos.y() = parent()->height() - 4;
  
    setCaptionSafe("#time", sec2str(issue ? issue->recordTimeSec : 0));
    setCaptionSafe("#dtime", "TODAY:" + sec2str(issue ? issue->recordTimeTodaySec : 0));
    setCaptionSafe("#txt", issue ? issue->summary : "No task recording");

    if (auto lb = findWidget<Label>("#warn"))
      lb->setVisible(issue && issue->recordTimeSec < 60);

    Frame::afterDraw(ctx);
  }
};

class TaskPanel : public Frame 
{
public:
  IssueInfo::Ptr issue;

  TaskPanel(IssueInfo::Ptr _issue)
    : Frame(nullptr, 
            BorderColor{ Color::ligthDarkGrey }, BorderSize{ 2.f }, CornerRadius{ 6.f }, 
            BackgroundColor{ Color::heavyDarkGrey },
            WidgetBoxLayout{ Orientation::Vertical, Alignment::Fill, 10, 10}) 
  {
    issue = _issue;
    elm::BeginChildren{ this };
      elm::HLayer{ 2, 2, FixedHeight{ 26 },
        Children{},
          elm::Link{
            Caption{ issue->id }, TextColor{ Color::grey }, FontSize { 14 },
            ButtonCallback{ [this] { issue->openUrl(account.url); } }
          },
          elm::Link{
            Caption{ issue->state }, TextColor{ Color::grey }, FontSize{ 14 },
            WidgetCursor{ Cursor::Arrow }
          },
          elm::Widget {},
          Element<TaskRecordButton>{ [this] { return issue; }, [this] { return inFocusChain(); }}
      };

      elm::Label{ Caption{ issue->summary }, FontSize{ 22 }, TextColor{ Color::white }, TextWrapped{ true } };
      elm::Line{
        BackgroundColor{ Color::ligthDarkGrey }, LineWidth{ 6 },
        DrawFlags{ Line::Vertical | Line::CenterV | Line::Left }, IsSubElement{ true }
      };
    elm::EndChildren{};
  }
};

bool parseBoardTasks(std::string body)
{
  account.issues.clear();
  Json::value response;
  Json::parse(response, body);
  response
    .get("issue")
    .update([&](auto& i) { account.issues.push_back(IssueInfo::fromJson(i)); return true; });
  return !account.issues.empty();
}

void requestTasks(std::string board, std::function<void(std::string)> onSuccess,
                                     std::function<void(int)> onFailed)
{
  std::string body = AllOf{ youytrack.filter_my_issues,
                            "%20Board%20", Url::encodeQueryData(board), ":%7BCurrent%20sprint%7D",
                            "%20%23Unresolved%20" };

  SSLGet{ body, sslHeaders }
    .onResponse([=](int status, std::string body) { status == 200 ? onSuccess(body) : onFailed(status); })
    .execute();
}

struct TasksWindow : public UniqueWindow
{
  TasksWindow()
    : UniqueWindow("#tasks_window", ShowHeader, WidgetStretchLayout{ Orientation::Vertical, 10, 10 })
  {
    auto hbutton = [&](auto caption, auto color, auto hover, auto func) {
      elm::Button{
        Caption{ caption }, DrawFlags{ Button::DrawCaption }, HoveredTextColor{ hover }, ButtonCallback{ func },
        elm::Line{ BackgroundColor{ color }, LineWidth{ 2 }, RelativeSize{ 0.9f, 0.f },
        DrawFlags{ Line::Horizontal | Line::Bottom | Line::CenterH }
      }
      };
    };

    auto action = [&](int icon, auto func) {
      elm::ToolButton{
        Icon{ icon }, FixedWidth{ 40 },
        BackgroundColor{ Color::darkGrey }, BackgroundHoverColor{ Color::heavyDarkGrey },
        DrawFlags{ Button::DrawBody | Button::DrawIcon }, CornerRadius{ 4 }, ButtonCallback{ func }
      };
    };

    elm::BeginChildren{ this };
      elm::HLayer{ 5, 2, FixedHeight{ 40 }, 
                   FillChildren{ [&] {
                     hbutton("Boards", Color::red, Color::grey, [] {});
                     hbutton("Records", Color::grey, Color::red, [=] { elm::create<RecordsWindow>(); });
                   }}
      };


      elm::HLayer{ 2, 2, FixedHeight{ 40 },
        elm::Button{
          Caption{ account.activeAgile }, Icon{ ENTYPO_ICON_FORWARD_OUTLINE }, CaptionHAlign{ TextHAlign::hLeft },
          DrawFlags{ Button::DrawCaption | Button::DrawIcon }, IconAlignment{ IconAlign::Right },
          HoveredTextColor{ Color::lightGray }, ButtonCallback{ [] { openAgileUrl(account.activeAgile); } },
          Children{},
            elm::Line{ BackgroundColor{ Color::black }, DrawFlags{ Line::Horizontal | Line::Bottom | Line::CenterH }, IsSubElement{ true } }
        }
      };

      elm::HStack{ 5, 2, FixedHeight{ 40 },
                   FillChildren{ [&] {
                     action(ENTYPO_ICON_PLUS, [] { createNewIssue(); });
                     action(ENTYPO_ICON_CCW, [] {
                        WaitingWindow::create();
                        requestTasks(account.activeAgile,
                          [](std::string body) {
                            if (parseBoardTasks(body)) elm::create<TasksWindow>();
                            else elm::create<AgilesWindow>();  },
                          [](int) { elm::create<AgilesWindow>(); });
                      });
                     }}
      };

      elm::VScrollPanel{ RelativeSize{ 1.f, 0.f },
        elm::VStack{ 5, 0,
          FillChildren{ [] {
            for (auto& issue : account.issues)
              elm::create<TaskPanel>(issue);
          }}
        }
      };

      elm::create<TaskRecordPanel>();
    elm::EndChildren{};

    performLayoutLater();
  }
};

struct AgilesWindow : public UniqueWindow
{
  AgilesWindow()
    : UniqueWindow("#agiles_window", ShowHeader, WidgetStretchLayout{ Orientation::Vertical, 10, 10 })
  {
    elm::BeginChildren{ this };
      elm::VStack{ 2, 2, FixedHeight{ 80 },
        elm::Label{ FixedHeight{ 60 }, Caption{ "Select agile boards" },
                    TextColor{ Color::white }, CaptionAlign{ hCenter, vBottom },
                    CaptionFont{ "sans-bold" }, FontSize{ 42 } },
        elm::Label{ Caption{ "You will be able to edit this selection later" },
                    CaptionAlign{ hCenter, vTop }, FixedHeight{ 20 } }
      };

      elm::VScrollPanel{ RelativeSize{ 1.f, 0.f },
        elm::VStack{ 10, 10, WidgetId{ "#agiles_board" },
          FillChildren { [] {
            for (auto& i : account.agiles)
            {
              elm::Button{ Caption{ i.name }, CaptionFont{ "sans" }, FixedHeight{ 50 },
                           ButtonFlags{ Button::RadioButton },
                           FontSize{ 24 }, Icon{ ENTYPO_ICON_OK }, IconColor{ Color::dimGrey }, IconPushedColor{ Color::aquamarine },
                           BorderColor{ Color::dimGrey }, BorderSize{ 2.f },
                           BackgroundColor{ Color::transparent }, BackgroundHoverColor{ Color::transparent },
                           ButtonCallback{ [name = i.name]{ account.activeAgile = name; } }
              };
            }
          }},
          elm::Button { Caption{ "Save" }, FixedHeight{ 50 },
              BorderSize{ 0 }, BackgroundColor{ Color::indianRed },
              BackgroundHoverColor{ Color::caesarRed },
              ButtonCallback{ [] {
                WaitingWindow::create();
                requestTasks(account.activeAgile,
                  // success
                  [](std::string body) {
                    if (parseBoardTasks(body)) elm::create<TasksWindow>();
                    else elm::create<AgilesWindow>();
                  },
                  // failed
                  [](int) { elm::create<AgilesWindow>(); });
            } }
          }
      }};

    elm::EndChildren{};
    performLayoutLater();
  }
};

void requestAgiles(std::function<void (std::string)> onSuccess, std::function<void (int)> onFailed)
{
  SSLGet{ "/api/agiles?fields=name,id,projects(id,shortName,name),columnSettings(columns(presentation))&$top=100", sslHeaders }
    .onResponse([onSuccess,onFailed](int status, std::string body) {
      if (status == 200)
        onSuccess(body);
      else
        onFailed(status);
    })
    .execute();
}

#define CA_CERT_FILE "./ca-bundle.crt"
void createSSLClient() 
{
  if (sslClient)
    return;

  Url url(account.url);
  account.path = url.path;
  sslClient = new httplib::SSLClient(url.host);

  //sslClient->set_ca_cert_path(CA_CERT_FILE);
  //sslClient->enable_server_certificate_verification(true);

  sslHeaders = httplib::Headers{
    { "Accept", "application/json" },
    { "Authorization", std::string("Bearer ") + account.token },
    { "Cache-Control", "no-cache" },
    { "Content-Type", "application/json" }
  };
}

bool parseAgilesData(std::string data)
{
  account.agiles.clear();

  Json::value agiles;
  Json::parse(agiles, data);

  agiles.update([&](auto& js) {
    AgileInfo agileInfo(js);
    if (agileInfo.valid())
      account.agiles.push_back(agileInfo);
    return true;
  });

  return !account.agiles.empty();
}

bool update_requests()
{
  createSSLClient();

  if (!SSLGet::requests.empty())
  {
    SSLGet r = SSLGet::requests.front();
    SSLGet::requests.erase(SSLGet::requests.begin());

    std::string path = account.path + r.path;
    auto nn = sslClient->Get( path.c_str(), r.headers);

    auto answer = r.answer;
    answer(nn ? nn->status : -1, nn ? nn->body : "");
  }

  if (!SSLPost::requests.empty() && (getTimeFromStart() > SSLPost::invalidRequestTime))
  {
    const SSLPost& r = SSLPost::requests.front();

    std::string path = account.path + r.path;
    auto nn = sslClient->Post( path.c_str(), r.headers, r.body, r.content_type.c_str());

    if (nn && nn->status == 201)
    {
      auto answer = r.answer;
      answer(nn ? nn->status : -1, nn ? nn->body : "");
      SSLPost::requests.erase(SSLPost::requests.begin());
      SSLPost::invalidRequestSent = 0;
    }
    else
    {
      SSLPost::invalidRequestSent++;
      if (SSLPost::invalidRequestSent > 10)
        SSLPost::requests.erase(SSLPost::requests.begin());
      else
        SSLPost::invalidRequestTime = getTimeFromStart() + 2.f;
    }
  }
  return !(SSLGet::requests.empty() || SSLPost::requests.empty());
}

void requestAdminData();
struct LoginWindow : public UniqueWindow
{
  LoginWindow()
    : UniqueWindow("#login_window", UniqueWindow::ShowHeader, WidgetBoxLayout{ Orientation::Vertical, Alignment::Fill, 20, 20 })
  {
    auto textfield = [&](std::string placeholder, std::string value, std::string id) {
      elm::Textbox{
        FontSize{ 24 }, IsEditable{ true }, TextAlignment::Left, TextPlaceholder{ placeholder },
        TextValue{ value }, BorderColor{ Color::dimGrey }, BorderSize{ 2.f }, BackgroundHoverColor{ Color::transparent },
        BackgroundColor{ Color::transparent }, WidgetId{ id }
      };
    };

    elm::BeginChildren{ this };
      elm::Label{ FixedHeight{ 100 }, Caption{ "Add new account" }, CaptionHAlign{ TextHAlign::hCenter }, FontSize{ 26 }};

      /* No need to store a pointer, the data structure will be automatically
      freed when the parent window is deleted */
      textfield("account name", account.title, "#account_name");
      textfield("youtrack url", account.url, "#youtrack_url");
      textfield("youtrack token", account.token, "#youtrack_token");

      elm::Link {
         Caption{ "How to obtain a new permament token?" },
         ButtonCallback{ [] { open_url("https://www.jetbrains.com/help/youtrack/standalone/Manage-Permanent-Token.html", ""); } }
      };

    /* Alternative construction notation using variadic template */
      elm::Button{
        Caption{ "Login" }, FontSize{ 32 },
        BackgroundColor{ 0, 0, 255, 25 },
        ButtonCallback{ [] { requestAdminData(); } }
      };
    elm::EndChildren{};
    performLayoutLater();
  }
};

void requestAdminData()
{
  if (   !checkAccountUrl(TextBox::find("#youtrack_url"))
      || !checkAccountToken(TextBox::find("#youtrack_token")) )
  {
    elm::create<LoginWindow>();
    return;
  }

  account.save();
  WaitingWindow::create();

  SSLGet{ "/api/admin/users/me?fields=id,login,name,email", sslHeaders }
    .onResponse([](int status, std::string body) {
      if (status != 200) {
        elm::create<LoginWindow>();
        return;
      }

      requestAgiles(
        //success
        [](std::string body) { if (parseAgilesData(body)) elm::create<AgilesWindow>(); }, 
        //failed
        [](int status) { account.agiles.clear(); elm::create<LoginWindow>(); }
      );
  })
    .execute();
}

class HonsuScreen : public Screen {
public:
    HonsuScreen(sample::WindowHandle hw, const Vector2i& size, const std::string& caption) : Screen(size, caption, false) 
    {
      hwindow = hw;
      initGPUTimer(&gpuTimer);

      account.load();
      elm::create<LoginWindow>();

      //fpsGraph = &wdg<PerfGraph>(GRAPH_RENDER_FPS, "Frame Time", Vector2i(width() - 210, 5));
      cpuGraph = &wdg<PerfGraph>(GRAPH_RENDER_MS, "CPU Time", Vector2i(width() - 205, 10));

      previousFrameTime = getTimeFromStart();

      needPerformLayout(this);

      theme()->keyboardNavigation = false;
      //theme()->debugHighlightMouseover = true;
    }

    bool cursorPosCallbackEvent(double x, double y) override
    {
      if (mDrag)
      {
        Vector2i currentPos = sample::get_cursor_pos();
        sample::set_window_pos(hwindow, mHwPos + (currentPos - mCursorPos));
      }
      return Screen::cursorPosCallbackEvent(x, y);
    }

    bool mouseButtonCallbackEvent(int button, int action, int modifiers) override
    {
      mDrag = (isMouseActionPress(action) && mMousePos.y() < 60);
      if (mDrag)
      {
        mCursorPos = sample::get_cursor_pos();
        mHwPos = sample::get_window_pos(hwindow);
      }
      return Screen::mouseButtonCallbackEvent(button, action, modifiers);
    }

    bool keyboardEvent(int key, int scancode, int action, int modifiers) override {
        if (Screen::keyboardEvent(key, scancode, action, modifiers))
            return true;
        return false;
    }

    virtual void draw(NVGcontext *ctx) 
    {
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

      InactiveWarning::update();
      ActivityWithNoTaskWarning::update();
    }

private:
    PerfGraph *fpsGraph = nullptr;
    PerfGraph *cpuGraph = nullptr;
    sample::WindowHandle hwindow;
    bool mDrag = false;
    Vector2i mCursorPos, mHwPos;
    double previousFrameTime = 0, cpuTime = 0;
};

int main(int /* argc */, char ** /* argv */) 
{
  nanogui::init();
  Vector2i size{ 400, 600 };
  auto window = nanogui::sample::create_window(size.x(), size.y(), "Honsu", true, false, false);
  showAppExclusive = [window](bool v, bool always) { sample::set_window_topmost(window, v, always); };
  nanogui::sample::create_context();

  {
    HonsuScreen screen(window, size, "");
    nanogui::sample::setup_window_params(window, &screen);
    screen.setVisible(true);
    screen.performLayout();
    
    bool requests_thread_active = true;
    auto requests_thread = std::thread([&] {
      while (requests_thread_active)
      {
        bool haveRequests = update_requests();
        if (!haveRequests)
          std::this_thread::sleep_for(0.5s);
      }
    });

    auto user_inactive_thread = std::thread([&] {
      Vector2i lastMousePos = sample::get_cursor_pos();
      while (requests_thread_active) {
        if (account.getActiveIssue())
        {
          Vector2i currentMousePos = sample::get_cursor_pos();
          if (abs(lastMousePos.x() - currentMousePos.x()) < 3
            && abs(lastMousePos.y() - currentMousePos.y() < 3))
          {
            account.inactiveTimeSec += 1.f;
          }
          else
          {
            if (!account.suspendInactiveTime)
            {
              account.inactiveTimeSec = 0.f;
              account.inactiveLastTimeSec = 0.f;
            }
          }
          lastMousePos = currentMousePos;
        }

        std::this_thread::sleep_for(1.0s);
      }
    });

    auto timerec_thread = std::thread([&] {
      int lasttime = getTimeFromStart();
      while (requests_thread_active)
      {
        for (auto& issue : account.issues)
          issue->updateTime(0.5);
        account.lastCheckActivityTimeSec += 0.5f;
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
    user_inactive_thread.join();
    nanogui::sample::poll_events();
  }

  nanogui::sample::destroy_window(window);
  nanogui::shutdown();
  return 0;
}
