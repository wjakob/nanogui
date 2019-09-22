#include <nanogui/editproperties.h>
#include <nanogui/table.h>
#include <nanogui/scrollbar.h>
#include <nanogui/widgetsfactory.h>
#include <nanogui/serializer/json.h>
#include <nanogui/label.h>
#include <nanogui/checkbox.h>
#include <nanogui/textbox.h>
#include <nanogui/layout.h>
#include <nanogui/screen.h>
#include <nanogui/vscrollpanel.h>

NAMESPACE_BEGIN(nanogui)

PropertiesEditor::PropertiesEditor( Widget* parent,const std::string& id ) 
  :  Widget(parent)
{
  setId(id);
  setSize(parent->size());
	// create attributes

  _propholder = new VScrollPanel(this);
  // refresh attrib list
  //updateAbsolutePosition();
  refreshAttribs();
}

PropertiesEditor::~PropertiesEditor()
{
  _clearAttributesList();
}

void PropertiesEditor::_clearAttributesList()
{
}

void PropertiesEditor::refreshAttribs()
{
  _clearAttributesList();  
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
      Widget* grid = new Widget(panel);
      grid->setLayout(new GridLayout());

      auto capvalue = jval.get_str("name");
      auto typevalue = jval.get_str("type");

      //_data->get(keyCaption, keyCaptionValue);
      auto wcaption = grid->add<Label>(capvalue.empty() ? obj.first : capvalue);
      std::cout << capvalue << std::endl;

      wcaption->setWidth(width() / 2);
      wcaption->setFixedWidth(width() / 2);
      if (typevalue == "position")
      {
        auto e = grid->add<IntBox<int>>(jval.get_int("x"), [&](int v) { jval.set_int("x", v); });
        e->setEditable(true); e->setWidth(width() / 2); e->setFixedWidth(width() / 2);
        grid->add<Label>("");
        e = grid->add<IntBox<int>>(jval.get_int("y"), [&](int v) { jval.set_int("y", v); });
        e->setEditable(true); e->setWidth(width() / 2); e->setFixedWidth(width() / 2);
      }
      else if (typevalue == "size")
      {
        auto e = grid->add<IntBox<int>>(jval.get_int("w"), [&](int v) { jval.set_int("w", v); });
        e->setEditable(true); e->setWidth(width() / 2); e->setFixedWidth(width() / 2);
        grid->add<Label>("");
        e = grid->add<IntBox<int>>(jval.get_int("h"), [&](int v) { jval.set_int("h", v); });
        e->setEditable(true); e->setWidth(width() / 2); e->setFixedWidth(width() / 2);
      }
      else if (typevalue == "boolean")
      {
        auto ch = grid->add<CheckBox>("", [](bool) {});
        ch->setWidth(width() / 2); ch->setFixedWidth(width() / 2); ch->setChecked(jval.get_bool("value"));
      }
      else if (typevalue == "integer")
      {
        auto e = grid->add<IntBox<int>>(jval.get_int("value"), [&](int v) { jval.set_int("value", v); });
        e->setEditable(true); e->setWidth(width() / 2); e->setFixedWidth(width() / 2);
      }
      else if (typevalue == "string")
      {
        auto e = grid->add<TextBox>(jval.get_str("value"), 
                                    [&](const std::string& v) -> bool { jval.set_str("value", v); return true; }, 
                                    [&](Widget* p) {} );
        e->setEditable(true); e->setWidth(width() / 2); e->setFixedWidth(width() / 2);
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