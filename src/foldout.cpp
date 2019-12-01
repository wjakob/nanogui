#include <nanogui/foldout.h>
#include <nanogui/button.h>
#include <nanogui/scrollbar.h>
#include <nanovg.h>
#include <nanogui/screen.h>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(Foldout, Widget)

void Foldout::_reparseChilds(NVGcontext* ctx)
{
  if ( !mScrollBar )
    return;

  int maxHeigth = 0;
  for (auto& page: mPages)
  {
      maxHeigth += page.button->height();
      if ( page.page->visible()  )
          maxHeigth += page.page->height();
  }

  bool lastVis = mScrollBar->visible();
  mScrollBar->setVisible( maxHeigth > height() );
  if ( !lastVis && mScrollBar->visible() )
  {
      mScrollValue = maxHeigth - height();
      mScrollBar->setScroll( 0 );
      mScrollBar->bringToFront();
  }

  auto it = std::find_if(mPages.begin(), mPages.end(), [](const Page& p) { return p.page->visible(); });
  bool haveActivePage = it != mPages.end();

  mActivePageIndex = -1;
  int pageButtonH = 20;
  if (haveActivePage)
  {
    Vector4i lastRect(0, 0, width(), pageButtonH);

    bool nextPageAtBottom = false;
    for (uint32_t index = 0; index < mPages.size(); index++)
    {
      Page& page = mPages[index];
      page.button->setGeometry(lastRect);
      page.number = index;

      if (nextPageAtBottom)
        page.button->sendToBack();

      if (page.page->visible())
      {
        lastRect.y() = height() - pageButtonH;
        lastRect.w() = lastRect.y() + pageButtonH;
        mActivePageIndex = index;
        nextPageAtBottom = true;
        page.button->bringToFront();
      }
    }
  }
  else
  {
    Vector4i lastRect(0, 0, width(), pageButtonH);
    for (uint32_t index = 0; index < mPages.size(); index++)
    {
      Page& page = mPages[index];
      lastRect.y() = index * pageButtonH;
      lastRect.w() = lastRect.y() + pageButtonH;
      page.button->setGeometry(lastRect);
      page.button->setFixedSize({ lastRect.z(), lastRect.w() });
      page.number = index;
    }
  }

  if ( mActivePageIndex >= 0 )
  {
    Page& p = mPages[ mActivePageIndex ];
    Vector4i currentRect(0, 0, p.button->width(), p.button->height());

    if (p.button->position().y() < 0)
    {
      p.button->setGeometry(currentRect);
    }

    if (p.page)
    {
      currentRect.y() = p.button->height();
      currentRect.w() = height();
      //if we have pages after current
      if (mPages.size() > 1 && mActivePageIndex < mPages.size() - 1)
        currentRect.w() -= p.button->height();

      p.page->setGeometry(currentRect);
      p.page->performLayout(ctx);
    }
  }
}

Foldout::Foldout( Widget* parent, const std::string& id )
  : Widget(parent)
{
  setId(id);

  mActivePageIndex = -1;

  mScrollBar = add<ScrollBar>(ScrollBar::Alignment::VerticalRight);
  mScrollBar->setSubElement( true );
}

void Foldout::removeChild(const Widget* child)
{
  for ( uint32_t index=0; index < mPages.size(); index++ )
  {
    if (mPages[ index ].page == child )
    {
      mPages.erase(mPages.begin() + index );
      mPageNames.erase(mPageNames.begin() + index );
      break;
    }
  }

  Widget::removeChild(child);
  mNeedReparseChilds = true;
}

void Foldout::draw(NVGcontext* ctx)
{
  if ( !mVisible )
    return;

  Widget::draw(ctx);
}

bool Foldout::mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers)
{
  return Widget::mouseButtonEvent(p, button, down, modifiers);
}

void Foldout::selectButton(Widget* w)
{
  Button* elm = w->cast<Button>();

  for (auto& p: mPages)
  {
    Widget* page = p.page;
    if (p.button == elm)
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

  mNeedReparseChilds = true;
}

Foldout::~Foldout() {}

Foldout::Page& Foldout::addPage(Widget* elm)
{
    Page descr;

    size_t curIndex = mPages.size();
    std::string pageName = std::string("Page_") + std::to_string(curIndex);
    std::string caption = pageName;
    if ( curIndex < mPageNames.size() )
    {
       pageName = mPageNames[ curIndex ].name;
       caption = mPageNames[ curIndex ].caption;
    }

    descr.name = pageName;
    descr.button = add<Button>();
    descr.button->setPosition(0, 0);
    descr.button->setWidth(width());
    descr.button->setCaption( pageName );
    descr.button->setToggleButton( true );
    descr.button->setSubElement( true );
    descr.button->setCallback([b = descr.button, this]() { selectButton(b); });
    elm->setVisible( false );
    descr.page = elm;

    Desc info = { pageName, caption };
    mPageNames.push_back( info );
    mPages.push_back( descr );

    return mPages.back();
}

void Foldout::afterDraw(NVGcontext* ctx)
{
  Widget::afterDraw(ctx);

  if (mNeedReparseChilds)
  {
    mNeedReparseChilds = false;
    _reparseChilds(ctx);
  }
}

void Foldout::_updateChilds()
{
  for (auto& c: children())
  {
    if ( !c->isSubElement() && getPage(c).button == nullptr)
      addPage(c);
  }

  mNeedReparseChilds = true;
}

void Foldout::performLayout(NVGcontext *ctx)
{
  Widget::performLayout(ctx);

  mNeedReparseChilds = true;
}

Foldout::Page& Foldout::getPage(Widget* child)
{
  auto it = std::find_if(mPages.begin(), mPages.end(), [child](const Page& c) { return c.page == child; });
  static auto invalidPage = Page{};
  return it != mPages.end() ? *it : invalidPage;
}

void Foldout::addPage( const std::string& pageName, const std::string& pageCaption, Widget* child )
{
  addChild( child );

  Page& page = addPage( child );
  if ( page.button )
  {
    page.name = pageName;
    page.button->setCaption( pageCaption );
    mNeedReparseChilds = true;
  }
}

void Foldout::setPageOpened( const std::string& /*pageName*/, bool /*opened*/ )
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
