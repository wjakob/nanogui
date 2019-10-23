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
#include <iostream>
#include <sstream>
#include <string>

// Includes for the GLTexture class.
#include <cstdint>
#include <memory>
#include <utility>

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

nanogui::GPUtimer gpuTimer;

class ExampleApplication : public nanogui::Screen {
public:
  ExampleApplication() : nanogui::Screen({ 1280, 800 }, "NanoGUI Test") {
    using namespace nanogui;

    initGPUTimer(&gpuTimer);

    auto& refw = window(Position{ 15, 15 },
                        WindowMovable{ Theme::WindowDraggable::dgFixed },
                        Caption{ "Button demo" },
                        WidgetLayout{ new GroupLayout() }
                        );
    Window *w = &refw;

    /* No need to store a pointer, the data structure will be automatically
       freed when the parent window is deleted */
    w->label(Caption{ "Push buttons" }, CaptionFont{ "sans-bold" });
    w->button(ButtonCallback{ [] { cout << "pushed!" << endl; } },
              Caption{ "Plain button" },
              
              TooltipText{ "short tooltip" });

    /* Alternative construction notation using variadic template */
    w->button(Caption{ "Styled" }, 
              Icon{ ENTYPO_ICON_ROCKET },
              BackgroundColor{ 0, 0, 255, 25 },
              ButtonCallback{ [] { cout << "pushed!" << endl; } },
              TooltipText{ "This button has a fairly long tooltip. It is so long, in "
                            "fact, that the shown text will span several lines." } );

    new Label(w, "Toggle buttons", "sans-bold");
    auto b = new Button(w, "Toggle me");
    b->setFlags(Button::ToggleButton);
    b->setChangeCallback([](bool state) { cout << "Toggle button state: " << state << endl; });

    new Label(w, "Radio buttons", "sans-bold");
    b = new Button(w, "Radio button 1");
    b->setFlags(Button::RadioButton);
    b = new Button(w, "Radio button 2");
    b->setFlags(Button::RadioButton);

    new Label(w, "A tool palette", "sans-bold");
    Widget *tools = new Widget(w);
    tools->setLayout(new BoxLayout(Orientation::Horizontal,
      Alignment::Middle, 0, 6));

    b = new ToolButton(tools, ENTYPO_ICON_CLOUD);
    b = new ToolButton(tools, ENTYPO_ICON_CONTROLLER_FAST_FORWARD);
    b = new ToolButton(tools, ENTYPO_ICON_COMPASS);
    b = new ToolButton(tools, ENTYPO_ICON_INSTALL);

    new Label(w, "Popup buttons", "sans-bold");
    PopupButton *popupBtn = new PopupButton(w, "Popup", ENTYPO_ICON_EXPORT);
    Popup *popup = popupBtn->popup();
    popup->setLayout(new GroupLayout());
    new Label(popup, "Arbitrary widgets can be placed here");
    new CheckBox(popup, "A check box");
    // popup right
    popupBtn = new PopupButton(popup, "Recursive popup", ENTYPO_ICON_FLASH);
    Popup *popupRight = popupBtn->popup();
    popupRight->setLayout(new GroupLayout());
    new CheckBox(popupRight, "Another check box");
    // popup left
    popupBtn = new PopupButton(popup, "Recursive popup", ENTYPO_ICON_FLASH);
    popupBtn->setSide(Popup::Side::Left);
    Popup *popupLeft = popupBtn->popup();
    popupLeft->setLayout(new GroupLayout());
    new CheckBox(popupLeft, "Another check box");

    w->wdg<Label>("A switch boxes", "sans-bold");
    auto& switchboxArea = w->wdg<Widget>();
    switchboxArea.setLayout(new GridLayout(Orientation::Horizontal, 2));

    auto& switchboxHorizontal = switchboxArea.switchbox(SwitchBox::Alignment::Horizontal, "");
    switchboxHorizontal.setFixedSize(Vector2i(80, 30));
    switchboxArea.switchbox(SwitchBox::Alignment::Vertical, "");

    auto& switchboxVerticalColored = switchboxArea.switchbox(SwitchBox::Alignment::Vertical, "");
    switchboxVerticalColored.setFixedSize(Vector2i(28, 60));
    switchboxVerticalColored.setBackgroundColor({ 0,0,129,255 });

    auto& switchboxHorizontalColored = switchboxArea.switchbox(SwitchBox::Alignment::Horizontal, "");
    switchboxHorizontalColored.setFixedSize(Vector2i(60, 25));
    switchboxHorizontalColored.setStateColor({ 0,129,0,255 }, { 255, 0, 0, 255 });

    w->wdg<Label>("A led buttons", "sans-bold");
    auto& ledbuttonsArea = w->wdg<Widget>();
    ledbuttonsArea.setLayout(new GridLayout(Orientation::Horizontal, 4));
    ledbuttonsArea.wdg<LedButton>(LedButton::circleBlack, 30, 30);
    ledbuttonsArea.wdg<LedButton>(LedButton::circleBlue, 30, 30);
    ledbuttonsArea.wdg<LedButton>(LedButton::circleGreen, 30, 30);
    ledbuttonsArea.wdg<LedButton>(LedButton::circleGray, 30, 30);
    ledbuttonsArea.wdg<LedButton>(LedButton::circleOrange, 30, 30);
    ledbuttonsArea.wdg<LedButton>(LedButton::circleRed, 30, 30);
    ledbuttonsArea.wdg<LedButton>(LedButton::circleYellow, 30, 30);
    ledbuttonsArea.wdg<LedButton>(LedButton::circlePurple, 30, 30);

    w = new Window(this, "Basic widgets");
    w->setPosition(Vector2i(200, 15));
    w->setLayout(new GroupLayout());

    new Label(w, "Message dialog", "sans-bold");
    tools = new Widget(w);
    tools->setLayout(new BoxLayout(Orientation::Horizontal,
      Alignment::Middle, 0, 6));
    b = new Button(tools, "Info");
    b->setCallback([&] {
      auto dlg = new MessageDialog(this, MessageDialog::Type::Information, "Title", "This is an information message");
      dlg->setCallback([](int result) { cout << "Dialog result: " << result << endl; });
    });
    b = new Button(tools, "Warn");
    b->setCallback([&] {
      auto dlg = new MessageDialog(this, MessageDialog::Type::Warning, "Title", "This is a warning message");
      dlg->setCallback([](int result) { cout << "Dialog result: " << result << endl; });
    });
    b = new Button(tools, "Ask");
    b->setCallback([&] {
      auto dlg = new MessageDialog(this, MessageDialog::Type::Warning, "Title", "This is a question message", "Yes", "No", true);
      dlg->setCallback([](int result) { cout << "Dialog result: " << result << endl; });
    });

    vector<pair<int, string>> icons = loadImageDirectory(mNVGContext, "icons");
#if defined(_WIN32)
    string resourcesFolderPath("../resources/");
#else
    string resourcesFolderPath("./");
#endif

    new Label(w, "Image panel & scroll panel", "sans-bold");
    PopupButton *imagePanelBtn = new PopupButton(w, "Image Panel");
    imagePanelBtn->setIcon(ENTYPO_ICON_FOLDER);
    popup = imagePanelBtn->popup();
    VScrollPanel *vscroll = new VScrollPanel(popup);
    ImagePanel *imgPanel = new ImagePanel(vscroll);
    imgPanel->setImages(icons);
    popup->setFixedSize(Vector2i(245, 150));

    auto imageWindow = new Window(this, "Selected image");
    imageWindow->setPosition(Vector2i(710, 15));
    imageWindow->setLayout(new GroupLayout());

    // Load all of the images by creating a GLTexture object and saving the pixel data.
    for (auto& icon : icons) {
      auto fullpath = resourcesFolderPath + icon.second + ".png";
      auto data = nvgCreateImage(mNVGContext, fullpath.c_str(), 0);
      mImagesData.emplace_back(data, fullpath);
    }

    // Set the first texture
    auto imageView = new ImageView(imageWindow, mImagesData[0].first);
    mCurrentImage = 0;
    // Change the active textures.
    imgPanel->setCallback([this, imageView](int i) {
      imageView->bindImage(mImagesData[i].first);
      mCurrentImage = i;
      cout << "Selected item " << i << '\n';
    });
    imageView->setGridThreshold(20);
    imageView->setPixelInfoThreshold(20);
    imageView->setPixelInfoCallback(
      [this, imageView](const Vector2i& index) -> pair<string, Color> {
      auto& imageData = mImagesData[mCurrentImage].second;
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
      return{ stringData, textColor };
    });

    new Label(w, "File dialog", "sans-bold");
    tools = new Widget(w);
    tools->setLayout(new BoxLayout(Orientation::Horizontal,
      Alignment::Middle, 0, 6));
    b = new Button(tools, "Open");
    b->setCallback([&] {
      cout << "File dialog result: " << file_dialog(
      { {"png", "Portable Network Graphics"}, {"txt", "Text file"} }, false) << endl;
    });
    b = new Button(tools, "Save");
    b->setCallback([&] {
      cout << "File dialog result: " << file_dialog(
      { {"png", "Portable Network Graphics"}, {"txt", "Text file"} }, true) << endl;
    });

    new Label(w, "Combo box", "sans-bold");
    new DropdownBox(w, { "Dropdown item 1", "Dropdown item 2", "Dropdown item 3" });
    new ComboBox(w, { "Combo box item 1", "Combo box item 2", "Combo box item 3" });
    new Label(w, "Check box", "sans-bold");
    CheckBox *cb = new CheckBox(w, "Flag 1",
      [](bool state) { cout << "Check box 1 state: " << state << endl; }
    );
    cb->setChecked(true);
    cb->setUncheckedColor(Color(128, 0, 0, 255));
    cb->setCheckedColor(Color(0, 128, 0, 255));
    cb->setPushedColor(Color(128, 128, 0, 255));
    cb = new CheckBox(w, "Flag 2",
      [](bool state) { cout << "Check box 2 state: " << state << endl; }
    );
    new Label(w, "Progress bar", "sans-bold");
    mProgress = new ProgressBar(w);

    new Label(w, "Slider and text box", "sans-bold");

    Widget *panel = new Widget(w);
    panel->setLayout(new BoxLayout(Orientation::Horizontal,
      Alignment::Middle, 0, 20));

    Slider *slider = new Slider(panel);
    slider->setValue(0.5f);
    slider->setFixedWidth(80);

    TextBox *textBox = new TextBox(panel);
    textBox->setFixedSize(Vector2i(60, 25));
    textBox->setValue("50");
    textBox->setUnits("%");
    slider->setCallback([textBox](float value) {
      textBox->setValue(std::to_string((int)(value * 100)));
    });
    slider->setFinalCallback([&](float value) {
      cout << "Final slider value: " << (int)(value * 100) << endl;
    });
    textBox->setFixedSize(Vector2i(60, 25));
    textBox->setFontSize(20);
    textBox->setAlignment(TextBox::Alignment::Right);

    w->label(Caption{ "Spinners" }, CaptionFont{"sans-bold"});
    auto& spinners = w->widget().boxlayout(Orientation::Horizontal,Alignment::Middle, 0, 6);
    spinners.spinner(FixedSize{ 40, 40 });
    spinners.spinner(SpinnerSpeed{ 0.5f }, FixedSize{ 40, 40 });
    spinners.spinner(SpinnerSpeed{ -0.7f }, FixedSize{ 40, 40 });

    {
      new Label(w, "Dial and text box", "sans-bold");

      panel = new Widget(w);
      panel->setLayout(new BoxLayout(Orientation::Horizontal,
        Alignment::Middle, 0, 20));

      Dial *dial = new Dial(panel);
      dial->setValue(0.01f);
      dial->setFixedWidth(80);

      auto dialTextBox = new TextBox(panel);
      dialTextBox->setFixedSize(Vector2i(60, 25));
      dialTextBox->setValue("0.01");
      dial->setCallback([dialTextBox](float value) {
        value = 0.01f + 99.99f*powf(value, 5.0f);
        std::ostringstream sval;
        sval.precision(2); sval << std::fixed << value;
        dialTextBox->setValue(sval.str());
      });
      dial->setFinalCallback([&, d = dial](float value) {
        d->setHighlightedRange(std::pair<float, float>(0.0f, value));
        value = 0.01f + 99.99f*powf(value, 5.0f);
        cout << "Final dial value: " << value << endl;
      });
      dialTextBox->setFixedSize(Vector2i(60, 25));
      dialTextBox->setFontSize(20);
      dialTextBox->setAlignment(TextBox::Alignment::Right);
    }

    w = new Window(this, "Misc. widgets");
    w->setPosition(Vector2i(425, 15));
    w->setLayout(new GroupLayout());

    TabWidget* tabWidget = w->add<TabWidget>();

    Widget* layer = tabWidget->createTab("Color Wheel");
    layer->setLayout(new GroupLayout());

    // Use overloaded variadic add to fill the tab widget with Different tabs.
    layer->add<Label>("Color wheel widget", "sans-bold");
    layer->add<ColorWheel>();

    layer = tabWidget->createTab("Function Graph");
    layer->setLayout(new GroupLayout());

    layer->add<Label>("Function graph widget", "sans-bold");

    Graph *graph = layer->add<Graph>("Some Function");

    graph->setHeader("E = 2.35e-3");
    graph->setFooter("Iteration 89");
    VectorXf &func = graph->values();
    func.resize(100);
    for (int i = 0; i < 100; ++i)
      func[i] = 0.5f * (0.5f * std::sin(i / 10.f) +
        0.5f * std::cos(i / 23.f) + 1);

    // Dummy tab used to represent the last tab button.
    tabWidget->createTab("+");

    // A simple counter.
    int counter = 1;
    tabWidget->setCallback([tabWidget, this, counter](int index) mutable {
      if (index == (tabWidget->tabCount() - 1)) {
        // When the "+" tab has been clicked, simply add a new tab.
        string tabName = "Dynamic " + to_string(counter);
        Widget* layerDyn = tabWidget->createTab(index, tabName);
        layerDyn->setLayout(new GroupLayout());
        layerDyn->add<Label>("Function graph widget", "sans-bold");
        Graph *graphDyn = layerDyn->add<Graph>("Dynamic function");

        graphDyn->setHeader("E = 2.35e-3");
        graphDyn->setFooter("Iteration " + to_string(index*counter));
        VectorXf &funcDyn = graphDyn->values();
        funcDyn.resize(100);
        for (int i = 0; i < 100; ++i)
          funcDyn[i] = 0.5f *
          std::abs((0.5f * std::sin(i / 10.f + counter) +
            0.5f * std::cos(i / 23.f + 1 + counter)));
        ++counter;
        // We must invoke perform layout from the screen instance to keep everything in order.
        // This is essential when creating tabs dynamically.
        performLayout();
        // Ensure that the newly added header is visible on screen
        tabWidget->ensureTabVisible(index);

      }
    });
    tabWidget->setActiveTab(0);

    // A button to go back to the first tab and scroll the window.
    panel = w->add<Widget>();
    panel->add<Label>("Jump to tab: ");
    panel->setLayout(new BoxLayout(Orientation::Horizontal,
      Alignment::Middle, 0, 6));

    auto ib = panel->add<IntBox<int>>();
    ib->setEditable(true);

    b = panel->add<Button>(Caption{ "" }, Icon{ ENTYPO_ICON_FORWARD });
    b->setFixedSize(Vector2i(22, 22));
    ib->setFixedHeight(22);
    b->setCallback([tabWidget, ib] {
      int value = ib->value();
      if (value >= 0 && value < tabWidget->tabCount()) {
        tabWidget->setActiveTab(value);
        tabWidget->ensureTabVisible(value);
      }
    });

    w = new Window(this, "Grid of small widgets");
    w->setPosition(Vector2i(425, 300));
    GridLayout *layout =
      new GridLayout(Orientation::Horizontal, 2,
        Alignment::Middle, 15, 5);
    layout->setColAlignment(
    { Alignment::Maximum, Alignment::Fill });
    layout->setSpacing(0, 10);
    w->setLayout(layout);

    /* FP widget */ {
      new Label(w, "Floating point :", "sans-bold");
      textBox = new TextBox(w);
      textBox->setEditable(true);
      textBox->setFixedSize(Vector2i(100, 20));
      textBox->setValue("50");
      textBox->setUnits("GiB");
      textBox->setDefaultValue("0.0");
      textBox->setFontSize(16);
      textBox->setFormat("[-]?[0-9]*\\.?[0-9]+");
    }

    /* Positive integer widget */ {
      new Label(w, "Positive integer :", "sans-bold");
      auto intBox = new IntBox<int>(w);
      intBox->setEditable(true);
      intBox->setFixedSize(Vector2i(100, 20));
      intBox->setValue(50);
      intBox->setUnits("Mhz");
      intBox->setDefaultValue("0");
      intBox->setFontSize(16);
      intBox->setFormat("[1-9][0-9]*");
      intBox->setSpinnable(true);
      intBox->setMinValue(1);
      intBox->setValueIncrement(2);
    }

    /* Checkbox widget */ {
      new Label(w, "Checkbox :", "sans-bold");

      cb = new CheckBox(w, "Check me");
      cb->setFontSize(16);
      cb->setChecked(true);
    }

    new Label(w, "Combo box :", "sans-bold");
    ComboBox *cobo =
      new ComboBox(w, { "Item 1", "Item 2", "Item 3" });
    cobo->setFontSize(16);
    cobo->setFixedSize(Vector2i(100, 20));

    new Label(w, "Color picker :", "sans-bold");
    auto cp = new ColorPicker(w, { 255, 120, 0, 255 });
    cp->setFixedSize({ 100, 20 });
    cp->setFinalCallback([](const Color &c) {
      std::cout << "ColorPicker Final Callback: ["
        << c.r() << ", "
        << c.g() << ", "
        << c.b() << ", "
        << c.w() << "]" << std::endl;
    });
    // setup a fast callback for the color picker widget on a new window
    // for demonstrative purposes
    w = new Window(this, "Color Picker Fast Callback");
    layout =
      new GridLayout(Orientation::Horizontal, 2,
        Alignment::Middle, 15, 5);
    layout->setColAlignment(
    { Alignment::Maximum, Alignment::Fill });
    layout->setSpacing(0, 10);
    w->setLayout(layout);
    w->setPosition(Vector2i(425, 500));
    new Label(w, "Combined: ");
    w->button(Caption{ "ColorWheel" }, Icon{ ENTYPO_ICON_500PX });
    new Label(w, "Red: ");
    auto redIntBox = new IntBox<int>(w);
    redIntBox->setEditable(false);
    new Label(w, "Green: ");
    auto greenIntBox = new IntBox<int>(w);
    greenIntBox->setEditable(false);
    new Label(w, "Blue: ");
    auto blueIntBox = new IntBox<int>(w);
    blueIntBox->setEditable(false);
    new Label(w, "Alpha: ");
    auto alphaIntBox = new IntBox<int>(w);
    cp->setCallback([b, redIntBox, blueIntBox, greenIntBox, alphaIntBox](const Color &c) {
      b->setBackgroundColor(c);
      b->setTextColor(c.contrastingColor());
      int red = (int)(c.r() * 255.0f);
      redIntBox->setValue(red);
      int green = (int)(c.g() * 255.0f);
      greenIntBox->setValue(green);
      int blue = (int)(c.b() * 255.0f);
      blueIntBox->setValue(blue);
      int alpha = (int)(c.w() * 255.0f);
      alphaIntBox->setValue(alpha);

    });

    //all widgets demo
    {
      Window& dw = window(WindowSimpleLayout{ Orientation::Horizontal },
                          Caption{ "All widgets demo" },
                          Position{725, 350},
                          FixedSize{400, 400});
      dw.submenu("File")
          .item("(dummy item)", []() {})
          .item("New", "Ctrl+N", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "New", "New Clicked!"); })
          .item("Very larget text", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Open", "New Clicked!"); })
          .item("Save", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Save", "New Clicked!"); });
      dw.submenu("File").item("(dummy item)").setEnabled(false);
      dw.submenu("File").item("Save").setShortcut("Ctrl+S");

      dw.submenu("Examples")
          .item("Global menu", [this](bool v) { toggleMainMenu(v); })
          .item("Console",     [this](bool v) { toggleConsoleWnd(v); }, [this](bool &enabled, bool &checked) {
                                                      enabled = true; 
                                                      auto* w = findWidgetGlobal("console_wnd");
                                                      checked = (w && w->visible());
                                                  })
          .item("Log", [this](bool v) { toggleLogWnd(v); }, [this](bool &enabled, bool &checked) {
                                                    enabled = true; 
                                                    auto* w = findWidgetGlobal("log_wnd");
                                                    checked = (w && w->visible());
                                                  })
          .item("Simple layout", [this](bool v) { toggleSimpleLayoutWnd(v); }, [this](bool &enabled, bool &checked) {
                                                    enabled = true; 
                                                    auto* w = findWidgetGlobal("simple_layout_wnd");
                                                    checked = (w && w->visible());
                                                  });
      dw.submenu("Help");
    }

      fpsGraph = &wdg<PerfGraph>(GRAPH_RENDER_FPS, "Frame Time", Vector2i(5, height() - 40 ));
      cpuGraph = &wdg<PerfGraph>(GRAPH_RENDER_MS, "CPU Time", Vector2i(5, height() - 40 * 2));
      gpuGraph = &wdg<PerfGraph>(GRAPH_RENDER_MS, "GPU Time", Vector2i(5, height() - 40 * 3));

      previousFrameTime = getTimeFromStart();
      performLayout();
    }

