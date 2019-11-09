#pragma once

#include <nanogui/widget.h>
#include <memory>

NAMESPACE_BEGIN(nanogui)

class TreeViewItem;
class ScrollBar;

//! Default tree view GUI element.
class NANOGUI_EXPORT TreeView : public Widget
{
public:
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
    TreeViewItem* selectedNode() const { return mSelected; }
    void setSelected(TreeViewItem* item) { mSelected = item; }
    bool getLinesVisible() const { return mLinesVisible; }
    void setLinesVisible( bool visible ) { mLinesVisible = visible; }
     
    void draw(NVGcontext* ctx) override;
    void afterDraw(NVGcontext* ctx) override;
    
    void setImageLeftOfIcon( bool bLeftOf );
    bool getImageLeftOfIcon() const;
    TreeViewItem* getLastEventNode() const;
    void updateItems(); 

    bool mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) override;
    bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override;
    bool scrollEvent(const Vector2i &p, const Vector2f &rel) override;
    bool focusEvent(bool focused) override;

private:
    void _recalculateItemsRectangle(NVGcontext* ctx);
    void _mouseAction( int xpos, int ypos, bool onlyHover = false );
    Color _getCurrentNodeColor( TreeViewItem* node  );
    std::string _getCurrentNodeFont( TreeViewItem* node );

    bool mNeedRecalculateItemsRectangle = false;
    TreeViewItem* mRoot;
    TreeViewItem*  mSelected;
    TreeViewItem*  mHoverNode;
    int      mItemHeight;
    int      mIndentWidth;

    ScrollBar*    mScrollBarH;
    ScrollBar*    mScrollBarV;

    TreeViewItem*  mLastEventNode;
    bool      mLinesVisible;
    bool      mSelecting;
    bool      mDrawBack;
    bool      mClip;
    bool      mImageLeftOfIcon;
    std::string mFont = "sans";

    //ElementState _currentDrawState;

    Vector2i mTotalItemSize;
    bool mNeedUpdateItems;
    float mScrollBarVscale = 1.f;
    float mScrollBarHscale = 1.f;
public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

NAMESPACE_END(nanogui)