#pragma once

#include <nanogui/object.h>

NANOGUI_NAMESPACE_BEGIN

/// Basic interface of a layout engine
class NANOGUI_EXPORT Layout : public Object {
public:
    virtual void performLayout(NVGcontext *ctx, Widget *widget) const = 0;
    virtual Vector2i preferredSize(NVGcontext *ctx, const Widget *widget) const = 0;
protected:
    virtual ~Layout() { }
};

/// Simple horizontal/vertical box layout
class NANOGUI_EXPORT BoxLayout : public Layout {
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

    BoxLayout(Orientation orientation, Alignment alignment = Alignment::Middle,
              int margin = 0, int spacing = 0)
        : mOrientation(orientation), mAlignment(alignment), mMargin(margin),
          mSpacing(spacing) {}

    Orientation orientation() const { return mOrientation; }
    void setOrientation(Orientation orientation) { mOrientation = orientation; }

    Alignment alignment() const { return mAlignment; }
    void setAlignment(Alignment alignment) { mAlignment = alignment; }

    int margin() const { return mMargin; }
    void setMargin(int margin) { mMargin = margin; }

    int spacing() const { return mSpacing; }
    void setSpacing(int spacing) { mSpacing = spacing; }

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
    Vector2i preferredSize(NVGcontext *ctx, const Widget *widget) const;
    void performLayout(NVGcontext *ctx, Widget *widget) const;
protected:
    int mMargin;
    int mSpacing;
    int mGroupSpacing;
    int mGroupIndent;
};

/**
 * \brief Grid layout
 *
 * Widgets are aranged in a grid that has a fixed grid resolution \c resolution
 * along one of the axes. The layout orientation indicates the fixed dimension;
 * widgets are also appended on this axis.
 */
class NANOGUI_EXPORT GridLayout : public Layout {
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

    /// Create a 2-column grid layout by default
    GridLayout(Orientation orientation = Orientation::Horizontal, int resolution = 2,
               Alignment alignment = Alignment::Middle,
               int margin = 0, int spacing = 0)
        : mOrientation(orientation), mResolution(resolution) {
        mDefaultAlignment[0] = mDefaultAlignment[1] = alignment;
        mSpacing = Vector2i::Constant(spacing);
        mMargin = Vector2i::Constant(margin);
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

    int margin(int axis) const { return mMargin[axis]; }
    void setMargin(int axis, int margin) { mMargin[axis] = margin; }
    void setMargin(int margin) { mMargin[0] = mMargin[1] = margin; }

    Alignment alignment(int axis, int item) const {
        if (item < (int) mAlignment[axis].size())
            return mAlignment[axis][item];
        else
            return mDefaultAlignment[axis];
    }
    void setAlignment(int axis, Alignment value) {
        mDefaultAlignment[axis] = value;
    }
    void setAlignment(int axis, int item, Alignment value) {
        if (item >= (int) mAlignment[axis].size())
            mAlignment[axis].resize(item+1, mDefaultAlignment[axis]);
        mAlignment[axis][item] = value;
    }
    void setColAlignment(const std::vector<Alignment> &value) { mAlignment[0] = value; }
    void setRowAlignment(const std::vector<Alignment> &value) { mAlignment[1] = value; }

    /* Implementation of the layout interface */
    Vector2i preferredSize(NVGcontext *ctx, const Widget *widget) const;
    void performLayout(NVGcontext *ctx, Widget *widget) const;

protected:
    // Compute the maximum row and column sizes
    void computeGridLayout(NVGcontext *ctx, const Widget *widget,
                           std::vector<int> *grid) const;

protected:
    Orientation mOrientation;
    Alignment mDefaultAlignment[2];
    std::vector<Alignment> mAlignment[2];
    int mResolution;
    Vector2i mMargin;
    Vector2i mSpacing;
};

NANOGUI_NAMESPACE_END
