#include <nanogui/treeview.h>
#include <nanogui/scrollbar.h>
#include <nanogui/treeviewitem.h>
#include <nanogui/entypo.h>
#include <nanogui/screen.h>
#include <nanovg.h>
#include <string>

#define DEFAULT_SCROLLBAR_SIZE 15

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(TreeView, Widget)

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
  mIcons[IconCollapsed] = ENTYPO_ICON_PLUS_CIRCLED;
  mIcons[IconExpanded] = ENTYPO_ICON_MINUS_CIRCLED;

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
  mRoot->setNodeId(TreeViewItem::RootNodeId);
  setExpanded(mRoot, true);

  mNeedRecalculateItemsRectangle = true;
  mNeedRecheckChildren = true;
  mSelected = TreeViewItem::BadNodeId;
}

TreeView::~TreeView() {}

void TreeView::removeAllNodes()
{
  mRoot->removeAllNodes();
}

void TreeView::removeNode(TreeViewItem::NodeId id)
{
  auto* node = findNode(id);
  if (node)
  {
    node->removeAllNodes();
    removeChild(node);
  }
}

TreeViewItem& TreeView::addNode()
{
  static TreeViewItem::NodeId nodeIdCounter = 1;
  auto& node = wdg<TreeViewItem>();
  node.setNodeId(nodeIdCounter++);
  return node;
}

TreeViewItem* TreeView::findNode(std::function<bool(TreeViewItem*)> f)
{
  if (!f)
    return nullptr;

  for (auto& c : children())
  {
    if (auto twi = TreeViewItem::cast(c))
    {
      if (f(twi))
        return twi;
    }
  }

  return nullptr;
}

TreeViewItem* TreeView::findNode(TreeViewItem::NodeId id)
{
  if (id == TreeViewItem::BadNodeId)
    return nullptr;

  for (auto& c : children())
  {
    if (auto twi = TreeViewItem::cast(c))
    {
      if (twi->nodeId() == id)
        return twi;
    }
  }

  return nullptr;
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
    mSelecting = true;
    requestFocus();
  }
  else if (isMouseButtonLeft(button) && !down)
  {
    _mouseAction(p, false);
    mSelecting = false;
  }

  return Widget::mouseButtonEvent(p, button, down, modifiers);
}

bool TreeView::mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers)
{
  if (isPointInsideRect(p, rect()))
  {
    _mouseAction(p, !mSelecting);
  }

  return Widget::mouseMotionEvent(p, rel, button, modifiers);
}

bool TreeView::scrollEvent(const Vector2i& /*p*/, const Vector2f &rel)
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

void TreeView::_mouseAction(Vector2i pos, bool onlyHover /*= false*/ )
{
  TreeViewItem* selectedPtr = nullptr;
  TreeViewItem* hitNode;
  TreeViewItem::NodeId selIdx = TreeViewItem::BadNodeId;
  int   n;
  TreeViewItem*  node;

  pos -= mPos;

  // find new selected item.
  if (mItemHeight != 0 && mScrollBarV)
  {
    selIdx = ( ( pos.y() - 1 ) + mScrollBarV->scroll() * mScrollBarVscale ) / mItemHeight;
  }

  hitNode = nullptr;
  node = mRoot->front();
  n = 0;

  while (node)
  {
    if (selIdx == n)
    {
      hitNode = node;
      break;
    }
    node = node->nextVisible();
    ++n;
  }

  if (onlyHover)
  {
    mHovered = (hitNode ? hitNode->nodeId() : TreeViewItem::BadNodeId);
    if (mHoverNodeCallback)
      mHoverNodeCallback(hitNode);
  }
  else
  {
    if (hitNode && pos.x() > hitNode->getLevel() * mIndentWidth)
    {
      selectedPtr = hitNode;
      mSelected = selectedPtr ? selectedPtr->nodeId() : TreeViewItem::BadNodeId;
    }

    if (hitNode
        && pos.x() < hitNode->getLevel() * mIndentWidth
        && pos.x() >(hitNode->getLevel() - 1) * mIndentWidth
        && hitNode->hasNodes())
    {
      hitNode->setExpanded(!hitNode->isExpanded());
      mNeedRecalculateItemsRectangle = true;
      mNeedUpdateItems = true;
    }

    if (selectedPtr && !selectedPtr->isVisible())
    {
      selectedPtr = nullptr;
      mSelected = TreeViewItem::BadNodeId;
    }

    if (mSelectNodeCallback)
      mSelectNodeCallback(selectedPtr);
  }
}

void TreeView::setExpanded(TreeViewItem* item, bool expanded)
{
  auto nid = item ? item->nodeId() : 0;
  if (expanded) mExpandedItems.insert(nid);
  else mExpandedItems.erase(nid);
}

