#include <nanogui/searchbox.h>
#include <nanovg.h>
#include <nanogui/button.h>
#include <nanogui/entypo.h>
#include <nanogui/theme.h>
#include <nanogui/serializer/core.h>
#include <regex>
#include <iostream>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(SearchBox, TextBox)

SearchBox::SearchBox(Widget* parent)
  : TextBox(parent, std::string(""))
{
  setPlaceholder("Search");
  setEditable(true);
  mAlignment = Alignment::Left;

  auto& btn = button(Caption{ "" }, 
                     WidgetId{ "#cancel_btn"},
                     Icon{ ENTYPO_ICON_CANCEL_CIRCLED },
                     HoveredColor{ Color(0xff0000ff) },
                     ButtonDrawFlags{ Button::DrawIcon },
                     ButtonCallback{ [this] { clearText();} });
}

int SearchBox::getCornerRadius() const { return mSize.y() / 2 - 1; }
void SearchBox::clearText() { setValue(""); }

void SearchBox::performLayout(NVGcontext* ctx)
{
  TextBox::performLayout(ctx);
  mTextOffset = mSize.y()*0.7f;
  auto* btn = findWidget<Button>("#cancel_btn");
  if (btn)
  {
    btn->setSize(height(), height());
    btn->setFixedSize(height(), height());
    btn->setPosition(width() - height(), 0);
  }
}

void SearchBox::draw(NVGcontext* ctx) {
  NVGpaint bg;

  nvgFontSize(ctx, mSize.y()*0.6f);
  nvgFontFace(ctx, "icons");
  nvgFillColor(ctx, nvgRGBA(255, 255, 255, 64));
  nvgTextAlign(ctx, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
  nvgText(ctx, mPos.x() + mSize.y()*0.55f, mPos.y() + mSize.y()*0.55f, utf8(ENTYPO_ICON_SEARCH).data(), nullptr);

  TextBox::draw(ctx);
}

NAMESPACE_END(nanogui)
