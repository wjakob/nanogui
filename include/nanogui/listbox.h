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


class NANOGUI_EXPORT Listbox : public Widget
{
public:
  Listbox(Widget* parent);

  void addItem(const std::string& str, const std::string& id = "");
  void performLayout(NVGcontext *ctx) override;

  void draw(NVGcontext* ctx) override;
  Vector2i preferredSize(NVGcontext *ctx) const override;

  void setSelectedCallback(std::function<void(ListboxItem*)> f) { mSelectCallback = f; }
  void selectItem(ListboxItem* item);

private:
  VScrollPanel* mPanel = nullptr;
  Widget* mItems = nullptr;

  std::function<void(ListboxItem*)> mSelectCallback;
};

NAMESPACE_END(nanogui)

