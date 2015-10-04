/*
    nanogui/formhelper.h -- helper class to construct forms for editing a set
    of variables of various types

    NanoGUI was developed by Wenzel Jakob <wenzel@inf.ethz.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#pragma once

#include <nanogui/screen.h>
#include <nanogui/label.h>
#include <nanogui/checkbox.h>
#include <nanogui/textbox.h>
#include <nanogui/combobox.h>
#include <nanogui/colorpicker.h>
#include <nanogui/layout.h>
#include <cassert>

NAMESPACE_BEGIN(nanogui)

NAMESPACE_BEGIN(detail)
template <typename T, typename sfinae = std::true_type> class FormWidget { };
NAMESPACE_END(detail)

/**
* \brief Convenience class to create simple AntTweakBar-style layouts that
*        expose variables of various types using NanoGUI widgets
*
* Example:
*
* <pre>
* [ ... initialize NanoGUI, construct screen ... ]
*
* FormHelper* h = new FormHelper(screen);
*
* // Add a new windows widget
* h->addWindow(Eigen::Vector2i(10,10),"Menu");
*
* // Start a new group
* h->addGroup("Group 1");
*
* // Expose an integer variable by reference
* h->addVariable("integer variable", aInt);
*
* // Expose a float variable via setter/getter functions
* h->addVariable(
*   [&](float value){ aFloat = value; },
*   [&](){ return *aFloat; },
*   "float variable");
*
* // add a new button
* h->addButton("Button",[&](){ std::cout << "Button pressed" << std::endl; });
* </pre>
*/

class NANOGUI_EXPORT FormHelper {
public:
    /// Create a helper class to construct NanoGUI widgets on the given screen
    FormHelper(Screen *screen) : mScreen(screen) { }

    /// Add a new top-level window
    Window *addWindow(const Vector2i &pos,
                         const std::string &title = "Untitled") {
        assert(mScreen);
        mWindow = new Window(mScreen, title);
        mLayout = new AdvancedGridLayout({10, 0, 10, 0}, {});
        mLayout->setMargin(10);
        mLayout->setColStretch(2, 1);
        mWindow->setPosition(pos);
        mWindow->setLayout(mLayout);
        return mWindow;
    }

    /// Add a new group that may contain several sub-widgets
    Label *addGroup(const std::string &caption) {
        Label* label = new Label(mWindow, caption, mGroupFontName, mGroupFontSize);
        if (mLayout->rowCount() > 0)
            mLayout->appendRow(mPreGroupSpacing); /* Spacing */
        mLayout->appendRow(0);
        mLayout->setAnchor(label, AdvancedGridLayout::Anchor(0, mLayout->rowCount()-1, 4, 1));
        mLayout->appendRow(mPostGroupSpacing);
        return label;
    }

    /// Add a new data widget controlled using custom getter/setter functions
    template <typename Type> detail::FormWidget<Type> *
    addVariable(const std::string &label, const std::function<void(Type)> &setter,
                const std::function<Type()> &getter, bool editable = true) {
        Label *labelW = new Label(mWindow, label, mLabelFontName, mLabelFontSize);
        auto widget = new detail::FormWidget<Type>(mWindow);
        auto refresh = [widget, getter] {
            widget->setValue(getter());
        };
        refresh();
        widget->setCallback(setter);
        widget->setEditable(editable);
        widget->setFontSize(mWidgetFontSize);
        Vector2i fs = widget->fixedSize();
        widget->setFixedSize(Vector2i(fs.x() != 0 ? fs.x() : mFixedSize.x(),
                                      fs.y() != 0 ? fs.y() : mFixedSize.y()));
        mRefreshCallbacks.push_back(refresh);
        if (mLayout->rowCount() > 0)
            mLayout->appendRow(mVariableSpacing);
        mLayout->appendRow(0);
        mLayout->setAnchor(labelW, AdvancedGridLayout::Anchor(1, mLayout->rowCount()-1));
        mLayout->setAnchor(widget, AdvancedGridLayout::Anchor(3, mLayout->rowCount()-1));
        return widget;
    }

    /// Add a new data widget that exposes a raw variable in memory
    template <typename Type> detail::FormWidget<Type> *
    addVariable(const std::string &label, Type &value, bool editable = true) {
        return addVariable<Type>(label,
            [&](Type v) { value = v; },
            [&]() -> Type { return value; },
            editable 
        );
    }

