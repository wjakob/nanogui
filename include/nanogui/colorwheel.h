#if !defined(__NANOGUI_COLORWHEEL_H)
#define __NANOGUI_COLORWHEEL_H

#include <nanogui/widget.h>

NANOGUI_NAMESPACE_BEGIN

class ColorWheel : public Widget {
public:
    ColorWheel(Widget *parent, const Vector3f& color = { 1., 0., 0. });

    virtual Vector2i preferredSize(NVGcontext *ctx) const;

    virtual void draw(NVGcontext *ctx);

    virtual bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers);
    virtual bool mouseDragEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers);

    /// Set the push callback (for any type of button)
    inline std::function<void(const Vector3f &)> callback() const           { return mCallback; }
    inline void setCallback(std::function<void(const Vector3f &)> callback) { mCallback = callback; }

    Vector3f color() const;

private:
    enum Region {
        None = 0,
        InnerTriangle = 1,
        OuterCircle = 2,
        Both = 3
    };

    Vector3f hue2rgb(float h) const;
    Region adjustPosition(const Vector2i &p, Region consideredRegions = Both);

protected:
    float mHue;
    float mWhite;
    float mBlack;
    Region mDragRegion;
    std::function<void(const Vector3f &)> mCallback;
};

NANOGUI_NAMESPACE_END

#endif /* __NANOGUI_COLORWHEEL_H */
