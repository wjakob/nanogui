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

void createButtonDemoWindow(Screen* screen)
{
  auto& w = screen->window(Position{ 15, 15 },
                           WindowMovable{ Theme::WindowDraggable::dgFixed },
                           Caption{ "Button demo" },
                           WidgetLayout{ new GroupLayout() });

  /* No need to store a pointer, the data structure will be automatically
  freed when the parent window is deleted */
  w.label(Caption{ "Push buttons" }, CaptionFont{ "sans-bold" });
  w.button(ButtonCallback{ [] { cout << "pushed!" << endl; } },
           Caption{ "Plain button" },
           TooltipText{ "short tooltip" });

  /* Alternative construction notation using variadic template */
  w.button(Caption{ "Styled" },
           Icon{ ENTYPO_ICON_ROCKET },
           BackgroundColor{ 0, 0, 255, 25 },
           ButtonCallback{ [] { cout << "pushed!" << endl; } },
           TooltipText{ "This button has a fairly long tooltip. It is so long, in "
                        "fact, that the shown text will span several lines." });

  w.label("Toggle buttons", "sans-bold");
  w.button(Caption{ "Toggle me" },
           ButtonFlags{ Button::ToggleButton },
           ButtonChangeCallback{ [](bool state) { cout << "Toggle button state: " << state << endl; } });

  w.label("Radio buttons", "sans-bold");
  w.button(Caption{ "Radio button 1" }, ButtonFlags{ Button::RadioButton } );
  w.button(Caption{ "Radio button 2" }, ButtonFlags{ Button::RadioButton } );

  w.label("A tool palette", "sans-bold");
  auto& tools = w.widget();
  tools.boxlayout(Orientation::Horizontal, Alignment::Middle, 0, 6);
  tools.toolbutton(Icon{ ENTYPO_ICON_CLOUD });
  tools.toolbutton(Icon{ ENTYPO_ICON_FAST_FORWARD });
  tools.toolbutton(Icon{ ENTYPO_ICON_VOLUME_UP });
  tools.toolbutton(Icon{ ENTYPO_ICON_INSTALL });

  w.label("Popup buttons", "sans-bold");
  auto& popupBtn = w.wdg<PopupButton>("Popup", ENTYPO_ICON_EXPORT);
  auto& popup = popupBtn.popupref();
  popup.withLayout<GroupLayout>();
  popup.label("Arbitrary widgets can be placed here");
  popup.checkbox("A check box");
  // popup right
  auto& popupBtnR = popup.wdg<PopupButton>("Recursive popup", ENTYPO_ICON_FLASH);
  auto& popupRight = popupBtnR.popupref();
  popupRight.withLayout<GroupLayout>();
  popupRight.checkbox("Another check box");
  // popup left
  auto& popupBtnL = popup.wdg<PopupButton>("Recursive popup", ENTYPO_ICON_FLASH);
  popupBtnL.setSide(Popup::Side::Left);
  auto& popupLeft = popupBtnL.popupref();
  popupLeft.withLayout<GroupLayout>();
  popupLeft.checkbox("Another check box");

  w.label("A switch boxes", "sans-bold");
  auto& switchboxArea = w.widget();
  switchboxArea.withLayout<GridLayout>(Orientation::Horizontal, 2);

  auto& switchboxHorizontal = switchboxArea.switchbox(SwitchBox::Alignment::Horizontal, "");
  switchboxHorizontal.setFixedSize(Vector2i(80, 30));
  switchboxArea.switchbox(SwitchBox::Alignment::Vertical, "");

  auto& switchboxVerticalColored = switchboxArea.switchbox(SwitchBox::Alignment::Vertical, "");
  switchboxVerticalColored.setFixedSize(Vector2i(28, 60));
  switchboxVerticalColored.setBackgroundColor({ 0,0,129,255 });

  auto& switchboxHorizontalColored = switchboxArea.switchbox(SwitchBox::Alignment::Horizontal, "");
  switchboxHorizontalColored.setFixedSize(Vector2i(60, 25));
  switchboxHorizontalColored.setStateColor({ 0,129,0,255 }, { 255, 0, 0, 255 });

  w.label("A led buttons", "sans-bold");
  auto& ledbuttonsArea = w.widget();
  ledbuttonsArea.withLayout<GridLayout>(Orientation::Horizontal, 4);
  ledbuttonsArea.wdg<LedButton>(LedButton::circleBlack, 30, 30);
  ledbuttonsArea.wdg<LedButton>(LedButton::circleBlue, 30, 30);
  ledbuttonsArea.wdg<LedButton>(LedButton::circleGreen, 30, 30);
  ledbuttonsArea.wdg<LedButton>(LedButton::circleGray, 30, 30);
  ledbuttonsArea.wdg<LedButton>(LedButton::circleOrange, 30, 30);
  ledbuttonsArea.wdg<LedButton>(LedButton::circleRed, 30, 30);
  ledbuttonsArea.wdg<LedButton>(LedButton::circleYellow, 30, 30);
  ledbuttonsArea.wdg<LedButton>(LedButton::circlePurple, 30, 30);
}

