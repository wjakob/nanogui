#pragma once

#include <nanogui/widget.h>
#include <vector>

NAMESPACE_BEGIN(nanogui)

class VScrollPanel;
namespace Json { class value; }

class NANOGUI_EXPORT PropertiesEditor : public Widget
{
public:
  PropertiesEditor(Widget* parent, const std::string& id="");
  virtual ~PropertiesEditor();

  void setColumnWidth(float nameColWidth, float valColWidth );

  //! Returns the type name of the gui element.
  std::string wtypename() const;

  void refreshAttribs();

  void parse(Widget* w);

protected:
	//void _resizeEvent();
  void _clearAttributesList();
  Json::value* _data = nullptr;
  Widget* _parsedw = nullptr;
  VScrollPanel * _propholder;

  float _nameColumnWidthPerc, _valueColumnWidthPerc;
};

NAMESPACE_END(nanogui)