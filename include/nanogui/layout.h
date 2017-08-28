/*
    nanogui/layout.h -- A collection of useful layout managers

    The grid layout was contributed by Christian Schueller.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/compat.h>
#include <nanogui/object.h>
#include <unordered_map>

NAMESPACE_BEGIN(nanogui)

/// The different kinds of alignments a layout can perform.
enum class Alignment : uint8_t {
    Minimum = 0,
    Middle,
    Maximum,
    Fill
};

/// The direction of data flow for a layout.
enum class Orientation {
    Horizontal = 0,
    Vertical
};

/**
 * \class Layout layout.h nanogui/layout.h
 *
 * \brief Basic interface of a layout engine.
 */
class NANOGUI_EXPORT Layout : public Object {
public:
    virtual void performLayout(NVGcontext *ctx, Widget *widget) const = 0;
    virtual Vector2i preferredSize(NVGcontext *ctx, const Widget *widget) const = 0;
protected:
    virtual ~Layout() { }
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
    /**
     * \brief Construct a box layout which packs widgets in the given \c Orientation
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

    Orientation orientation() const { return mOrientation; }
    void setOrientation(Orientation orientation) { mOrientation = orientation; }

    Alignment alignment() const { return mAlignment; }
    void setAlignment(Alignment alignment) { mAlignment = alignment; }

    int margin() const { return mMargin; }
    void setMargin(int margin) { mMargin = margin; }

    int spacing() const { return mSpacing; }
    void setSpacing(int spacing) { mSpacing = spacing; }

    /* Implementation of the layout interface */
    virtual Vector2i preferredSize(NVGcontext *ctx, const Widget *widget) const override;
    virtual void performLayout(NVGcontext *ctx, Widget *widget) const override;

protected:
    Orientation mOrientation;
    Alignment mAlignment;
    int mMargin;
    int mSpacing;
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
    GroupLayout(int margin = 15, int spacing = 6, int groupSpacing = 14,
                int groupIndent = 20)
        : mMargin(margin), mSpacing(spacing), mGroupSpacing(groupSpacing),
          mGroupIndent(groupIndent) {}

    int margin() const { return mMargin; }
    void setMargin(int margin) { mMargin = margin; }

    int spacing() const { return mSpacing; }
    void setSpacing(int spacing) { mSpacing = spacing; }

    int groupIndent() const { return mGroupIndent; }
    void setGroupIndent(int groupIndent) { mGroupIndent = groupIndent; }

    int groupSpacing() const { return mGroupSpacing; }
    void setGroupSpacing(int groupSpacing) { mGroupSpacing = groupSpacing; }

    /* Implementation of the layout interface */
    virtual Vector2i preferredSize(NVGcontext *ctx, const Widget *widget) const override;
    virtual void performLayout(NVGcontext *ctx, Widget *widget) const override;

protected:
    int mMargin;
    int mSpacing;
    int mGroupSpacing;
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
class NANOGUI_EXPORT GridLayout : public Layout {
public:
    /// Create a 2-column grid layout by default
    GridLayout(Orientation orientation = Orientation::Horizontal, int resolution = 2,
               Alignment alignment = Alignment::Middle,
               int margin = 0, int spacing = 0)
        : mOrientation(orientation), mResolution(resolution), mMargin(margin) {
        mDefaultAlignment[0] = mDefaultAlignment[1] = alignment;
        mSpacing = Vector2i::Constant(spacing);
    }

    Orientation orientation() const { return mOrientation; }
    void setOrientation(Orientation orientation) {
        mOrientation = orientation;
    }

    int resolution() const { return mResolution; }
    void setResolution(int resolution) { mResolution = resolution; }

    int spacing(int axis) const { return mSpacing[axis]; }
    void setSpacing(int axis, int spacing) { mSpacing[axis] = spacing; }
    void setSpacing(int spacing) { mSpacing[0] = mSpacing[1] = spacing; }

    int margin() const { return mMargin; }
    void setMargin(int margin) { mMargin = margin; }

    Alignment alignment(int axis, int item) const {
        if (item < (int) mAlignment[axis].size())
            return mAlignment[axis][item];
        else
            return mDefaultAlignment[axis];
    }

    void setColAlignment(Alignment value) { mDefaultAlignment[0] = value; }
    void setRowAlignment(Alignment value) { mDefaultAlignment[1] = value; }
    void setColAlignment(const std::vector<Alignment> &value) { mAlignment[0] = value; }
    void setRowAlignment(const std::vector<Alignment> &value) { mAlignment[1] = value; }

    /* Implementation of the layout interface */
    virtual Vector2i preferredSize(NVGcontext *ctx, const Widget *widget) const override;
    virtual void performLayout(NVGcontext *ctx, Widget *widget) const override;

protected:
    // Compute the maximum row and column sizes
    void computeLayout(NVGcontext *ctx, const Widget *widget,
                       std::vector<int> *grid) const;

protected:
    Orientation mOrientation;
    Alignment mDefaultAlignment[2];
    std::vector<Alignment> mAlignment[2];
    int mResolution;
    Vector2i mSpacing;
    int mMargin;
public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
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
    /**
     * \struct Anchor layout.h nanogui/layout.h
     *
     * \brief Helper struct to coordinate anchor points for the layout.
     */
    struct Anchor {
        uint8_t pos[2];
        uint8_t size[2];
        Alignment align[2];

        Anchor() { }

        Anchor(int x, int y, Alignment horiz = Alignment::Fill,
              Alignment vert = Alignment::Fill) {
            pos[0] = (uint8_t) x; pos[1] = (uint8_t) y;
            size[0] = size[1] = 1;
            align[0] = horiz; align[1] = vert;
        }

        Anchor(int x, int y, int w, int h,
              Alignment horiz = Alignment::Fill,
              Alignment vert = Alignment::Fill) {
            pos[0] = (uint8_t) x; pos[1] = (uint8_t) y;
            size[0] = (uint8_t) w; size[1] = (uint8_t) h;
            align[0] = horiz; align[1] = vert;
        }

        operator std::string() const {
            char buf[50];
            NANOGUI_SNPRINTF(buf, 50, "Format[pos=(%i, %i), size=(%i, %i), align=(%i, %i)]",
                pos[0], pos[1], size[0], size[1], (int) align[0], (int) align[1]);
            return buf;
        }
    };

    /// Create an AdvancedGridLayout by specifying a set of column and row sizes.
    AdvancedGridLayout(const std::vector<int> &cols = {}, const std::vector<int> &rows = {}, int margin = 0);

    int margin() const { return mMargin; }
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

    /// Remove the anchor data structure for a given widget.
    void removeAnchor(const Widget *widget) { mAnchor.erase(widget); }

    /// Retrieve the anchor data structure for a given widget
    Anchor anchor(const Widget *widget) const {
        auto it = mAnchor.find(widget);
        if (it == mAnchor.end())
            throw std::runtime_error("Widget was not registered with the grid layout!");
        return it->second;
    }

    /* Implementation of the layout interface */
    virtual Vector2i preferredSize(NVGcontext *ctx, const Widget *widget) const override;
    virtual void performLayout(NVGcontext *ctx, Widget *widget) const override;

protected:
    void computeLayout(NVGcontext *ctx, const Widget *widget,
                       std::vector<int> *grid) const;

protected:
    std::vector<int> mCols, mRows;
    std::vector<float> mColStretch, mRowStretch;
    std::unordered_map<const Widget *, Anchor> mAnchor;
    int mMargin;
};

NAMESPACE_END(nanogui)
