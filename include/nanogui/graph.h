#pragma once

#include <nanogui/widget.h>

NANOGUI_NAMESPACE_BEGIN

class NANOGUI_EXPORT Graph : public Widget {
public:
    Graph(Widget *parent, const std::string &caption = "Untitled");

    inline const std::string &caption() const { return mCaption; }
    inline void setCaption(const std::string &caption) { mCaption = caption; }

    inline const std::string &header() const { return mHeader; }
    inline void setHeader(const std::string &header) { mHeader = header; }

    inline const std::string &footer() const { return mFooter; }
    inline void setFooter(const std::string &footer) { mFooter = footer; }

    inline const Color &backgroundColor() const { return mBackgroundColor; }
    inline void setBackgroundColor(const Color &backgroundColor) { mBackgroundColor = backgroundColor; }

    inline const Color &foregroundColor() const { return mForegroundColor; }
    inline void setForegroundColor(const Color &foregroundColor) { mForegroundColor = foregroundColor; }

    inline const Color &textColor() const { return mTextColor; }
    inline void setTextColor(const Color &textColor) { mTextColor = textColor; }

    inline const Eigen::VectorXf &values() const { return mValues; }
    inline Eigen::VectorXf &values() { return mValues; }
    inline void setValues(const Eigen::VectorXf &values) { mValues = values; }

    virtual Vector2i preferredSize(NVGcontext *ctx) const;
    virtual void draw(NVGcontext *ctx);
protected:
    std::string mCaption, mHeader, mFooter;
    Color mBackgroundColor, mForegroundColor, mTextColor;
    Eigen::VectorXf mValues;
};

NANOGUI_NAMESPACE_END
