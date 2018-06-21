/*
    nanogui/widget.h -- Base class of all widgets

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/object.h>
#include <nanogui/theme.h>
#include <vector>

NAMESPACE_BEGIN(nanogui)

enum class Cursor;// do not put a docstring, this is already documented

/**
 * \class Widget widget.h nanogui/widget.h
 *
 * \brief Base class of all widgets.
 *
 * \ref Widget is the base class of all widgets in \c nanogui. It can
 * also be used as an panel to arrange an arbitrary number of child
 * widgets using a layout generator (see \ref Layout).
 */
class NANOGUI_EXPORT Widget : public Object {
public:
    /**
     * \brief Constructs a Widget.
     *
     * \param parent
     *     The parent of this Widget.
     *
     * \param font
     *     \rst
     *     The font face to start with.  When anything other than the empty
     *     string, this implies that a user has explicitly requested a specific
     *     font face.  For example, ``new Label(parent, "text", "sans-bold")``.
     *     That is, sub-classes that draw text should always provide an explicit
     *     ``font`` parameter in their constructor, with a default value of
     *     ``""``.  See :ref:`nanogui_usage_how_fonts_are_used`.
     *     \endrst
     */
    Widget(Widget *parent, const std::string &font = "");

    /// Return the parent widget
    Widget *parent() { return mParent; }
    /// Return the parent widget
    const Widget *parent() const { return mParent; }
    /// Set the parent widget
    void setParent(Widget *parent) { mParent = parent; }

    /// Return the used \ref Layout generator
    Layout *layout() { return mLayout; }
    /// Return the used \ref Layout generator
    const Layout *layout() const { return mLayout.get(); }
    /// Set the used \ref Layout generator
    void setLayout(Layout *layout) { mLayout = layout; }

    /// Return the \ref Theme used to draw this widget
    Theme *theme() { return mTheme; }
    /// Return the \ref Theme used to draw this widget
    const Theme *theme() const { return mTheme.get(); }
    /// Set the \ref Theme used to draw this widget
    virtual void setTheme(Theme *theme);

    /// Return the position relative to the parent widget
    const Vector2i &position() const { return mPos; }
    /// Set the position relative to the parent widget
    void setPosition(const Vector2i &pos) { mPos = pos; }

    /// Return the absolute position on screen
    Vector2i absolutePosition() const {
        return mParent ?
            (parent()->absolutePosition() + mPos) : mPos;
    }

    /// Return the size of the widget
    const Vector2i &size() const { return mSize; }
    /// set the size of the widget
    void setSize(const Vector2i &size) { mSize = size; }

    /// Return the width of the widget
    int width() const { return mSize.x(); }
    /// Set the width of the widget
    void setWidth(int width) { mSize.x() = width; }

    /// Return the height of the widget
    int height() const { return mSize.y(); }
    /// Set the height of the widget
    void setHeight(int height) { mSize.y() = height; }

    /**
     * \brief Set the fixed size of this widget
     *
     * If nonzero, components of the fixed size attribute override any values
     * computed by a layout generator associated with this widget. Note that
     * just setting the fixed size alone is not enough to actually change its
     * size; this is done with a call to \ref setSize or a call to \ref performLayout()
     * in the parent widget.
     */
    void setFixedSize(const Vector2i &fixedSize) { mFixedSize = fixedSize; }

    /// Return the fixed size (see \ref setFixedSize())
    const Vector2i &fixedSize() const { return mFixedSize; }

    // Return the fixed width (see \ref setFixedSize())
    int fixedWidth() const { return mFixedSize.x(); }
    // Return the fixed height (see \ref setFixedSize())
    int fixedHeight() const { return mFixedSize.y(); }
    /// Set the fixed width (see \ref setFixedSize())
    void setFixedWidth(int width) { mFixedSize.x() = width; }
    /// Set the fixed height (see \ref setFixedSize())
    void setFixedHeight(int height) { mFixedSize.y() = height; }

