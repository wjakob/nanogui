#if !defined(__NANOGUI_LAYOUT_H)
#define __NANOGUI_LAYOUT_H

#include <nanogui/nanogui.h>

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

NANOGUI_NAMESPACE_END

#endif /* __NANOGUI_LAYOUT_H */
