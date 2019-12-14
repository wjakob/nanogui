#include <nanogui/themebuilder.h>
#include <nanogui/entypo.h>
#include <nanogui/tabwidget.h>
#include <nanogui/vscrollpanel.h>
#include <nanogui/theme.h>
#include <nanogui/screen.h>
#include <nanogui/label.h>
#include <nanogui/colorpicker.h>
#include <nanogui/colorwheel.h>
#include <nanogui/textbox.h>
#include <nanogui/button.h>
#include <nanogui/popup.h>
#include <map>
#include <iostream>

NAMESPACE_BEGIN(nanogui)

class NANOGUI_EXPORT ExplicitColorPicker : public ColorPicker
{
public:
  RTTI_CLASS_UID("ECPK")
  RTTI_DECLARE_INFO(ExplicitColorPicker)

  Color MARKED_COLOR = Color(0, 0, 0, 0);
  static std::vector<ExplicitColorPicker*> ALL_PICKERS;
  bool marked = false;

  IntBox<int>* textColor(Widget& wrapper, std::string text, float initValue)
  {
    wrapper.label(Caption{ text });
    auto& box = wrapper.intbox<int>(InitialValue{ initValue * 255.f }
                               /*FixedWidth{80},
                               MinValue{0.f}, MaxValue{255.f},
                               IsSpinnable{ true }, IsEditable{ true }*/ );
    //WTF box.setFont("mono");
    box.setMinValue(0);
    box.setMaxValue(255);
    box.setSpinnable(true);
    box.setEditable(true);
    return &box;
  };

  IntBox<int>* redBox = nullptr;
  IntBox<int>* greenBox = nullptr;
  IntBox<int>* blueBox = nullptr;
  IntBox<int>* alphaBox = nullptr;
  std::function<void(const Color&)> cb;

  ExplicitColorPicker(Widget* parent, const Color& clr,std::function<void (const Color&)> callback)
    : ColorPicker(parent, clr)
  {
    ALL_PICKERS.push_back(this);
    auto* mpopup = popup();
    marked = false;

      // really dirty hack to fix the sizes of the buttons created by the parent
    mpopup->forEachChild([](Widget* w) {
      if (auto* child = Button::cast(w))
        child->setFixedWidth(180);
    });

    auto& wrapper = mpopup->widget();
    wrapper.setLayout(new GridLayout());

    redBox = textColor(wrapper, "Red : ", clr.r());
    greenBox = textColor(wrapper, "Green : ", clr.g());
    blueBox = textColor(wrapper, "Blue : ", clr.b());
    alphaBox = textColor(wrapper, "Alpha : ", clr.w());

    auto red_cb = [this](float val) {
      float r = float(val) / 255.0;
      float g = float(greenBox->value()) / 255.0;
      float b = float(blueBox->value()) / 255.0;
      float a = float(alphaBox->value()) / 255.0;
      reset(Color(r, g, b, a), true);
    };
    redBox->setCallback(red_cb);

    auto green_cb = [this](float val) {
      float r = float(redBox->value()) / 255.0;
      float g = float(val) / 255.0;
      float b = float(blueBox->value()) / 255.0;
      float a = float(alphaBox->value()) / 255.0;
      reset(Color(r, g, b, a), true);
    };
    greenBox->setCallback(green_cb);

    auto blue_cb = [this](float val) {
      float r = float(redBox->value()) / 255.0;
      float g = float(greenBox->value()) / 255.0;
      float b = float(val) / 255.0;
      float a = float(alphaBox->value()) / 255.0;
      reset(Color(r, g, b, a), true);
    };
    blueBox->setCallback(blue_cb);

    auto alpha_cb = [this](float val) {
      float r = float(redBox->value()) / 255.0;
      float g = float(greenBox->value()) / 255.0;
      float b = float(blueBox->value()) / 255.0;
      float a = float(val) / 255.0;
      reset(Color(r, g, b, a), true);
    };
    alphaBox->setCallback(alpha_cb);

    {
      auto& b = wrapper.button("Darken 5%");
      //WTF b.setFont("sans");
      auto darken_cb = [this]() {
        Color curr = color();
        float r = curr.r() - (0.05 * curr.r());
        float g = curr.g() - (0.05 * curr.g());
        float b = curr.b() - (0.05 * curr.b());
        Color next_color = Color(r, g, b, curr.w());
        master_callback(next_color);

        popup()->forEachChild([&](Widget* child) {
          if (Button *btn = Button::cast(child))
          {
            if (btn->caption() == "Pick")
            {
              btn->setBackgroundColor(next_color);
              btn->setTextColor(next_color.contrastingColor());
            }
          }
          else if (ColorWheel* wheel = ColorWheel::cast(child))
          {
            wheel->setColor(next_color);
          }
        });
      };
      b.setCallback(darken_cb);
    }

    {
      auto& b = wrapper.button("Lighten 5%");
      // WTF b.setFont("sans");
      auto lighten_cb = [this]() {
        Color curr = color();
        float r = curr.r() + (0.05 * (1.0 - curr.r()));
        float g = curr.g() + (0.05 * (1.0 - curr.g()));
        float b = curr.b() + (0.05 * (1.0 - curr.b()));
        Color next_color = Color(r, g, b, curr.w());
        master_callback(next_color);

        popup()->forEachChild([&](Widget* child) {
          if (Button* btn = Button::cast(child))
          {
            if (btn->caption() == "Pick")
            {
              btn->setBackgroundColor(next_color);
              btn->setTextColor(next_color.contrastingColor());
            }
          }
          else if (ColorWheel* wheel = ColorWheel::cast(child))
          {
            wheel->setColor(next_color);
          }
        });
      };

      b.setCallback(lighten_cb);
    }

    cb = callback;
    setCallback([this](const Color& x) { master_callback(x); });
    auto final_cb = [this](const Color& color) {
      if (marked)
      {
        MARKED_COLOR = color;
        master_callback(color);
      }
    };
    setFinalCallback(final_cb);
  }


