/*
    src/example1.cpp -- C++ version of an example application that shows
    how to use the various widget classes. For a Python implementation, see
    '../python/example1.py'.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
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
#include <nanogui/splitter.h>
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

void createButtonDemoWindow()
{
  elm::BeginWindow{ Position{ 15, 15 },
                    WindowGroupLayout{},
                    Caption{ "Button demo" },
                    WindowMovable{ Theme::WindowDraggable::dgFixed } };

    elm::Label{ Caption{ "Push buttons" }, CaptionFont{ "sans-bold" } };
    elm::Button{ ButtonCallback{ [] { cout << "pushed!" << endl; } }, Caption{ "Plain button" }, TooltipText{ "short tooltip" } };
    elm::Splitter{ Orientation::Horizontal };
    /* Alternative construction notation using variadic template */
    elm::Button{ Caption{ "Styled" }, Icon{ ENTYPO_ICON_ROCKET }, BackgroundColor{ 0, 0, 255, 25 },
                 ButtonCallback{ [] { cout << "pushed!" << endl; } },
                 TooltipText{ "This button has a fairly long tooltip. It is so long, in "
                              "fact, that the shown text will span several lines." } };

    elm::Label{ Caption{ "Toggle buttons" }, CaptionFont{ "sans-bold" } };
    elm::Button { 
        Caption{ "Toggle me" }, ButtonFlags{ Button::ToggleButton },
        ButtonChangeCallback{ [](Button* b) { cout << "Toggle button state: " << b->pushed() << endl; } } 
    };

    elm::Label{ Caption{ "Radio buttons" }, CaptionFont{ "sans-bold" } };
    elm::Button{ Caption{ "Radio button 1" }, ButtonFlags{ Button::RadioButton } };
    elm::Button{ Caption{ "Radio button 2" }, ButtonFlags{ Button::RadioButton } };

    elm::Label{ "A tool palette", "sans-bold" };
    elm::Widget{ WidgetBoxLayout{ Orientation::Horizontal, Alignment::Middle, 0, 6 },
                 Children{},
                   elm::ToolButton{ Icon{ ENTYPO_ICON_CLOUD } },
                   elm::ToolButton{ Icon{ ENTYPO_ICON_FAST_FORWARD } },
                   elm::ToolButton{ Icon{ ENTYPO_ICON_VOLUME_UP } },
                   elm::ToolButton{ Icon{ ENTYPO_ICON_INSTALL } }
    };

    elm::Label{ Caption{ "Popup buttons" }, CaptionFont{ "sans-bold" } };

    elm::PopupButton { 
      Caption{ "Popup" }, Icon{ ENTYPO_ICON_EXPORT },
      PopupWidget<> { WidgetGroupLayout{},
        elm::Label{ "Arbitrary widgets can be placed here" },
        elm::CheckBox{ Caption{ "A check box" } },
        // popup right
        elm::PopupButton {
          Caption{ "Recursive popup" }, Icon{ ENTYPO_ICON_FLASH },
          PopupWidget<> { WidgetGroupLayout{}, 
            elm::CheckBox{ Caption{ "Another check box" } }
          }
        },
        // popup left
        elm::PopupButton {
          Caption{ "Recursive popup" }, Icon{ ENTYPO_ICON_FLASH }, PopupSide{ Popup::Side::Left },
          PopupWidget<> { WidgetGroupLayout{}, 
            elm::CheckBox{ Caption{ "Another check box" } }
          }
        }
      } 
    };

    elm::Label{ Caption{ "A switch boxes" }, CaptionFont{ "sans-bold" } };
    elm::Widget{ WidgetGridLayout{ Orientation::Horizontal, 2 },
                 Children{},
                   elm::Switchbox{ FixedSize{ 80, 30 } },
                   elm::Switchbox{ FixedSize{ 28, 60 }, SwitchboxAlign{ (int)SwitchBox::Alignment::Vertical }, BackgroundColor{ 0x0000c0ff } },
                   elm::Switchbox{ FixedSize{ 60, 25 }, BackgroundColor{ 0x0000c0ff }, CheckedColor{ 0x00c000ff }, UncheckedColor { 0xff0000ff } }
    };

    elm::Label{ Caption{ "A led buttons" }, CaptionFont{ "sans-bold" } };
    elm::Widget{ WidgetGridLayout{ Orientation::Horizontal, 4 },
                 Children{},
                   Element<LedButton>{ LedButton::circleBlack, 30, 30 },
                   Element<LedButton>{ LedButton::circleBlue, 30, 30 },
                   Element<LedButton>{ LedButton::circleGreen, 30, 30 },
                   Element<LedButton>{ LedButton::circleGray, 30, 30 },
                   Element<LedButton>{ LedButton::circleOrange, 30, 30 },
                   Element<LedButton>{ LedButton::circleRed, 30, 30 },
                   Element<LedButton>{ LedButton::circleYellow, 30, 30 },
                   Element<LedButton>{ LedButton::circlePurple, 30, 30 }
    };
  elm::EndWindow{};
}

using ImagesDataType = std::vector<pair<int, std::string>>;

