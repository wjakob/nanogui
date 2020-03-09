/*
    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/**
 * \file nanogui/layout.h
 *
 * \brief A collection of useful layout managers.  The \ref nanogui::GridLayout
 *        was contributed by Christian Schueller.
 */

#pragma once

#include <nanogui/object.h>
#include <cstdio>
#include <unordered_map>

NAMESPACE_BEGIN(nanogui)

/// The different kinds of alignments a layout can perform.
enum class Alignment : uint8_t {
    Minimum = 0, ///< Take only as much space as is required.
    Middle,      ///< Center align.
    Maximum,     ///< Take as much space as is allowed.
    Fill         ///< Fill according to preferred sizes.
};

/// !
enum class TextAlignment { 
  Auto = 0, ///<
  Left,     ///<
  Center,   ///<
  Right     ///<
};

/// The direction of data flow for a layout.
enum class Orientation {
    Horizontal = 0, ///< Layout expands on horizontal axis.
    Vertical,       ///< Layout expands on vertical axis.
    ReverseHorizontal,
    ReverseVertical
};

/**
 * \class Layout layout.h nanogui/layout.h
 *
 * \brief Basic interface of a layout engine.
 */
class Widget;
class NANOGUI_EXPORT Layout : public Object {
public:
    RTTI_CLASS_UID(Layout)
    RTTI_DECLARE_INFO(Layout)
    /**
     * Performs any and all resizing applicable.
     *
     * \param ctx
     *     The ``NanoVG`` context being used for drawing.
     *
     * \param widget
     *     The Widget this layout is controlling sizing for.
     */
    virtual void performLayout(NVGcontext *ctx, Widget *widget) const = 0;

    /**
     * The preferred size for this layout.
     *
     * \param ctx
     *     The ``NanoVG`` context being used for drawing.
     *
     * \param widget
     *     The Widget this layout's preferred size is considering.
     *
     * \return
     *     The preferred size, accounting for things such as spacing, padding
     *     for icons, etc.
     */
    virtual Vector2i preferredSize(NVGcontext *ctx, const Widget *widget) const = 0;

    template<typename FF, typename none = void> void set() {}
    void setId(int id) { mId = id; }

    int getId() const { return mId; }
       
protected:
    /// Default destructor (exists for inheritance).
    virtual ~Layout() { }
    int mId = -1;
};

/**
 * \class BoxLayout layout.h nanogui/layout.h
 *
 * \brief Simple horizontal/vertical box layout
 *
 * This widget stacks up a bunch of widgets horizontally or vertically. It adds
 * margins around the entire container and a custom spacing between adjacent
 * widgets.
 */
class NANOGUI_EXPORT BoxLayout : public Layout {
public:
    RTTI_CLASS_UID(BoxLayout)
    RTTI_DECLARE_INFO(BoxLayout)

    /**
     * \brief Construct a box layout which packs widgets in the given \c Orientation
     *
     * \param orientation
     *     The Orientation this BoxLayout expands along
     *
     * \param alignment
     *     Widget alignment perpendicular to the chosen orientation
     *
     * \param margin
     *     Margin around the layout container
     *
     * \param spacing
     *     Extra spacing placed between widgets
     */
    BoxLayout(Orientation orientation, Alignment alignment = Alignment::Middle,
              int margin = 0, int spacing = 0);

    /// The Orientation this BoxLayout is using.
    Orientation orientation() const { return mOrientation; }

    /// Sets the Orientation of this BoxLayout.
    void setOrientation(Orientation orientation) { mOrientation = orientation; }

    /// The Alignment of this BoxLayout.
    Alignment alignment() const { return mAlignment; }

    /// Sets the Alignment of this BoxLayout.
    void setAlignment(Alignment alignment) { mAlignment = alignment; }

    /// The margin of this BoxLayout.
    int margin() const { return mMargin; }

    /// Sets the margin of this BoxLayout.
    void setMargin(int margin) { mMargin = margin; }

