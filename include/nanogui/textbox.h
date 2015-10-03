#pragma once

#include <nanogui/widget.h>

NANOGUI_NAMESPACE_BEGIN

class NANOGUI_EXPORT TextBox : public Widget {
public:
    enum class Alignment {
        Left,
        Center,
        Right
    };

    TextBox(Widget *parent, const std::string &value = "Untitled");

    bool editable() const { return mEditable; }
    void setEditable(bool editable);

    const std::string &value() const { return mValue; }
    void setValue(const std::string &value) { mValue = value; }

    const std::string &defaultValue() const { return mDefaultValue; }
    void setDefaultValue(const std::string &defaultValue) { mDefaultValue = defaultValue; }

    Alignment alignment() const { return mAlignment; }
    void setAlignment(Alignment align) { mAlignment = align; }

    const std::string &units() const { return mUnits; }
    void setUnits(const std::string &units) { mUnits = units; }

    int unitsImage() const { return mUnitsImage; }
    void setUnitsImage(int image) { mUnitsImage = image; }

    /// Return the underlying regular expression specifying valid formats
    const std::string &format() const { return mFormat; }
    /// Specify a regular expression specifying valid formats
    void setFormat(const std::string &format) { mFormat = format; }

    /// Set the change callback
    std::function<bool(const std::string& str)> callback() const { return mCallback; }
    void setCallback(std::function<bool(const std::string& str)> callback) { mCallback = callback; }

    bool mouseButtonEvent(const Vector2i &p,int button,bool down,int modifiers);
    bool mouseMotionEvent(const Vector2i &p,const Vector2i &rel,int button,int modifiers);
    bool mouseDragEvent(const Vector2i &p,const Vector2i &rel,int button,int modifiers);
    bool mouseEnterEvent(const Vector2i &p,bool enter);
    bool focusEvent(bool focused);
    bool keyboardEvent(int key, int scancode, int action, int modifiers);
    bool keyboardCharacterEvent(unsigned int codepoint);

    Vector2i preferredSize(NVGcontext *ctx) const;
    void draw(NVGcontext* ctx);
protected:
    bool checkFormat(const std::string& input,const std::string& format);
    bool copySelection();
    void pasteFromClipboard();
    bool deleteSelection();

    void updateCursor(NVGcontext *ctx, float lastx,
                      const NVGglyphPosition *glyphs, int size);
    float cursorIndex2Position(int index, float lastx,
                               const NVGglyphPosition *glyphs, int size);
    int position2CursorIndex(float posx, float lastx,
                             const NVGglyphPosition *glyphs, int size);
protected:
    bool mEditable;
    bool mCommitted;
    std::string mValue;
    std::string mDefaultValue;
    Alignment mAlignment;
    std::string mUnits;
    std::string mFormat;
    int mUnitsImage;
    std::function<bool(const std::string& str)> mCallback;
    bool mValidFormat;
    std::string mValueTemp;
    int mCursorPos;
    int mSelectionPos;
    Vector2i mMousePos;
    Vector2i mMouseDownPos;
    Vector2i mMouseDragPos;
    int mMouseDownModifier;
    float mTextOffset;
    double mLastClick;
};

class IntBox : public TextBox {
public:
    IntBox(Widget *parent, const std::string &value = "0") : TextBox(parent, value) {
        setDefaultValue("0");
        setFormat("[-]?[0-9]*");
    }

    int value() const {
        return std::stoi(TextBox::value());
    }

    void setValue(int value) {
        TextBox::setValue(std::to_string(value));
    }

    void setCallback(const std::function<void(int)> cb) {
        TextBox::setCallback(
            [cb](const std::string &str) { cb(std::stoi(str)); return true; });
    }
};

class UIntBox : public TextBox {
protected:
    static unsigned stou(std::string const & str, size_t * idx = 0, int base = 10) {
        unsigned long result = std::stoul(str, idx, base);
        if (result > std::numeric_limits<unsigned>::max())
            throw std::out_of_range("stou");
        return result;
    }
public:
    UIntBox(Widget *parent, const std::string &value = "0") : TextBox(parent, value) {
        setDefaultValue("0");
        setFormat("[0-9]*");
    }

    unsigned value() const {
        return stou(TextBox::value());
    }

    void setValue(unsigned value) {
        TextBox::setValue(std::to_string(value));
    }

    void setCallback(const std::function<void(unsigned)> cb) {
        TextBox::setCallback(
            [cb](const std::string &str) { cb(stou(str)); return true; });
    }
};

template <typename Scalar> class FloatBox : public TextBox {
public:
    FloatBox(Widget *parent, const std::string &value = "0") : TextBox(parent, value) {
        setDefaultValue("0");
        setFormat("[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?");
    }

    Scalar value() const {
        return (Scalar) std::stod(TextBox::value());
    }

    void setValue(Scalar value) {
        char buffer[30];
        snprintf(buffer, 30, sizeof(Scalar) == sizeof(float) ? "%.4g" : "%.7g", value);
        TextBox::setValue(buffer);
    }

    void setCallback(const std::function<void(Scalar)> cb) {
        TextBox::setCallback(
            [cb](const std::string &str) { cb(std::stof(str)); return true; });
    }
};

NANOGUI_NAMESPACE_END
