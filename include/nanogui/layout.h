#pragma once

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
class NANOGUI_EXPORT BoxLayout : public Layout {
public:
    enum Orientation {
        Horizontal = 0,
        Vertical
    };

    enum Alignment {
        Minimum,
        Middle,
        Maximum
    };

    BoxLayout(Orientation orientation, Alignment alignment = Middle, int margin = 0, int spacing = 0)
        : mOrientation(orientation), mAlignment(alignment), mMargin(margin), mSpacing(spacing) {}

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

NANOGUI_NAMESPACE_END
