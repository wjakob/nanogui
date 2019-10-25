#pragma once

#include <nanogui/button.h>

NAMESPACE_BEGIN(nanogui)

class NANOGUI_EXPORT ListboxItem : public Button
{
public:
  ListboxItem(Widget* parent, const std::string& str, bool inlist = true);
  void performLayout(NVGcontext *ctx) override;

  void draw(NVGcontext *ctx) override;
  void beforeDoCallback() override;
};


DECLSETTER(ListboxCallback, std::function<void(ListboxItem*)>)
DECLSETTER(ListboxContent, std::function<void(Listbox&)>)

class NANOGUI_EXPORT Listbox : public Widget
{
public:
  Listbox(Widget* parent);

  using Widget::set;
  template<typename... Args>
  Listbox(Widget* parent, const Args&... args)
    : Listbox(parent) { set<Listbox, Args...>(args...); }

  void addItem(const std::string& str, const std::string& id = "");
  void performLayout(NVGcontext *ctx) override;

  void draw(NVGcontext* ctx) override;
  Vector2i preferredSize(NVGcontext *ctx) const override;

  void setSelectedCallback(std::function<void(ListboxItem*)> f) { mSelectCallback = f; }
  void selectItem(ListboxItem* item);

  void addContent(std::function<void(Listbox&)> f);

private:
  VScrollPanel* mPanel = nullptr;
  Widget* mItems = nullptr;

  std::function<void(ListboxItem*)> mSelectCallback;

public:
  PROPSETTER(ListboxCallback,setSelectedCallback)
  PROPSETTER(ListboxContent, addContent)
};

NAMESPACE_END(nanogui)

