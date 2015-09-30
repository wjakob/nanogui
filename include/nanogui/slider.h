#pragma once

#include <nanogui/widget.h>

NANOGUI_NAMESPACE_BEGIN

class NANOGUI_EXPORT Slider : public Widget {
public:
    Slider(Widget *parent, const std::string &caption = "Untitled");

    inline float value() const { return mValue; }
    inline void setValue(float value) { mValue = value; }

    inline std::pair<float, float> unsafeRange() const { return mUnsafeRange; }
    inline void setUnsafeRange(std::pair<float, float> unsafeRange) { mUnsafeRange = unsafeRange; }

    inline std::function<void(float)> callback() const { return mCallback; }
    inline void setCallback(std::function<void(float)> callback) { mCallback = callback; }

    inline std::function<void(float)> finalCallback() const { return mFinalCallback; }
    inline void setFinalCallback(std::function<void(float)> callback) { mFinalCallback = callback; }

    virtual Vector2i preferredSize(NVGcontext *ctx) const;
    virtual bool mouseDragEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers);
    virtual bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers);
    virtual void draw(NVGcontext* ctx);

protected:
    float mValue;
    std::function<void(float)> mCallback;
    std::function<void(float)> mFinalCallback;
    std::pair<float, float> mUnsafeRange;
};

NANOGUI_NAMESPACE_END
