#include <nanogui/treeviewitem.h>
#include <nanogui/treeview.h>
#include <nanovg.h>
#include <algorithm>

NAMESPACE_BEGIN(nanogui)

RTTI_IMPLEMENT_INFO(TreeViewItem, Widget)

TreeViewItem::TreeViewItem( Widget* parent )
  : Label(parent, ""), mNodeId(BadNodeId)
{
  _init();
  if (parent)
  {
    if (auto p = TreeView::cast(parent))
      mOwner = p;
  }

  mPreviewArea = &hlayer(WidgetStretchLayout{ Orientation::ReverseHorizontal });
  mPreviewArea->setRelativeSize({ 1, 1 });
}

void TreeViewItem::_init()
{
  mParentId = BadNodeId;
  mImageIndex = -1;
  SelectedImageIndex = -1;
  mData = 0;
  mFontColor = Color();

  sendToBack();
}

TreeViewItem::~TreeViewItem()
{
  if (mOwner && mNodeId == mOwner->selectedNode() )
    setSelected(false);
}

int TreeViewItem::nodesCount() const { return mChildrenIds.size(); }
bool TreeViewItem::hasNodes() const { return !mChildrenIds.empty(); }
TreeView* TreeViewItem::source() const { return mOwner; }
void TreeViewItem::setIcon( int icon ) { mIcon = icon; }

TreeViewItem* TreeViewItem::addNodeBack(
  const std::string&    text,
  int    icon,
  int    imageIndex,
  int    selectedImageIndex,
  intptr_t  data)
{
  auto& node = source()->addNode();

  mChildrenIds.push_back( node.nodeId() );
  node.setCaption( text );
  node.mIcon = icon;
  node.mImageIndex = imageIndex;
  node.mParentId = mNodeId;
  node.SelectedImageIndex = selectedImageIndex;
  node.mData = data;

  return &node;
}

TreeViewItem* TreeViewItem::addNodeFront(
  const std::string&    text,
  int    icon /*= 0*/,
  int          imageIndex /*= -1*/,
  int          selectedImageIndex /*= -1*/,
  intptr_t          data /*= 0*/)
{
  auto& node = source()->addNode();

  mChildrenIds.push_front( node.nodeId() );
  node.setCaption( text );
  node.mIcon = icon;
  node.mImageIndex = imageIndex;
  node.mParentId = mNodeId;
  node.SelectedImageIndex = selectedImageIndex;
  node.mData = data;

  return &node;
}

TreeViewItem* TreeViewItem::insertNodeAfter(
  TreeViewItem*  other,
  const std::string&  text,
  int    icon /*= 0*/,
  int          imageIndex /*= -1*/,
  int          selectedImageIndex /*= -1*/,
  intptr_t          data /*= 0*/)
{
  NodeId insertId = other->nodeId();
  for (auto it = mChildrenIds.begin(); it != mChildrenIds.end(); it++ )
  {
    if (insertId == *it)
    {
      auto& node = source()->addNode();
      node.setCaption( text );
      node.mIcon = icon;
      node.mImageIndex = imageIndex;
      node.SelectedImageIndex = selectedImageIndex;
      node.mData = data;
      node.mParentId = mNodeId;
      mChildrenIds.insert( it, node.nodeId() );
      return &node;
    }
  }

  return nullptr;
}

bool TreeViewItem::isAliveId(NodeId id)
{
  auto it = std::find(mChildrenIds.begin(), mChildrenIds.end(), id);
  return it != mChildrenIds.end();
}

void TreeViewItem::removeChild(const Widget *widget)
{
  removeNode(widget);
  Label::removeChild(widget);
}

void TreeViewItem::removeAllNodes()
{
  for (auto& id : mChildrenIds) source()->removeNode(id);
  mChildrenIds.clear();
  source()->recheckChildren();
}

void TreeViewItem::removeNode(const Widget* node)
{
  if (auto twi = TreeViewItem::cast(node))
  {
    auto it = std::find(mChildrenIds.begin(), mChildrenIds.end(), twi->nodeId());
    if (it != mChildrenIds.end())
    {
      source()->recheckChildren();
      source()->removeNode(*it);
      mChildrenIds.erase(it);
    }
  }
}