void createBasicWidgets()
{
#if defined(_WIN32)
  string resourcesFolderPath("../resources/");
#else
  string resourcesFolderPath("./");
#endif
  static vector<pair<int, string>> icons = loadImageDirectory(elm::active_screen()->nvgContext(), "icons");
  static ImagesDataType mImagesData;

  // Load all of the images by creating a GLTexture object and saving the pixel data.
  for (auto& icon : icons) {
    auto fullpath = resourcesFolderPath + icon.second;
    auto data = nvgCreateImage(elm::active_screen()->nvgContext(), fullpath.c_str(), 0);
    mImagesData.emplace_back(data, fullpath);
  }

  elm::BeginWindow{ Caption{ "Basic widgets" }, Position{ 200, 15 },
                    WidgetId{ "#basic_widgets_wnd"}, WidgetGroupLayout{} };

    elm::Label{ "Searchboxes", "sans-bold" };
    Element<SearchBox>();

    elm::Label{ "Message dialog", "sans-bold" };
    elm::Widget{ WidgetBoxLayout{ Orientation::Horizontal, Alignment::Middle, 0, 6 },
           Element<Button>{
              Caption{ "Info" },
              ButtonCallback { [=] {
                 elm::MessageDialog(DialogTitle{ "Title" },
                                    DialogMessage{ "This is an information message" },
                                    DialogResult{ [](int result) { cout << "Dialog result: " << result << endl; }});
               }}
           },
           Element<Button>{
              Caption{ "Warn" },
              ButtonCallback{ [=] {
                 elm::MessageDialog(DialogType{ (int)MessageDialog::Type::Warning },
                                   DialogTitle{ "Title" },
                                   DialogMessage{ "This is a warning message" },
                                   DialogResult{ [](int result) { cout << "Dialog result: " << result << endl; }});
              }}
           },
           Element<Button>{
              Caption{ "Ask" },
              ButtonCallback{ [=] {
                 elm::MessageDialog(DialogType{ (int)MessageDialog::Type::Question },
                                   DialogTitle{ "Title" },
                                   DialogMessage{ "This is a question message" },
                                   DialogButton{ "Yes" }, DialogAltButton{ "No" },
                                   DialogResult{ [](int result) { cout << "Dialog result: " << result << endl; }});
              }}
           }
    };
    elm::Widget{ WidgetBoxLayout{ Orientation::Horizontal, Alignment::Middle, 0, 6 },
          Element<Button> {
            Caption{ "Notification" },
            ButtonCallback{ [=]() {
              Element<InAppNotification>( DialogMessage{ "This is an notification message" },
                DialogButton{ "Yes" }, DialogAltButton{ "No" },
                DialogResult{ [](int result) { std::cout << "Notification result: " << result << std::endl; } });
            }}
          }
    };

    //need for change texture
    static int currentImage = 0;

    elm::Label{ Caption{"Image panel & scroll panel"}, CaptionFont{"sans-bold"} };
    elm::PopupButton{ 
      Caption{ "Image Panel" }, Icon{ENTYPO_ICON_FOLDER},
      PopupWidget<>{ FixedSize{ 245, 150 },
        elm::VScrollPanel{
          elm::ImagePanel{ ImagePanelIcons{ icons },
            ImagePamelCallback{ [&](int i) {
              if (auto imageView = ImageView::find("#image_view"))
              {
                imageView->bindImage(mImagesData[i].first);
                currentImage = i;
                cout << "Selected item " << i << '\n';
              }
            }}
      }}}
    };

    elm::Label{ Caption{ "File dialog" }, CaptionFont{ "sans-bold" } };

    elm::Widget{
      WidgetBoxLayout{ Orientation::Horizontal, Alignment::Middle, 0, 6 },
      elm::Button{ Caption{ "Open" }, ButtonCallback{ [&] {
          cout << "File dialog result: "
            << file_dialog({ { "png", "Portable Network Graphics" },{ "txt", "Text file" } }, false)
            << endl;
        }}
      },
      elm::Button{ Caption{ "Save" }, ButtonCallback{ [&] {
          cout << "File dialog result: "
            << file_dialog({ { "png", "Portable Network Graphics" },{ "txt", "Text file" } }, true)
            << endl;
        }}
      }
    };

    elm::Label{ Caption{ "Combo box" }, CaptionFont{ "sans-bold" } };
    elm::DropdownBox{ DropdownBoxItems{ "Dropdown item 1", "Dropdown item 2", "Dropdown item 3" }, WidgetId{ "1" } };
    elm::ComboBox{ ComboBoxItems{ "Combo box item 1", "Combo box item 2", "Combo box item 3" }, WidgetId{ "2" } };
    elm::DropdownBox {
      DropdownBoxFill{ [](string& r) { static char i = '1'; r = std::string(5, i); return i++ < '9';  } },
      ItemHeight{ 18 }
    };

    elm::Label{ Caption{ "Check box" }, CaptionFont{ "sans-bold" } };
    elm::CheckBox{ Caption{ "Flag 1" }, CheckboxState{ true },
      CheckboxCallback{ [](bool state) { cout << "Check box 1 state: " << state << endl; } },
      UncheckedColor{ 0xC00000ff }, CheckedColor{ 0x00c000ff }, PushedColor{ 0xc0c000ff } 
    };

    elm::CheckBox{ Caption{ "Flag 2" }, CheckboxCallback{ [](bool state) { cout << "Check box 2 state: " << state << endl; }} };
    elm::Label{ Caption{ "Progress bar" }, CaptionFont{ "sans-bold" } };

    elm::Widget{ WidgetBoxLayout{ Orientation::Horizontal, Alignment::Middle, 0, 6 },
      Children{},
        Element<ProgressBar>{ WidgetId{ "#lineprogressbar" }},
        Element<CircleProgressBar>{ WidgetId{ "#circleprogressbar" }, FixedSize{ 40, 40 } }
    };

    elm::Label(Caption{ "Slider and text box" }, CaptionFont{ "sans-bold" });

    FloatObservable sliderValue(0.5f);
    elm::Widget{ WidgetBoxLayout{ Orientation::Horizontal, Alignment::Middle, 0, 20 },
      elm::Textbox{ FixedSize{ 60, 25 }, FontSize{ 20 },
        TextAlignment::Right,
        TextBoxUnits{ "%" }, TextValue{ "50" },
        TextBoxUpdateCallback{ [sliderValue](TextBox* tb) {
          static int lastValue = 0;
          if (lastValue != (int)(sliderValue * 100)) {
            lastValue = (int)(sliderValue * 100);
            tb->setValue(std::to_string(lastValue));
          }
        }}
      },
      elm::Slider{ SliderObservable{ sliderValue }, FixedWidth{ 80 } }
    };

    elm::Label{ Caption{ "Spinners" }, CaptionFont{ "sans-bold" } };
    elm::Widget{ WidgetBoxLayout{ Orientation::Horizontal, Alignment::Middle, 0, 6 },
                 elm::Spinner{ FixedSize{ 40, 40 } },
                 elm::Spinner{ SpinnerSpeed{ 0.5f }, FixedSize{ 40, 40 } },
                 elm::Spinner{ SpinnerSpeed{ -0.7f }, FixedSize{ 40, 40 } }
    };

    elm::Label{ Caption{ "Dial and text box" }, CaptionFont{ "sans-bold" } };

    elm::BeginWidget<Widget>{ WidgetBoxLayout{ Orientation::Horizontal, Alignment::Middle, 0, 20 } };
      elm::Dial{ FloatValue{ 0.01f }, FixedWidth{ 80 }, WidgetId{"#dial"},
        DialCallback{ [&](float value) {
          value = 0.01f + 99.99f*powf(value, 5.0f);
          std::ostringstream sval;
          sval.precision(2);
          sval << std::fixed << value;
          if (auto tb = TextBox::find("#dial_textbox"))
            tb->setValue(sval.str());
        }},
        DialFinalCallback { [&](float value) {
          if (auto dial = Dial::find("#dial"))
            dial->setHighlightedRange(std::pair<float, float>(0.0f, value));
          value = 0.01f + 99.99f*powf(value, 5.0f);
          cout << "Final dial value: " << value << endl;
        }}
      };
        
      elm::Textbox{ FixedSize{ 60, 25 }, TextValue{ "0.01" },  WidgetId{ "#dial_textbox" }, TextAlignment::Right };

    elm::EndWidget{};
  elm::EndWindow{};


  elm::BeginWindow{ Caption{ "Selected image" }, Position{ 710, 15 }, WidgetGroupLayout{} };

  // Set the first texture
  elm::ImageView{ InitiailImage{ (uint32_t)mImagesData[0].first },
                  ImageViewGridTreshold{ 20 },
                  FixedSize{ 220,220 },
                  WidgetId{ "#image_view" },
                  PixelInfoThreshold{ 20 }
  };

  elm::EndWindow{};
}

