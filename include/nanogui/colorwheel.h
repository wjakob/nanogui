#if !defined(__NANOGUI_COLORWHEEL_H)
#define __NANOGUI_COLORWHEEL_H

#include <nanogui/widget.h>
#include <nanogui/opengl.h>

NANOGUI_NAMESPACE_BEGIN

class ColorWheel : public Widget {
public:
    ColorWheel(Widget *parent, const Vector3f& color = { 1., 0., 0. });

    virtual Vector2i preferredSize(NVGcontext *ctx) const;

    virtual void draw(NVGcontext *ctx);

    virtual bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers);
    virtual bool mouseDragEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers);

    void         adjustPosition(const Vector2i& p);

    /// Set the push callback (for any type of button)
    inline std::function<void()> callback() const           { return mCallback; }
    inline void setCallback(std::function<void()> callback) { mCallback = callback; }

    Vector3f color() const;

private:
    Vector3f hue2rgb(float h) const;


protected:
    float       mHue;
    float       mWhite;
    float       mBlack;
    std::function<void()> mCallback;
};

NANOGUI_NAMESPACE_END

#endif /* __NANOGUI_COLORWHEEL_H */