    /// The spacing this BoxLayout is using to pad in between widgets.
    int spacing() const { return mSpacing; }

    /// Sets the spacing of this BoxLayout.
    void setSpacing(int spacing) { mSpacing = spacing; }

    /* Implementation of the layout interface */
    /// See \ref Layout::preferredSize.
    virtual Vector2i preferredSize(NVGcontext *ctx, const Widget *widget) const override;

    /// See \ref Layout::performLayout.
    virtual void performLayout(NVGcontext *ctx, Widget *widget) const override;

protected:
    /// The Orientation of this BoxLayout.
    Orientation mOrientation;

    /// The Alignment of this BoxLayout.
    Alignment mAlignment;

    /// The margin of this BoxLayout.
    int mMargin;

    /// The spacing between widgets of this BoxLayout.
    int mSpacing;
};

class NANOGUI_EXPORT StretchLayout : public BoxLayout
{
public:
  RTTI_CLASS_UID(StretchLayout)
  RTTI_DECLARE_INFO(StretchLayout)

  StretchLayout(Orientation orientation) : BoxLayout(orientation) {}
  StretchLayout(Orientation orientation, int margin, int spacing)
    : BoxLayout(orientation, Alignment::Middle, margin, spacing) {}

  void performLayout(NVGcontext* ctx, Widget* widget) const override;
  Vector2i preferredSize(NVGcontext *ctx, const Widget *widget) const override;
};

/**
 * \class GroupLayout layout.h nanogui/layout.h
 *
 * \brief Special layout for widgets grouped by labels.
 *
 * This widget resembles a box layout in that it arranges a set of widgets
 * vertically. All widgets are indented on the horizontal axis except for
 * \ref Label widgets, which are not indented.
 *
 * This creates a pleasing layout where a number of widgets are grouped
 * under some high-level heading.
 */
class NANOGUI_EXPORT GroupLayout : public Layout {
public:
  RTTI_CLASS_UID(GroupLayout)
  RTTI_DECLARE_INFO(GroupLayout)

    /**
     * Creates a GroupLayout.
     *
     * \param margin
     *     The margin around the widgets added.
     *
     * \param spacing
     *     The spacing between widgets added.
     *
     * \param groupSpacing
     *     The spacing between groups (groups are defined by each Label added).
     *
     * \param groupIndent
     *     The amount to indent widgets in a group (underneath a Label).
     */
    GroupLayout(int margin = 15, int spacing = 6, int groupSpacing = 14,
                int groupIndent = 20)
        : mMargin(margin), mSpacing(spacing), mGroupSpacing(groupSpacing),
          mGroupIndent(groupIndent) {}

    /// The margin of this GroupLayout.
    int margin() const { return mMargin; }

    /// Sets the margin of this GroupLayout.
    void setMargin(int margin) { mMargin = margin; }

    /// The spacing between widgets of this GroupLayout.
    int spacing() const { return mSpacing; }

    /// Sets the spacing between widgets of this GroupLayout.
    void setSpacing(int spacing) { mSpacing = spacing; }

    /// The indent of widgets in a group (underneath a Label) of this GroupLayout.
    int groupIndent() const { return mGroupIndent; }

    /// Sets the indent of widgets in a group (underneath a Label) of this GroupLayout.
    void setGroupIndent(int groupIndent) { mGroupIndent = groupIndent; }

    /// The spacing between groups of this GroupLayout.
    int groupSpacing() const { return mGroupSpacing; }

    /// Sets the spacing between groups of this GroupLayout.
    void setGroupSpacing(int groupSpacing) { mGroupSpacing = groupSpacing; }

    /* Implementation of the layout interface */
    /// See \ref Layout::preferredSize.
    Vector2i preferredSize(NVGcontext *ctx, const Widget *widget) const override;

    /// See \ref Layout::performLayout.
    void performLayout(NVGcontext *ctx, Widget *widget) const override;

protected:
    /// The margin of this GroupLayout.
    int mMargin;

