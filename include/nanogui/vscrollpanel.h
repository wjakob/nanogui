#if !defined(__NANOGUI_VSCROLLPANEL_H)
#define __NANOGUI_VSCROLLPANEL_H

#include <nanogui/widget.h>

NANOGUI_NAMESPACE_BEGIN

class VScrollPanel : public Widget {
public:
    VScrollPanel(Widget *parent);

    virtual void performLayout(NVGcontext *ctx);
    virtual Vector2i preferredSize(NVGcontext *ctx) const;
    virtual bool mouseDragEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers);
    virtual bool scrollEvent(const Vector2i &p, const Vector2f &rel);
    virtual bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers);
    virtual bool mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers);
    virtual void draw(NVGcontext *ctx);
protected:
    int mChildPreferredHeight;
    float mScroll;
};

NANOGUI_NAMESPACE_END

#endif /* __NANOGUI_VSCROLLPANEL_H */
