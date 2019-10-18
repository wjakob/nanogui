#pragma once

#include <nanogui/widget.h>

NAMESPACE_BEGIN(nanogui)

class NANOGUI_EXPORT Listbox : public Widget
{
public:
  Listbox(Widget* parent);

  void addItem(const std::string& str, const std::string& id = "");
  void performLayout(NVGcontext *ctx) override;

  void draw(NVGcontext* ctx) override;
  Vector2i preferredSize(NVGcontext *ctx) const override;

private:
  VScrollPanel* mPanel = nullptr;
  Widget* mItems = nullptr;
};

NAMESPACE_END(nanogui)