  void master_callback(const Color& clr)
  {
    cb(clr);
    setBackgroundColor(clr);
    setTextColor(color().contrastingColor());
    redBox->setValue(int(clr.r() * 255.0));
    greenBox->setValue(int(clr.g() * 255.0));
    blueBox->setValue(int(clr.b() * 255.0));
    alphaBox->setValue(int(clr.w() * 255.0));
  }

  void reset(const Color& color, bool fromTextBox = false)
  {
    master_callback(color);
    //Same as ColorPicker::setColor, only forcibly changes(no check for mPushed)
    Color fg = color.contrastingColor();
    setBackgroundColor(color);
    setTextColor(fg);
    popup()->forEachChild([&](Widget* child) {
      if (Button* btn = Button::cast(child))
      {
        bool change = true;
        if (fromTextBox && btn->caption() == "Reset")
          change = false;
        if (change)
        {
          btn->setBackgroundColor(color);
          btn->setTextColor(fg);
        }
      }
      else if (ColorWheel* wheel = ColorWheel::cast(child))
      {
        wheel->setColor(color);
      }
    });
  }

  void setMarked(bool m)
  {
    if (m)
    {
      MARKED_COLOR = color();
      marked = true;
      setCaption("marked");
    }
    else
    {
      MARKED_COLOR = Color(0, 0, 0, 0);
      marked = false;
      setCaption("");
    }
  }

  void mark(ExplicitColorPicker* picker)
  {
    for (auto& p : ALL_PICKERS)
    {
      if (p != picker)
        p->setMarked(false);
    }

    picker->setMarked(!picker->marked);
  }

  bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override
  {
    if (down && isMouseButtonLeft(button))
    {
      if (isKeyboardModifierShift(modifiers))
      {
        mark(this);
        return true;
      }
      else if (isKeyboardModifierCtrl(modifiers))
      {
        if (MARKED_COLOR != Color(0, 0, 0, 0))
          reset(MARKED_COLOR);
        else
          std::cout << "WTF" << std::endl;

        return true;
      }
    }

    return ColorPicker::mouseButtonEvent(p, button, down, modifiers);
  }
};

RTTI_IMPLEMENT_INFO(ExplicitColorPicker, ColorPicker)