void createTextAreaWindow()
{
  elm::BeginWindow{ Caption{"TextArea window"}, FixedSize{ 300, 200 },
                    WidgetStretchLayout{ Orientation::Horizontal }, Position{ 1015, 405 } };
    std::string longText = "void createTextAreaWindow(Sreen* screen) {\n"
      "    auto& mw = screen->window(Caption{ \"TextArea window\" }\n"
      "                              FixedSize{ 300, 200 },\n"
      "                              WidgetStretchLayout{ Orientation::Horizontal },\n"
      "                              Position{ 1015, 405 });\n"
      "    std::string longText = \"\";\n"
      "    auto& area = mw.wdg<TextArea>(RelativeSize{ 1.f, 1.f }, Text{ longText }); }";
    elm::TextArea{ RelativeSize{ 1.f, 1.f }, LongText{ longText }};
  elm::EndWindow{};
}

void createPicflowWindow()
{
#if defined(_WIN32)
  string resourcesFolderPath("../resources/");
#else
  string resourcesFolderPath("./");
#endif
  elm::BeginWindow{ Caption{ "Picflow" }, FixedSize{ 400, 250 },
                    WidgetStretchLayout{ Orientation::Horizontal }, Position{ 715, 305 } };

    static vector<pair<int, string>> icons = loadImageDirectory(elm::active_screen()->nvgContext(), "icons");
    static ImagesDataType picflowImagesData;

    // Load all of the images by creating a GLTexture object and saving the pixel data.
     Element<Picflow>{ PicflowImageSize{ 0.35f, 0.35f },
                       PicflowFill { 
                        [&] (Widget* w) {
                          if (auto flow = Picflow::cast(w))
                            for (auto& icon : icons) {
                              auto fullpath = resourcesFolderPath + icon.second;
                              auto data = nvgCreateImage(elm::active_screen()->nvgContext(), fullpath.c_str(), 0);
                              picflowImagesData.emplace_back(data, fullpath);
                              flow->addItem(data);
                            }                              
                        }
                      }
    };
  elm::EndWindow{};
}

void createMiscWidgets(Screen* screen)
{
  auto& mw = screen->window(Caption{ "Misc. widgets" },
                            Position{ 425, 15 });
  mw.withLayout<GroupLayout>();

  auto& tabWidget = mw.tabs();
  tabWidget.setId("#tabs");

  auto& layer = *tabWidget.createTab("Color Wheel");
  layer.withLayout<GroupLayout>();

  // Use overloaded variadic add to fill the tab widget with Different tabs.
  layer.label("Color wheel widget", "sans-bold");
  layer.wdg<ColorWheel>();

  auto& layer2 = *tabWidget.createTab("Function Graph");
  layer2.withLayout<GroupLayout>();
  layer2.label("Function graph widget", "sans-bold");

  auto& graph = layer2.graph("Some Function");
  graph.setHeader("E = 2.35e-3");
  graph.setFooter("Iteration 89");

  VectorXf &func = graph.values();
  func.resize(100);
  for (int i = 0; i < 100; ++i)
    func[i] = 0.5f * (0.5f * std::sin(i / 10.f) +
      0.5f * std::cos(i / 23.f) + 1);

  // Dummy tab used to represent the last tab button.
  tabWidget.createTab("+");

  // A simple counter.
  int counter = 1;
  tabWidget.setCallback([screen, &counter](int index) mutable {
    auto& tabs = *screen->findWidget<TabWidget>("#tabs");
    if (index == (tabs.tabCount() - 1)) {
      // When the "+" tab has been clicked, simply add a new tab.
      string tabName = "Dynamic " + to_string(counter);
      auto& layerDyn = *tabs.createTab(index, tabName);
      layerDyn.withLayout<GroupLayout>();
      layerDyn.label("Function graph widget", "sans-bold");

      auto& graphDyn = layerDyn.graph("Dynamic function");

      graphDyn.setHeader("E = 2.35e-3");
      graphDyn.setFooter("Iteration " + to_string(index*counter));
      VectorXf &funcDyn = graphDyn.values();
      funcDyn.resize(100);
      for (int i = 0; i < 100; ++i)
        funcDyn[i] = 0.5f *
        std::abs((0.5f * std::sin(i / 10.f + counter) +
          0.5f * std::cos(i / 23.f + 1 + counter)));
      ++counter;
      // We must invoke perform layout from the screen instance to keep everything in order.
      // This is essential when creating tabs dynamically.
      screen->performLayout();
      // Ensure that the newly added header is visible on screen
      tabs.ensureTabVisible(index);
    }
  });
  tabWidget.setActiveTab(0);

  // A button to go back to the first tab and scroll the window.
  auto& panelJump = mw.widget();
  panelJump.boxlayout(Orientation::Horizontal, Alignment::Middle, 0, 6);
  panelJump.label("Jump to tab: ");

  auto& ib = panelJump.intbox<int>(IsEditable{ true },
                                   FixedHeight{ 22 });

  /*auto& bf = */panelJump.button(Caption{ "" },
                              Icon{ ENTYPO_ICON_FORWARD },
                              FixedSize{ 22, 22 },
                              ButtonCallback{ [&] {
                                int value = ib.value();
                                if (value >= 0 && value < tabWidget.tabCount()) {
                                  tabWidget.setActiveTab(value);
                                  tabWidget.ensureTabVisible(value);
                                }
                              }});

  auto& led = mw.wdg<LedMatrix>();
  led.setId("#led");
  led.setFixedSize({ mw.width(), 40 });
  led.setRowCount(8);
}