bool TreeView::isExpanded(const TreeViewItem* item) const
{
  return mExpandedItems.count(item ? item->nodeId() : 0) > 0;
}

void TreeView::updateItems() { mNeedUpdateItems = true; }

Color TreeView::_getCurrentNodeColor( TreeViewItem* node )
{
  Color textCol = 0xffc0c0c0;
  if (node && enabled())
    textCol = ( node->nodeId() == mSelected ) ? 0xffffffff : 0xff000000;
  return textCol;
}

std::string TreeView::_getCurrentNodeFont( TreeViewItem* node)
{
  return "sans";
}

void TreeView::recheckChildren() { mNeedRecheckChildren = true; }

void TreeView::performLayout(NVGcontext *ctx)
{
  Widget::performLayout(ctx);

  mNeedRecheckChildren = true;
  mNeedUpdateItems = true;
}

void TreeView::afterDraw(NVGcontext* ctx)
{
  if (mNeedRecheckChildren)
  {
    mNeedRecheckChildren = false;

    std::vector<TreeViewItem*> nodes = findAll<TreeViewItem>();
    struct NodeExist { TreeViewItem::NodeId id; bool alive; };
    std::vector<NodeExist> all_ids;
    //collect all id from nodes, dont add root node, because it always present
    for (auto& n : nodes)
    {
      if (n != mRoot)
        all_ids.push_back({ n->nodeId(), false });
    }

    //check each id, that it contain at least in once node
    for (auto& c: all_ids)
    {
      for (auto& n : nodes)
      {
        bool exist = n->isAliveId(c.id);
        if (exist)
        {
          c.alive = true;
          break;
        }
      }
    }

    //now all_ids contain information about which nodes are empty
    for (auto& c : all_ids)
    {
      if (!c.alive)
      {
        removeNode(c.id);
      }
    }
  }

  if ( mNeedUpdateItems )
  {
    mNeedUpdateItems = false;

    _recalculateItemsRectangle(ctx); // if the font changed

    // draw background
    Vector2i framePos = { 6, 6 };

    framePos -= Vector2i( mScrollBarH->scroll() * mScrollBarHscale,
                          mScrollBarV->scroll() * mScrollBarVscale );

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
      node->setPosition(0, (pos + offset).y() );
      node->setAnchorPosition(pos);
      node->setSize(width(), pfsize.y());
      node->setFixedSize(width(), pfsize.y());

      node = node->nextVisible();

      framePos.y() += mItemHeight;
    }
    screen()->needPerformLayout(this);
  }

  Widget::afterDraw(ctx);
}

void TreeView::setActionIcon(uint32_t action, uint32_t icon)
{
  if (action < IconCount)
    mIcons[action] = icon;
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
    nvgRect(ctx, mPos, mSize);
    nvgStrokeColor(ctx, mTheme->mBorderMedium);
    nvgStroke(ctx);
  }

  TreeViewItem* node = mRoot->front();
  Vector2i rsize(mIndentWidth - 4, mIndentWidth - 4);

  Vector2i framePos;
  while (node)
  {
    framePos = node->anchorPosition();
    Vector2i ns = node->size();
    int centerYofs = (ns.y() - rsize.y()) / 2;
    Vector2i center = mPos + framePos + rsize / 2;
    center.y() += centerYofs;

    if ( node->hasNodes() )
    {
      nvgFontFace(ctx, "icons");
      nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
      nvgFillColor(ctx, theme()->mBorderLight);

      //need cache this calcs
      float bounds[4];
      auto icon = utf8(mIcons[node->isExpanded() ? IconExpanded : IconCollapsed]);

      nvgTextBounds(ctx, 0, 0, icon.data(), nullptr, bounds);
      Vector2i iconSize(bounds[2] - bounds[0], bounds[3] - bounds[1]);

      nvgText(ctx, framePos.x(), center.y(), icon.data(), nullptr);
    }

    // draw the lines if neccessary
    if ( mLinesVisible )
    {
      Vector2i rc_s, rc_e;
      nvgBeginPath(ctx);

      auto baseNode = node->baseNode();
      if (baseNode != mRoot )
      {
        Vector2i prevCenter = mPos + baseNode->anchorPosition();
        prevCenter += Vector2i( rsize.x() / 2, baseNode->height());

        int nodeh = baseNode->height();
        //prevCenter += Vector2i( nodeh - rsize.x(), nodeh - rsize.x() )/2;

        rc_s = Vector2i( prevCenter.x(), center.y());
        rc_e = Vector2i( rc_s.x(), prevCenter.y());

        nvgMoveTo(ctx, center.x(), center.y());
        nvgLineTo(ctx, rc_s.x(), rc_s.y());
        nvgLineTo(ctx, rc_e.x(), rc_e.y());
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

NAMESPACE_END(nanogui)