std::vector<ExplicitColorPicker*> ExplicitColorPicker::ALL_PICKERS;
namespace {
  std::map<std::string, ExplicitColorPicker*> color_pickers;
  std::map<std::string, TextBox*> number_pickers;
}

void ThemeBuilder::colorPicker(Widget& parent, std::string name)
{
  auto& wrapper = parent.widget();
  wrapper.flexlayout(Orientation::Horizontal);
  wrapper.label(Caption{ name }, CaptionFont{ "sans" }, RelativeSize{0.5, 0});

  Color val = mDynTheme->get<Color>(name);
  auto cp_func = [this, name](const Color& color) {
    mDynTheme->set<Color>(name, color);
    screen()->performLayout();
  };

  auto& cp = wrapper.wdg<ExplicitColorPicker>(val, cp_func);
  cp.setRelativeSize(0.4f, 0);

  color_pickers[name] = &cp;

  auto& breset = wrapper.button(Caption{ "" }, Icon{ ENTYPO_ICON_BACK });
  breset.setTooltip("Reset to NanoGUI Default?");
    //val is going to change with the color picker since the theme member
    //variable passed here is a reference(? ), save the values now and create a
    //new color instance inside the callback
  float r = val.r();
  float g = val.g();
  float b = val.b();
  float a = val.w();
  auto reset_cb = [&]() { cp.reset(Color(r, g, b, a)); };

  breset.setCallback(reset_cb);
}


FloatBox<float>& propEditor(Widget& parent,float v) { return parent.wdg<FloatBox<float>>(v); }
IntBox<int>& propEditor(Widget& parent,int v) { return parent.wdg<IntBox<int>>(v); }

template<typename ValueType>
void numberPicker(Widget& parent, Theme* dynamicTheme, std::string name)
{
  auto& wrapper = parent.widget();
  wrapper.flexlayout(Orientation::Horizontal);
  wrapper.label(Caption{ name },
                CaptionFont{ "sans" },
                RelativeSize{ 0.5, 0 } );

  auto val = dynamicTheme->get<ValueType>(name);
  auto& box = propEditor(wrapper, val);
  box.setValue(val);
  box.setSpinnable(true);
  box.setEditable(true);
  box.setDefaultValue(std::to_string(val));

  auto set_value = [&,dynamicTheme,name](ValueType v) {
    dynamicTheme->set<ValueType>(name, v);
    parent.screen()->performLayout();
  };

  box.setCallback(set_value);
  box.setRelativeSize(0.4f, 0);
  // WTF box.setFont("mono");

  number_pickers[name] = &box;

  auto& reset = wrapper.button(Caption{ "" }, Icon{ ENTYPO_ICON_BACK });
  reset.setTooltip("Reset to NanoGUI Default?");
  auto reset_cb = [&] () {
    box.setValue(val);
    set_value(val);
  };

  reset.setCallback(reset_cb);
}

