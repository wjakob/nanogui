/*
    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/**
 * \file nanogui/formhelper.h
 *
 * \brief Helper class to construct forms for editing a set of variables of
 *        various types.
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
/**
 * \class FormWidget formhelper.h nanogui/formhelper.h
 *
 * \brief A template wrapper class for assisting in the creation of various form widgets.
 *
 * \rst
 * The partial template specializations are:
 *
 * - Inheritance from :class:`nanogui::ComboBox` for ``enum`` types:
 *
 *   .. code-block:: cpp
 *
 *      template <typename T>
 *      class FormWidget<T, typename std::is_enum<T>::type> : public ComboBox
 *
 * - Inheritance from :class:`nanogui::IntBox` for integral types:
 *
 *   .. code-block:: cpp
 *
 *      template <typename T>
 *      class FormWidget<T, typename std::is_integral<T>::type> : public IntBox<T>
 *
 * - Inheritance from :class:`nanogui::FloatBox` for floating point types:
 *
 *   .. code-block:: cpp
 *
 *      template <typename T>
 *      class FormWidget<T, typename std::is_floating_point<T>::type> : public FloatBox<T>
 *
 * The full template specializations are:
 *
 * - Inheritance from :class:`nanogui::CheckBox` for booleans:
 *
 *   .. code-block:: cpp
 *
 *      template <>
 *      class FormWidget<bool, std::true_type> : public CheckBox
 *
 * - Inheritance from :class:`nanogui::TextBox` for strings:
 *
 *   .. code-block:: cpp
 *
 *      template <>
 *      class FormWidget<std::string, std::true_type> : public TextBox
 *
 * - Inheritance from :class:`nanogui::ColorPicker` for :class:`nanogui::Color` types:
 *
 *   .. code-block:: cpp
 *
 *      template <>
 *      class FormWidget<Color, std::true_type> : public ColorPicker
 *
 * Please refer to the bottom of :ref:`program_listing_file_nanogui_formhelper.h`
 * for the implementation details.
 * \endrst
 */
template <typename T, typename sfinae = std::true_type> class FormWidget { };
NAMESPACE_END(detail)

/**
 * \class FormHelper formhelper.h nanogui/formhelper.h
 *
 * \brief Convenience class to create simple AntTweakBar-style layouts that
 *        expose variables of various types using NanoGUI widgets
 *
 * **Example**:
 *
 * \rst
 * .. code-block:: cpp
 *
 *    // [ ... initialize NanoGUI, construct screen ... ]
 *
 *    FormHelper* h = new FormHelper(screen);
 *
 *    // Add a new windows widget
 *    h->addWindow(Eigen::Vector2i(10,10),"Menu");
 *
 *    // Start a new group
 *    h->addGroup("Group 1");
 *
 *    // Expose an integer variable by reference
 *    h->addVariable("integer variable", aInt);
 *
 *    // Expose a float variable via setter/getter functions
 *    h->addVariable(
 *      [&](float value) { aFloat = value; },
 *      [&]() { return *aFloat; },
 *      "float variable");
 *
 *    // add a new button
 *    h->addButton("Button", [&]() { std::cout << "Button pressed" << std::endl; });
 *
 * \endrst
 */
