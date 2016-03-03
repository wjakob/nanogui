/*
    nanogui/textbox.h -- Fancy text box with builtin regular
    expression-based validation

    The text box widget was contributed by Christian Schueller.

    NanoGUI was developed by Wenzel Jakob <wenzel@inf.ethz.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#pragma once

#include <nanogui/compat.h>
#include <nanogui/widget.h>
#include <sstream>

NAMESPACE_BEGIN(nanogui)

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

    /// Set the \ref Theme used to draw this widget
    virtual void setTheme(Theme *theme) override;

    /// Set the change callback
    std::function<bool(const std::string& str)> callback() const { return mCallback; }
    void setCallback(const std::function<bool(const std::string& str)> &callback) { mCallback = callback; }

    bool mouseButtonEvent(const Vector2i &p,int button,bool down,int modifiers);
    bool mouseMotionEvent(const Vector2i &p,const Vector2i &rel,int button,int modifiers);
    bool mouseDragEvent(const Vector2i &p,const Vector2i &rel,int button,int modifiers);
    bool mouseEnterEvent(const Vector2i &p,bool enter);
    bool focusEvent(bool focused);
    bool keyboardEvent(int key, int scancode, int action, int modifiers);
    bool keyboardCharacterEvent(unsigned int codepoint);

    Vector2i preferredSize(NVGcontext *ctx) const;
    void draw(NVGcontext* ctx);
    virtual void save(Serializer &s) const;
    virtual bool load(Serializer &s);
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

template <typename Scalar> class IntBox : public TextBox {
public:
    IntBox(Widget *parent, Scalar value = (Scalar) 0) : TextBox(parent) {
        setDefaultValue("0");
        setFormat(std::is_signed<Scalar>::value ? "[-]?[0-9]*" : "[0-9]*");
        setValue(value);
    }

    Scalar value() const {
        Scalar value;
        std::istringstream iss(TextBox::value());
        if (!(iss >> value))
            throw std::invalid_argument("Could not parse integer value!");
        return value;
    }

    void setValue(Scalar value) {
        TextBox::setValue(std::to_string(value));
    }

    void setCallback(const std::function<void(Scalar)> &cb) {
        TextBox::setCallback(
            [cb, this](const std::string &str) {
                std::istringstream iss(str);
                Scalar value;
                if (!(iss >> value))
                    throw std::invalid_argument("Could not parse integer value!");
                setValue(value);
                cb(value);
                return true;
            }
        );
    }
};

template <typename Scalar> class FloatBox : public TextBox {
public:
    FloatBox(Widget *parent, Scalar value = (Scalar) 0.f) : TextBox(parent) {
        mNumberFormat = sizeof(Scalar) == sizeof(float) ? "%.4g" : "%.7g";
        setDefaultValue("0");
        setFormat("[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?");
        setValue(value);
    }

    std::string numberFormat() const { return mNumberFormat; }
    void numberFormat(const std::string &format) { mNumberFormat = format; }

    Scalar value() const {
        return (Scalar) std::stod(TextBox::value());
    }

    void setValue(Scalar value) {
        char buffer[50];
        NANOGUI_SNPRINTF(buffer, 50, mNumberFormat.c_str(), value);
        TextBox::setValue(buffer);
    }

    void setCallback(const std::function<void(Scalar)> &cb) {
        TextBox::setCallback([cb, this](const std::string &str) {
            Scalar scalar = (Scalar) std::stod(str);
            setValue(scalar);
            cb(scalar);
            return true;
        });
    }
private:
    std::string mNumberFormat;
};

NAMESPACE_END(nanogui)