    /// Add a button with a custom callback
    Button *addButton(const std::string &label, const std::function<void()> &cb) {
        Button *button = new Button(mWindow, label);
        button->setCallback(cb);
        button->setFixedHeight(25);
        if (mLayout->rowCount() > 0)
            mLayout->appendRow(mVariableSpacing);
        mLayout->appendRow(0);
        mLayout->setAnchor(button, AdvancedGridLayout::Anchor(1, mLayout->rowCount()-1, 3, 1));
        return button;
    }

    /// Cause all widgets to re-synchronize with the underlying variable state
    void refresh() {
        for (auto const &callback : mRefreshCallbacks)
            callback();
    }

    /// Access the currently active \ref Window instance
    Window *window() { return mWindow; }
    void setWindow(Window *window) {
        mWindow = window;
        mLayout = dynamic_cast<AdvancedGridLayout *>(window->layout());
        if (mLayout == nullptr)
            throw std::runtime_error(
                "Internal error: window has an incompatible layout!");
    }

    /// Specify a fixed size for newly added widgets
    void setFixedSize(const Vector2i &fw) { mFixedSize = fw; }
    Vector2i fixedSize() { return mFixedSize; }

    /* Set the font size / name of labels, group headers, and data widgets */
    const std::string &groupFontName() const { return mGroupFontName; }
    void setGroupFontName(const std::string &name) { mGroupFontName = name; }
    const std::string &labelFontName() const { return mLabelFontName; }
    void setLabelFontName(const std::string &name) { mLabelFontName = name; }
    int groupFontSize() const { return mGroupFontSize; }
    void setGroupFontSize(int value) { mGroupFontSize = value; }
    int labelFontSize() const { return mLabelFontSize; }
    void setLabelFontSize(int value) { mLabelFontSize = value; }
    int widgetFontSize() const { return mWidgetFontSize; }
    void setWidgetFontSize(int value) { mWidgetFontSize = value; }

protected:
    ref<Screen> mScreen;
    ref<Window> mWindow;
    ref<AdvancedGridLayout> mLayout;
    std::vector<std::function<void()>> mRefreshCallbacks;
    std::string mGroupFontName = "sans-bold";
    std::string mLabelFontName = "sans";
    Vector2i mFixedSize = Vector2i(0, 20);
    int mGroupFontSize = 20;
    int mLabelFontSize = 16;
    int mWidgetFontSize = 16;
    int mPreGroupSpacing = 15;
    int mPostGroupSpacing = 5;
    int mVariableSpacing = 5;
};

NAMESPACE_BEGIN(detail)
/* Various types of form widgets for different input types below */
template <> class FormWidget<bool, std::true_type> : public CheckBox {
public:
    FormWidget(Widget *p) : CheckBox(p, "") { setFixedWidth(20); }
    void setValue(bool v) { setChecked(v); }
    void setEditable(bool e) { setEnabled(e); }
};

template <typename T> class FormWidget<T, typename std::is_enum<T>::type> : public ComboBox {
public:
    FormWidget(Widget *p) : ComboBox(p) { }
    void setValue(T value) { setSelectedIndex((int) value); mSelectedIndex = (int) value; }
    void setCallback(const std::function<void(T)> &cb) {
        ComboBox::setCallback([cb](int v) { cb((T) v); });
    }
    void setEditable(bool e) { setEnabled(e); }
};

template <typename T> class FormWidget<T, typename std::is_integral<T>::type> : public IntBox<T> {
public:
    FormWidget(Widget *p) : IntBox<T>(p) { this->setAlignment(TextBox::Alignment::Right); }
};

template <typename T> class FormWidget<T, typename std::is_floating_point<T>::type> : public FloatBox<T> {
public:
    FormWidget(Widget *p) : FloatBox<T>(p) { this->setAlignment(TextBox::Alignment::Right); }
};

template <> class FormWidget<std::string, std::true_type> : public TextBox {
public:
    FormWidget(Widget *p) : TextBox(p) { setAlignment(TextBox::Alignment::Left); }
    void setCallback(const std::function<void(const std::string&)> &cb) {
        TextBox::setCallback([cb](const std::string &str) { cb(str); return true; });
    }
};

template <> class FormWidget<Color, std::true_type> : public ColorPicker {
public:
    FormWidget(Widget *p) : ColorPicker(p) { }
    void setValue(const Color &c) { setColor(c); }
    void setEditable(bool e) { setEnabled(e); }
};

NAMESPACE_END(detail)
NAMESPACE_END(nanogui)