void createGridSmallObjects(Screen* screen)
{
  auto& w = screen->window(Caption{ "Grid of small widgets" },
                           Position{ 425, 300 },
                           WidgetGridLayout{ LayoutMargin { 15 }, 
                                             LayoutHorSpacing { 5 },
                                             ColumnsAligment { Alignment::Maximum, Alignment::Fill }  });

  /* FP widget */
  w.label("Floating point :", "sans-bold");
  auto& textBox = w.textbox();
  textBox.setEditable(true);
  textBox.setFixedSize(Vector2i(100, 20));
  textBox.setValue("50");
  textBox.setUnits("GiB");
  textBox.setDefaultValue("0.0");
  textBox.setFontSize(16);
  textBox.setFormat("[-]?[0-9]*\\.?[0-9]+");

  /* Positive integer widget */
  w.label("Positive integer :", "sans-bold");
  auto& intBox = w.intbox<int>();
  intBox.setEditable(true);
  intBox.setFixedSize(Vector2i(100, 20));
  intBox.setValue(50);
  intBox.setUnits("Mhz");
  intBox.setDefaultValue("0");
  intBox.setFontSize(16);
  intBox.setFormat("[1-9][0-9]*");
  intBox.setSpinnable(true);
  intBox.setMinValue(1);
  intBox.setValueIncrement(2);

  /* Checkbox widget */
  w.label("Checkbox :", "sans-bold");
  w.checkbox(Caption{ "Check me" }, CheckboxState{ true }, FontSize{ 16 });

  w.label("Combo box :", "sans-bold");
  w.combobox(ComboBoxItems{ "Item 1", "Item 2", "Item 3" },
             FontSize{ 16 },
             FixedSize{ 100, 20 });

  w.label("Color picker :", "sans-bold");
  auto& cp = w.wdg<ColorPicker>(Color{ 255, 120, 0, 255 });
  cp.setFixedSize({ 100, 20 });
  cp.setFinalCallback([](const Color &c) {
    std::cout << "ColorPicker Final Callback: ["
      << c.r() << ", "
      << c.g() << ", "
      << c.b() << ", "
      << c.w() << "]" << std::endl;
  });

  auto& cpw = screen->window("Color Picker Fast Callback");
  auto* layout = new GridLayout(Orientation::Horizontal, 2, Alignment::Middle, 15, 5);
  layout->setColAlignment({ Alignment::Maximum, Alignment::Fill });
  layout->setSpacing(0, 10);
  cpw.setLayout(layout);
  cpw.setPosition(425, 500);

  cpw.label("Combined: ");
  auto& btn = cpw.button(Caption{ "ColorWheel" }, Icon{ ENTYPO_ICON_500PX });
  cpw.label("Red: ");
  auto& re = cpw.intbox<int>(IsEditable{ false });
  cpw.label("Green: ");
  auto& ge = cpw.intbox<int>(IsEditable{ false });
  cpw.label("Blue: ");
  auto& be = cpw.intbox<int>(IsEditable{ false });
  cpw.label("Alpha: ");
  auto& ae = cpw.intbox<int>(IsEditable{ false });

  cp.setCallback([&](const Color &c) {
    btn.setBackgroundColor(c);
    btn.setTextColor(c.contrastingColor());
    re.setValue((int)(c.r() * 255.0f));
    ge.setValue((int)(c.g() * 255.0f));
    be.setValue((int)(c.b() * 255.0f));
    ae.setValue((int)(c.w() * 255.0f));
  });
}

void createMeter(Screen* screen)
{
  auto& meter = screen->wdg<Meter>();
  meter.setFixedSize({ 160, 160 });
  meter.setPosition(screen->width() - 165, screen->height() - 165);
  meter.setId("#meter");
}

void toggleMainMenu(Screen* screen, bool show)
{
  using namespace nanogui;

  auto menus = screen->findAll<WindowMenu>();

  if (menus.empty())
  {
    auto& mmenu = screen->wdg<WindowMenu>();
    mmenu.activate({ 0, 0 });
    auto dlg = [screen](std::string title) { screen->msgdialog(DialogTitle{ title }, DialogMessage{ "New Clicked" }); };
    mmenu.submenu("File")
      .item("New", [dlg]() { dlg("New"); })
      .item("Open", [dlg]() { dlg("Open"); })
      .item("Save", [dlg]() { dlg("Save"); });
    mmenu.submenu("Edit")
      .item("Undo", "Ctrl+Z", [dlg]() { dlg("Undo"); })
      .item("Redo", "Ctrl+Y", [dlg]() { dlg("Redo"); })
      .item("Cut", "Ctrl+X", [dlg]() { dlg("Cut"); })
      .item("Copy", "Ctrl+C", [dlg]() { dlg("Copy"); })
      .item("Paste", "Ctrl+V", [dlg]() { dlg("Paste"); });

    screen->performLayout();
  }
  else
  {
    menus.front()->setVisible(show);
  }
}

void toggleConsoleWnd(Screen* screen, bool show)
{
  using namespace nanogui;

  auto console = screen->findWidgetGlobal("#console_wnd");
  if (!console)
  {
    auto& wnd = screen->window("Example: Console");
    wnd.setPosition(60, 60);
    wnd.withLayout<GroupLayout>();
    wnd.setFixedSize({ 300, 300 });
    wnd.setId("#console_wnd");
    screen->performLayout();
  }
  else
  {
    console->setVisible(show);
  }
}

void toggleLogWnd(Screen* screen, bool show)
{
  using namespace nanogui;

  auto logwnd = screen->findWidgetGlobal("#log_wnd");
  if (!logwnd)
  {
    auto& wnd = screen->window("Example: Log");
    wnd.setPosition(120, 120);
    wnd.setFixedSize({ 400, 300 });
    wnd.setId("#log_wnd");
    screen->performLayout();
  }
  else
  {
    logwnd->setVisible(show);
  }
}

void toggleSimpleLayoutWnd(Screen* screen, bool show)
{
  using namespace nanogui;

  auto logwnd = screen->findWidgetGlobal("#simple_layout_wnd");
  if (!logwnd)
  {
    auto& wnd = screen->window(Caption{ "Example: Simple layout" },
                               WidgetStretchLayout{ Orientation::Horizontal },
                               Position{ 180, 180 },
                               MinimumSize{ 400, 400 },
                               WidgetId{ "#simple_layout_wnd" });

    wnd.listbox(RelativeSize{ 0.33, 0 },
                ListboxCallback{ [screen](ListboxItem* i) {
                  if (Label* lb = screen->findWidget<Label>("#simple_layout_lb"))
                    lb->setCaption("MyObject: " + i->caption());
                }},
                ListboxContent{ [](Listbox& l) {
                  for (int i = 0; i < 100; i++)
                    l.addItem("Item " + std::to_string(i));
                }});
    auto& desc = wnd.widget(WidgetStretchLayout{ Orientation::Vertical });
    desc.label(Caption{ "MyObject: id" },
               CaptionHAlign{ TextHAlign::hLeft },
               FixedHeight{ 15 },
               WidgetId{ "#simple_layout_lb" });
    desc.tabs(TabNames{ "Description", "Details" });
    screen->performLayout();
  }
  else
  {
    logwnd->setVisible(show);
  }
}