ThemeBuilder::ThemeBuilder(Widget* parent)
  : Window(parent)
{
  mDynTheme = theme();

  setTitle("Theme Settings");
  flexlayout(Orientation::Horizontal);
  setSize( 400, 425 );

  auto& tabWidget = this->tabs();
  tabWidget.setFixedSize({ width(), height() });

  // Font Sizes
  {
    auto& layerSizes = *tabWidget.createTab("Sizes");
    auto& vscroll = layerSizes.vscrollpanel();
    auto& wrapper = vscroll.widget();
    wrapper.setLayout(new GroupLayout(5, 5, 5, 10));
    vscroll.setFixedSize({ width() - 8, height() - 60 });

    wrapper.label(Caption{ "Font Sizes" }, CaptionFont{ "sans-bold" });
    numberPicker<int>(wrapper, mDynTheme, "mStandardFontSize");
    numberPicker<int>(wrapper, mDynTheme, "mButtonFontSize");
    numberPicker<int>(wrapper, mDynTheme, "mTextBoxFontSize");
    numberPicker<int>(wrapper, mDynTheme, "mWindowFontSize");
    // Curvature Sizes
    wrapper.label(Caption{ "Curvature Sizes" }, CaptionFont{ "sans-bold" });
    numberPicker<int>(wrapper, mDynTheme, "mWindowCornerRadius");
    numberPicker<int>(wrapper, mDynTheme, "mWindowHeaderHeight");
    numberPicker<int>(wrapper, mDynTheme, "mWindowDropShadowSize");
    numberPicker<int>(wrapper, mDynTheme, "mButtonCornerRadius");
    // TabWidget Sizes
    wrapper.label(Caption{ "TabWidget Sizes" }, CaptionFont{ "sans-bold" });
    numberPicker<int>(wrapper, mDynTheme, "mTabBorderWidth");
    numberPicker<int>(wrapper, mDynTheme, "mTabInnerMargin");
    numberPicker<int>(wrapper, mDynTheme, "mTabMinButtonWidth");
    numberPicker<int>(wrapper, mDynTheme, "mTabMaxButtonWidth");
    numberPicker<int>(wrapper, mDynTheme, "mTabControlWidth");
    numberPicker<int>(wrapper, mDynTheme, "mTabButtonHorizontalPadding");
    numberPicker<int>(wrapper, mDynTheme, "mTabButtonVerticalPadding");
  }
  // Core Colors
  {
    auto& layerColors = *tabWidget.createTab("Colors");
    auto& vscroll = layerColors.vscrollpanel();
    auto& wrapper = vscroll.widget();
    wrapper.setLayout(new GroupLayout(5, 5, 5, 10));
    vscroll.setFixedSize({ width() - 8, height() - 60 });

    wrapper.label(Caption{ "Help" }, CaptionFont{ "sans-bold" });
    auto& help_wrapper = wrapper.widget();
    help_wrapper.setLayout(new GroupLayout(5, 5, 5, 10));
    help_wrapper.label(Caption{ "<shift + left click> : mark / un-mark color for copy" });
    help_wrapper.label(Caption{ "<ctrl + left click> : paste color (if one is marked)" });

    wrapper.label(Caption{ "Core Colors" }, CaptionFont{ "sans-bold" });
    colorPicker(wrapper, "mDropShadow");
    colorPicker(wrapper, "mTransparent");
    colorPicker(wrapper, "mBorderDark");
    colorPicker(wrapper, "mBorderLight");
    colorPicker(wrapper, "mBorderMedium");
    colorPicker(wrapper, "mTextColor");
    colorPicker(wrapper, "mDisabledTextColor");
    colorPicker(wrapper, "mTextColorShadow");
    colorPicker(wrapper, "mIconColor");
    // Button Colors
    wrapper.label(Caption{ "Button Colors" }, CaptionFont{ "sans-bold" });
    colorPicker(wrapper, "mButtonGradientTopFocused");
    colorPicker(wrapper, "mButtonGradientBotFocused");
    colorPicker(wrapper, "mButtonGradientTopUnfocused");
    colorPicker(wrapper, "mButtonGradientBotUnfocused");
    colorPicker(wrapper, "mButtonGradientTopPushed");
    colorPicker(wrapper, "mButtonGradientBotPushed");
      // Tooltip Colors
    wrapper.label("Tooltip Colors", "sans-bold");
    numberPicker<int>(wrapper, mDynTheme, "mTooltipOpacity");
    colorPicker(wrapper, "mTooltipBackgroundColor");
    colorPicker(wrapper, "mTooltipTextColor");
      // Window Colors
    wrapper.label("Window Colors", "sans-bold");
    colorPicker(wrapper, "mWindowFillUnfocused");
    colorPicker(wrapper, "mWindowFillFocused");
    colorPicker(wrapper, "mWindowTitleUnfocused");
    colorPicker(wrapper, "mWindowTitleFocused");
    colorPicker(wrapper, "mWindowHeaderGradientTop");
    colorPicker(wrapper, "mWindowHeaderGradientBot");
    colorPicker(wrapper, "mWindowHeaderSepTop");
    colorPicker(wrapper, "mWindowHeaderSepBot");
    colorPicker(wrapper, "mWindowPopup");
    colorPicker(wrapper, "mWindowPopupTransparent");
  }
}

NAMESPACE_END(nanogui)