    /// The spacing between widgets of this GroupLayout.
    int mSpacing;

    /// The spacing between groups of this GroupLayout.
    int mGroupSpacing;

    /// The indent amount of a group under its defining Label of this GroupLayout.
    int mGroupIndent;
};

/**
 * \class GridLayout layout.h nanogui/layout.h
 *
 * \brief Grid layout.
 *
 * Widgets are arranged in a grid that has a fixed grid resolution \c resolution
 * along one of the axes. The layout orientation indicates the fixed dimension;
 * widgets are also appended on this axis. The spacing between items can be
 * specified per axis. The horizontal/vertical alignment can be specified per
 * row and column.
 */
struct NANOGUI_EXPORT ColumnsAligment {
  std::vector<Alignment> value;
  ColumnsAligment(std::initializer_list<Alignment> l)
  { for (auto& a: l) value.push_back(a); };
};

DECLSETTER(GridLayoutColAlignment, Alignment)
DECLSETTERILIST(GridLayoutSplit,std::vector<float>)
DECLSETTER(LayoutMargin, int)
DECLSETTER(LayoutSpacing, int)
DECLSETTER(LayoutHorSpacing, int)

class NANOGUI_EXPORT GridLayout : public Layout {
public:
  RTTI_CLASS_UID(GridLayout)
  RTTI_DECLARE_INFO(GridLayout)

    /**
     * Create a 2-column grid layout by default.
     *
     * \param orientation
     *     The fixed dimension of this GridLayout.
     *
     * \param resolution
     *     The number of rows or columns in the grid (depending on the Orientation).
     *
     * \param alignment
     *     How widgets should be aligned within each grid cell.
     *
     * \param margin
     *     The amount of spacing to add around the border of the grid.
     *
     * \param spacing
     *     The amount of spacing between widgets added to the grid.
     */
    explicit GridLayout(Orientation orientation = Orientation::Horizontal, int resolution = 2,
                        Alignment alignment = Alignment::Middle,
                        int margin = 0, int spacing = 0)
        : mOrientation(orientation), mResolution(resolution), mMargin(margin) {
        mDefaultAlignment[0] = mDefaultAlignment[1] = alignment;
        mSpacing = Vector2i::Constant(spacing);
    }

    using Layout::set;
    template<typename... Args>
    GridLayout(const Args&... args)
      : GridLayout(Orientation::Horizontal,2,Alignment::Middle,0,0) { set<GridLayout, Args...>(args...); }

    /// The Orientation of this GridLayout.
    Orientation orientation() const { return mOrientation; }

    /// Sets the Orientation of this GridLayout.
    void setOrientation(Orientation orientation) { mOrientation = orientation; }

    /// The number of rows or columns (depending on the Orientation) of this GridLayout.
    int resolution() const { return mResolution; }

    /// Sets the number of rows or columns (depending on the Orientation) of this GridLayout.
    void setResolution(int resolution) { mResolution = resolution; }

    /// The spacing at the specified axis (row or column number, depending on the Orientation).
    int spacing(int axis) const { return mSpacing[axis]; }

    /// Sets the spacing for a specific axis.
    void setSpacing(int axis, int spacing) { mSpacing[axis] = spacing; }
    void setHorSpacing(int spacing) { mSpacing.x() = spacing; }

    /// Sets the spacing for all axes.
    void setSpacing(int spacing) { mSpacing.x() = mSpacing.y() = spacing; }

    /// The margin around this GridLayout.
    int margin() const { return mMargin; }

    /// Sets the margin of this GridLayout.
    void setMargin(int margin) { mMargin = margin; }

    /**
     * The Alignment of the specified axis (row or column number, depending on
     * the Orientation) at the specified index of that row or column.
     */
    Alignment alignment(int axis, int item) const {
        if (item < (int) mAlignment[axis].size())
            return mAlignment[axis][item];
        else
            return mDefaultAlignment[axis];
    }