using ImagesDataType = std::vector<pair<int, std::string>>;

void createBasicWidgets(Screen* parent)
{
#if defined(_WIN32)
  string resourcesFolderPath("../resources/");
#else
  string resourcesFolderPath("./");
#endif
  static vector<pair<int, string>> icons = loadImageDirectory(parent->nvgContext(), "icons");
  static ImagesDataType mImagesData;

  // Load all of the images by creating a GLTexture object and saving the pixel data.
  for (auto& icon : icons) {
    auto fullpath = resourcesFolderPath + icon.second;
    auto data = nvgCreateImage(parent->nvgContext(), fullpath.c_str(), 0);
    mImagesData.emplace_back(data, fullpath);
  }

  auto& w = parent->window(Caption{ "Basic widgets" },
                           Position{ 200, 15 },
                           WidgetId{ "#basic_widgets_wnd"},
                           WidgetLayout{ new GroupLayout() });

  w.label("Searchboxes", "sans-bold");
  w.wdg<SearchBox>();

  w.label("Message dialog", "sans-bold");
  auto& tools = w.widget();
  tools.boxlayout(Orientation::Horizontal, Alignment::Middle, 0, 6);
  tools.button(Caption{ "Info" },
               ButtonCallback { [=] {
                 auto& dlg = parent->msgdialog(MessageDialog::Type::Information, "Title", "This is an information message");
                 dlg.setCallback([](int result) { cout << "Dialog result: " << result << endl; });
               }});

  tools.button(Caption{ "Warn" },
               ButtonCallback{ [=] {
                 auto& dlg = parent->msgdialog( MessageDialog::Type::Warning, "Title", "This is a warning message");
                 dlg.setCallback([](int result) { cout << "Dialog result: " << result << endl; });
               }});


  tools.button(Caption{ "Ask" },
               ButtonCallback{ [=] {
                 auto& dlg = parent->msgdialog( MessageDialog::Type::Warning, "Title", "This is a question message", "Yes", "No", true);
                 dlg.setCallback([](int result) { cout << "Dialog result: " << result << endl; });
               }});

  w.label("Image panel & scroll panel", "sans-bold");
  auto& imagePanelBtn = w.popupbutton("Image Panel");
  imagePanelBtn.setIcon(ENTYPO_ICON_FOLDER);
  auto& popup = imagePanelBtn.popupref();
  popup.setFixedSize({ 245, 150 });

  auto& vscroll = popup.vscrollpanel();

  //need for change texture
  static int currentImage = 0;

  auto& imgPanel = vscroll.wdg<ImagePanel>();
  imgPanel.setImages(icons);
  imgPanel.setCallback([=](int i) {
    if (auto* imageView = parent->findWidget<ImageView>("#image_view"))
    {
      imageView->bindImage(mImagesData[i].first);
      currentImage = i;
      cout << "Selected item " << i << '\n';
    }
  });

  auto& imageWindow = parent->window("Selected image");
  imageWindow.setPosition(Vector2i(710, 15));
  imageWindow.withLayout<GroupLayout>();

  // Set the first texture
  auto& imageView = imageWindow.wdg<ImageView>(mImagesData[0].first);
  imageView.setGridThreshold(20);
  imageView.setFixedSize({220,220});
  imageView.setId("#image_view");
  imageView.setPixelInfoThreshold(20);
  imageView.setPixelInfoCallback(
    [&](const Vector2i& index) -> pair<string, Color> {
      auto& imageData = mImagesData[currentImage].second;
      if (auto* imageView = parent->findWidget<ImageView>("#image_view"))
      {
        auto& textureSize = imageView->imageSize();
        string stringData;
        uint16_t channelSum = 0;
        for (int i = 0; i != 4; ++i) {
          auto& channelData = imageData[4 * index.y()*textureSize.x() + 4 * index.x() + i];
          channelSum += channelData;
          stringData += (to_string(static_cast<int>(channelData)) + "\n");
        }
        float intensity = static_cast<float>(255 - (channelSum / 4)) / 255.0f;
        float colorScale = intensity > 0.5f ? (intensity + 1) / 2 : intensity / 2;
        Color textColor = Color(colorScale, 1.0f);
        return { stringData, textColor };
      }
      return { "", Color() };
  });

  w.label("File dialog", "sans-bold");
  auto& fdtools = w.widget();
  fdtools.boxlayout(Orientation::Horizontal, Alignment::Middle, 0, 6);
  fdtools.button(Caption{ "Open" },
                 ButtonCallback{ [&] {
                    cout << "File dialog result: "
                         << file_dialog({ { "png", "Portable Network Graphics" },{ "txt", "Text file" } }, false)
                         << endl;
                 }});
  fdtools.button(Caption{ "Save" },
                 ButtonCallback{ [&] {
                    cout << "File dialog result: "
                         << file_dialog({ { "png", "Portable Network Graphics" },{ "txt", "Text file" } }, true)
                         << endl;
                 }});

  w.label("Combo box", "sans-bold");
  w.wdg<DropdownBox>(DropdownBoxItems{ "Dropdown item 1", "Dropdown item 2", "Dropdown item 3" }, WidgetId{"1"});
  w.combobox(ComboBoxItems{ "Combo box item 1", "Combo box item 2", "Combo box item 3" }, WidgetId{ "2" });
  w.label("Check box", "sans-bold");
  auto& cb = w.checkbox("Flag 1", [](bool state) { cout << "Check box 1 state: " << state << endl; });
  cb.setChecked(true);
  cb.setUncheckedColor(Color(128, 0, 0, 255));
  cb.setCheckedColor(Color(0, 128, 0, 255));
  cb.setPushedColor(Color(128, 128, 0, 255));
  w.checkbox("Flag 2", [](bool state) { cout << "Check box 2 state: " << state << endl; });

  w.label("Progress bar", "sans-bold");

  auto& bars = w.widget();
  bars.boxlayout(Orientation::Horizontal, Alignment::Middle, 0, 6);
  bars.wdg<ProgressBar>(WidgetId{ "#lineprogressbar" });
  bars.wdg<CircleProgressBar>(WidgetId{ "#circleprogressbar" },
                              FixedSize{ 40, 40 });

  w.label("Slider and text box", "sans-bold");

  auto& panel = w.widget();
  panel.boxlayout(Orientation::Horizontal, Alignment::Middle, 0, 20);

  auto& slider = panel.slider();
  slider.setValue(0.5f);
  slider.setFixedWidth(80);

  auto& textBox = panel.textbox();
  textBox.setFixedSize(Vector2i(60, 25));
  textBox.setValue("50");
  textBox.setUnits("%");

  slider.setCallback([&](float value) { textBox.setValue(std::to_string((int)(value * 100))); });
  slider.setFinalCallback([&](float value) { cout << "Final slider value: " << (int)(value * 100) << endl; });
  textBox.setFixedSize(Vector2i(60, 25));
  textBox.setFontSize(20);
  textBox.setAlignment(TextBox::Alignment::Right);

  w.label(Caption{ "Spinners" }, CaptionFont{ "sans-bold" });
  auto& spinners = w.widget().boxlayout(Orientation::Horizontal, Alignment::Middle, 0, 6);
  spinners.spinner(FixedSize{ 40, 40 });
  spinners.spinner(SpinnerSpeed{ 0.5f }, FixedSize{ 40, 40 });
  spinners.spinner(SpinnerSpeed{ -0.7f }, FixedSize{ 40, 40 });

  w.label("Dial and text box", "sans-bold");

  auto& dials = w.widget();
  dials.boxlayout(Orientation::Horizontal, Alignment::Middle, 0, 20);

  auto& dial = dials.wdg<Dial>();
  dial.setValue(0.01f);
  dial.setFixedWidth(80);

  auto& dialTextBox = dials.textbox();
  dialTextBox.setFixedSize({ 60, 25 });
  dialTextBox.setValue("0.01");
  dial.setCallback([&](float value) {
    value = 0.01f + 99.99f*powf(value, 5.0f);
    std::ostringstream sval;
    sval.precision(2);
    sval << std::fixed << value;
    dialTextBox.setValue(sval.str());
  });
  dial.setFinalCallback([&](float value) {
    dial.setHighlightedRange(std::pair<float, float>(0.0f, value));
    value = 0.01f + 99.99f*powf(value, 5.0f);
    cout << "Final dial value: " << value << endl;
  });
  dialTextBox.setFixedSize({ 60, 25 });
  dialTextBox.setFontSize(20);
  dialTextBox.setAlignment(TextBox::Alignment::Right);
}

