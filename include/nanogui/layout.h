#if !defined(__NANOGUI_LAYOUT_H)
#define __NANOGUI_LAYOUT_H

#include <nanogui/common.h>

NANOGUI_NAMESPACE_BEGIN

/// Basic interface of a layout engine
class Layout {
public:
    virtual void performLayout(NVGcontext *ctx, Widget *widget) const = 0;
    virtual Vector2i preferredSize(NVGcontext *ctx, const Widget *widget) const = 0;
    virtual ~Layout() { }
};

/// Simple horizontal/vertical box layout
class BoxLayout : public Layout {
public:
    enum class Orientation {
        Horizontal = 0,
        Vertical
    };

    enum class Alignment {
        Minimum,
        Middle,
        Maximum
    };

    BoxLayout(Orientation orientation,Alignment alignment = Alignment::Middle,int margin = 0,int spacing = 0)
        : mOrientation(orientation), mAlignment(alignment), mMargin(margin), mSpacing(spacing) {}

    inline Orientation orientation() const { return mOrientation; }
    inline void setOrientation(Orientation orientation) { mOrientation = orientation; }

    inline Alignment alignment() const { return mAlignment; }
    inline void setAlignment(Alignment alignment) { mAlignment = alignment; }

    inline int margin() const { return mMargin; }
    inline void setMargin(int margin) { mMargin = margin; }

    inline int spacing() const { return mSpacing; }
    inline void setSpacing(int spacing) { mSpacing = spacing; }

    /* Implementation of the layout interface */
    Vector2i preferredSize(NVGcontext *ctx, const Widget *widget) const;
    void performLayout(NVGcontext *ctx, Widget *widget) const;

protected:
    Orientation mOrientation;
    Alignment mAlignment;
    int mMargin;
    int mSpacing;
};

/// Group layout: vertical arrangement of indented widgets with (non-indented) group labels
class GroupLayout : public Layout {
public:
    GroupLayout(int margin = 15, int spacing = 6, int groupSpacing = 14,
                int groupIndent = 20)
        : mMargin(margin), mSpacing(spacing), mGroupSpacing(groupSpacing),
          mGroupIndent(groupIndent) {}
    
    inline int margin() const { return mMargin; }
    inline void setMargin(int margin) { mMargin = margin; }

    inline int spacing() const { return mSpacing; }
    inline void setSpacing(int spacing) { mSpacing = spacing; }

    inline int groupIndent() const { return mGroupIndent; }
    inline void setGroupIndent(int groupIndent) { mGroupIndent = groupIndent; }

    inline int groupSpacing() const { return mGroupSpacing; }
    inline void setGroupSpacing(int groupSpacing) { mGroupSpacing = groupSpacing; }

    /* Implementation of the layout interface */
    Vector2i preferredSize(NVGcontext *ctx, const Widget *widget) const;
    void performLayout(NVGcontext *ctx, Widget *widget) const;
protected:
    int mMargin;
    int mSpacing;
    int mGroupSpacing;
    int mGroupIndent;
};

/// Grid layout: widgets are aranged in a grid where size determines the max numbers of rows/cols depending on the orientation
class GridLayout: public Layout {
public:
  enum class Orientation {
    Horizontal = 0,
    Vertical
  };

  enum class Alignment {
    Minimum,
    Middle,
    Maximum,
    Fill
  };

  GridLayout(Orientation orientation,int size,int margin = 0,int spacing = 0,Alignment rowAlignment = Alignment::Middle,Alignment colAlignment = Alignment::Middle)
    : mOrientation(orientation),mSize(size),mMargin(margin),mSpacing(spacing),mDefaultRowAlignment(rowAlignment),mDefaultColAlignment(colAlignment){}

  inline Orientation orientation() const { return mOrientation; }
  inline void setOrientation(Orientation orientation) { mOrientation = orientation; }

  inline int size() const { return mSize; }
  inline void setSize(int size) { mSize = size; }

  inline int margin() const { return mMargin; }
  inline void setCols(int margin) { mMargin = margin; }

  inline int spacing() const { return mSpacing; }
  inline void setSpacing(int spacing) { mSpacing = spacing; }

  inline Alignment defaultRowAlignment() const { return mDefaultRowAlignment; }
  inline void setDefaultRowAlignment(Alignment defaultRowAlignment) { mDefaultRowAlignment = defaultRowAlignment; }

  inline Alignment defaultColAlignment() const { return mDefaultColAlignment; }
  inline void setDefaultColAlignment(Alignment defaultColAlignment) { mDefaultColAlignment = defaultColAlignment; }

  inline const std::vector<Alignment>& rowAlignment() const { return mRowAlignment; }
  inline void setRowAlignment(const std::vector<Alignment>& rowAlignment) { mRowAlignment = rowAlignment; }

  inline const std::vector<Alignment>& colAlignment() const { return mColAlignment; }
  inline void setColAlignment(const std::vector<Alignment>& colAlignment) { mColAlignment = colAlignment; }

  /* Implementation of the layout interface */
  Vector2i preferredSize(NVGcontext *ctx,const Widget *widget) const;
  void performLayout(NVGcontext *ctx,Widget *widget) const;

  // helper functions
  void computeGridLayout(NVGcontext *ctx,const Widget *widget,std::vector<std::vector<int>>& grid) const;

protected:
  Orientation mOrientation;
  Alignment mDefaultRowAlignment;
  Alignment mDefaultColAlignment;
  std::vector<Alignment> mRowAlignment;
  std::vector<Alignment> mColAlignment;
  int mSize;
  int mMargin;
  int mSpacing;
};

NANOGUI_NAMESPACE_END

#endif /* __NANOGUI_LAYOUT_H */
