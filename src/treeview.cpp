#include <nanogui/treeview.h>
#include <nanogui/scrollbar.h>
#include <nanogui/treeviewitem.h>
#include <nanovg.h>
#include <string>

#define DEFAULT_SCROLLBAR_SIZE 15

NAMESPACE_BEGIN(nanogui)

TreeView::TreeView( Widget* parent, bool clip,
  bool drawBack, bool scrollBarVertical, bool scrollBarHorizontal)
  : Widget( parent ),
  mItemHeight( 0 ),
  mIndentWidth( 0 ),
  mScrollBarH( nullptr ),
  mScrollBarV( nullptr ),
  mLinesVisible( true ),
  mSelecting( false ),
  mClip( clip ),
  mDrawBack( drawBack ),
  mImageLeftOfIcon( true )
{
  int s = DEFAULT_SCROLLBAR_SIZE;

  if ( scrollBarVertical )
  {
    mScrollBarV = add<ScrollBar>(ScrollBar::VerticalRight);
    mScrollBarV->setSubElement( true );
  }

  if ( scrollBarHorizontal )
  {
    mScrollBarH = add<ScrollBar>(ScrollBar::HorizontalBottom);
    mScrollBarH->setSubElement(true);
  }

  mRoot = add<TreeViewItem>();
  mRoot->mExpanded = true;
  mNeedRecalculateItemsRectangle = true;
  mSelected = nullptr;
}

TreeView::~TreeView() {}

void TreeView::removeAllNodes()
{
  mRoot->removeAllNodes();
}

void TreeView::_recalculateItemsRectangle(NVGcontext* ctx)
{
  if (!mNeedRecalculateItemsRectangle)
    return;

  mNeedRecalculateItemsRectangle = false;
  TreeViewItem*  node;

  nvgFontFace(ctx, mFont.c_str());
  mItemHeight = nvgTextHeight(ctx, 0, 0, "A", nullptr, nullptr ) + 4;

  mIndentWidth = clamp<int>( mItemHeight, 9, 15) - 1;

  mTotalItemSize = Vector2i( 0, 0 );
  node = mRoot->front();
  while (node)
  {
    mTotalItemSize.y() += mItemHeight;
    mTotalItemSize.x() = std::max( mTotalItemSize.x(), node->right() - mRoot->left() );
    node = node->nextVisible();
  }

  mScrollBarVscale = std::max(0, mTotalItemSize.y() - height() + mItemHeight);
  mScrollBarV->setVisible( mTotalItemSize.y() > height() );

  mScrollBarH->setVisible( mTotalItemSize.x() > width() );
  mScrollBarHscale = std::max(0, mTotalItemSize.x() - width());
}

bool TreeView::mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers)
{
  if (isMouseButtonLeft(button) && down)
  {
    if ( focused() && !isPointInsideRect(p, rect()) )
    {
      //removeFocus();
      return false;
    }

    /*if (focused() &&
      (
           ( mScrollBarV && ScrollBarV->getAbsoluteRect().isPointInside( p ) && ScrollBarV->onEvent( event ) )
        || ( mScrollBarH && ScrollBarH->getAbsoluteRect().isPointInside( p ) && ScrollBarH->onEvent( event ) )
      )
      )
    {
      return true;
    }*/

    mSelecting = true;
    requestFocus();
    return true;
  }

  if (isMouseButtonLeft(button) && !down)
  {
     /*if ( focused() &&
      (
           ( mScrollBarV && ScrollBarV->getAbsoluteRect().isPointInside( p ) && ScrollBarV->onEvent( event ) )
        || ( mScrollBarH && ScrollBarH->getAbsoluteRect().isPointInside( p ) &&  ScrollBarH->onEvent( event ) )
      )
      )
    {
      return true;
    }*/

    mSelecting = false;
    //removeFocus();
    _mouseAction( p.x(), p.y() );
    return true;
  }

  return Widget::mouseButtonEvent(p, button, down, modifiers);
}

bool TreeView::mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers)
{
  if (mSelecting)
  {
    if (isPointInsideRect(p, rect()))
    {
      _mouseAction( p.x(), p.y(), true );
      return true;
    }
  }
  return Widget::mouseMotionEvent(p, rel, button, modifiers);
}

bool TreeView::scrollEvent(const Vector2i &p, const Vector2f &rel)
{
  if (mScrollBarV)
    mScrollBarV->setScroll( mScrollBarV->scroll() + (rel.y() < 0 ? -0.1 : 0.1) );

  return true;
}

