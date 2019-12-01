#pragma once

#include <nanogui/widget.h>
#include <bitset>

NAMESPACE_BEGIN(nanogui)

class Button;
class ScrollBar;


class NANOGUI_EXPORT Foldout : public Widget
{
public:
  RTTI_CLASS_UID("FLDO")
  RTTI_DECLARE_INFO(Foldout)

  struct Page
  {
    Button* button = nullptr;
    Widget* page = nullptr;
    std::string name;
    int number = -1;
  };

  struct Desc
  {
    std::string name;
    std::string caption;
  };
  Foldout( Widget* parent, const std::string& id="" );
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
  void afterDraw(NVGcontext* ctx) override;

  //! reparse childs inside
  void performLayout(NVGcontext *ctx) override;

  Page& getPage(Widget* child);
  Page& addPage(Widget* elemnt);

  //virtual void save(* out ) const;
  //virtual void load(* in );
protected:

    void _reparseChilds(NVGcontext* ctx);
    void _updateChilds();

    typedef std::vector<Page> _Pages;
    typedef std::vector<Desc> _Names;
    _Pages mPages;
    _Names mPageNames;

    int mActivePageIndex;

    ScrollBar* mScrollBar = nullptr;

    std::bitset<16> mFlags;
    int mScrollValue = 0;
    bool mNeedReparseChilds = false;
};

NAMESPACE_END(gui)
