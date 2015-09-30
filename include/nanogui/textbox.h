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

    bool isEditable() const { return mEditable; }
    void setEditable(bool editable);

    const std::string &value() const { return mValue; }
    void setValue(const std::string &value) { mValue = value; }

    const std::string &defaultValue() const { return mDefaultValue; }
    void setDefaultValue(const std::string &defaultValue) { mDefaultValue = defaultValue; }

    Alignment alignment() const { return mAlignment; }
    void setAlignment(Alignment align) { mAlignment = align; }

    const std::string &units() const { return mUnits; }
    void setUnits(const std::string &units) { mUnits = units; }

    /// Return the underlying regular expression specifying valid formats
    const std::string &format() const { return mFormat; }
    /// Specify a regular expression specifying valid formats
    void setFormat(const std::string &format) { mFormat = format; }

    int unitsImage() const { return mUnitsImage; }
    void setUnitsImage(int image) { mUnitsImage = image; }

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
};

NANOGUI_NAMESPACE_END