    /// Return whether or not the widget is currently visible (assuming all parents are visible)
    bool visible() const { return mVisible; }
    /// Set whether or not the widget is currently visible (assuming all parents are visible)
    void setVisible(bool visible) { mVisible = visible; }

    /// Check if this widget is currently visible, taking parent widgets into account
    bool visibleRecursive() const {
        bool visible = true;
        const Widget *widget = this;
        while (widget) {
            visible &= widget->visible();
            widget = widget->parent();
        }
        return visible;
    }

    /// Return the number of child widgets
    int childCount() const { return (int) mChildren.size(); }

    /// Return the list of child widgets of the current widget
    const std::vector<Widget *> &children() const { return mChildren; }

    /**
     * \brief Add a child widget to the current widget at
     * the specified index.
     *
     * This function almost never needs to be called by hand,
     * since the constructor of \ref Widget automatically
     * adds the current widget to its parent
     */
    virtual void addChild(int index, Widget *widget);

    /// Convenience function which appends a widget at the end
    void addChild(Widget *widget);

    /// Remove a child widget by index
    void removeChild(int index);

    /// Remove a child widget by value
    void removeChild(const Widget *widget);

    /// Retrieves the child at the specific position
    const Widget* childAt(int index) const { return mChildren[index]; }

    /// Retrieves the child at the specific position
    Widget* childAt(int index) { return mChildren[index]; }

    /// Returns the index of a specific child or -1 if not found
    int childIndex(Widget* widget) const;

    /// Variadic shorthand notation to construct and add a child widget
    template<typename WidgetClass, typename... Args>
    WidgetClass* add(const Args&... args) {
        return new WidgetClass(this, args...);
    }

    /// Walk up the hierarchy and return the parent window
    Window *window();

    /// Walk up the hierarchy and return the parent screen
    Screen *screen();

    /// Associate this widget with an ID value (optional)
    void setId(const std::string &id) { mId = id; }
    /// Return the ID value associated with this widget, if any
    const std::string &id() const { return mId; }

    /// Return whether or not this widget is currently enabled
    bool enabled() const { return mEnabled; }
    /// Set whether or not this widget is currently enabled
    void setEnabled(bool enabled) { mEnabled = enabled; }

    /// Return whether or not this widget is currently focused
    bool focused() const { return mFocused; }
    /// Set whether or not this widget is currently focused
    void setFocused(bool focused) { mFocused = focused; }
    /// Request the focus to be moved to this widget
    void requestFocus();

    /**
     * \brief Convenience method to get a valid font size.
     *
     * If \ref mFontSize is greater than ``0`` (e.g., \ref setFontSize has been
     * called), then \ref mFontSize will be returned.  Otherwise,
     * ``defaultFontSize`` is returned.
     *
     * \param defaultFontSize
     *     The default value to return when \ref mFontSize is not set.  For
     *     example, the \ref Label class uses
     *     ``fontSize(mTheme->mStandardFontSize)`` whereas \ref Button will use
     *     ``fontSize(mTheme->mButtonFontSize)``.
     *
     * \throws std::runtime_error
     *     When ``defaultFontSize < 1.0f``.
     */
    float fontSize(const float &defaultFontSize) const;
    /**
     * \brief Set the font size of this widget.  Set to ``-1`` to revert to
     *        a derived class default (e.g. \ref Theme::mStandardFontSize for
     *        \ref Label).
     *
     * \throws std::runtime_error
     *     When ``size >= 0 && size < 1.0f``.
     */
    void setFontSize(float size);
    /// Return whether the font size is explicitly specified for this widget
    bool hasFontSize() const { return mFontSize > 0.0f; }

    /// The current font being used for this Widget (if text is being drawn).
    std::string font() const;
    /// Sets the font to be used for this Widget (if text is being drawn).
    void setFont(const std::string &font) { mFont = font; }
    /// Called by \ref Widget::font when \ref mFont is the empty string.
    virtual std::string defaultFont() const;