void toggleTreeView(Screen* screen, bool show)
{
  using namespace nanogui;

  auto treeview = screen->findWidgetGlobal("#tree_view_wnd");
  if (!treeview)
  {
    auto& wnd = screen->window(Caption{ "Example: Tree view" },
                               WidgetStretchLayout{ Orientation::Horizontal },
                               Position{ 180, 180 },
                               MinimumSize{ 400, 400 },
                               WidgetId{ "#tree_view_wnd" });

    auto& view = wnd.treeview(RelativeSize{ 0.5, 0 }, WidgetId{"#treeview"});
    auto n1 = view.rootNode()->addNode("Node1");
    auto n1_c1 = n1->addNode("Node1_C1");
    n1_c1->addNode("NodeC1_c1");
    auto n2 = view.rootNode()->addNode("Node2");
    n2->addNode("Some text here");
    view.rootNode()->addNode("Node3");
    view.rootNode()->addNode("Node4");
    auto n5 = view.rootNode()->addNode("Node5");
    n5->addNode("Node5_C1")
        ->addNode("yet one node")
          ->addNode("yet second node")
            ->addNode("and third node")
              ->addNode("anybody show me?");
  }
  else
  {
    treeview->setVisible(show);
  }
}

void createAllWidgetsDemo(Screen* screen)
{
  Window& dw = screen->window(WidgetStretchLayout{ Orientation::Vertical },
                              Caption{ "All widgets demo" },
                              Position{ 725, 350 },
                              FixedSize{ 400, 400 });

  dw.submenu("File")
    .item("(dummy item)", []() {})
    .item("New", "Ctrl+N", [screen]() { screen->msgdialog(DialogTitle{ "New" }, DialogMessage{ "New Clicked!" }); })
    .item("Very larget text", [screen]() { screen->msgdialog(DialogTitle{ "Open" }, DialogMessage{ "New Clicked!" }); })
    .item("Save", [screen]() { screen->msgdialog(DialogTitle{ "Save" }, DialogMessage{ "New Clicked!" }); });
  dw.submenu("File").item("(dummy item)").setEnabled(false);
  dw.submenu("File").item("Save").setShortcut("Ctrl+S");

  auto toggleVisible = [screen](const std::string& /*wname*/, bool& enabled, bool& checked) {
    enabled = true;
    auto* w = screen->findWidgetGlobal("#console_wnd");
    checked = (w && w->visible());
  };
  dw.submenu("Examples")
    .item("Global menu", [screen](bool v) { toggleMainMenu(screen, v); })
    .item("Console", [screen](bool v) { toggleConsoleWnd(screen, v); }, [=](bool &e, bool &c) { toggleVisible("#console_wnd", e, c); })
    .item("Log", [screen](bool v) { toggleLogWnd(screen, v); }, [=](bool &e, bool &c) { toggleVisible("#log_wnd", e, c); })
    .item("Simple layout", [screen](bool v) { toggleSimpleLayoutWnd(screen, v); }, [=](bool &e, bool &c) { toggleVisible("#simple_layout_wnd", e, c); })
    .item("Tree view", [screen](bool v) { toggleTreeView(screen, v); }, [=](bool &e, bool &c) { toggleVisible("#tree_view_wnd", e, c); });

  dw.submenu("Help");

  auto& pw = dw.vscrollpanel(RelativeSize{ 1.f, 1.f }).vstack(2, 2);
  auto& help = pw.panel(Caption{ "Help" }, WindowCollapsed{ true });
  help.text()
    .header("PROGRAMMER GUIDE:")
    .bulletLine("Please see the createAllWidgetsDemo() code in example1.cpp. <- you are here!")
    .bulletLine("Please see the examples/example1.cpp.")
    .bulletLine("Enable 'theme.keyboardNavigation= true' for keyboard controls.")
    .bulletLine("Enable 'theme.nav.gamepad.enable= true' for gamepad controls.")
    .separator("")
    .header("USER GUIDE:")
    .bulletLine("Double-click on title bar to collapse window.")
    .bulletLine("Click and drag on lower corner to resize window")
    .bulletLine("Double-click to auto fit window to its contents")
    .bulletLine("CTRL+Click on a slider or drag box to input value as text.")
    .bulletLine("TAB/SHIFT+TAB to cycle through keyboard editable fields.")
    .bulletLine("CTRL+Mouse Wheel to zoom window contents.")
    .bulletLine("While inputing text:\n")
    .indent(15)
    .bulletLine("CTRL+Left/Right to word jump.")
    .bulletLine("CTRL+A or double-click to select all.")
    .bulletLine("CTRL+X/C/V to use clipboard cut/copy/paste.")
    .bulletLine("CTRL+Z,CTRL+Y to undo/redo.")
    .bulletLine("ESCAPE to revert.")
    .bulletLine("You can apply arithmetic operators +,*,/ on numerical values. Use +- to subtract.")
    .unindent()
    .header("With keyboard navigation enabled:")
    .indent(15)
    .bulletLine("Arrow keys to navigate.")
    .bulletLine("Space to activate a widget.")
    .bulletLine("Return to input text into a widget.")
    .bulletLine("Escape to deactivate a widget, close popup, exit child window.")
    .bulletLine("Alt to jump to the menu layer of a window.")
    .bulletLine("CTRL+Tab to select a window.")
    .unindent();

  auto& iocfg = pw.panel(Caption{ "Configuration" }, WindowCollapsed{ true });
  auto& nav = iocfg.panel(Caption{ "Configuration" }, WindowCollapsed{ true }, PanelHighlightHeader{ false });
  auto gs = [screen] { return screen->theme(); };
  nav.checkbox(Caption{ "theme.nav.mouse.enable" }, BoolObservable{ [gs]{return gs()->nav.mouse.enable; },
                                                                    [gs](bool v) { gs()->nav.mouse.enable = v; }});
  nav.checkbox(Caption{ "theme.nav.mouse.drawCursor" }, BoolObservable{ [gs] {return gs()->nav.mouse.drawCursor; },
                                                                        [gs](bool v) { gs()->nav.mouse.drawCursor = v; }});
  nav.checkbox(Caption{ "theme.textAreaBlinkCursor" }, BoolObservable{ [gs] {return gs()->textAreaBlinkCursor; },
                                                                       [gs](bool v) { gs()->textAreaBlinkCursor = v; }});
  nav.checkbox(Caption{ "theme.windowMoveFromTitlebarOnly" }, BoolObservable{ [gs] {return gs()->windowMoveFromTitlebarOnly; },
                                                                              [gs](bool v) { gs()->windowMoveFromTitlebarOnly = v; }});
  nav.checkbox(Caption{ "theme.windowEesizeFromEdge" }, BoolObservable{ [gs] {return gs()->windowResizeFromEdge; },
                                                                        [gs](bool v) { gs()->windowResizeFromEdge = v; }});
  nav.checkbox(Caption{ "theme.windowMoveInParent" }, BoolObservable{ [gs] {return gs()->windowMoveInParent; },
                                                                      [gs](bool v) { gs()->windowMoveInParent = v; }});
  nav.checkbox(Caption{ "theme.windowDrawBorder" }, BoolObservable{ gs()->windowDrawBorder });
  nav.checkbox(Caption{ "theme.frameDrawBorder" }, BoolObservable{ [gs] {return gs()->frameDrawBorder; },
                                                                   [gs](bool v) { gs()->frameDrawBorder = v; }});
  nav.checkbox(Caption{ "theme.debugHighlightMouseover" }, BoolObservable{ [gs] {return gs()->debugHighlightMouseover; },
                                                                           [gs](bool v) { gs()->debugHighlightMouseover = v; }});
  nav.checkbox(Caption{ "theme.keyboardNavigation" }, BoolObservable{ [gs] {return gs()->keyboardNavigation; },
                                                                      [gs](bool v) { gs()->keyboardNavigation = v; }});

  /*auto& bfcfg = */iocfg.panel(Caption{ "Backend flags" }, WindowCollapsed{ true }, PanelHighlightHeader{ false });
  auto& stcfg = iocfg.hgrid2(0.3f, Caption{ "Style" }, WindowCollapsed{ true }, PanelHighlightHeader{ false });
  stcfg.label("Theme");
  stcfg.wdg<DropdownBox>(DropdownBoxItems{ "Default", "White" },
    DropdownBoxStrCallback{ [screen](std::string item) {
      if (item == "Default") screen->setTheme<DefaultTheme>();
      else if (item == "White") screen->setTheme<WhiteTheme>();
    }});
  auto screenPerform = SliderCallback{ [screen](float) { screen->needPerformLayout(screen); }};
  stcfg.add(elm::Label{ "Window border size" }, Element<Slider>{ SliderObservable{ gs()->windowPaddingLeft }, SliderRange{ 0.f, 20.f }, screenPerform});
  stcfg.add(elm::Label{ "Window border size" }, Element<Slider>{ SliderObservable{ gs()->windowBorderSize }, SliderRange{ 0.f, 5.f }, screenPerform });
  stcfg.add(elm::Label{ "Window padding top" }, Element<Slider>{ SliderObservable{ gs()->windowPaddingTop }, SliderRange{ 0.f, 20.f }, screenPerform });
  stcfg.add(elm::Label{ "Frame padding left" }, Element<Slider>{ SliderObservable{ gs()->framePaddingLeft }, SliderRange{ 0.f, 20.f }, screenPerform });
  stcfg.add(elm::Label{ "Frame padding top" },  Element<Slider>{ SliderObservable{ gs()->framePaddingTop }, SliderRange{ 0.f, 20.f }, screenPerform });
  stcfg.add(elm::Label{ "Inner spacing left" }, Element<Slider>{ SliderObservable{ gs()->innerSpacingCommon }, SliderRange{ 0.f, 20.f }, screenPerform});
  stcfg.add(elm::Label{ "Tool button side" },   
            Element<Slider>{ 
                 SliderObservable{ [=] { return (float)gs()->toolButtonSide; }, [=](float v) { gs()->toolButtonSide = (int)v; }}, 
                 SliderRange{ 15.f, 50.f }, screenPerform 
            });

  auto& wopt = iocfg.hgrid2(0.5f, Caption{ "Window options" }, WindowCollapsed{ true });
  auto dwf = [screen, w = &dw](int f, int v = -1) { 
    if (v < 0) return w->haveDrawFlag(f);
    w->setDrawFlag(f, v>0); 
    screen->needPerformLayout(screen);
    return false;
  };
  wopt.checkbox(Caption{ "No header" }, BoolObservable{ [=] {return !dwf(Window::DrawHeader); },
                                                        [=] (bool v) { dwf(Window::DrawHeader, !v); } });
  wopt.checkbox(Caption{ "No title" },  BoolObservable{ [=] {return !dwf(Window::DrawTitle); },
                                                        [=](bool v) { dwf(Window::DrawTitle, !v); } });
  wopt.checkbox(Caption{ "No collapse icon" }, BoolObservable{ [=] {return !dwf(Window::DrawCollapseIcon); },
                                                               [=](bool v) { dwf(Window::DrawCollapseIcon, !v); } });
  wopt.checkbox(Caption{ "No move" }, BoolObservable{ [w=&dw] { return !w->isDraggable(); }, [w=&dw](bool v) { w->setDraggable((Theme::WindowDraggable)!v); } });
  wopt.checkbox(Caption{ "No resize" }, BoolObservable{ [w = &dw] { return !w->canResize(); }, [w = &dw](bool v) { w->setCanResize(!v); } });
  wopt.checkbox(Caption{ "No background" }, BoolObservable{ [=] {return !dwf(Window::DrawBody); }, [=](bool v) { dwf(Window::DrawBody, !v); }});
  wopt.checkbox(Caption{ "No bring to front" }, BoolObservable{[w=&dw]{return !w->canBringToFront();}, [w=&dw](bool v){w->setBringToFront(!v); } });

  auto& wwidgets = pw.panel(Caption{ "Widgets" }, WindowCollapsed{ true });
  auto& wbasic = wwidgets.panel(Caption{ "Basic" }, WindowCollapsed{ true }, PanelHighlightHeader{ false });
  wbasic.hstack(5, 2, 
                elm::Button{
                      Caption{"Button"}, FixedHeight{17},
                      ButtonCallback{ [w = &dw] { if (auto l = Label::find("#btn_action", w)) l->setCaption("Thanks for cliking me!!!"); }}
                },
                elm::Label{ WidgetId{ "#btn_action" }});
  wbasic.checkbox(Caption{ "checkbox" });
  auto radio_action = ButtonChangeCallback{ [w = &dw](Button* b) { 
    if (b && !b->pushed())
      return;
    if (auto l = Label::find("#radio_action", w)) 
      l->setCaption("Clicked " + b->caption()); 
  }};
  auto makerbtn = [=](std::string text) { return elm::RadioBtn{ Caption{text}, radio_action, FixedHeight{17} }; };
  wbasic.hstack(5, 2, 
                makerbtn("radio a"), makerbtn("radio b"), makerbtn("radio c"),
                elm::Label{ WidgetId{ "#radio_action" }});
                
  auto makecbtn = [=](Color c) { return elm::Button{ Caption{ "Click" }, FixedHeight{ 17 }, BackgroundColor{c}}; };
  wbasic.hstack(5, 2, makecbtn(Color::red), makecbtn(Color::yellow), makecbtn(Color::green),
                      makecbtn(Color::blue), makecbtn(Color::purple), makecbtn(Color::pink));

  wbasic.hstack(5, 2, 
                elm::Label{"Hold to repeat:"}, 
                elm::UpDownButton{UpDownCallback{ [w = &dw] (bool v) {
                    static int value = 0;
                    value += (v ? 1 : -1);
                    if (auto l = Label::find("#updown_action", w))
                      l->setCaption(std::to_string(value));
                  }
                }},
                elm::Label{WidgetId{ "#updown_action"}});

  wbasic.hstack(5, 2,
                elm::Label{Caption{"Hover over me"}, TooltipText{"I am tooltip"}},
                elm::Label{" - "},
                elm::Label{Caption{"or me"},                           
                           TooltipWidget<Window>{ 
                              IsVisible{ false },
                              WidgetStretchLayout{ Orientation::Vertical },
                              Caption{ "And I am tooltip too" },
                              FixedSize{ 100, 60 },
                              makecbtn(Color::red), makecbtn(Color::yellow), makecbtn(Color::green)
                           }
                          });
  wbasic.widget(FixedHeight{2}, elm::SplitLine{});
  wbasic.frame(WidgetGridLayout{ GridLayoutSplit{ 0.7f, 0.3f }, GridLayoutColAlignment{ Alignment::Fill } },
               elm::Label{"Value"}, 
                    elm::Label{"Caption"},
               elm::DropdownBox{ 
                 DropdownBoxFill{ [](string& r) { static char i = 'a'; r = std::string(5, i); return i++ < 'z';  }},
                 ItemHeight{ 20 }
               }, 
                    elm::Label{ Caption{"combo (?)"}, TooltipText{ "Combo section fill function example" }},
               elm::Textbox{
                 TextValue{"Input text here!"}, IsEditable{true}, FixedHeight{20},
                 TextBoxEditCallback{ [](const std::string& s, bool) { if (auto l = Label::find("#inp_txt_smp")) l->setCaption(s); }}
               }, 
                    elm::Label{ Caption{"Input text"}, WidgetId{"#inp_txt_smp"}},
               elm::Textbox{ TextPlaceholder{"input text here"}, IsEditable{ true }, FixedHeight{ 20 }}, 
                    elm::Label{ Caption{ "Input text (w/ hint)" }}

    );
}