bool TreeView::focusEvent(bool focused)
{
  if (!focused)
  {
    mSelecting = false;
    return false;
  }

  return Widget::focusEvent(focused);
}

//! called if an event happened.
/*bool TreeView::onEvent( const NEvent &event )
{
  if (isEnabled())
  {
    switch( event.EventType )
    {
    case NRP_GUI_EVENT:
      switch( event.GuiEvent.EventType )
      {
      case NRP_SCROLL_BAR_CHANGED:
        if ( event.GuiEvent.Caller == ScrollBarV || event.GuiEvent.Caller == ScrollBarH )
        {
          //int pos = ( ( gui::IGUIScrollBar* )event.GUIEvent.Caller )->getPos();
          updateItems();
          return true;
        }
        break;
      }
      break;
    }
  }

  return getParent()->onEvent( event );
}*/

void TreeView::_mouseAction( int xpos, int ypos, bool onlyHover /*= false*/ )
{
  auto oldSelected = mSelected;
  TreeViewItem* hitNode;
  int   selIdx;
  int   n;
  TreeViewItem*  node;

  xpos -= mPos.x();//_absoluteRect.UpperLeftCorner.X;
  ypos -= mPos.y();//_absoluteRect.UpperLeftCorner.Y;

  // find new selected item.
  if (mItemHeight != 0 && mScrollBarV)
  {
    selIdx = ( ( ypos - 1 ) + mScrollBarV->scroll() * mScrollBarVscale ) / mItemHeight;
  }

  hitNode = nullptr;
  node = mRoot->front();
  n = 0;

  while ( node )
  {
    if (selIdx == n)
    {
      hitNode = node;
      break;
    }
    node = node->nextVisible();
    ++n;
  }

  if (hitNode && xpos > hitNode->getLevel() * mIndentWidth)
  {
    mSelected = hitNode;
  }

  if (hitNode && !onlyHover
      && xpos < hitNode->getLevel() * mIndentWidth
      && xpos > ( hitNode->getLevel() - 1 ) * mIndentWidth
      && hitNode->hasNodes() )
  {
        hitNode->setExpanded( !hitNode->isExpanded() );

    // post expand/collaps news
    bool expanded = hitNode->isExpanded();
    mLastEventNode = hitNode;
    //nodeExpand(mLastEventNode, !expanded);
    mLastEventNode = nullptr;
  }

  if (mSelected && !mSelected->isVisible())
  {
    mSelected = nullptr;
  }

  // post selection news

  if (!onlyHover && mSelected != oldSelected)
  {
    if ( oldSelected )
    {
      mLastEventNode = oldSelected;
      //nodeDeselect(mLastEventNode);
      mLastEventNode = nullptr;
    }
    if ( mSelected )
    {
      mLastEventNode = mSelected;
      //nodeSelect(_lastEventNode);
      mLastEventNode = nullptr;
    }
  }
}

void TreeView::updateItems() { mNeedUpdateItems = true; }

Color TreeView::_getCurrentNodeColor( TreeViewItem* node )
{
    Color textCol = 0xffc0c0c0;

    if ( enabled() )
      textCol = ( node == mSelected ) ? 0xffffffff : 0xff000000;

    return textCol;
}

std::string TreeView::_getCurrentNodeFont( TreeViewItem* node)
{
  return "sans";
}

void TreeView::afterDraw(NVGcontext* ctx)
{
  if ( !visible() )
    return;

  if ( mNeedUpdateItems )
  {
    mNeedUpdateItems = false;

    _recalculateItemsRectangle(ctx); // if the font changed

    // draw background
    Vector2i framePos = { 6, 6 };

    framePos.x() -= mScrollBarH->scroll() * mScrollBarHscale;
    framePos.y() -= mScrollBarV->scroll() * mScrollBarVscale;

    Vector2i pos = framePos;
    TreeViewItem* node = mRoot->front();

    while ( node )
    {
      pos.x() = framePos.x() + (node->getLevel()-1) * mIndentWidth;
      pos.y() = framePos.y();

      Vector2i offset = { 0, 0 };
      TreeViewItem* itemOffset = node->baseNode();
      while ( itemOffset != nullptr )
      {
        //offset += itemOffset->position();
        itemOffset = itemOffset->baseNode();
      }

      std::string fontNode = node->getActiveFont();
      if ( fontNode.empty() )
        fontNode = "sans";

      Vector2i pfsize = node->preferredSize(ctx);
      node->setPosition( pos + offset );
      node->setSize(pfsize);

      node = node->nextVisible();

      framePos.y() += mItemHeight;
    }
  }

  Widget::afterDraw(ctx);
}

