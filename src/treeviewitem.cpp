#include <nanogui/treeviewitem.h>
#include <nanogui/treeview.h>
#include <nanovg.h>

NAMESPACE_BEGIN(nanogui)

TreeViewItem::TreeViewItem( Widget* parent )
  : Label(parent, "")
{
  init_();
  if (auto p = dynamic_cast<TreeView*>(parent))
    mOwner = p;
}

void TreeViewItem::init_()
{
  itemParent_ = 0;
  mImageIndex = -1;
  SelectedImageIndex = -1;
  Data = 0;
  mExpanded = false;
  mFontColor = Color();

  sendToBack();
}

TreeViewItem::~TreeViewItem()
{
  if(mOwner && this == mOwner->selectedNode() )
  {
    setSelected( false );
  }

  clearNodes();
}

int TreeViewItem::nodesCount() const { return mNodeChildren.size(); }
bool TreeViewItem::hasNodes() const { return !mNodeChildren.empty(); }
TreeView* TreeViewItem::source() const { return mOwner; }
void TreeViewItem::setIcon( int icon ) { mIcon = icon; }
void TreeViewItem::clearNodes()
{
  mNodeChildren.clear();
}

TreeViewItem* TreeViewItem::addNodeBack(
  const std::string&    text,
  int    icon,
  int    imageIndex,
  int    selectedImageIndex,
  void*  data)
{
  auto&  node = source()->wdg<TreeViewItem>();

  mNodeChildren.push_back( &node );
  node.setCaption( text );
  node.mIcon = icon;
  node.mImageIndex = imageIndex;
  node.itemParent_ = this;
  node.SelectedImageIndex = selectedImageIndex;
  node.Data = data;
  
  return &node;
}

TreeViewItem* TreeViewItem::addNodeFront(
  const std::string&    text,
  int    icon /*= 0*/,
  int          imageIndex /*= -1*/,
  int          selectedImageIndex /*= -1*/,
  void*          data /*= 0*/)
{
  auto& node = source()->wdg<TreeViewItem>();

  mNodeChildren.push_front( &node );
  node.setCaption( text );
  node.mIcon = icon;
  node.mImageIndex = imageIndex;
  node.itemParent_ = this;
  node.SelectedImageIndex = selectedImageIndex;
  node.Data = data;

  return &node;
}

TreeViewItem* TreeViewItem::insertNodeAfter(
  TreeViewItem*  other,
  const std::string&  text,
  int    icon /*= 0*/,
  int          imageIndex /*= -1*/,
  int          selectedImageIndex /*= -1*/,
  void*          data /*= 0*/)
{
  for(auto it = mNodeChildren.begin(); it != mNodeChildren.end(); it++ )
  {
    if( other == *it )
    {
      auto& node = source()->wdg<TreeViewItem>();
      node.setCaption( text );
      node.mIcon = icon;
      node.mImageIndex = imageIndex;
      node.SelectedImageIndex = selectedImageIndex;
      node.Data = data;
      node.itemParent_ = this;
      mNodeChildren.insert( it, &node );
      return &node;
    }
  }

  return nullptr;
}

TreeViewItem* TreeViewItem::insertNodeBefore(
  TreeViewItem*  other,
  const std::string&    text,
  int    icon /*= 0*/,
  int    imageIndex /*= -1*/,
  int    selectedImageIndex /*= -1*/,
  void*  data /*= 0*/)
{
  for(auto it = mNodeChildren.begin(); it != mNodeChildren.end(); it++ )
  {
    if( other == *it )
    {
      auto& node = source()->wdg<TreeViewItem>();
      node.setCaption( text );
      node.mIcon = icon;
      node.mImageIndex = imageIndex;
      node.SelectedImageIndex = selectedImageIndex;
      node.Data = data;
      node.itemParent_ = this;
      mNodeChildren.insert( it, &node );
      return &node;
    }
  }
  return nullptr;
}

TreeViewItem* TreeViewItem::front() const
{
  return mNodeChildren.empty()
          ? nullptr
          : mNodeChildren.front();
}

TreeViewItem* TreeViewItem::back() const
{
  return mNodeChildren.empty()
    ? nullptr
    : mNodeChildren.back();
}

TreeViewItem* TreeViewItem::prevSibling() const
{
  TvIterator  itThis;
  TvIterator  itOther;
  TreeViewItem*                  other = 0;

  if( itemParent_ )
  {
    for( itThis = itemParent_->mNodeChildren.begin(); itThis != itemParent_->mNodeChildren.end(); itThis++ )
    {
      if( this == *itThis )
      {
        if( itThis != itemParent_->mNodeChildren.begin() )
          other = *itOther;
        break;
      }
      itOther = itThis;
    }
  }
  return other;
}

TreeViewItem* TreeViewItem::nextSibling() const
{
  TreeViewItem*                  other = nullptr;

  if( itemParent_ )
  {
    for(auto it = itemParent_->mNodeChildren.begin(); it != itemParent_->mNodeChildren.end(); it++ )
    {
      if( this == *it )
      {
        if( *it != itemParent_->mNodeChildren.back() )
          other = *( ++it );
        break;
      }
    }
  }
  return other;
}