TreeViewItem* TreeViewItem::insertNodeBefore(
  TreeViewItem*  other,
  const std::string&    text,
  int    icon /*= 0*/,
  int    imageIndex /*= -1*/,
  int    selectedImageIndex /*= -1*/,
  intptr_t  data /*= 0*/)
{
  for (auto it = mChildrenIds.begin(); it != mChildrenIds.end(); it++ )
  {
    if ( other->nodeId() == *it )
    {
      auto& node = source()->addNode();
      node.setCaption( text );
      node.mIcon = icon;
      node.mImageIndex = imageIndex;
      node.SelectedImageIndex = selectedImageIndex;
      node.mData = data;
      node.mParentId = mNodeId;
      mChildrenIds.insert( it, node.nodeId() );
      return &node;
    }
  }
  return nullptr;
}

TreeViewItem* TreeViewItem::front() const
{
  return mChildrenIds.empty()
          ? nullptr
          : source()->findNode(mChildrenIds.front());
}

TreeViewItem* TreeViewItem::back() const
{
  return mChildrenIds.empty()
    ? nullptr
    : source()->findNode(mChildrenIds.back());
}

TreeViewItem* TreeViewItem::prevSibling() const
{
  TreeViewItem::NodeId                  result = BadNodeId;

  NodeList::iterator self, other;
  if ( mParentId != BadNodeId )
  {
    auto itemParent = baseNode();

    for ( self = itemParent->mChildrenIds.begin(); self != itemParent->mChildrenIds.end(); self++ )
    {
      if ( mNodeId == *self )
      {
        if ( self != itemParent->mChildrenIds.begin() )
          result = *other;
        break;
      }
      other = self;
    }
  }
  return source()->findNode(result);
}

TreeViewItem* TreeViewItem::nextSibling() const
{
  TreeViewItem::NodeId                  other = BadNodeId;

  if ( mParentId != BadNodeId )
  {
    auto itemParent = baseNode();

    for (auto it = itemParent->mChildrenIds.begin(); it != itemParent->mChildrenIds.end(); it++ )
    {
      if ( mNodeId == *it )
      {
        if ( *it != itemParent->mChildrenIds.back() )
          other = *( ++it );
        break;
      }
    }
  }
  return source()->findNode(other);
}

Vector2i TreeViewItem::preferredSize(NVGcontext *ctx) const {
  return Label::preferredSize(ctx) + Vector2i(15,0);
}

TreeViewItem* TreeViewItem::nextVisible() const
{
  TreeViewItem*  next = nullptr;
  TreeViewItem*  node = const_cast<TreeViewItem*>(this);

  if ( node->isExpanded() && node->hasNodes() )
  {
    next = node->front();
  }
  else
  {
    next = node->nextSibling();
  }
  while ( !next && node->baseNode() )
  {
    next = node->baseNode()->nextSibling();
    if ( !next )
    {
      node = node->baseNode();
    }
  }

  return next;
}

bool TreeViewItem::deleteNode( TreeViewItem* child )
{
  auto it = std::find(mChildrenIds.begin(), mChildrenIds.end(), child->nodeId());
  if (it != mChildrenIds.end())
  {
    mChildrenIds.erase(it);
    source()->recheckChildren();
    return true;
  }
  return false;
}

bool TreeViewItem::moveNodeUp( TreeViewItem* child )
{
  NodeList::iterator  itOther;
  TreeViewItem::NodeId nodeTmp = BadNodeId;
  bool                moved = false;

  for (auto it = mChildrenIds.begin(); it != mChildrenIds.end(); it++ )
  {
    if ( child->nodeId() == *it )
    {
      if ( it != mChildrenIds.begin() )
      {
        nodeTmp = *it;
        *it = *itOther;
        *itOther = nodeTmp;
        moved = true;
      }
      break;
    }
    itOther = it;
  }
  return moved;
}

bool TreeViewItem::moveNodeDown( TreeViewItem* child )
{
  NodeList::iterator  itChild;
  NodeList::iterator  itOther;
  TreeViewItem::NodeId nodeTmp = BadNodeId;
  bool                 moved = false;

  for ( itChild = mChildrenIds.begin(); itChild != mChildrenIds.end(); itChild++ )
  {
    if ( child->nodeId() == *itChild )
    {
      if ( *itChild != mChildrenIds.back() )
      {
        itOther = itChild;
        ++itOther;
        nodeTmp = *itChild;
        *itChild = *itOther;
        *itOther = nodeTmp;
        moved = true;
      }
      break;
    }
  }
  return moved;
}

