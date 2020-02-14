#pragma once

#include <nanogui/window.h>
#include <vector>

NAMESPACE_BEGIN(nanogui)

class VScrollPanel;
namespace Json { class value; }

class NANOGUI_EXPORT PropertiesEditor : public Window
{
public:
  RTTI_CLASS_UID(PropertiesEditor)
  RTTI_DECLARE_INFO(PropertiesEditor)

  PropertiesEditor(Widget* parent, const std::string& id="");
  virtual ~PropertiesEditor();

  void setColumnWidth(float nameColWidth, float valColWidth );

  using Window::set;
  template<typename... Args>
  PropertiesEditor(Widget* parent, const Args&... args)
    : PropertiesEditor(parent, std::string("")) { set<PropertiesEditor, Args...>(args...); }

  //! Returns the type name of the gui element.
  std::string wtypename() const;

  void updateAttribs();

  void parse(Widget* w);
  void draw(NVGcontext* ctx) override;

  void addChild(int index, Widget *widget) override;

protected:
  Json::value* _data = nullptr;
  Widget* _parsedw = nullptr;
  VScrollPanel * _propholder;

  float mNameColumnWidthPerc, mValueColumnWidthPerc;
};

NAMESPACE_END(nanogui)