void makePropEditor(Screen* screen)
{
  /*auto& cwindow =*/screen->wdg<PropertiesEditor>(Caption{ "Properties" },
                                 WidgetId{ "#prop_editor" },
                                 Position{ screen->width() - 300, 0 },
                                 FixedSize{ 300, screen->height() },
                                 WindowMovable{ Theme::dgFixed });

}

void makeCustomThemeWindow(Screen* screen, const std::string &title)
{
  auto& cwindow = screen->window(Caption{ title }, WidgetId{ "#dialog_w_theme" });
  cwindow.setPosition(1100, 300);
  cwindow.withTheme<WhiteTheme>(screen->nvgContext());
  cwindow.withLayout<GroupLayout>(15, 6, 6);

  /* test text box fonts */
  {
    cwindow.label("Text Boxes");
    cwindow.widget(WidgetGridLayout{ ColumnsAligment{ Alignment::Maximum, Alignment::Fill } },
                   elm::Label{ Caption{"TextBox : "}},
                   Element<TextBox>{ TextValue{ "Some Text" }, IsEditable{ true }},
                   elm::Label{ Caption{"IntBox : "}},
                   Element<IntBox<int>>{ IsSpinnable{ true }},
                   elm::Label{ Caption{ "FloatBox : " }},
                   Element<FloatBox<float>>{ IsSpinnable{ true }},
                   elm::Label{ Caption{ "Volume : " }},
                   Element<ToleranceBar>{ FixedSize{ 120, 30 }} );
  }

  /* Message dialogs */
  {
    cwindow.label("Message Dialogues");
    auto& tools = cwindow.widget(WidgetBoxLayout{ Orientation::Horizontal, Alignment::Middle, 0, 6 });
    auto& b = tools.button(Caption{ "Info" },
      ButtonCallback{ []() {
        elm::MessageDialog {
          DialogTitle{ "Title" }, DialogMessage{ "This is an information message" },
          DialogResult{ [](int result) { std::cout << "Dialog result: " << result << std::endl; }},
          CopyThemeFrom { "#dialog_w_theme" }
      };
     }});

    tools.button(Caption{ "Warn" },
      ButtonCallback{ []() {
        auto w = Window::find("#dialog_w_theme");
        elm::MessageDialog {
          DialogType{ (int)MessageDialog::Type::Warning },
          DialogTitle{ "Title" },
          DialogMessage{ "This is a warning message" },
          DialogResult{ [](int result) { std::cout << "Dialog result: " << result << std::endl; }},
          CopyThemeFrom{ "#dialog_w_theme" }
        };
      }});

    tools.button(Caption{ "Ask" },
      ButtonCallback{ []() {
        elm::MessageDialog{
          DialogType{ (int)MessageDialog::Type::Question },
          DialogTitle{ "Title" }, DialogMessage{ "This is a question message" },
          DialogButton{ "Yes" }, DialogAltButton{ "No" },
          DialogResult{ [](int result) { std::cout << "Dialog result: " << result << std::endl; }},
          CopyThemeFrom{ "#dialog_w_theme" }
        };
      }});
  }

  // TabWidget used to test TabHeader and others while keeping the size manageable
  cwindow.label("Tab Widget");
  auto& tabWidget = cwindow.tabs();

  /* test button and checkbox fonts */
  {
    auto& layer = *tabWidget.createTab("Button Like");
    layer.withLayout<GroupLayout>();

    // green color, produces white chevron at start
    auto& cp = layer.colorpicker(Color{ 0.28573f, 0.56702f, 0.25104f, 1.0f });
    cp.setFinalCallback([](const Color &c) { std::cout << "Color: " << c.transpose().toInt() << std::endl; });

    // combobox
    layer.combobox(ComboBoxItems{ "Combo box item 1", "Combo box item 2", "Combo box item 3" });

    // popup button
    auto& popup = layer.popupbutton(Caption{ "Popup" }, Icon{ ENTYPO_ICON_EXPORT })
                          .popupset(WidgetGroupLayout{}, 
                                    elm::Label{ "Arbitrary widgets can be placed here" },
                                    elm::CheckBox{ Caption{ "A check box" }});
    // popup right
    popup.popupbutton(Caption{ "Recursive popup" }, Icon{ ENTYPO_ICON_FLASH })
            .popupset(WidgetGroupLayout{}, elm::CheckBox{ Caption{ "Another check box" }});
    // popup left
    popup.popupbutton(Caption{ "Recursive popup" }, Icon{ ENTYPO_ICON_FLASH }, PopupSide{ Popup::Side::Left })
            .popupset(WidgetGroupLayout{}, elm::CheckBox{ Caption{ "Another check box" }} );

    // regular buttons
    /*auto& button =*/layer.button(Caption{ "PushButton" });

    // test that non-bold fonts for buttons work (applying to radio buttons)
    //std::string radio_font = cwindow.theme()->mDefaultFont;

    layer.button(Caption{ "Radio 1 (Hover for Tooltip)" },
                 ButtonFlags{ Button::Flag::RadioButton },
                 TooltipText{ "Short tooltip!" });

    layer.button(Caption{ "Radio 2 (Hover for Tooltip)" },
                 ButtonFlags{ Button::Flag::RadioButton },
                 TooltipText{ "This is a much longer tooltip that will get wrapped automatically!" });

    layer.button(Caption{ "ToggleButton" },
                 ButtonFlags{ Button::Flag::ToggleButton });

    // checkbox (top level)
    layer.checkbox(Caption{ "A CheckBox" });
  }

  /* test the graph widget fonts */
  {
    auto& layer = *tabWidget.createTab("Graph");
    layer.withLayout<GroupLayout>();

    layer.label("Function Graph Widget");
    auto& graph = layer.graph("Some Function");

    graph.setHeader("E = 2.35e-3");
    graph.setFooter("Iteration 89");

    VectorXf &func = graph.values();
    func.resize(100);
    for (int i = 0; i < 100; ++i)
      func[i] = 0.5f * (0.5f * std::sin(i / 10.f) +
        0.5f * std::cos(i / 23.f) + 1);
  }

  // Dummy tab used to represent the last tab button.
  tabWidget.createTab("+");

  // A simple counter.
  int counter = 1;
  tabWidget.setCallback([&](int index)
  {
    if (index == (tabWidget.tabCount() - 1))
    {
      // When the "+" tab has been clicked, simply add a new tab.
      std::string tabName = "Dynamic " + std::to_string(counter);
      auto& layerDyn = *tabWidget.createTab(index, tabName);
      layerDyn.withLayout<GroupLayout>();
      layerDyn.label("Function graph widget", "spectrum-bold");
      auto& graphDyn = layerDyn.graph("Dynamic function");

      graphDyn.setHeader("E = 2.35e-3");
      graphDyn.setFooter("Iteration " + std::to_string(index*counter));

      VectorXf &funcDyn = graphDyn.values();
      funcDyn.resize(100);
      for (int i = 0; i < 100; ++i)
        funcDyn[i] = 0.5f * std::abs((0.5f * std::sin(i / 10.f + counter) +
          0.5f * std::cos(i / 23.f + 1 + counter)));
      ++counter;
      // We must invoke perform layout from the screen instance to keep everything in order.
      // This is essential when creating tabs dynamically.
      tabWidget.screen()->performLayout();
      // Ensure that the newly added header is visible on screen
      tabWidget.ensureTabVisible(index);

    }
  });
  tabWidget.setActiveTab(0);
}