void TreeViewItem::setExpanded( bool expanded )
{
  source()->setExpanded(this, expanded);
  if (mOwner)
    mOwner->updateItems();
}

void TreeViewItem::setSelected( bool selected )
{
  if (mOwner)
  {
    if ( selected )
    {
      mOwner->setSelected(this);
    }
    else
    {
      if (mOwner->selectedNode() == mNodeId)
      {
        mOwner->setSelected(nullptr);
      }
    }
  }
}

bool TreeViewItem::isSelected() const
{
  if (mOwner)
  {
    return mOwner->selectedNode() == mNodeId;
  }
  else
  {
    return false;
  }
}

bool TreeViewItem::isRoot() const { return (mOwner && ( this == mOwner->rootNode() ) ); }

int TreeViewItem::getLevel() const
{
  auto p = source()->findNode(mParentId);
  return p ? p->getLevel() + 1 : 0;
}

TreeViewItem* TreeViewItem::baseNode() const { return source()->findNode(mParentId); }

bool TreeViewItem::isVisible() const
{
  auto p = source()->findNode(mParentId);

  return (p ? (p->isExpanded() && p->isVisible()) : true);
}

void TreeViewItem::setColor( const Color& color ) {  mFontColor = color; }
bool TreeViewItem::isExpanded() const { return source()->isExpanded(this); }

void TreeViewItem::setNodeId(NodeId id)
{
  auto oldId = mNodeId;
  mNodeId = id;
  auto bn = baseNode();
  if (bn) 
    bn->swapChildId(oldId, id);
}

void TreeViewItem::swapChildId(NodeId oldid, NodeId newid)
{
  auto it = std::find(mChildrenIds.begin(), mChildrenIds.end(), oldid);
  if (it != mChildrenIds.end())
    *it = newid;
  else
    mChildrenIds.push_back(newid);
}

TreeView* TreeViewItem::view() { return findParent<TreeView>(); }

void TreeViewItem::performLayout(NVGcontext* ctx)
{
  Label::performLayout(ctx);
}

void TreeViewItem::draw(NVGcontext *ctx)
{
  if ( !isVisible() )
    return;

  nvgFontFace(ctx, mFont.c_str());
  nvgFontSize(ctx, fontSize());
  Color color;
  if (enabled()) color = (mColor.w() > 0) ? mColor : mTheme->mTextColor;
  else color = (mDisabledColor.w() > 0) ? mDisabledColor : mTheme->mLabelTextDisabledColor;

  if (source()->selectedNode() == mNodeId)
    color = Color(0xff, 0, 0, 0xff);

  if (source()->hoveredNode() == mNodeId)
    color = Color(0, 0xff, 0, 0xff);

  nvgFillColor(ctx, color);

  int halign = (mFixedSize.x() > 0 ? (1 << mTextAlign.h) : (1 << TextHAlign::hLeft));
  int valign = (mFixedSize.y() > 0 ? (1 << mTextAlign.v) : (1 << TextVAlign::vTop));

  int xpos = 15, ypos = 0;
  switch (mTextAlign.h)
  {
  case TextHAlign::hCenter: xpos = (mSize.x() - mTextRealSize.x()) / 2; break;
  case TextHAlign::hRight: xpos = (mSize.x() - mTextRealSize.x()); break;
  }

  switch (mTextAlign.v)
  {
  case TextVAlign::vMiddle: ypos = (mSize.y() - mTextRealSize.y()) / 2; break;
  case TextVAlign::vBottom: ypos = (mSize.y() - mTextRealSize.y()); break;
  }

  nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
  xpos += mPos.x() + mAnchorPotsition.x();
  ypos += mPos.y();
  if (mFixedSize.x() > 0)
    nvgTextBox(ctx, xpos, ypos, mFixedSize.x(), mCaption.c_str(), nullptr);
  else
    nvgText(ctx, xpos, ypos, mCaption.c_str(), nullptr);

  Widget::draw(ctx);
}

NAMESPACE_END(nanogui)