    ~ExampleApplication() {}

    void toggleConsoleWnd(bool show)
    {
      using namespace nanogui;

      auto console = findWidgetGlobal("console_wnd");
      if (!console)
      {
        auto& wnd = window("Example: Console");
        wnd.setPosition(60, 60);
        wnd.setLayout(new GroupLayout());
        wnd.setFixedSize({ 300, 300 });
        wnd.setId("console_wnd");
        performLayout();
      }
      else
      {
        console->setVisible(show);
      }
    }
    
    void toggleSimpleLayoutWnd(bool show)
    {
      using namespace nanogui;

      auto logwnd = findWidgetGlobal("simple_layout_wnd");
      if (!logwnd)
      {
        auto& wnd = window("Example: Simple layout");
        wnd.flexlayout(Orientation::Horizontal);
        wnd.setPosition(180, 180);
        wnd.setFixedSize({ 400, 300 });
        auto& lst = wnd.listbox();
        lst.setRelativeSize(0.33, 0);
        lst.setSelectedCallback([this](ListboxItem* i) {
          Label* lb = findWidget<Label>("#simple_layout_lb");
          if (lb)
            lb->setCaption("MyObject: " + i->caption());
        });
        for (int i = 0; i < 12; i++)
          lst.addItem("Item " + std::to_string(i));
        wnd.setId("simple_layout_wnd");
        auto& desc = wnd.widget();
        desc.flexlayout(Orientation::Vertical);
        desc.label(Caption{ "MyObject: id" }, 
                   CaptionHAlign{ TextHAlign::hLeft },
                   FixedHeight{ 15 },
                   WidgetId{ "#simple_layout_lb" } );
        desc.tabs(TabNames{"Description", "Details"});
        performLayout();
      }
      else
      {
        logwnd->setVisible(show);
      }
    }