    /// Sets the Alignment of the columns.
    void setColAlignment(Alignment value) { mDefaultAlignment[0] = value; }

    /// Sets the Alignment of the rows.
    void setRowAlignment(Alignment value) { mDefaultAlignment[1] = value; }

    /// Use this to set variable Alignment for columns.
    void setColAlignment(const std::vector<Alignment> &value) { mAlignment[0] = value; }

    /// Use this to set variable Alignment for rows.
    void setRowAlignment(const std::vector<Alignment> &value) { mAlignment[1] = value; }

    /* Implementation of the layout interface */
    /// See \ref Layout::preferredSize.
    Vector2i preferredSize(NVGcontext *ctx, const Widget *widget) const override;

    /// See \ref Layout::performLayout.
    void performLayout(NVGcontext *ctx, Widget *widget) const override;

    void setRelWidth(std::vector<float> w) { mRelWidth = w; }

protected:
    /// Compute the maximum row and column sizes
    void computeLayout(NVGcontext *ctx, const Widget *widget,
                       std::vector<int> *grid) const;

protected:
    /// The Orientation defining this GridLayout.
    Orientation mOrientation;

    /// The default Alignment for this GridLayout.
    Alignment mDefaultAlignment[2];

    /// The actual Alignment being used.
    std::vector<Alignment> mAlignment[2];

    /// The number of rows or columns before starting a new one, depending on the Orientation.
    int mResolution;

    /// The spacing used for each dimension.
    Vector2i mSpacing;

    std::vector<float> mRelWidth;

    /// The margin around this GridLayout.
    int mMargin;
public:
    PROPSETTER(ColumnsAligment,setColAlignment)
    PROPSETTER(GridLayoutSplit,setRelWidth)
    PROPSETTER(GridLayoutColAlignment,setColAlignment)
    PROPSETTER(LayoutMargin,setMargin)
    PROPSETTER(LayoutSpacing,setSpacing)
    PROPSETTER(LayoutHorSpacing,setHorSpacing)
};

/**
 * \class AdvancedGridLayout layout.h nanogui/layout.h
 *
 * \brief Advanced Grid layout.
 *
 * The is a fancier grid layout with support for items that span multiple rows
 * or columns, and per-widget alignment flags. Each row and column additionally
 * stores a stretch factor that controls how additional space is redistributed.
 * The downside of this flexibility is that a layout anchor data structure must
 * be provided for each widget.
 *
 * An example:
 *
 * \rst
 * .. code-block:: cpp
 *
 *    using AdvancedGridLayout::Anchor;
 *    Label *label = new Label(window, "A label");
 *    // Add a centered label at grid position (1, 5), which spans two horizontal cells
 *    layout->setAnchor(label, Anchor(1, 5, 2, 1, Alignment::Middle, Alignment::Middle));
 *
 * \endrst
 *
 * The grid is initialized with user-specified column and row size vectors
 * (which can be expanded later on if desired). If a size value of zero is
 * specified for a column or row, the size is set to the maximum preferred size
 * of any widgets contained in the same row or column. Any remaining space is
 * redistributed according to the row and column stretch factors.
 *
 * The high level usage somewhat resembles the classic HIG layout:
 *
 * - https://web.archive.org/web/20070813221705/http://www.autel.cz/dmi/tutorial.html
 * - https://github.com/jaapgeurts/higlayout
 */
class NANOGUI_EXPORT AdvancedGridLayout : public Layout {
public:
    RTTI_CLASS_UID(AdvancedGridLayout)
    RTTI_DECLARE_INFO(AdvancedGridLayout)
    /**
     * \struct Anchor layout.h nanogui/layout.h
     *
     * \brief Helper struct to coordinate anchor points for the layout.
     */
    struct Anchor {
        uint8_t pos[2];    ///< The ``(x, y)`` position.
        uint8_t size[2];   ///< The ``(x, y)`` size.
        Alignment align[2];///< The ``(x, y)`` Alignment.

