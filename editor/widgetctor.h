#include <nanogui/button.h>

NAMESPACE_BEGIN(nanogui)

class WidgetCtor : public Button
{
public:
  RTTI_CLASS_UID("WCTR")
  RTTI_DECLARE_INFO(WidgetCtor)

  using Button::set;
  template<typename... Args>
  WidgetCtor(Widget* parent, const Args&... args)
    : Button(parent) { set<WidgetCtor, Args...>(args...); }  

  std::string mWtype;
};

NAMESPACE_END(nanogui)
