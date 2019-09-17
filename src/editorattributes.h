#pragma once

#include <nanogui/widget.h>

NAMESPACE_BEGIN(nanogui)

class EditorAttributeType;
class Table;

class AttributeEditor : public Widget
{
public:

	//! constructor
    AttributeEditor( Widget* parent, s32 id=-1);

	//! destructor
    ~AttributeEditor();

	// gets the current attributes list
    virtual core::VariantArray* getAttribs();

    void SetColumnWidth( f32 nameColWidth, f32 valColWidth );

	// update the attribute list after making a change
	void refreshAttribs();

	// save the attributes
	void updateAttribs();

	//! Returns the type name of the gui element.
    String getTypeName() const;

    bool onEvent(const NEvent &event);

protected:
	void resizeEvent_();
    
private:
    void _ClearAttributesList();
    void createTable_();
    void updateTable_();
    AbstractAttribute* createAttributElm_( String typeStr, const String& attrName );
    void addAttribute2Table_( AbstractAttribute* n, const String& offset=L"" );
    core::Array< AbstractAttribute* >	_attribList;	// attributes editing controls
    core::VariantArray*		_attribs;	// current attributes
    Table* _attribTable;

    f32 _nameColumnWidth, _valueColumnWidth;
};

NAMESPACE_END(nanogui)