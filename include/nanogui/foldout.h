#pragma once

#include <nanogui/widget.h>
#include <bitset>

NAMESPACE_BEGIN(nanogui)

class Button;
class ScrollBar;


class NANOGUI_EXPORT Foldout : public Widget
{
public:
	struct Page
	{
	  Button* button;
	  Widget* page;
	  std::string name;
	  int number;
	};

	struct Desc
	{
	  std::string name;
	  std::string caption;
	};
	Foldout( Widget* parent, const Vector4i& rectangle, const std::string& id="" );
	virtual ~Foldout();

	//! draws the element and its children
	void draw(NVGcontext* painter);

  void addPage( const std::string& pageName, const std::string& pageCaption, Widget* page );
  void setPageOpened( const std::string& pageName, bool opened );

  /// The callback that is called when any type of mouse button event is issued to this Button.
  bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override;

  void removeChild(const Widget* child) override;
  void selectButton(Widget* w);

  //! return element type name for gui factory
  std::string wtypename() const override;

  Page* getPage(Widget* child);

  Page* addPage(Widget* elemnt);

	//virtual void save(* out ) const;
  //virtual void load(* in );
protected:
    
    void _reparseChilds();
    void _updateChilds();
    void _resizeEvent();

    typedef std::vector<Page*> _Pages;
  	typedef std::vector<Desc> _Names;
  	_Pages _pages;
  	_Names _pageNames;

    uint32_t _lastChildCount;
    int _activePageIndex;

    ScrollBar* _scrollBar = nullptr;

    std::bitset<16> _flags;
    int _scrollValue = 0;
};

NAMESPACE_END(gui)