    /// The tooltip for this Widget (displays on mouse hover).
    const std::string &tooltip() const { return mTooltip; }
    /// Sets the tooltip for this Widget (displays on mouse hover).
    void setTooltip(const std::string &tooltip) { mTooltip = tooltip; }
    /// Get the font name being used to draw \ref mTooltip
    std::string tooltipFont() const;
    /// Set the font to be used to draw \ref mTooltip.
    void setTooltipFont(const std::string &font) { mTooltipFont = font; }
    /// Called by \ref Widget::tooltipFont when \ref mTooltipFont is the empty string.
    virtual std::string defaultTooltipFont() const;

    /**
     * The amount of extra scaling applied to *icon* fonts.
     * See \ref nanogui::Widget::mIconExtraScale.
     */
    float iconExtraScale() const { return mIconExtraScale; }
    /**
     * Sets the amount of extra scaling applied to *icon* fonts.
     * See \ref nanogui::Widget::mIconExtraScale.
     */
    void setIconExtraScale(float scale) { mIconExtraScale = scale; }
    /// Get the font name being used to draw icons for this widget.
    std::string iconFont() const;
    /// Sets the icon font for this Widget (assumes this font is valid / already loaded).
    void setIconFont(const std::string &iconFont) { mIconFont = iconFont; }
    /// Called by \ref Widget::iconFont when \ref mIconFont is the empty string.
    virtual std::string defaultIconFont() const;

    /// Return a pointer to the cursor of the widget
    Cursor cursor() const { return mCursor; }
    /// Set the cursor of the widget
    void setCursor(Cursor cursor) { mCursor = cursor; }

    /// Check if the widget contains a certain position
    bool contains(const Vector2i &p) const {
        auto d = (p-mPos).array();
        return (d >= 0).all() && (d < mSize.array()).all();
    }

    /// Determine the widget located at the given position value (recursive)
    Widget *findWidget(const Vector2i &p);

    /// Handle a mouse button event (default implementation: propagate to children)
    virtual bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers);

    /// Handle a mouse motion event (default implementation: propagate to children)
    virtual bool mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers);

    /// Handle a mouse drag event (default implementation: do nothing)
    virtual bool mouseDragEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers);

    /// Handle a mouse enter/leave event (default implementation: record this fact, but do nothing)
    virtual bool mouseEnterEvent(const Vector2i &p, bool enter);

    /// Handle a mouse scroll event (default implementation: propagate to children)
    virtual bool scrollEvent(const Vector2i &p, const Vector2f &rel);

    /// Handle a focus change event (default implementation: record the focus status, but do nothing)
    virtual bool focusEvent(bool focused);

    /// Handle a keyboard event (default implementation: do nothing)
    virtual bool keyboardEvent(int key, int scancode, int action, int modifiers);

    /// Handle text input (UTF-32 format) (default implementation: do nothing)
    virtual bool keyboardCharacterEvent(unsigned int codepoint);

    /// Compute the preferred size of the widget
    virtual Vector2i preferredSize(NVGcontext *ctx) const;

    /// Invoke the associated layout generator to properly place child widgets, if any
    virtual void performLayout(NVGcontext *ctx);

    /// Draw the widget (and all child widgets)
    virtual void draw(NVGcontext *ctx);

    /// Save the state of the widget into the given \ref Serializer instance
    virtual void save(Serializer &s) const;

    /// Restore the state of the widget from the given \ref Serializer instance
    virtual bool load(Serializer &s);

protected:
    /// Free all resources used by the widget and any children
    virtual ~Widget();

    /**
     * Convenience definition for subclasses to get the full icon scale for this
     * class of Widget.  It simple returns the value
     * ``mTheme->mIconScale * this->mIconExtraScale``.
     *
     * \remark
     *     See also: \ref nanogui::Theme::mIconScale and
     *     \ref nanogui::Widget::mIconExtraScale.  This tiered scaling
     *     strategy may not be appropriate with fonts other than ``entypo.ttf``.
     */
    inline float icon_scale() const { return mTheme->mIconScale * mIconExtraScale; }