//! draws the element and its children
void TreeView::draw(NVGcontext* ctx)
{
  if ( !visible() )
    return;

  _recalculateItemsRectangle(ctx);

  // draw background
  if (mDrawBack)
  {
    nvgStrokeWidth(ctx, 1.0f);
    nvgBeginPath(ctx);
    nvgRect(ctx, mPos.x(), mPos.y(), mSize.x(), mSize.y());
    nvgStrokeColor(ctx, mTheme->mBorderMedium);
    nvgStroke(ctx);
  }

  TreeViewItem* node = mRoot->front();
  Vector2i rsize(mIndentWidth - 4, mIndentWidth - 4);

  Vector2i framePos;
  while (node)
  {
    framePos = node->position();
    Vector2i ns = node->size();
    int centerYofs = (ns.y() - rsize.y()) / 2;
    Vector2i center = mPos + framePos + rsize / 2;
    center.y() += centerYofs;

    if ( node->hasNodes() )
    {
      //rect for '+/-' sign
      nvgStrokeWidth(ctx, 1.0f);
      nvgBeginPath(ctx);
      nvgRect(ctx, mPos.x() + framePos.x(), mPos.y() + framePos.y() + centerYofs,
                   rsize.x(), rsize.y());
      nvgStrokeColor(ctx, theme()->mBorderLight );
      nvgStroke(ctx);

      // horizontal '-' line
      int offset = rsize.x()/2-2;

      nvgStrokeWidth(ctx, 1.0f);
      nvgBeginPath(ctx);
      nvgMoveTo(ctx, center.x() - offset, center.y());
      nvgLineTo(ctx, center.x() + offset, center.y());

      if ( !node->isExpanded() )
      {
        // vertical '+' line
        nvgMoveTo(ctx, center.x(), center.y() - offset);
        nvgLineTo(ctx, center.x(), center.y() + offset);
      }

      nvgStrokeColor(ctx, theme()->mBorderLight);
      nvgStroke(ctx);
    }

    // draw the lines if neccessary
    if ( mLinesVisible )
    {
      Vector2i rc_s, rc_e;
      nvgBeginPath(ctx);

      auto baseNode = node->baseNode();
      if (baseNode != mRoot )
      {
        Vector2i prevCenter = mPos + baseNode->position();
        prevCenter += Vector2i( rsize.x() / 2, baseNode->height());

        int nodeh = baseNode->height();
        //prevCenter += Vector2i( nodeh - rsize.x(), nodeh - rsize.x() )/2;

        rc_s = Vector2i( prevCenter.x(), center.y());
        rc_e = Vector2i( rc_s.x(), prevCenter.y());

        nvgMoveTo(ctx, center.x(), center.y());
        nvgLineTo(ctx, rc_s.x(), rc_s.y());
        nvgLineTo(ctx, rc_e.x(), rc_e.y());

        // vertical line
        /*int offsetY = ( node == node->baseNode()->front() ? mIndentWidth : 0 );
        rc_s.y() = framePos.y() + ( mItemHeight - offsetY ) / 2;
        rc_e.x() = rc_s.x() + 1;

        nvgMoveTo(ctx, rc_s.x(), rc_s.y());
        nvgLineTo(ctx, rc_e.x(), rc_e.y());

        // the vertical lines of all parents
        TreeViewItem* nodeTmp = node->baseNode();
        rc_s.y() = framePos.y();

        for ( int n = 0; n < node->getLevel() - 2; ++n )
        {
          rc_s.x() -= mIndentWidth;
          rc_e.x() -= mIndentWidth;

          if ( nodeTmp != nodeTmp->baseNode()->back() )
          {
            nvgMoveTo(ctx, rc_s.x(), rc_s.y());
            nvgLineTo(ctx, rc_e.x(), rc_e.y());
          }
          nodeTmp = nodeTmp->baseNode();
        }*/
      }

      nvgStrokeColor(ctx, theme()->mBorderLight);
      nvgStroke(ctx);
    }

    node = node->nextVisible();
  }

  // draw items
  Widget::draw(ctx);
}


void TreeView::setImageLeftOfIcon( bool bLeftOf ) { mImageLeftOfIcon = bLeftOf; }
bool TreeView::getImageLeftOfIcon() const { return mImageLeftOfIcon; }
TreeViewItem* TreeView::getLastEventNode() const { return mLastEventNode; }

NAMESPACE_END(nanogui)