class FormHelper {
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
        mWindow->setVisible(true);
        return mWindow;
    }

    /// Add a new group that may contain several sub-widgets
    Label *addGroup(const std::string &caption) {
      Label* label = new Label(mWindow, Caption{ caption }, CaptionFont{ mGroupFontName }, FontSize{ mGroupFontSize });
        if (mLayout->rowCount() > 0)
            mLayout->appendRow(mPreGroupSpacing); /* Spacing */
        mLayout->appendRow(0);
        mLayout->setAnchor(label, AdvancedGridLayout::Anchor(0, mLayout->rowCount()-1, 4, 1));
        mLayout->appendRow(mPostGroupSpacing);
        return label;
    }

    /// Add a new data widget controlled using custom getter/setter functions
    template <typename Type> detail::FormWidget<Type> *
    addVariable(const std::string &label, const std::function<void(const Type &)> &setter,
                const std::function<Type()> &getter, bool editable = true) {
      Label *labelW = new Label(mWindow, Caption{ label }, CaptionFont{ mLabelFontName }, FontSize{ mLabelFontSize });
        auto widget = new detail::FormWidget<Type>(mWindow);
        auto refresh = [widget, getter] {
            Type value = getter(), current = widget->value();
            if (value != current)
                widget->setValue(value);
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
            [&](const Type & v) { value = v; },
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

    /// Add an arbitrary (optionally labeled) widget to the layout
    void addWidget(const std::string &label, Widget *widget) {
        mLayout->appendRow(0);
        if (label == "") {
            mLayout->setAnchor(widget, AdvancedGridLayout::Anchor(1, mLayout->rowCount()-1, 3, 1));
        } else {
          Label *labelW = new Label(mWindow, Caption{ label }, CaptionFont{ mLabelFontName }, FontSize{ mLabelFontSize });
            mLayout->setAnchor(labelW, AdvancedGridLayout::Anchor(1, mLayout->rowCount()-1));
            mLayout->setAnchor(widget, AdvancedGridLayout::Anchor(3, mLayout->rowCount()-1));
        }
    }

    /// Cause all widgets to re-synchronize with the underlying variable state
    void refresh() {
        for (auto const &callback : mRefreshCallbacks)
            callback();
    }

    /// Access the currently active \ref Window instance
    Window *window() { return mWindow; }

    /// Set the active \ref Window instance.
    void setWindow(Window *window) {
        mWindow = window;
        auto newLayout = window->layout()->cast<AdvancedGridLayout>();
        if (newLayout)
          mLayout = newLayout;
        else
          logic_error("Internal error: window has an incompatible layout!", __FILE__, __LINE__);
    }

    /// Specify a fixed size for newly added widgets.
    void setFixedSize(const Vector2i &fw) { mFixedSize = fw; }

    /// The current fixed size being used for newly added widgets.
    Vector2i fixedSize() { return mFixedSize; }

    /// The font name being used for group headers.
    const std::string &groupFontName() const { return mGroupFontName; }

    /// Sets the font name to be used for group headers.
    void setGroupFontName(const std::string &name) { mGroupFontName = name; }

    /// The font name being used for labels.
    const std::string &labelFontName() const { return mLabelFontName; }

    /// Sets the font name being used for labels.
    void setLabelFontName(const std::string &name) { mLabelFontName = name; }

    /// The size of the font being used for group headers.
    int groupFontSize() const { return mGroupFontSize; }

    /// Sets the size of the font being used for group headers.
    void setGroupFontSize(int value) { mGroupFontSize = value; }

    /// The size of the font being used for labels.
    int labelFontSize() const { return mLabelFontSize; }

    /// Sets the size of the font being used for labels.
    void setLabelFontSize(int value) { mLabelFontSize = value; }

    /// The size of the font being used for non-group / non-label widgets.
    int widgetFontSize() const { return mWidgetFontSize; }

    /// Sets the size of the font being used for non-group / non-label widgets.
    void setWidgetFontSize(int value) { mWidgetFontSize = value; }

protected:
    /// A reference to the \ref nanogui::Screen this FormHelper is assisting.
    ref<Screen> mScreen;

    /// A reference to the \ref nanogui::Window this FormHelper is controlling.
    ref<Window> mWindow;

    /// A reference to the \ref nanogui::AdvancedGridLayout this FormHelper is using.
    ref<AdvancedGridLayout> mLayout;

    /// The callbacks associated with all widgets this FormHelper is managing.
    std::vector<std::function<void()>> mRefreshCallbacks;

    /// The group header font name.
    std::string mGroupFontName = "sans-bold";

    /// The label font name.
    std::string mLabelFontName = "sans";

    /// The fixed size for newly added widgets.
    Vector2i mFixedSize = Vector2i(0, 20);

    /// The font size for group headers.
    int mGroupFontSize = 20;

    /// The font size for labels.
    int mLabelFontSize = 16;

    /// The font size for non-group / non-label widgets.
    int mWidgetFontSize = 16;

    /// The spacing used **before** new groups.
    int mPreGroupSpacing = 15;

    /// The spacing used **after** each group.
    int mPostGroupSpacing = 5;

    /// The spacing between all other widgets.
    int mVariableSpacing = 5;

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

NAMESPACE_BEGIN(detail)

/**
 * A specialization for adding a CheckBox to a FormHelper.
 */
template <> class FormWidget<bool, std::true_type> : public CheckBox {
public:
    /// Creates a new FormWidget with underlying type CheckBox.
    FormWidget(Widget *p) : CheckBox(p, "") { setFixedWidth(20); }

    /// Pass-through function for \ref nanogui::CheckBox::setChecked.
    void setValue(bool v) { setChecked(v); }

    /// Pass-through function for \ref nanogui::Widget::setEnabled.
    void setEditable(bool e) { setEnabled(e); }

    /// Returns the value of \ref nanogui::CheckBox::checked.
    bool value() const { return checked(); }

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

/**
 * A specialization for adding a ComboBox to a FormHelper.
 *
 * \tparam T
 *     The type being used inside the ComboBox.
 */
template <typename T> class FormWidget<T, typename std::is_enum<T>::type> : public ComboBox {
public:
    /// Creates a new FormWidget with underlying type ComboBox.
    FormWidget(Widget *p) : ComboBox(p) { }

    /// Pass-through function for \ref nanogui::ComboBox::selectedIndex.
    T value() const { return (T) selectedIndex(); }

    /// Pass-through function for \ref nanogui::ComboBox::setSelectedIndex.
    void setValue(T value) { setSelectedIndex((int) value); mSelectedIndex = (int) value; }

    /// Pass-through function for \ref nanogui::ComboBox::setCallback.
    void setCallback(const std::function<void(const T &)> &cb) {
        ComboBox::setCallback([cb](int v) { cb((T) v); });
    }

    /// Pass-through function for \ref nanogui::Widget::setEnabled.
    void setEditable(bool e) { setEnabled(e); }

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

/**
 * A specialization for adding an IntBox to a FormHelper.
 *
 * \tparam T
 *     The **integral** type being used for the IntBox.
 */
template <typename T> class FormWidget<T, typename std::is_integral<T>::type> : public IntBox<T> {
public:
    /// Creates a new FormWidget with underlying type IntBox.
    FormWidget(Widget *p) : IntBox<T>(p) { this->setAlignment(TextBox::Alignment::Right); }

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

/**
 * A specialization for adding a FloatBox to a FormHelper.
 *
 * \tparam T
 *     The **floating point** type being used for the FloatBox.
 */
template <typename T> class FormWidget<T, typename std::is_floating_point<T>::type> : public FloatBox<T> {
public:
    /// Creates a new FormWidget with underlying type FloatBox.
    FormWidget(Widget *p) : FloatBox<T>(p) { this->setAlignment(TextBox::Alignment::Right); }

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

/**
 * A specialization for adding a TextBox to a FormHelper.
 */
template <> class FormWidget<std::string, std::true_type> : public TextBox {
public:
    /// Creates a new FormWidget with underlying type TextBox.
    FormWidget(Widget *p) : TextBox(p) { setAlignment(TextBox::Alignment::Left); }

    /// Pass-through function for \ref nanogui::TextBox::setCallback.
    void setCallback(const std::function<void(const std::string&)> &cb) {
        TextBox::setCallback([cb](const std::string &str) { cb(str); return true; });
    }

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

/**
 * A specialization for adding a ColorPicker to a FormHelper.
 */
template <> class FormWidget<Color, std::true_type> : public ColorPicker {
public:
    /// Creates a new FormWidget with underlying type ColorPicker.
    FormWidget(Widget *p) : ColorPicker(p) { }

    /// Pass-through function for \ref nanogui::ColorPicker::setColor.
    void setValue(const Color &c) { setColor(c); }

    /// Pass-through function for \ref nanogui::Widget::setEnabled.
    void setEditable(bool e) { setEnabled(e); }

    /// Returns the value of \ref nanogui::ColorPicker::color.
    Color value() const { return color(); }

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

NAMESPACE_END(detail)
NAMESPACE_END(nanogui)
