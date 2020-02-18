#include <nanogui/editproperties.h>
#include <nanogui/table.h>
#include <nanogui/scrollbar.h>
#include <nanogui/widgetsfactory.h>
#include <nanogui/serializer/json.h>
#include <nanogui/label.h>
#include <nanogui/checkbox.h>
#include <nanogui/colorpicker.h>
#include <nanogui/textbox.h>
#include <nanogui/layout.h>
#include <nanogui/screen.h>
#include <nanogui/vscrollpanel.h>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(PropertiesEditor, Window)

PropertiesEditor::PropertiesEditor( Widget* parent,const std::string& id )
  :  Window(parent, "Properties")
{
  setId(id);
  setSize(parent->size());
  setMayCollapse(Theme::clNonCollapse);
  mNameColumnWidthPerc = 0.48f;
  mValueColumnWidthPerc = 0.48f;
  // create attributes

  _propholder = new VScrollPanel(this);
  _propholder->setPosition(0, getHeaderHeight());
}

PropertiesEditor::~PropertiesEditor()
{
}

void PropertiesEditor::updateAttribs()
{
  if (_parsedw)
  {
    _parsedw->load(*_data);
  }
}

void PropertiesEditor::addChild(int index, Widget *widget)
{
  Widget::addChild(index, widget);

  if (widget)
    widget->setDebugDraw(true);
}

void PropertiesEditor::draw(NVGcontext* ctx)
{
  Window::draw(ctx);
}

void PropertiesEditor::parse(Widget* w)
{
  while (_propholder->childCount())
    _propholder->removeChild(0);

  if (w)
  {
    delete _data;

    auto& panel = _propholder->widget();
    panel.withLayout<GroupLayout>(0, 0, 0, 0);
    _parsedw = w;
    _data = new Json::value();
    w->save(*_data);
    Json::object& objects = _data->get_obj();

    int hh = 20;
    auto titleSize = FixedSize{ width() * mNameColumnWidthPerc, hh };
    auto valueSize = FixedSize{ width() * mValueColumnWidthPerc, hh };

    auto& typeProp = panel.widget(WidgetGridLayout{});
    typeProp.label(Caption{ "Widget type" }, titleSize);
    typeProp.textbox(TextValue{ w->rttiClass()->mRttiName }, valueSize);

    for (auto& obj : objects)
    {
      Json::value& jval = obj.second;
      auto& grid = panel.widget(WidgetGridLayout{});

      auto capvalue = jval.get_str("name");
      auto typevalue = jval.get_str("type");

      //_data->get(keyCaption, keyCaptionValue);
      auto& wcaption = grid.label(Caption{ capvalue.empty() ? obj.first : capvalue },
                                  titleSize);
      std::cout << capvalue << std::endl;

      if (typevalue == "position")
      {
        auto& ex = grid.intbox<int>(InitialValue{ (float)jval.get_int("x") }, valueSize);
        ex.setCallback([&](int v) { jval.set_int("x", v); updateAttribs(); });
        ex.setEditCallback( [&](int v, bool c) { if (c) { jval.set_int("x", v); updateAttribs(); } });
        ex.setEditable(true);
        grid.label("");
        auto& ey = grid.intbox<int>(InitialValue{ (float)jval.get_int("y") }, valueSize);
        ey.setCallback([&](int v) { jval.set_int("y", v); updateAttribs(); });
        ey.setEditCallback([&](int v, bool c) { if (c) { jval.set_int("y", v); updateAttribs(); } });
        ey.setEditable(true);
      }
      else if (typevalue == "size")
      {
        auto& ew = grid.intbox<int>(InitialValue{ (float)jval.get_int("w") }, valueSize);
        ew.setCallback([&](int v) { jval.set_int("w", v); updateAttribs(); });
        ew.setEditCallback([&](int v, bool c) { if (c) { jval.set_int("w", v); updateAttribs(); } });
        ew.setEditable(true);
        grid.label("");
        auto& eh = grid.intbox<int>(InitialValue{ (float)jval.get_int("h") }, valueSize);
        eh.setCallback([&](int v) { jval.set_int("h", v); updateAttribs(); });
        eh.setEditCallback([&](int v, bool c) { if (c) { jval.set_int("h", v); updateAttribs(); } });
        eh.setEditable(true);
      }
      else if (typevalue == "boolean")
      {
        auto& ch = grid.checkbox(Caption{ "" },
                                 CheckboxCallback{ [&](bool v) { jval.set_bool("value", v); updateAttribs(); } }, 
                                 CheckboxState{ jval.get_bool("value") },
                                 valueSize);
      }
      else if (typevalue == "integer")
      {
        auto& e = grid.intbox<int>(InitialValue{ (float)jval.get_int("value") }, valueSize);
        e.setCallback([&](int v) { jval.set_int("value", v); updateAttribs(); });
        e.setEditable(true);
      }
      else if (typevalue == "string")
      {
        auto& e = grid.textbox(TextValue{ jval.get_str("value") }, valueSize);
        e.setCallback([&](const std::string& v) -> bool { jval.set_str("value", v); updateAttribs(); return true; });
        e.setEditCallback([&](const std::string& v, bool) { jval.set_str("value", v); updateAttribs(); } );
        e.setEditable(true);
      }
      else if (typevalue == "color")
      {
        auto& cp = grid.wdg<ColorPicker>(InitialColor{ jval.get_int("value") }, valueSize);
        cp.setSide(Popup::Side::Left);
        cp.setFinalCallback([&, this](const Color &c) {jval.set_int("value", c.toInt()); updateAttribs(); });
        cp.setCallback([&, this](const Color &c) {jval.set_int("value", c.toInt()); updateAttribs(); });
      }
    }

    screen()->performLayout();
    _propholder->setSize(size());
  }
}

void PropertiesEditor::setColumnWidth(float nameColWidth, float valColWidth )
{
  mNameColumnWidthPerc = nameColWidth;
  mValueColumnWidthPerc = valColWidth;
}

std::string PropertiesEditor::wtypename() const
{
  return "PropertiesEditor";
}

NAMESPACE_END(nanogui)