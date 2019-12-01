#include <nanogui/listbox.h>
#include <nanogui/vscrollpanel.h>
#include <nanogui/theme.h>
#include <nanogui/button.h>
#include <nanogui/layout.h>
#include <nanovg.h>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(ListboxItem, Button)
RTTI_IMPLEMENT_INFO(Listbox, Widget)

ListboxItem::ListboxItem(Widget* parent, const std::string& str, bool inlist)
    : Button(parent, str) {}

void ListboxItem::performLayout(NVGcontext *ctx)
{
  Button::performLayout(ctx);
  setWidth(parent()->width());
}

void ListboxItem::beforeDoCallback()
{
  if (Listbox* lst = findParent<Listbox>())
  {
    lst->selectItem(this);
  }
}

void ListboxItem::draw(NVGcontext *ctx)
{
  if (mMouseFocus && mEnabled)
  {
    NVGcolor gradTop = mTheme->mButtonGradientTopFocused;
    NVGcolor gradBot = mTheme->mButtonGradientBotFocused;

    nvgBeginPath(ctx);

    nvgRoundedRect(ctx, mPos.x() + 1, mPos.y() + 1.0f, mSize.x() - 2,
      mSize.y() - 2, mTheme->mButtonCornerRadius - 1);

    if (mBackgroundColor.w() != 0) {
      nvgFillColor(ctx, Color(mBackgroundColor.head<3>(), 1.f));
      nvgFill(ctx);
      if (mPushed) {
        gradTop.a = gradBot.a = 0.8f;
      }
      else {
        double v = 1 - mBackgroundColor.w();
        gradTop.a = gradBot.a = mEnabled ? v : v * .5f + .5f;
      }
    }

    NVGpaint bg = nvgLinearGradient(ctx, mPos.x(), mPos.y(), mPos.x(),
      mPos.y() + mSize.y(), gradTop, gradBot);

    nvgFillPaint(ctx, bg);
    nvgFill(ctx);
  }

  NVGcolor textColor = mTextColor.w() == 0 ? mTheme->mTextColor : mTextColor;
  if (mPushed)
  {
    Vector2f center = mPos.cast<float>() + mSize.cast<float>() * 0.5f;
    nvgBeginPath(ctx);
    nvgCircle(ctx, width() * 0.05f, center.y(), 2);
    nvgFillColor(ctx, textColor);
    nvgFill(ctx);
  }

  int fontSize = mFontSize == -1 ? mTheme->mButtonFontSize : mFontSize;
  nvgFontSize(ctx, fontSize);
  nvgFontFace(ctx, "sans-bold");
  float tw = nvgTextBounds(ctx, 0, 0, mCaption.c_str(), nullptr, nullptr);

  Vector2f center = mPos.cast<float>() + mSize.cast<float>() * 0.5f;
  Vector2f textPos(center.x() - tw * 0.5f, center.y() - 1);
  if (!mEnabled)
    textColor = mTheme->mDisabledTextColor;

  if (mIcon) {
    auto icon = utf8(mIcon);

    float iw, ih = fontSize;
    if (nvgIsFontIcon(mIcon)) {
      ih *= icon_scale();
      nvgFontSize(ctx, ih);
      nvgFontFace(ctx, "icons");
      iw = nvgTextBounds(ctx, 0, 0, icon.data(), nullptr, nullptr);
    }
    else {
      int w, h;
      ih *= 0.9f;
      nvgImageSize(ctx, mIcon, &w, &h);
      iw = w * ih / h;
    }
    if (mCaption != "")
      iw += mSize.y() * 0.15f;
    nvgFillColor(ctx, textColor);
    nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    Vector2f iconPos = center;
    iconPos.y() -= 1;

    if (mIconPosition == IconPosition::LeftCentered) {
      iconPos.x() -= (tw + iw) * 0.5f;
      textPos.x() += iw * 0.5f;
    }
    else if (mIconPosition == IconPosition::RightCentered) {
      textPos.x() -= iw * 0.5f;
      iconPos.x() += tw * 0.5f;
    }
    else if (mIconPosition == IconPosition::Left) {
      iconPos.x() = mPos.x() + 8;
    }
    else if (mIconPosition == IconPosition::Right) {
      iconPos.x() = mPos.x() + mSize.x() - iw - 8;
    }

    if (nvgIsFontIcon(mIcon)) {
      nvgText(ctx, iconPos.x(), iconPos.y() + 1, icon.data(), nullptr);
    }
    else {
      NVGpaint imgPaint = nvgImagePattern(ctx,
        iconPos.x(), iconPos.y() - ih / 2, iw, ih, 0, mIcon, mEnabled ? 0.5f : 0.25f);

      nvgFillPaint(ctx, imgPaint);
      nvgFill(ctx);
    }
  }

  nvgFontSize(ctx, fontSize);
  nvgFontFace(ctx, "sans-bold");
  nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
  nvgFillColor(ctx, mTheme->mTextColorShadow);
  nvgText(ctx, textPos.x(), textPos.y(), mCaption.c_str(), nullptr);
  nvgFillColor(ctx, textColor);
  nvgText(ctx, textPos.x(), textPos.y() + 1, mCaption.c_str(), nullptr);
}

class WidgetsArea : public Widget
{
public:
  WidgetsArea(Widget* parent) : Widget(parent) {}
  void performLayout(NVGcontext *ctx) override
  {
    VScrollPanel* vpanel = parent()->cast<VScrollPanel>();
    int xoffset = 0;
    if (vpanel)
      xoffset = vpanel->getSliderAreaWidth();
    setSize(parent()->size() - Vector2i(xoffset, 0));
    Widget::performLayout(ctx);
  }

  void draw(NVGcontext* ctx) override
  {
    Widget::draw(ctx);
  }
};

Vector2i Listbox::preferredSize(NVGcontext *ctx) const {
  Vector2i ret = mSize;
  if (minWidth() > ret.x()) ret.x() = minWidth();
  if (minHeight() > ret.y()) ret.y() = minHeight();
  if (fixedWidth() > 0) ret.x() = fixedWidth();
  if (fixedHeight() > 0) ret.y() = fixedHeight();
  return ret;
}

Listbox::Listbox(Widget* parent)
  : Widget(parent)
{
  mPanel = add<VScrollPanel>();
  mItems = mPanel->add<WidgetsArea>();
  mItems->boxlayout(Orientation::Vertical, Alignment::Minimum);
}

void Listbox::selectItem(ListboxItem* item)
{
  if (mSelectCallback)
    mSelectCallback(item);
}

void Listbox::addContent(std::function<void(Listbox&)> f)
{
  if (f) f(*this);
}

void Listbox::draw(NVGcontext* ctx)
{
  Widget::draw(ctx);

  nvgStrokeWidth(ctx, 1.0f);
  nvgBeginPath(ctx);
  nvgRect(ctx, mPos.x(), mPos.y(), mSize.x(), mSize.y());
  nvgStrokeColor(ctx, mTheme->mBorderDark);
  nvgStroke(ctx);
}

void Listbox::addItem(const std::string& str, const std::string& id)
{
  if (!mPanel)
    return;

  auto& i = mItems->wdg<ListboxItem>(str);
  i.setId(id);

}

void Listbox::performLayout(NVGcontext *ctx) {
  if (mPanel)
    mPanel->setFixedSize(size());
  Widget::performLayout(ctx);
}

NAMESPACE_END(nanogui)