        /// Creates a ``0`` Anchor.
        Anchor() { }

        /// Create an Anchor at position ``(x, y)`` with specified Alignment.
        Anchor(int x, int y, Alignment horiz = Alignment::Fill,
              Alignment vert = Alignment::Fill) {
            pos[0] = (uint8_t) x; pos[1] = (uint8_t) y;
            size[0] = size[1] = 1;
            align[0] = horiz; align[1] = vert;
        }

        /// Create an Anchor at position ``(x, y)`` of size ``(w, h)`` with specified alignments.
        Anchor(int x, int y, int w, int h,
              Alignment horiz = Alignment::Fill,
              Alignment vert = Alignment::Fill) {
            pos[0] = (uint8_t) x; pos[1] = (uint8_t) y;
            size[0] = (uint8_t) w; size[1] = (uint8_t) h;
            align[0] = horiz; align[1] = vert;
        }

        /// Allows for printing out Anchor position, size, and alignment.
        operator std::string() const {
            char buf[50];
            std::snprintf(buf, 50, "Format[pos=(%i, %i), size=(%i, %i), align=(%i, %i)]",
                pos[0], pos[1], size[0], size[1], (int) align[0], (int) align[1]);
            return buf;
        }
    };

    /// Creates an AdvancedGridLayout with specified columns, rows, and margin.
    AdvancedGridLayout(const std::vector<int> &cols = {}, const std::vector<int> &rows = {}, int margin = 0);

    /// The margin of this AdvancedGridLayout.
    int margin() const { return mMargin; }

    /// Sets the margin of this AdvancedGridLayout.
    void setMargin(int margin) { mMargin = margin; }

    /// Return the number of cols
    int colCount() const { return (int) mCols.size(); }

    /// Return the number of rows
    int rowCount() const { return (int) mRows.size(); }

    /// Append a row of the given size (and stretch factor)
    void appendRow(int size, float stretch = 0.f) { mRows.push_back(size); mRowStretch.push_back(stretch); };

    /// Append a column of the given size (and stretch factor)
    void appendCol(int size, float stretch = 0.f) { mCols.push_back(size); mColStretch.push_back(stretch); };

    /// Set the stretch factor of a given row
    void setRowStretch(int index, float stretch) { mRowStretch.at(index) = stretch; }

    /// Set the stretch factor of a given column
    void setColStretch(int index, float stretch) { mColStretch.at(index) = stretch; }

    /// Specify the anchor data structure for a given widget
    void setAnchor(const Widget *widget, const Anchor &anchor) { mAnchor[widget] = anchor; }

    /// Retrieve the anchor data structure for a given widget
    Anchor anchor(const Widget *widget) const {
        auto it = mAnchor.find(widget);
        if (it == mAnchor.end())
            throw std::runtime_error("Widget was not registered with the grid layout!");
        return it->second;
    }

    /* Implementation of the layout interface */
    /// See \ref Layout::preferredSize.
    virtual Vector2i preferredSize(NVGcontext *ctx, const Widget *widget) const override;

    /// See \ref Layout::performLayout.
    virtual void performLayout(NVGcontext *ctx, Widget *widget) const override;

protected:
    /// Computes the layout
    void computeLayout(NVGcontext *ctx, const Widget *widget,
                       std::vector<int> *grid) const;

protected:
    /// The columns of this AdvancedGridLayout.
    std::vector<int> mCols;

    /// The rows of this AdvancedGridLayout.
    std::vector<int> mRows;

    /// The stretch for each column of this AdvancedGridLayout.
    std::vector<float> mColStretch;

    /// The stretch for each row of this AdvancedGridLayout.
    std::vector<float> mRowStretch;

    /// The mapping of widgets to their specified anchor points.
    std::unordered_map<const Widget *, Anchor> mAnchor;

    /// The margin around this AdvancedGridLayout.
    int mMargin;
};

NAMESPACE_END(nanogui)