void createThemeBuilderWindow(Screen* screen)
{
  screen->wdg<ThemeBuilder>(Position{ 1050, 15 });
}

class ExampleScreen : public Screen {
public:
  ExampleScreen(const Vector2i& size, const std::string& caption) : Screen(size, caption, false) {
      initGPUTimer(&gpuTimer);

      makePropEditor(this);
      createButtonDemoWindow();
      createBasicWidgets();
      createMiscWidgets(this);
      createGridSmallObjects(this);
      createMeter(this);
      createAllWidgetsDemo(this);
      createThemeBuilderWindow(this);
      makeCustomThemeWindow(this, "Custom theme");
      createPicflowWindow();
      createTextAreaWindow();
      toggleTreeView(this, true);

      fpsGraph = &wdg<PerfGraph>(GRAPH_RENDER_FPS, "Frame Time", Vector2i(5, height() - 40));
      cpuGraph = &wdg<PerfGraph>(GRAPH_RENDER_MS, "CPU Time", Vector2i(5, height() - 40 * 2));
      gpuGraph = &wdg<PerfGraph>(GRAPH_RENDER_MS, "GPU Time", Vector2i(5, height() - 40 * 3));

      previousFrameTime = getTimeFromStart();

      performLayout();
    }

    ~ExampleScreen() {}