protected:
    Widget *mParent;
    ref<Theme> mTheme;
    ref<Layout> mLayout;
    std::string mId;
    Vector2i mPos, mSize, mFixedSize;
    std::vector<Widget *> mChildren;

    /**
     * Whether or not this Widget is currently visible.  When a Widget is not
     * currently visible, no time is wasted executing its drawing method.
     */
    bool mVisible;

    /**
     * Whether or not this Widget is currently enabled.  Various different kinds
     * of derived types use this to determine whether or not user input will be
     * accepted.  For example, when ``mEnabled == false``, the state of a
     * CheckBox cannot be changed, or a TextBox will not allow new input.
     */
    bool mEnabled;
    bool mFocused, mMouseFocus;

    /// The tooltip to draw for this widget (on mouse hover).  Empty string means no tooltip.
    std::string mTooltip;

    /**
     * \brief The amount of extra icon scaling used in addition the the theme's
     *        default icon font scale.  Default value is ``1.0``, which implies
     *        that \ref nanogui::Widget::icon_scale simply returns the value
     *        of \ref nanogui::Theme::mIconScale.
     *
     * Most widgets do not need extra scaling, but some (e.g., CheckBox, TextBox)
     * need to adjust the Theme's default icon scaling
     * (\ref nanogui::Theme::mIconScale) to properly display icons within their
     * bounds (upscale, or downscale).
     *
     * \rst
     * .. note::
     *
     *    When using ``nvgFontSize`` for icons in subclasses, make sure to call
     *    the :func:`nanogui::Widget::icon_scale` function.  Expected usage when
     *    *drawing* icon fonts is something like:
     *
     *    .. code-block:: cpp
     *
     *       virtual void draw(NVGcontext *ctx) {
     *           // fontSize depends on the kind of Widget.  Search for `FontSize`
     *           // in the Theme class (e.g., standard vs button)
     *           float ih = fontSize;
     *           // assuming your Widget has a declared `mIcon`
     *           if (nvgIsFontIcon(mIcon)) {
     *               ih *= icon_scale();
     *               nvgFontFace(ctx, "icons");
     *               nvgFontSize(ctx, ih);
     *               /// remaining drawing code (see button.cpp for more)
     *           }
     *       }
     * \endrst
     */
    float mIconExtraScale;

    Cursor mCursor;

private:
    /* Font related utilities.  To use in derived classes, call the getter.
     * E.g. for `mFont`, call `font()` and `mTooltipFont` call `tooltipFont()`.
     * To set it, call the setter.  E.g. set `mFont` with `setFont` and
     * `mTooltipFont` with `setTooltipFont`.
     */

    /**
     * Used to set the font size of a widget explicitly.  The initial value is
     * ``-1``, and a negative number indicates that the theme's font size should
     * be used instead.  Specifically, \ref fontSize will return
     * \ref Theme::mStandardFontSize when ``mFontSize < 0``.
     */
    float mFontSize;

    /**
     * \brief The explicitly specified font face to use for drawing generic text.
     *
     * \rst
     * The value of this variable is typically the empty string (``""``),
     * indicating that no font has been explicitly specified by a user.  In this
     * event, either the class or theme will define the font face.  See
     * :ref:`nanogui_usage_how_fonts_are_used`.
     * \endrst
     */
    std::string mFont;

    /**
     * \brief The explicitly specified font face for drawing \ref mTooltip with.
     *
     * \rst
     * The value of this variable is typically the empty string (``""``),
     * indicating that no font has been explicitly specified by a user.  In this
     * event, either the class or theme will define the font face.  See
     * :ref:`nanogui_usage_how_fonts_are_used`.
     * \endrst
     */
    std::string mTooltipFont;

    /**
     * \brief The explicitly specified font face for drawing icon text.
     *
     * \rst
     * The value of this variable is typically the empty string (``""``),
     * indicating that no font has been explicitly specified by a user.  In this
     * event, either the class or theme will define the font face.  See
     * :ref:`nanogui_usage_how_fonts_are_used`.
     * \endrst
     */
    std::string mIconFont;

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

NAMESPACE_END(nanogui)
