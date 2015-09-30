#pragma once

#include <nanogui/widget.h>

NANOGUI_NAMESPACE_BEGIN

class NANOGUI_EXPORT CheckBox : public Widget {
public:
    CheckBox(Widget *parent, const std::string &caption = "Untitled",
             const std::function<void(bool)> &callback = std::function<void(bool)>());

    inline const std::string &caption() const { return mCaption; }
    inline void setCaption(const std::string &caption) { mCaption = caption; }

    inline const bool &checked() const { return mChecked; }
    inline void setChecked(const bool &checked) { mChecked = checked; }

    inline const bool &pushed() const { return mPushed; }
    inline void setPushed(const bool &pushed) { mPushed = pushed; }

    inline std::function<void(bool)> callback() const { return mCallback; }
    inline void setCallback(std::function<void(bool)> callback) { mCallback = callback; }

    virtual bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers);
    virtual Vector2i preferredSize(NVGcontext *ctx) const;
    virtual void draw(NVGcontext *ctx);
protected:
    std::string mCaption;
    bool mPushed, mChecked;
    std::function<void(bool)> mCallback;
};

NANOGUI_NAMESPACE_END
