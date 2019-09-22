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
  _nameColumnWidthPerc = 0.5f;
  _valueColumnWidthPerc = 0.5f;
	// create attributes

  _propholder = new VScrollPanel(this);
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

void PropertiesEditor::draw(NVGcontext* ctx)
{
  if (_propsUpdated)
  {
    _propsUpdated = false;
    updateAttribs();
  }

  Widget::draw(ctx);
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

      int wname = width() * _nameColumnWidthPerc;
      int wvalue = width() * _valueColumnWidthPerc;
      wcaption->setWidth(wname);
      wcaption->setFixedWidth(wvalue);
      if (typevalue == "position")
      {
        auto e = grid->add<IntBox<int>>(jval.get_int("x"), 
            [&](int v) { jval.set_int("x", v); updateAttribs(); },
            [&](int v, bool c) { if (c) { jval.set_int("x", v); updateAttribs(); } });
        e->setEditable(true); e->setWidth(wvalue); e->setFixedWidth(wvalue);
        grid->add<Label>("");
        e = grid->add<IntBox<int>>(jval.get_int("y"), 
            [&](int v) { jval.set_int("y", v); updateAttribs(); },
            [&](int v, bool c) { if (c) { jval.set_int("y", v); updateAttribs(); } });
        e->setEditable(true); e->setWidth(wvalue); e->setFixedWidth(wvalue);
      }
      else if (typevalue == "size")
      {
        auto e = grid->add<IntBox<int>>(jval.get_int("w"), 
            [&](int v) { jval.set_int("w", v); updateAttribs(); },
            [&](int v, bool c) { if (c) { jval.set_int("w", v); updateAttribs(); } });
        e->setEditable(true); e->setWidth(wvalue); e->setFixedWidth(wvalue);
        grid->add<Label>("");
        e = grid->add<IntBox<int>>(jval.get_int("h"), 
            [&](int v) { jval.set_int("h", v); updateAttribs(); },
            [&](int v, bool c) { if (c) { jval.set_int("h", v); updateAttribs(); } });
        e->setEditable(true); e->setWidth(wvalue); e->setFixedWidth(wvalue);
      }
      else if (typevalue == "boolean")
      {
        auto ch = grid->add<CheckBox>("", [&](bool v) { jval.set_bool("value", v); updateAttribs(); });
        ch->setWidth(wvalue); ch->setFixedWidth(wvalue); ch->setChecked(jval.get_bool("value"));
      }
      else if (typevalue == "integer")
      {
        auto e = grid->add<IntBox<int>>(jval.get_int("value"), [&](int v) { jval.set_int("value", v); updateAttribs(); });
        e->setEditable(true); e->setWidth(wvalue); e->setFixedWidth(wvalue);
      }
      else if (typevalue == "string")
      {
        auto e = grid->add<TextBox>(jval.get_str("value"), 
                                    [&](const std::string& v) -> bool { jval.set_str("value", v); updateAttribs(); return true; },
                                    [&](const std::string& v, bool) { jval.set_str("value", v); updateAttribs(); } );
        e->setEditable(true); e->setWidth(wvalue); e->setFixedWidth(wvalue);
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