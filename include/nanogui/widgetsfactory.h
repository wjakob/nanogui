#pragma once

#include <nanogui/widget.h>

NAMESPACE_BEGIN(nanogui)

class Widget;

class NANOGUI_EXPORT WidgetFactory
{
public:
  using creatorFunc = std::function<Widget* (Widget*)>;
  static WidgetFactory& instance();

  void registerFactory(WidgetFactory* anotherFactory);
  void registerCreator(std::string wtypename, creatorFunc creator);
  
  virtual WidgetFactory* factory(size_t index) const;
  virtual std::vector<WidgetFactory*> factories() const;
  virtual Widget* createWidget(const std::string& wtypename, Widget* parent);
  virtual std::vector<std::string> types() const { return {}; }
  virtual std::string name() const { return "global"; }

protected:
  WidgetFactory();
  virtual ~WidgetFactory();
  WidgetFactory(WidgetFactory&) = delete;
};

NAMESPACE_END(nanogui)
