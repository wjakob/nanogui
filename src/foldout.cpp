#include <nanogui/foldout.h>
#include <nanogui/button.h>
#include <nanogui/scrollbar.h>
#include <nanovg.h>
#include <nanogui/screen.h>

NAMESPACE_BEGIN(nanogui)

void Foldout::_reparseChilds()
{
  if( !_scrollBar )
    return;

  int maxHeigth = 0;
  for (auto& page: _pages)
  {
      maxHeigth += page->button->height();
      if( page->page->visible()  )
          maxHeigth += page->page->height();
  }

  bool lastVis = _scrollBar->visible();
  _scrollBar->setVisible( maxHeigth > height() );
  if( !lastVis && _scrollBar->visible() )
  {
      _scrollValue = maxHeigth - height();
      _scrollBar->setScroll( 0 );
      _scrollBar->bringToFront();
  }

  auto it = std::find_if(_pages.begin(), _pages.end(), [](Page* p) { return p->page->visible(); });
  bool haveActivePage = it != _pages.end();

  _activePageIndex = -1;
  int pageButtonH = 20;
  if (haveActivePage)
  {
    Vector4i lastRect(0, 0, width(), pageButtonH);

    bool nextPageAtBottom = false;
    for (uint32_t index = 0; index < _pages.size(); index++)
    {
      Page* page = _pages[index];
      page->button->setGeometry(lastRect);
      page->number = index;

      if (nextPageAtBottom)
        page->button->sendToBack();

      if (page->page->visible())
      {
        lastRect.y() = height() - pageButtonH;
        lastRect.w() = lastRect.y() + pageButtonH;
        _activePageIndex = index;
        nextPageAtBottom = true;
        page->button->bringToFront();
      }
    }
  }
  else
  {
    Vector4i lastRect(0, 0, width(), pageButtonH);
    for (uint32_t index = 0; index < _pages.size(); index++)
    {
      Page* page = _pages[index];
      lastRect.y() = index * pageButtonH;
      lastRect.w() = lastRect.y() + pageButtonH;
      page->button->setGeometry(lastRect);
      page->button->setFixedSize({ lastRect.z(), lastRect.w() });
      page->number = index;
    }
  }

  if( _activePageIndex >= 0 )
  {
    Button* btn = _pages[ _activePageIndex ]->button;
    if( btn->position().y() < 0 )
    {
      Vector4i currentRect( 0, 0, btn->width(), btn->height() );
      btn->setGeometry(currentRect);
    }
  }
}

Foldout::Foldout( Widget* parent, const Vector4i& rectangle, const std::string& id )
  : Widget(parent)
{
  setId(id);
  setGeometry(rectangle);

  _lastChildCount = 0;
  _activePageIndex = -1;

  _scrollBar = add<ScrollBar>(ScrollBar::Alignment::VerticalRight);
  _scrollBar->setSubElement( true );
}

void Foldout::removeChild(const Widget* child)
{
  for( uint32_t index=0; index < _pages.size(); index++ )
  {
    if( _pages[ index ]->page == child )
    {
      delete _pages[ index ];
      _pages.erase(_pages.begin() + index );
      _pageNames.erase(_pageNames.begin() + index );
      break;
    }
  }

  Widget::removeChild(child);
  _reparseChilds();
}

void Foldout::draw(NVGcontext* ctx)
{
  if( !mVisible )
    return;

  Widget::draw(ctx);
}

bool Foldout::mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers)
{
  return Widget::mouseButtonEvent(p, button, down, modifiers);
}

void Foldout::selectButton(Widget* w)
{
  Button* elm = dynamic_cast<Button*>(w);

  for (auto& p: _pages)
  {
    Widget* page = p->page;
    if (p->button == elm)
    {
      page->setVisible(!page->visible());
      elm->setPushed(page->visible());
    }
    else
    {
      page->setVisible(false);
      elm->setPushed(false);
    }
  }

  _reparseChilds();
}

Foldout::~Foldout() {}

Foldout::Page* Foldout::addPage(Widget* elm)
{
    Page* descr = new Page();

    uint32_t curIndex = _pages.size();
    std::string pageName = std::string("Page_") + std::to_string(curIndex);
    std::string caption = pageName;
    if( curIndex < _pageNames.size() )
    {
       pageName = _pageNames[ curIndex ].name;
       caption = _pageNames[ curIndex ].caption;
    }

    descr->name = pageName;
    descr->button = new Button(this);
    descr->button->setPosition(0, 0);
    descr->button->setWidth(width());
    descr->button->setCaption( pageName );
    descr->button->setToggleButton( true );
    descr->button->setSubElement( true );
    descr->button->setCallback([b = descr->button, this]() { selectButton(b); });
    elm->setVisible( false );
    descr->page = elm;

    Desc info = { pageName, caption };
    _pageNames.push_back( info );
    _pages.push_back( descr );

    return descr;
}

void Foldout::_updateChilds()
{
    for (auto& c: children())
    {
      if( !c->isSubElement() && getPage(c) == nullptr)
        addPage(c);
    }

    _lastChildCount = children().size();
    _reparseChilds();
}

Foldout::Page* Foldout::getPage(Widget* child)
{
  for (auto& c:_pages)
    if(c->page == child )
      return c;

  return nullptr;
}

void Foldout::addPage( const std::string& pageName, const std::string& pageCaption, Widget* child )
{
  addChild( child );

  Page* page = addPage( child );
  if( page )
  {
    page->name = pageName;
    page->button->setCaption( pageCaption );
    _reparseChilds();
  }
}

void Foldout::setPageOpened( const std::string& pageName, bool opened )
{

}

//void Foldout::save( core::VariantArray* out ) const
/*{
}*/

//void Foldout::load( core::VariantArray* in )
/*{
}*/

std::string Foldout::wtypename() const { return "foldout"; }

NAMESPACE_END(gui)
