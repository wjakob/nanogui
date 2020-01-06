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
/** \file */

#pragma once

#include <nanogui/widget.h>
#include <cstdio>
#include <sstream>
#include <limits>

NAMESPACE_BEGIN(nanogui)

/**
 * \class TextBox textbox.h nanogui/textbox.h
 *
 * \brief Fancy text box with builtin regular expression-based validation.
 *
 * \remark
 *     This class overrides \ref nanogui::Widget::mIconExtraScale to be ``0.8f``,
 *     which affects all subclasses of this Widget.  Subclasses must explicitly
 *     set a different value if needed (e.g., in their constructor).
 */
DECLSETTER(IsSpinnable,bool)
DECLSETTER(IsEditable,bool)
DECLSETTER(TextValue,std::string)

class NANOGUI_EXPORT TextBox : public Widget {
public:
    RTTI_CLASS_UID("TXTB")
    RTTI_DECLARE_INFO(TextBox)

    /// How to align the text in the text box.
    enum class Alignment {
        Left,
        Center,
        Right
    };

    explicit TextBox(Widget   *parent, const std::string &value = "Untitled");

    using Widget::set;
    template<typename... Args>
    TextBox(Widget* parent, const Args&... args)
      : TextBox(parent, std::string("")) { set<TextBox, Args...>(args...); }

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

    int fontSize() const override;

    /// Return the underlying regular expression specifying valid formats
    const std::string &format() const { return mFormat; }
    /// Specify a regular expression specifying valid formats
    void setFormat(const std::string &format) { mFormat = format; }

    /// Return the placeholder text to be displayed while the text box is empty.
    const std::string &placeholder() const { return mPlaceholder; }
    /// Specify a placeholder text to be displayed while the text box is empty.
    void setPlaceholder(const std::string &placeholder) { mPlaceholder = placeholder; }

    /// Set the \ref Theme used to draw this widget
    void setTheme(Theme *theme) override;

    /// The callback to execute when the value of this TextBox has changed.
    std::function<bool(const std::string& str)> callback() const { return mCallback; }

    /// Sets the callback to execute when the value of this TextBox has changed.
    void setCallback(const std::function<bool(const std::string&)> &callback) { mCallback = callback; }
    void setEditCallback(const std::function<void(const std::string&, bool)> &callback) { mEditCallback = callback; }
    void setComitCallback(const std::function<void(Widget*)> &callback) { mComitCallback = callback; }

    virtual int getCornerRadius() const;

    bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override;
    bool mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) override;
    bool mouseDragEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) override;
    bool focusEvent(bool focused) override;
    bool keyboardEvent(int key, int scancode, int action, int modifiers) override;
    bool keyboardCharacterEvent(unsigned int codepoint) override;

    Vector2i preferredSize(NVGcontext *ctx) const override;
    void draw(NVGcontext* ctx) override;
    void save(Serializer &s) const override;
    bool load(Serializer &s) override;
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

    /// The location (if any) for the spin area.
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
    std::function<bool(const std::string&)> mCallback;
    std::function<void(const std::string&, bool)> mEditCallback;
    std::function<void(Widget*)> mComitCallback;
    bool mValidFormat;
    std::string mValueTemp;
    std::string mPlaceholder;
    int mCursorPos;
    int mSelectionPos;
    Vector2i mMousePos;
    Vector2i mMouseDownPos;
    Vector2i mMouseDragPos;
    int mMouseDownModifier;
    float mTextOffset;
    double mLastClick;
public:
    PROPSETTER(IsSpinnable,setSpinnable)
    PROPSETTER(IsEditable,setEditable)
    PROPSETTER(TextValue,setValue)
};

/**
 * \class IntBox textbox.h nanogui/textbox.h
 *
 * \brief A specialization of TextBox for representing integral values.
 *
 * Template parameters should be integral types, e.g. ``int``, ``long``,
 * ``uint32_t``, etc.
 */
DECLSETTER(InitialValue,float)
DECLSETTER(MinValue,float)
DECLSETTER(MaxValue,float)

template <typename Scalar>
class IntBox : public TextBox {
public:
    explicit IntBox(Widget *parent, Scalar value = (Scalar) 0) : TextBox(parent, std::string("")) {
        setDefaultValue("0");
        setFormat(std::is_signed<Scalar>::value ? "[-]?[0-9]*" : "[0-9]*");
        setValueIncrement(1);
        setMinMaxValues(std::numeric_limits<Scalar>::lowest(), std::numeric_limits<Scalar>::max());
        setValue(value);
        setSpinnable(false);
    }

    using TextBox::set;
    template<typename... Args>
    IntBox(Widget* parent, const Args&... args)
      : IntBox(parent, 0) { set<IntBox<Scalar>, Args...>(args...); }

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

    void setEditCallback(const std::function<void(Scalar,bool)> &cb) {
      TextBox::setEditCallback(
        [cb, this](const std::string &str, bool v) {
          std::istringstream iss(str);
          Scalar value = 0;
          iss >> value;
          setValue(value);
          cb(value, v);
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
              if (mCallback)
                  mCallback(mValue);
              return true;
        }
        return false;
    }
private:
    Scalar mMouseDownValue;
    Scalar mValueIncrement;
    Scalar mMinValue, mMaxValue;
public:
    PROPSETTER(InitialValue,setValue)
    PROPSETTER(MinValue,setMinValue)
    PROPSETTER(MaxValue,setMaxValue)
};

/**
 * \class FloatBox textbox.h nanogui/textbox.h
 *
 * \brief A specialization of TextBox representing floating point values.

 * Template parameters should be float types, e.g. ``float``, ``double``,
 * ``float64_t``, etc.
 */
template <typename Scalar>
class FloatBox : public TextBox {
public:
    explicit FloatBox(Widget *parent, Scalar value = (Scalar) 0.f) : TextBox(parent) {
        mNumberFormat = sizeof(Scalar) == sizeof(float) ? "%.4g" : "%.7g";
        setDefaultValue("0");
        setFormat("[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?");
        setValueIncrement((Scalar) 0.1);
        setMinMaxValues(std::numeric_limits<Scalar>::lowest(), std::numeric_limits<Scalar>::max());
        setValue(value);
        setSpinnable(false);
    }

    using TextBox::set;
    template<typename... Args>
    FloatBox(Widget* parent, const Args&... args)
      : FloatBox(parent, 0.f) { set<FloatBox<Scalar>, Args...>(args...); }

    std::string numberFormat() const { return mNumberFormat; }
    void numberFormat(const std::string &format) { mNumberFormat = format; }

    Scalar value() const {
        return (Scalar) std::stod(TextBox::value());
    }

    void setValue(Scalar value) {
        Scalar clampedValue = std::min(std::max(value, mMinValue),mMaxValue);
        char buffer[50];
        std::snprintf(buffer, 50, mNumberFormat.c_str(), clampedValue);
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
            if (mCallback)
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
public:
    PROPSETTER(InitialValue, setValue)
    PROPSETTER(MinValue, setMinValue)
    PROPSETTER(MaxValue, setMaxValue)
};

NAMESPACE_END(nanogui)