Vector2i TreeViewItem::preferredSize(NVGcontext *ctx) const {
  return Label::preferredSize(ctx) + Vector2i(15,0);
}

TreeViewItem* TreeViewItem::nextVisible() const
{
  TreeViewItem*  next = nullptr;
  TreeViewItem*  node = node = const_cast< TreeViewItem* >( this );

  if( node->isExpanded() && node->hasNodes() )
  {
    next = node->front();
  }
  else
  {
    next = node->nextSibling();
  }
  while( !next && node->baseNode() )
  {
    next = node->baseNode()->nextSibling();
    if( !next )
    {
      node = node->baseNode();
    }
  }

  return next;
}

bool TreeViewItem::deleteNode( TreeViewItem* child )
{
  auto it = std::find(mNodeChildren.begin(), mNodeChildren.end(), child);
  if (it != mNodeChildren.end())
  {
    mNodeChildren.erase(it);
    return true;
  }
  return false;
}

bool TreeViewItem::moveNodeUp( TreeViewItem* child )
{
  TvIterator  itOther;
  TreeViewItem*                  nodeTmp;
  bool                          moved = false;

  for(auto it = mNodeChildren.begin(); it != mNodeChildren.end(); it++ )
  {
    if( child == *it )
    {
      if( it != mNodeChildren.begin() )
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
  TvIterator  itChild;
  TvIterator  itOther;
  TreeViewItem*                  nodeTmp;
  bool                          moved = false;

  for( itChild = mNodeChildren.begin(); itChild != mNodeChildren.end(); itChild++ )
  {
    if( child == *itChild )
    {
      if( *itChild != mNodeChildren.back() )
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
  mExpanded = expanded;
  if(mOwner)
    mOwner->updateItems();
}

void TreeViewItem::setSelected( bool selected )
{
  if(mOwner)
  {
    if( selected )
    {
      mOwner->setSelected(this);
    }
    else
    {
      if(mOwner->selectedNode() == this)
      {
        mOwner->setSelected(nullptr);
      }
    }
  }
}

bool TreeViewItem::isSelected() const
{
  if(mOwner)
  {
    return mOwner->selectedNode() == this;
  }
  else
  {
    return false;
  }
}

bool TreeViewItem::isRoot() const { return (mOwner && ( this == mOwner->rootNode() ) ); }

int TreeViewItem::getLevel() const
{
  return itemParent_
            ? itemParent_->getLevel() + 1
            : 0;
}

TreeViewItem* TreeViewItem::baseNode() const { return itemParent_; }

bool TreeViewItem::isVisible() const
{
  if( itemParent_ )
  {
    return itemParent_->isExpanded() && itemParent_->isVisible();
  }
  else
  {
    return true;
  }
}

void TreeViewItem::setColor( const Color& color ) {  mFontColor = color; }
bool TreeViewItem::isExpanded() const { return mExpanded; }

TreeView* TreeViewItem::view() { return findParent<TreeView>(); }

void TreeViewItem::draw(NVGcontext *ctx)
{
  if( !isVisible() )
    return;

  Widget::draw(ctx);
  nvgFontFace(ctx, mFont.c_str());
  nvgFontSize(ctx, fontSize());
  Color color;
  if (enabled()) color = (mColor.w() > 0) ? mColor : mTheme->mTextColor;
  else color = (mDisabledColor.w() > 0) ? mDisabledColor : mTheme->mLabelTextDisabledColor;

  nvgFillColor(ctx, color);

  int halign = (mFixedSize.x() > 0 ? (1 << mTextHAlign) : (1 << TextHAlign::hLeft));
  int valign = (mFixedSize.y() > 0 ? (1 << mTextVAlign) : (1 << TextVAlign::vTop));

  int xpos = 15, ypos = 0;
  switch (mTextHAlign)
  {
  case TextHAlign::hCenter: xpos = (mSize.x() - mTextRealSize.x()) / 2; break;
  case TextHAlign::hRight: xpos = (mSize.x() - mTextRealSize.x()); break;
  }

  switch (mTextVAlign)
  {
  case TextVAlign::vMiddle: ypos = (mSize.y() - mTextRealSize.y()) / 2; break;
  case TextVAlign::vBottom: ypos = (mSize.y() - mTextRealSize.y()); break;
  }

  nvgTextAlign(ctx, NVG_ALIGN_LEFT | NVG_ALIGN_TOP);
  if (mFixedSize.x() > 0)
    nvgTextBox(ctx, mPos.x() + xpos, mPos.y() + ypos, mFixedSize.x(), mCaption.c_str(), nullptr);
  else
    nvgText(ctx, mPos.x() + xpos, mPos.y() + ypos, mCaption.c_str(), nullptr);
}

NAMESPACE_END(nanogui)