    bool keyboardEvent(int key, int scancode, int action, int modifiers) override {
        if (Screen::keyboardEvent(key, scancode, action, modifiers))
            return true;
        if (nanogui::isKeyboardKeyEscape(key) && nanogui::isKeyboardActionPress(action)) {
            setVisible(false);
            return true;
        }
        return false;
    }

    void draw(NVGcontext *ctx) override {
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

        float t = getTimeFromStart();
        float dt = float(t - previousFrameTime);
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
        if (cpuGraph) cpuGraph->update((float)cpuTime);
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
          int nvalue = (int)(1.f / dt);

          ledvalues.push_back(nvalue);
          if (ledvalues.size() > (size_t)led->columnCount())
            ledvalues.pop_front();
          int k = 0;
          for (auto& c : ledvalues)
          {
            led->clearColumn(k);
            int tt = led->rowCount() * c / 50;
            for (int i = 0; i <= tt; i++)
            {
              int rk(0xff * (i / (float)led->rowCount()));
              led->setColorAt(led->rowCount() - i, k, Color(0xff, 0, 0, rk));
            }
            k++;
          }
        }

    }

    void drawContents() override {
        using namespace nanogui;
    }

    bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override {
        if (Widget::mouseButtonEvent(p, button, down, modifiers))
          return true;
        if (down && isMouseButtonRight(button) && findWidget(p)==this) {
          auto menu = new nanogui::ContextMenu(this, "", true);
          menu->addItem("Item 1", [this]() { msgdialog(DialogTitle{ "Item 1" }, DialogMessage{ "Item 1 Clicked!" }); }, ENTYPO_ICON_PLUS);

          auto submenu = menu->addSubMenu("Submenu");
          submenu->addItem("Subitem 1", [this]() { msgdialog(DialogTitle{ "Subitem 1" }, DialogMessage{ "Subitem 1 Clicked!" }); });
          auto subsubmenu = submenu->addSubMenu("Subsubmenu", ENTYPO_ICON_LOOP);
          submenu->addItem("Subitem 2", [this]() { msgdialog(DialogTitle{ "Subitem 2" }, DialogMessage{ "Subitem 2 Clicked!" }); });

          subsubmenu->addItem("Subsubitem 1", [this]() { msgdialog(DialogTitle{ "Subsubitem 1" }, DialogMessage{ "Subsubitem 1 Clicked!" }); });
          subsubmenu->addItem("Subsubitem 2", [this]() { msgdialog(DialogTitle{ "Subsubitem 2" }, DialogMessage{ "Subsubitem 2 Clicked!" }); });

          menu->activate(p - mPos);
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
  Vector2i size{ 1600, 900 };
  auto window = nanogui::sample::create_window(size.x(), size.y(), "Example Nanogui", true, false, true);
  nanogui::sample::create_context();

  {
    ExampleScreen screen(size, "");
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