    void toggleLogWnd(bool show)
    {
      using namespace nanogui;

      auto logwnd = findWidgetGlobal("log_wnd");
      if (!logwnd)
      {
        auto& wnd = window("Example: Log");
        wnd.setPosition(120, 120);
        wnd.setFixedSize({ 400, 300 });
        wnd.setId("log_wnd");
        performLayout();
      }
      else
      {
        logwnd->setVisible(show);
      }
    }

    void toggleMainMenu(bool show)
    {
      using namespace nanogui;

      auto menus = findAll<WindowMenu>();

      if (menus.empty())
      {
        auto& mmenu = wdg<WindowMenu>();
        mmenu.activate({ 0, 0 });
        mmenu.submenu("File")
                .item("New", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "New", "New Clicked!"); })
                .item("Open", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Open", "New Clicked!"); })
                .item("Save", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Save", "New Clicked!"); });
        mmenu.submenu("Edit")
                .item("Undo", "Ctrl+Z", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "New", "New Clicked!"); })
                .item("Redo", "Ctrl+Y", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "New", "New Clicked!"); })
                .item("Cut", "Ctrl+X", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "New", "New Clicked!"); })
                .item("Copy", "Ctrl+C", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "New", "New Clicked!"); })
                .item("Paste", "Ctrl+V", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "New", "New Clicked!"); });

        performLayout();
      }
      else
      {
        menus.front()->setVisible(show);
      }
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
        /* Animate the scrollbar */
        if (mProgress)
          mProgress->setValue(std::fmod((float) nanogui::getTimeFromStart() / 10, 1.0f));

        startGPUTimer(&gpuTimer);

        double t = nanogui::getTimeFromStart();
        double dt = t - previousFrameTime;
        previousFrameTime = t;

        /* Draw the user interface */
        Screen::draw(ctx);

        cpuTime = nanogui::getTimeFromStart() - t;

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

    bool mouseButtonEvent(const Eigen::Vector2i &p, int button, bool down, int modifiers) override {
        if (Widget::mouseButtonEvent(p, button, down, modifiers))
            return true;
        if(down && nanogui::isMouseButtonRight(button) && findWidget(p)==this) {
            auto menu = new nanogui::ContextMenu(this, "", true);
            menu->addItem("Item 1", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Item 1", "Item 1 Clicked!"); }, ENTYPO_ICON_PLUS);

            auto submenu = menu->addSubMenu("Submenu");
            submenu->addItem("Subitem 1", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Subitem 1", "Subitem 1 Clicked!"); });
            auto subsubmenu = submenu->addSubMenu("Subsubmenu", ENTYPO_ICON_LOOP);
            submenu->addItem("Subitem 2", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Subitem 2", "Subitem 2 Clicked!"); });

            subsubmenu->addItem("Subsubitem 1", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Subsubitem 1", "Subsubitem 1 Clicked!"); });
            subsubmenu->addItem("Subsubitem 2", [this]() { new nanogui::MessageDialog(this, nanogui::MessageDialog::Type::Information, "Subsubitem 2", "Subsubitem 2 Clicked!"); });

            menu->activate(p-mPos);
            performLayout();
        }
        return true;
    }

private:
    nanogui::ProgressBar *mProgress = nullptr;
    //nanogui::GLShader mShader;
    nanogui::PerfGraph *fpsGraph = nullptr;
    nanogui::PerfGraph *cpuGraph = nullptr;
    nanogui::PerfGraph *gpuGraph = nullptr;
    double previousFrameTime = 0, cpuTime = 0;

    using ImagesDataType = vector<pair<int, std::string>>;
    ImagesDataType mImagesData;
    int mCurrentImage;
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
