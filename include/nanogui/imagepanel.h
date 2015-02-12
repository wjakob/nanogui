#if !defined(__NANOGUI_IMAGEPANEL_H)
#define __NANOGUI_IMAGEPANEL_H

#include <nanogui/widget.h>

NANOGUI_NAMESPACE_BEGIN

class ImagePanel : public Widget {
public:
    ImagePanel(Widget *parent);

    void setImageData(const std::vector<std::pair<int, std::string>> &data) { mImages = data; }

    inline std::function<void(int)> callback() const { return mCallback; }
    inline void setCallback(std::function<void(int)> callback) { mCallback = callback; }

    virtual bool mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers);
    virtual bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers);
    virtual Vector2i preferredSize(NVGcontext *ctx) const;
    virtual void draw(NVGcontext* ctx);
protected:
    Vector2i gridSize() const;
    int indexForPosition(const Vector2i &p) const;
protected:
    std::vector<std::pair<int, std::string>> mImages;
    std::function<void(int)> mCallback;
    int mThumbSize;
    int mSpacing;
    int mMargin;
    int mMouseIndex;
};

NANOGUI_NAMESPACE_END

#endif /* __NANOGUI_IMAGEPANEL_H */
