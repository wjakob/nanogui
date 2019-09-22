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
  size_t factoriesCount() const;
  WidgetFactory* getFactory(int index) const;

  virtual Widget* createWidget(const std::string& wtypename, Widget* parent);

protected:
  WidgetFactory();
  virtual ~WidgetFactory();
  WidgetFactory(WidgetFactory&) = delete;
};

NAMESPACE_END(nanogui)
