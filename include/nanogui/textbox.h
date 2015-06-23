#if !defined(__NANOGUI_TEXTBOX_H)
#define __NANOGUI_TEXTBOX_H

#include <nanogui/widget.h>

struct NVGglyphPosition;

NANOGUI_NAMESPACE_BEGIN

class TextBox : public Widget {
public:
    enum Alignment {
      ALIGNMENT_LEFT,
      ALIGNMENT_CENTER,
      ALIGNMENT_RIGHT
    };

    TextBox(Widget *parent,const std::string &value = "Untitled");

    inline bool isEditable() const { return mEditable; }
    void setEditable(bool editable);

    inline const std::string &value() const { return mValue; }
    inline void setValue(const std::string &value) { mValue = value; }

    inline const std::string &defaultValue() const { return mDefaultValue; }
    inline void setDefaultValue(const std::string &defaultValue) { mDefaultValue = defaultValue; }

    inline Alignment alignment() const { return mAlignment; }
    inline void setAlignment(Alignment align) { mAlignment = align; }

    inline const std::string &units() const { return mUnits; }
    inline void setUnits(const std::string &units) { mUnits = units; }

    inline const std::string &Format() const { return mFormat; }
    inline void setFormat(const std::string &format) { mFormat = format; }

    inline int unitsImage() const { return mUnitsImage; }
    inline void setUnitsImage(int image) { mUnitsImage = image; }
    
    /// Set the change callback
    inline std::function<bool(const std::string& str)> callback() const { return mCallback; }
    inline void setCallback(std::function<bool(const std::string& str)> callback) { mCallback = callback; }

    bool mouseButtonEvent(const Vector2i &p,int button,bool down,int modifiers);
    bool mouseMotionEvent(const Vector2i &p,const Vector2i &rel,int button,int modifiers);
    bool mouseDragEvent(const Vector2i &p,const Vector2i &rel,int button,int modifiers);
    bool mouseEnterEvent(const Vector2i &p,bool enter);
    bool focusEvent(bool focused);
    bool keyboardEvent(int key,int scancode,int action,int modifiers);
    bool keyboardEvent(unsigned int codepoint);

    Vector2i preferredSize(NVGcontext *ctx) const;
    void draw(NVGcontext* ctx);
protected:
    bool mEditable; 
    bool mCommited;
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

    bool checkFormat(const std::string& input,const std::string& format);
    bool copySelection();
    void pasteFromClipboard();
    bool deleteSelection();

    void updateCursor(NVGcontext* ctx,float lastx,const NVGglyphPosition* glyphs,int size);
    float cursorIndex2Position(int index,float lastx,const NVGglyphPosition* glyphs,int size);
    int position2CursorIndex(float posx,float lastx,const NVGglyphPosition* glyphs,int size);
};

NANOGUI_NAMESPACE_END

#endif /* __NANOGUI_TEXTBOX_H */
