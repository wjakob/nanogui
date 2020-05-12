/*
    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/**
 * \file nanogui/checkbox.h
 *
 * \brief Two-state check box Widget.
 */

#pragma once

#include <nanogui/widget.h>

NAMESPACE_BEGIN(nanogui)

/**
 * \class CheckBox checkbox.h nanogui/checkbox.h
 *
 * \brief Two-state check box widget.
 *
 * \remark
 *     This class overrides \ref nanogui::Widget::mIconExtraScale to be ``1.2f``,
 *     which affects all subclasses of this Widget.  Subclasses must explicitly
 *     set a different value if needed (e.g., in their constructor).
 */

DECLSETTER(CheckboxState, bool)
DECLSETTERARGS(UncheckedColor, Color)
DECLSETTERARGS(CheckedColor, Color)
DECLSETTERARGS(PushedColor, Color)
DECLSETTER(CheckboxCallback, std::function<void(bool)>)

class NANOGUI_EXPORT CheckBox : public Widget {
public:
    RTTI_CLASS_UID(CheckBox)
    RTTI_DECLARE_INFO(CheckBox)

    /**
     * Adds a CheckBox to the specified ``parent``.
     *
     * \param parent
     *     The Widget to add this CheckBox to.
     *
     * \param caption
     *     The caption text of the CheckBox (default ``"Untitled"``).
     *
     * \param callback
     *     If provided, the callback to execute when the CheckBox is checked or
     *     unchecked.  Default parameter function does nothing.  See
     *     \ref nanogui::CheckBox::mPushed for the difference between "pushed"
     *     and "checked".
     */
    CheckBox(Widget *parent, const std::string &caption,
             std::function<void(bool)> callback = nullptr,
             bool checked = false);

    using Widget::set;
    template<typename... Args>
    CheckBox(Widget* parent, const Args&... args)
      : CheckBox(parent, std::string("")) { set<CheckBox, Args...>(args...); }

    /// The caption of this CheckBox.
    const std::string &caption() const { return mCaption; }

    /// Sets the caption of this CheckBox.
    void setCaption(const std::string &caption) { mCaption = caption; }

    /// Whether or not this CheckBox is currently checked.
    bool checked() const { return (bool)mChecked; }

    /// Sets whether or not this CheckBox is currently checked.
    void setChecked(bool checked) { mChecked = checked; }

    /// Whether or not this CheckBox is currently pushed.  See \ref nanogui::CheckBox::mPushed.
    bool pushed() const { return mPushed; }

    BoolObservable observable() { return mChecked; }
    void setObservable(BoolObservable value) { mChecked = value; }

    /// Sets whether or not this CheckBox is currently pushed.  See \ref nanogui::CheckBox::mPushed.
    void setPushed(bool pushed) { mPushed = pushed; }

    bool keyboardEvent(int key, int scancode, int action, int mods) override;

    void setPushedColor(const Color& c) { mPushedColor = c; }
    void setCheckedColor(const Color& c) { mCheckedColor = c; }
    void setUncheckedColor(const Color& c) { mUncheckedColor = c; }
    bool tabstop(CanTabStop) const override { return true; }

    void setStateColor(const Color& checked, const Color& unchecked = {}, const Color& pushed = {})
    { mPushedColor = pushed; mCheckedColor = checked; mUncheckedColor = unchecked; }

    /// Returns the current callback of this CheckBox.
    std::function<void(bool)> callback() const { return mCallback; }

    /// Sets the callback to be executed when this CheckBox is checked / unchecked.
    void setCallback(const std::function<void(bool)> &callback) { mCallback = callback; }

    /**
     * The mouse button callback will return ``true`` when all three conditions are met:
     *
     * 1. This CheckBox is "enabled" (see \ref nanogui::Widget::mEnabled).
     * 2. ``p`` is inside this CheckBox.
     * 3. ``button`` is ``GLFW_MOUSE_BUTTON_1`` (left mouse click).
     *
     * Since a mouse button event is issued for both when the mouse is pressed, as well
     * as released, this function sets \ref nanogui::CheckBox::mPushed to ``true`` when
     * parameter ``down == true``.  When the second event (``down == false``) is fired,
     * \ref nanogui::CheckBox::mChecked is inverted and \ref nanogui::CheckBox::mCallback
     * is called.
     *
     * That is, the callback provided is only called when the mouse button is released,
     * **and** the click location remains within the CheckBox boundaries.  If the user
     * clicks on the CheckBox and releases away from the bounds of the CheckBox,
     * \ref nanogui::CheckBox::mPushed is simply set back to ``false``.
     */
    virtual bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override;

    /// The preferred size of this CheckBox.
    virtual Vector2i preferredSize(NVGcontext *ctx) const override;

    /// Draws this CheckBox.
    virtual void draw(NVGcontext *ctx) override;

    /// Saves this CheckBox to the specified Serializer.
    virtual void save(Json::value &save) const override;

    /// Loads the state of the specified Serializer to this CheckBox.
    virtual bool load(Json::value &load) override;

protected:
    void toggleCheck();

    /// The caption text of this CheckBox.
    std::string mCaption;

    /**
     * Internal tracking variable to distinguish between mouse click and release.
     * \ref nanogui::CheckBox::mCallback is only called upon release.  See
     * \ref nanogui::CheckBox::mouseButtonEvent for specific conditions.
     */
    bool mPushed;

    /// Whether or not this CheckBox is currently checked or unchecked.
    BoolObservable mChecked;
    Color mPushedColor, mUncheckedColor, mCheckedColor;

    /// The function to execute when \ref nanogui::CheckBox::mChecked is changed.
    std::function<void(bool)> mCallback;

public:
  PROPSETTER(CheckboxState, setChecked)
  PROPSETTER(Caption, setCaption)
  PROPSETTER(CheckboxCallback, setCallback)
  PROPSETTERVAL(BoolObservableRef, setObservable)
  PROPSETTERVAL(BoolObservable, setObservable)
  PROPSETTER(FontSize,setFontSize)
  PROPSETTER(UncheckedColor, setUncheckedColor)
  PROPSETTER(CheckedColor, setCheckedColor)
  PROPSETTER(PushedColor, setPushedColor)
};

namespace elm { using CheckBox = Element<CheckBox>; }

NAMESPACE_END(nanogui)
