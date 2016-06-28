/*
    nanogui/textbox.h -- Fancy text box with builtin regular
    expression-based validation

    The text box widget was contributed by Christian Schueller.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
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

    bool spinnable() const { return mSpinnable; }
    void setSpinnable(bool spinnable) { mSpinnable = spinnable; }

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

    virtual bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override;
    virtual bool mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) override;
    virtual bool mouseDragEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) override;
    virtual bool focusEvent(bool focused) override;
    virtual bool keyboardEvent(int key, int scancode, int action, int modifiers) override;
    virtual bool keyboardCharacterEvent(unsigned int codepoint) override;

    virtual Vector2i preferredSize(NVGcontext *ctx) const override;
    virtual void draw(NVGcontext* ctx) override;
    virtual void save(Serializer &s) const override;
    virtual bool load(Serializer &s) override;
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

    enum class SpinArea { None, Top, Bottom };
    SpinArea spinArea(const Vector2i & pos);

protected:
    bool mEditable;
    bool mSpinnable;
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
        setValueIncrement(1);
        setMinMaxValues(std::numeric_limits<Scalar>::lowest(), std::numeric_limits<Scalar>::max());
        setValue(value);
        setSpinnable(false);
    }

    Scalar value() const {
        std::istringstream iss(TextBox::value());
        Scalar value = 0;
        iss >> value;
        return value;
    }

    void setValue(Scalar value) {
        Scalar clampedValue = std::min(std::max(value, mMinValue),mMaxValue);
        TextBox::setValue(std::to_string(clampedValue));
    }

    void setCallback(const std::function<void(Scalar)> &cb) {
        TextBox::setCallback(
            [cb, this](const std::string &str) {
                std::istringstream iss(str);
                Scalar value = 0;
                iss >> value;
                setValue(value);
                cb(value);
                return true;
            }
        );
    }

    void setValueIncrement(Scalar incr) {
        mValueIncrement = incr;
    }
    void setMinValue(Scalar minValue) {
        mMinValue = minValue;
    }
    void setMaxValue(Scalar maxValue) {
        mMaxValue = maxValue;
    }
    void setMinMaxValues(Scalar minValue, Scalar maxValue) {
        setMinValue(minValue);
        setMaxValue(maxValue);
    }

    virtual bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override {
        if ((mEditable || mSpinnable) && down)
            mMouseDownValue = value();

        SpinArea area = spinArea(p);
        if (mSpinnable && area != SpinArea::None && down && !focused()) {
            if (area == SpinArea::Top) {
                setValue(value() + mValueIncrement);
                if (mCallback)
                    mCallback(mValue);
            } else if (area == SpinArea::Bottom) {
                setValue(value() - mValueIncrement);
                if (mCallback)
                    mCallback(mValue);
            }
            return true;
        }

        return TextBox::mouseButtonEvent(p, button, down, modifiers);
    }
    virtual bool mouseDragEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) override {
        if (TextBox::mouseDragEvent(p, rel, button, modifiers)) {
            return true;
        }
        if (mSpinnable && !focused() && button == 2 /* 1 << GLFW_MOUSE_BUTTON_2 */ && mMouseDownPos.x() != -1) {
                int valueDelta = static_cast<int>((p.x() - mMouseDownPos.x()) / float(10));
                setValue(mMouseDownValue + valueDelta * mValueIncrement);
                if (mCallback)
                    mCallback(mValue);
                return true;
        }
        return false;
    }
    virtual bool scrollEvent(const Vector2i &p, const Vector2f &rel) override {
        if (Widget::scrollEvent(p, rel)) {
            return true;
        }
        if (mSpinnable && !focused()) {
              int valueDelta = (rel.y() > 0) ? 1 : -1;
              setValue(value() + valueDelta*mValueIncrement);
              if(mCallback)
                  mCallback(mValue);
              return true;
        }
        return false;
    }
private:
    Scalar mMouseDownValue;
    Scalar mValueIncrement;
    Scalar mMinValue, mMaxValue;
};

template <typename Scalar> class FloatBox : public TextBox {
public:
    FloatBox(Widget *parent, Scalar value = (Scalar) 0.f) : TextBox(parent) {
        mNumberFormat = sizeof(Scalar) == sizeof(float) ? "%.4g" : "%.7g";
        setDefaultValue("0");
        setFormat("[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?");
        setValueIncrement(0.1);
        setMinMaxValues(std::numeric_limits<Scalar>::lowest(), std::numeric_limits<Scalar>::max());
        setValue(value);
        setSpinnable(false);
    }

    std::string numberFormat() const { return mNumberFormat; }
    void numberFormat(const std::string &format) { mNumberFormat = format; }

    Scalar value() const {
        return (Scalar) std::stod(TextBox::value());
    }

    void setValue(Scalar value) {
        Scalar clampedValue = std::min(std::max(value, mMinValue),mMaxValue);
        char buffer[50];
        NANOGUI_SNPRINTF(buffer, 50, mNumberFormat.c_str(), clampedValue);
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

    void setValueIncrement(Scalar incr) {
        mValueIncrement = incr;
    }
    void setMinValue(Scalar minValue) {
        mMinValue = minValue;
    }
    void setMaxValue(Scalar maxValue) {
        mMaxValue = maxValue;
    }
    void setMinMaxValues(Scalar minValue, Scalar maxValue) {
        setMinValue(minValue);
        setMaxValue(maxValue);
    }

    virtual bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override {
        if ((mEditable || mSpinnable) && down)
            mMouseDownValue = value();

        SpinArea area = spinArea(p);
        if (mSpinnable && area != SpinArea::None && down && !focused()) {
            if (area == SpinArea::Top) {
                setValue(value() + mValueIncrement);
                if (mCallback)
                    mCallback(mValue);
            } else if (area == SpinArea::Bottom) {
                setValue(value() - mValueIncrement);
                if (mCallback)
                    mCallback(mValue);
            }
            return true;
        }

        return TextBox::mouseButtonEvent(p, button, down, modifiers);
    }
    virtual bool mouseDragEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) override {
        if (TextBox::mouseDragEvent(p, rel, button, modifiers)) {
            return true;
        }
        if (mSpinnable && !focused() && button == 2 /* 1 << GLFW_MOUSE_BUTTON_2 */ && mMouseDownPos.x() != -1) {
            int valueDelta = static_cast<int>((p.x() - mMouseDownPos.x()) / float(10));
            setValue(mMouseDownValue + valueDelta * mValueIncrement);
            if (mCallback)
                mCallback(mValue);
            return true;
        }
        return false;
    }
    virtual bool scrollEvent(const Vector2i &p, const Vector2f &rel) override {
        if (Widget::scrollEvent(p, rel)) {
            return true;
        }
        if (mSpinnable && !focused()) {
            int valueDelta = (rel.y() > 0) ? 1 : -1;
            setValue(value() + valueDelta*mValueIncrement);
            if(mCallback)
                mCallback(mValue);
            return true;
        }
        return false;
    }

private:
    std::string mNumberFormat;
    Scalar mMouseDownValue;
    Scalar mValueIncrement;
    Scalar mMinValue, mMaxValue;
};

NAMESPACE_END(nanogui)