void createTextAreaWindow(Screen* screen)
{
  auto& mw = screen->window(Caption{"TextArea window"},
                            FixedSize{ 300, 200 },
                            WidgetStretchLayout{ Orientation::Horizontal },
                            Position{ 1015, 405 } );
  std::string longText = "void createTextAreaWindow(Sreen* screen) {\n"
    "    auto& mw = screen->window(Caption{ \"TextArea window\" }\n"
    "                              FixedSize{ 300, 200 },\n"
    "                              WidgetStretchLayout{ Orientation::Horizontal },\n"
    "                              Position{ 1015, 405 });\n"
    "    std::string longText = \"\";\n"
    "    auto& area = mw.wdg<TextArea>(RelativeSize{ 1.f, 1.f }, Text{ longText }); }\n";
  auto& area = mw.wdg<TextArea>(RelativeSize{ 1.f, 1.f }, LongText{ longText } );
}

void createPicflowWindow(Screen* screen)
{
#if defined(_WIN32)
  string resourcesFolderPath("../resources/");
#else
  string resourcesFolderPath("./");
#endif
  auto& mw = screen->window(Caption{ "Picflow" },
                            FixedSize{ 400, 250 },
                            WidgetStretchLayout{ Orientation::Horizontal },
                            Position{ 715, 305 });

  static vector<pair<int, string>> icons = loadImageDirectory(screen->nvgContext(), "icons");
  static ImagesDataType picflowImagesData;

  // Load all of the images by creating a GLTexture object and saving the pixel data.
  for (auto& icon : icons) {
    auto fullpath = resourcesFolderPath + icon.second;
    auto data = nvgCreateImage(screen->nvgContext(), fullpath.c_str(), 0);
    picflowImagesData.emplace_back(data, fullpath);
  }

  auto& picflow = mw.wdg<Picflow>(Vector2f(0.35f, 0.35f));
  for (auto& icon : picflowImagesData)
    picflow.addItem(icon.first);
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

  auto& bf = panelJump.button(Caption{ "" },
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
  auto& w = screen->window("Grid of small widgets");
  w.setPosition(425, 300);
  auto layoutw = new GridLayout(Orientation::Horizontal, 2, Alignment::Middle, 15, 5);
  layoutw->setColAlignment({ Alignment::Maximum, Alignment::Fill });
  layoutw->setSpacing(0, 10);
  w.setLayout(layoutw);


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
  auto& cb = w.checkbox("Check me");
  cb.setFontSize(16);
  cb.setChecked(true);

  w.label("Combo box :", "sans-bold");
  auto& cobo = w.combobox(ComboBoxItems{ "Item 1", "Item 2", "Item 3" });
  cobo.setFontSize(16);
  cobo.setFixedSize(Vector2i(100, 20));

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
    mmenu.submenu("File")
      .item("New", [=]() { screen->msgdialog(MessageDialog::Type::Information, "New", "New Clicked!"); })
      .item("Open", [=]() { screen->msgdialog(MessageDialog::Type::Information, "Open", "New Clicked!"); })
      .item("Save", [=]() { screen->msgdialog(nanogui::MessageDialog::Type::Information, "Save", "New Clicked!"); });
    mmenu.submenu("Edit")
      .item("Undo", "Ctrl+Z", [=]() { screen->msgdialog(nanogui::MessageDialog::Type::Information, "New", "New Clicked!"); })
      .item("Redo", "Ctrl+Y", [=]() { screen->msgdialog(nanogui::MessageDialog::Type::Information, "New", "New Clicked!"); })
      .item("Cut", "Ctrl+X", [=]() { screen->msgdialog(nanogui::MessageDialog::Type::Information, "New", "New Clicked!"); })
      .item("Copy", "Ctrl+C", [=]() { screen->msgdialog(nanogui::MessageDialog::Type::Information, "New", "New Clicked!"); })
      .item("Paste", "Ctrl+V", [=]() { screen->msgdialog(nanogui::MessageDialog::Type::Information, "New", "New Clicked!"); });

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
  Window& dw = screen->window(WindowSimpleLayout{ Orientation::Horizontal },
    Caption{ "All widgets demo" },
    Position{ 725, 350 },
    MinimumSize{ 400, 400 });

  dw.submenu("File")
    .item("(dummy item)", []() {})
    .item("New", "Ctrl+N", [screen]() { screen->msgdialog(MessageDialog::Type::Information, "New", "New Clicked!"); })
    .item("Very larget text", [screen]() { screen->msgdialog(MessageDialog::Type::Information, "Open", "New Clicked!"); })
    .item("Save", [screen]() { screen->msgdialog(MessageDialog::Type::Information, "Save", "New Clicked!"); });
  dw.submenu("File").item("(dummy item)").setEnabled(false);
  dw.submenu("File").item("Save").setShortcut("Ctrl+S");

  dw.submenu("Examples")
    .item("Global menu", [screen](bool v) { toggleMainMenu(screen, v); })
    .item("Console", [screen](bool v) { toggleConsoleWnd(screen, v); },
                     [screen](bool &enabled, bool &checked) {
                       enabled = true;
                       auto* w = screen->findWidgetGlobal("#console_wnd");
                       checked = (w && w->visible());
                     })
    .item("Log", [screen](bool v) { toggleLogWnd(screen, v); },
                 [screen](bool &enabled, bool &checked) {
                   enabled = true;
                   auto* w = screen->findWidgetGlobal("#log_wnd");
                   checked = (w && w->visible());
                 })
    .item("Simple layout", [screen](bool v) { toggleSimpleLayoutWnd(screen, v); },
                           [screen](bool &enabled, bool &checked) {
                             enabled = true;
                             auto* w = screen->findWidgetGlobal("#simple_layout_wnd");
                             checked = (w && w->visible());
                           })
    .item("Tree view", [screen](bool v) { toggleTreeView(screen, v); },
                       [screen](bool &enabled, bool &checked) {
                         enabled = true;
                         auto* w = screen->findWidgetGlobal("#tree_view_wnd");
                         checked = (w && w->visible());
                       });
  dw.submenu("Help");
}

void makeCustomThemeWindow(Screen* screen, const std::string &title)
{
  auto& cwindow = screen->window(title);
  cwindow.setPosition(1100, 300);
  cwindow.withTheme<WhiteTheme>(screen->nvgContext());
  cwindow.withLayout<GroupLayout>(15, 6, 6);

  /* test text box fonts */
  {
    cwindow.label("Text Boxes");
    auto& wrapper = cwindow.widget();
    wrapper.withLayout<GridLayout>(ColumnsAligment{ Alignment::Maximum, Alignment::Fill });
    wrapper.label("TextBox : ");
    wrapper.textbox(TextValue{ "Some Text" }, IsEditable{ true });
    wrapper.label("IntBox : ");
    wrapper.intbox<int>(IsSpinnable{ true });
    wrapper.label("FloatBox : ");
    wrapper.floatbox<float>(IsSpinnable{ true });

    wrapper.label("Volume : ");
    wrapper.tolerancebar().setFixedSize(Vector2i(120, 30));
  }

  /* Message dialogs */
  {
    cwindow.label("Message Dialogues");
    auto& tools = cwindow.widget();
    tools.boxlayout(Orientation::Horizontal, Alignment::Middle, 0, 6);
    auto& b = tools.button("Info");
    Theme* ctheme = cwindow.theme();
    b.setCallback([screen, ctheme]() {
      auto& dlg = screen->msgdialog(MessageDialog::Type::Information,
        "Title",
        "This is an information message");
      dlg.setTheme(ctheme);
      dlg.setCallback([](int result) { std::cout << "Dialog result: " << result << std::endl; });
    });
    auto& bw = tools.button("Warn");
    bw.setCallback([screen, ctheme]() {
      auto& dlg = screen->msgdialog(MessageDialog::Type::Warning,
        "Title",
        "This is a warning message");
      dlg.setTheme(ctheme);
      dlg.setCallback([](int result) { std::cout << "Dialog result: " << result << std::endl; });
    });
    auto& ba = tools.button("Ask");
    ba.setCallback([&, ctheme]() {
      auto& dlg = screen->msgdialog(MessageDialog::Type::Question,
        "Title",
        "This is a question message",
        "Yes",
        "No",
        true);
      dlg.setTheme(ctheme);
      dlg.setCallback([](int result) { std::cout << "Dialog result: " << result << std::endl; });
    });
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
    cp.setFinalCallback([](const Color &c) { std::cout << "Color: " << c.transpose() << std::endl; });

    // combobox
    std::vector<std::string> items{ "Combo box item 1", "Combo box item 2", "Combo box item 3" };
    layer.combobox(items);

    // popup button
    int icon = ENTYPO_ICON_EXPORT;
    auto& popupBtn = layer.popupbutton("Popup", icon);
    auto& popup = popupBtn.popupref();
    popup.withLayout<GroupLayout>();
    popup.label("Arbitrary widgets can be placed here");
    popup.checkbox("A check box");
    // popup right
    icon = ENTYPO_ICON_FLASH;
    auto& popupBtn2 = popup.popupbutton("Recursive popup", icon);
    auto& popupRight = popupBtn2.popupref();
    popupRight.withLayout<GroupLayout>();
    popupRight.checkbox("Another check box");
    // popup left
    auto& popupBtn3 = popup.popupbutton("Recursive popup", icon);
    popupBtn3.setSide(Popup::Side::Left);
    auto& popupLeft = popupBtn3.popupref();
    popupLeft.withLayout<GroupLayout>();
    popupLeft.checkbox("Another check box");

    // regular buttons
    auto& button = layer.button("PushButton");

    // test that non-bold fonts for buttons work (applying to radio buttons)
    //std::string radio_font = cwindow.theme()->mDefaultFont;

    auto& button2 = layer.button("Radio 1 (Hover for Tooltip)");
    //button2.setFont(radio_font);
    button2.setFlags(Button::Flag::RadioButton);
    button2.setTooltip("Short tooltip!");

    auto& button3 = layer.button("Radio 2 (Hover for Tooltip)");
    //button3.setFont(radio_font);
    button3.setFlags(Button::Flag::RadioButton);
    button3.setTooltip("This is a much longer tooltip that will get wrapped automatically!");

    auto& button4 = layer.button("ToggleButton");
    button4.setFlags(Button::Flag::ToggleButton);

    // checkbox (top level)
    layer.checkbox("A CheckBox");
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

class ExampleApplication : public nanogui::Screen {
public:
  ExampleApplication() : nanogui::Screen({ 1600, 900 }, "NanoGUI Test") {
      initGPUTimer(&gpuTimer);

      createButtonDemoWindow(this);
      createBasicWidgets(this);
      createMiscWidgets(this);
      createGridSmallObjects(this);
      createMeter(this);
      createAllWidgetsDemo(this);
      createThemeBuilderWindow(this);
      makeCustomThemeWindow(this, "Custom theme");
      createPicflowWindow(this);
      createTextAreaWindow(this);
      toggleTreeView(this, true);

      fpsGraph = &wdg<PerfGraph>(GRAPH_RENDER_FPS, "Frame Time", Vector2i(5, height() - 40));
      cpuGraph = &wdg<PerfGraph>(GRAPH_RENDER_MS, "CPU Time", Vector2i(5, height() - 40 * 2));
      gpuGraph = &wdg<PerfGraph>(GRAPH_RENDER_MS, "GPU Time", Vector2i(5, height() - 40 * 3));

      previousFrameTime = getTimeFromStart();

      performLayout();
    }

    ~ExampleApplication() {}

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
};

int main(int /* argc */, char ** /* argv */) {
    try {
        nanogui::init();

        /* scoped variables */ {
            nanogui::ref<ExampleApplication> app = new ExampleApplication();
            app->drawAll();
            app->setVisible(true);
            nanogui::mainloop();
        }

        nanogui::shutdown();
    } catch (const std::runtime_error &e) {
        std::string error_msg = std::string("Caught a fatal error: ") + std::string(e.what());
        #if defined(_WIN32)
            MessageBoxA(nullptr, error_msg.c_str(), NULL, MB_ICONERROR | MB_OK);
        #else
            std::cerr << error_msg << endl;
        #endif
        return -1;
    }

    return 0;
}
