#pragma once

#include <nanogui/treeviewitem.h>
#include <memory>
#include <set>

NAMESPACE_BEGIN(nanogui)

class ScrollBar;

//! Default tree view GUI element.
class NANOGUI_EXPORT TreeView : public Widget
{
public:
    RTTI_CLASS_UID(TreeView)
    RTTI_DECLARE_INFO(TreeView)

    enum IconState { IconCollapsed=0, IconExpanded, IconCount };

    explicit TreeView( Widget* parent,
        bool clip = true,
        bool drawBack = true, bool scrollBarVertical = true,
        bool scrollBarHorizontal = true );

    using Widget::set;
    template<typename... Args>
    TreeView(Widget* parent, const Args&... args)
      : TreeView(parent, true, true, true, true) { set<Widget, Args...>(args...);  }

    virtual ~TreeView();

    TreeViewItem* rootNode() const { return mRoot; }
    TreeViewItem::NodeId selectedNode() const { return mSelected; }
    TreeViewItem::NodeId hoveredNode() const { return mHovered; }

    void setSelected(TreeViewItem* item) { mSelected = item ? item->nodeId() : (TreeViewItem::NodeId)TreeViewItem::BadNodeId; }
    void setHovered(TreeViewItem* item) { mHovered = item ? item->nodeId() : (TreeViewItem::NodeId)TreeViewItem::BadNodeId; }

    bool getLinesVisible() const { return mLinesVisible; }
    void setLinesVisible( bool visible ) { mLinesVisible = visible; }

    void draw(NVGcontext* ctx) override;
    void afterDraw(NVGcontext* ctx) override;
    void performLayout(NVGcontext *ctx) override;

    void setExpanded(TreeViewItem* item, bool expanded);
    bool isExpanded(const TreeViewItem* item) const;

    void setImageLeftOfIcon( bool bLeftOf );
    bool getImageLeftOfIcon() const;
    void updateItems();
    void removeAllNodes();

    bool mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) override;
    bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override;
    bool scrollEvent(const Vector2i &p, const Vector2f &rel) override;
    bool focusEvent(bool focused) override;

    void setSelectNodeCallback(std::function<void(TreeViewItem*)> f) { mSelectNodeCallback = f; }
    void setHoverNodeCallback(std::function<void(TreeViewItem*)> f) { mHoverNodeCallback = f; }

    TreeViewItem& addNode();
    void removeNode(TreeViewItem::NodeId id);
    TreeViewItem* findNode(TreeViewItem::NodeId id);
    TreeViewItem* findNode(std::function<bool(TreeViewItem*)> f);

    void setActionIcon(uint32_t action, uint32_t icon);

    void recheckChildren();

private:
    void _recalculateItemsRectangle(NVGcontext* ctx);
    void _mouseAction(Vector2i pos, bool onlyHover = false );
    Color _getCurrentNodeColor( TreeViewItem* node  );
    std::string _getCurrentNodeFont( TreeViewItem* node );

    std::function<void(TreeViewItem*)> mSelectNodeCallback;
    std::function<void(TreeViewItem*)> mHoverNodeCallback;

    bool mNeedRecalculateItemsRectangle = false;
    TreeViewItem* mRoot;
    TreeViewItem::NodeId mSelected;
    TreeViewItem::NodeId mHovered;
    int           mItemHeight;
    int           mIndentWidth;
    std::set<TreeViewItem::NodeId> mExpandedItems;

    ScrollBar*    mScrollBarH;
    ScrollBar*    mScrollBarV;

    int mIcons[IconCount];

    bool          mLinesVisible;
    bool          mSelecting;
    bool          mDrawBack;
    bool          mClip;
    bool          mImageLeftOfIcon;
    std::string mFont = "sans";

    Vector2i      mTotalItemSize;
    bool          mNeedRecheckChildren;
    bool          mNeedUpdateItems;
    float         mScrollBarVscale = 1.f;
    float         mScrollBarHscale = 1.f;
};

NAMESPACE_END(nanogui)