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

PropertiesEditor::PropertiesEditor( Widget* parent,const std::string& id )
  :  Window(parent, "Properties")
{
  setId(id);
  setSize(parent->size());
  _nameColumnWidthPerc = 0.48f;
  _valueColumnWidthPerc = 0.48f;
  // create attributes

  _propholder = new VScrollPanel(this);
  _propholder->setPosition(0, mTheme->mWindowHeaderHeight);
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

    Widget* panel = new Widget(_propholder);
    panel->setLayout(new GroupLayout(0, 0, 0, 0));
    _parsedw = w;
    _data = new Json::value();
    w->save(*_data);
    Json::object& objects = _data->get_obj();

    for (auto& obj : objects)
    {
      Json::value& jval = obj.second;
      auto& grid = panel->widget();
      grid.setLayout(new GridLayout());

      auto capvalue = jval.get_str("name");
      auto typevalue = jval.get_str("type");

      //_data->get(keyCaption, keyCaptionValue);
      auto& wcaption = grid.label(Caption{ capvalue.empty() ? obj.first : capvalue });
      std::cout << capvalue << std::endl;

      int wname = width() * _nameColumnWidthPerc;
      int ww = width() * _valueColumnWidthPerc;
      int hh = 20;
      wcaption.setWidth(wname);
      wcaption.setFixedWidth(ww);
      if (typevalue == "position")
      {
        auto& ex = grid.intbox<int>(InitialValue{ (float)jval.get_int("x") });
        ex.setCallback([&](int v) { jval.set_int("x", v); updateAttribs(); });
        ex.setEditCallback( [&](int v, bool c) { if (c) { jval.set_int("x", v); updateAttribs(); } });
        ex.setEditable(true);
        ex.setSize(ww, hh);
        ex.setFixedSize({ ww, hh });
        grid.label("");
        auto& ey = grid.intbox<int>(InitialValue{ (float)jval.get_int("y") });
        ey.setCallback([&](int v) { jval.set_int("y", v); updateAttribs(); });
        ey.setEditCallback([&](int v, bool c) { if (c) { jval.set_int("y", v); updateAttribs(); } });
        ey.setEditable(true);
        ey.setSize(ww, hh);
        ey.setFixedSize({ ww, hh });
      }
      else if (typevalue == "size")
      {
        auto& ew = grid.intbox<int>(InitialValue{ (float)jval.get_int("w") });
        ew.setCallback([&](int v) { jval.set_int("w", v); updateAttribs(); });
        ew.setEditCallback([&](int v, bool c) { if (c) { jval.set_int("w", v); updateAttribs(); } });
        ew.setEditable(true);
        ew.setSize(ww, hh);
        ew.setFixedSize({ ww, hh });
        grid.label("");
        auto& eh = grid.intbox<int>(InitialValue{ (float)jval.get_int("h") });
        eh.setCallback([&](int v) { jval.set_int("h", v); updateAttribs(); });
        eh.setEditCallback([&](int v, bool c) { if (c) { jval.set_int("h", v); updateAttribs(); } });
        eh.setEditable(true);
        eh.setSize(ww, hh);
        eh.setFixedSize({ ww, hh });
      }
      else if (typevalue == "boolean")
      {
        auto& ch = grid.checkbox("", [&](bool v) { jval.set_bool("value", v); updateAttribs(); });
        ch.setSize(ww, hh);
        ch.setFixedSize({ ww, hh });
        ch.setChecked(jval.get_bool("value"));
      }
      else if (typevalue == "integer")
      {
        auto& e = grid.intbox<int>(InitialValue{ (float)jval.get_int("value") });
        e.setCallback([&](int v) { jval.set_int("value", v); updateAttribs(); });
        e.setEditable(true);
        e.setSize(ww, hh);
        e.setFixedSize({ ww, hh });
      }
      else if (typevalue == "string")
      {
        auto& e = grid.wdg<TextBox>(TextValue{ jval.get_str("value") });
        e.setCallback([&](const std::string& v) -> bool { jval.set_str("value", v); updateAttribs(); return true; });
        e.setEditCallback([&](const std::string& v, bool) { jval.set_str("value", v); updateAttribs(); } );
        e.setEditable(true);
        e.setSize(ww, hh);
        e.setFixedSize({ ww, hh });
      }
      else if (typevalue == "color")
      {
        auto& cp = grid.wdg<ColorPicker>(Color(jval.get_int("color")));
        cp.setFixedSize({ww, hh});
        cp.setSide(Popup::Side::Left);
        cp.setFinalCallback([&, this](const Color &c) {jval.set_int("color", c.toInt()); updateAttribs(); });
        cp.setCallback([&, this](const Color &c) {jval.set_int("color", c.toInt()); updateAttribs(); });
      }
    }

    screen()->performLayout();
    _propholder->setSize(size());
  }
}

void PropertiesEditor::setColumnWidth(float nameColWidth, float valColWidth )
{
  _nameColumnWidthPerc = nameColWidth;
  _valueColumnWidthPerc = valColWidth;
}

std::string PropertiesEditor::wtypename() const
{
  return "PropertiesEditor";
}

NAMESPACE_END(nanogui)