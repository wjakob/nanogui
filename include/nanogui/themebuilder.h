#pragma once

#include <nanogui/window.h>
#include <memory>

NAMESPACE_BEGIN(nanogui)

class ScrollBar;
class TextBox;

class NANOGUI_EXPORT ThemeBuilder : public Window
{
public:
  using Window::set;
  template<typename... Args>
  ThemeBuilder(Widget* parent, const Args&... args)
    : ThemeBuilder(parent) { set<Label, Args...>(args...); }

  void colorPicker(Widget& parent, std::string name);

  ThemeBuilder(Widget* screen);
  Theme* mDynTheme;
};

NAMESPACE_END(nanogui)
