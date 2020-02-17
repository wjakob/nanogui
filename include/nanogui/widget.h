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
#include <nanogui/layout.h>
#include <vector>

NAMESPACE_BEGIN(nanogui)

class Serializer;
class Window;
class Label;
class ToolButton;
class LinkButton;
class MessageDialog;
class PopupButton;
class Button;
class ComboBox;
class CheckBox;
class TabWidget;
class VScrollPanel;
class ProgressBar;
class Slider;
class ImagePanel;
class TextBox;
class SwitchBox;
class Listbox;
class Spinner;
class Graph;
class TreeView;
class ToleranceBar;
class TextArea;
class Panel;
class ColorPicker;
template<class X> class IntBox;
template<class X> class FloatBox;
template<class X> class NumberPicker;

enum class Cursor;// do not put a docstring, this is already documented
namespace Json { class value; }

enum TextHAlign { hLeft = 0, hCenter, hRight };
enum TextVAlign { vTop = 3, vMiddle, vBottom };
enum Corner { cLeftTop = 0, cLeftBottom, cRigthTop, cRightBottom };

DECLSETTER(MaxHeight, int)
DECLSETTER(FixedHeight, int)
DECLSETTER(FixedWidth, int)
DECLSETTER(WidgetLayout, Layout*)
DECLSETTERARGSNEW(WidgetStretchLayout, StretchLayout)
DECLSETTERARGSNEW(WidgetGridLayout, GridLayout)
DECLSETTERARGSNEW(WidgetBoxLayout, BoxLayout)
DECLSETTERARGS(FixedSize, Vector2i)
DECLSETTER(WidgetId, std::string)
DECLSETTER(FloatValue, float)
DECLSETTER(Icon, int)
DECLSETTER(Caption, std::string)
DECLSETTERARGS(Position, Vector2i)
DECLSETTERARGS(WidgetSize, Vector2i)
DECLSETTERARGS(MinimumSize, Vector2i)
DECLSETTERARGS(RelativeSize, Vector2f)
DECLSETTERARGS(BackgroundColor, Color)
DECLSETTER(TooltipText, std::string)
DECLSETTER(CaptionFont, std::string)
DECLSETTER(FontSize, int)
DECLSETTER(CaptionHAlign, TextHAlign)
DECLSETTER(InitialValue, float)
DECLSETTER(MinValue, float)
DECLSETTER(MaxValue, float)

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
    RTTI_CLASS_UID(Widget)
    RTTI_DECLARE_INFO(Widget)

    /// Construct a new widget with the given parent widget
    Widget(Widget *parent);
    Widget(Widget&) = delete;
    Widget& operator =(Widget&) = delete;

    template<typename... Args>
    Widget(Widget* parent, const Args&... args)
      : Widget(parent) { set<Widget, Args...>(args...);  }

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

    Vector2i corner(Corner c) {
      switch (c) {
      case Corner::cLeftTop: return position();
      case Corner::cLeftBottom: return{ left(), position().y() + size().y() };
      case Corner::cRigthTop: return{ left() + size().x(), position().y() };
      case Corner::cRightBottom: return Vector2i( position() + size() );
      }
      return { 0, 0 };
    }
    int right() const { return mPos.x() + mSize.x(); }
    int left() const { return mPos.x(); }
    /// Set the position relative to the parent widget
    void setPosition(const Vector2i &pos) { mPos = pos; }
    void setPosition(int x, int y) { setPosition(Vector2i(x, y)); }

    void setGeometry(const Vector4i &vec) {
      setPosition(vec.x(), vec.y());
      setSize(vec.z() - vec.x(), vec.w() - vec.y());
    }

    bool sendChildToBack(Widget* child);
    bool sendToBack();

    /// Return the absolute position on screen
    Vector2i absolutePosition() const {
        return mParent ?
            (parent()->absolutePosition() + mPos) : mPos;
    }

    Vector4i absoluteRect() const {
      Vector2i ap = absolutePosition();
      return Vector4i(ap.x(), ap.y(), ap.x() + width(), ap.y() + height());
    }

    Vector4i rect() const {
      Vector2i p = position();
      return Vector4i(p.x(), p.y(), p.x() + width(), p.y() + height());
    }

    Widget *findWidget(const std::string& id, bool inchildren = true);
    Widget *findWidget(std::function<bool(Widget*)> cond, bool inchildren = true);

    Widget *findWidgetGlobal(const std::string& id);
    Widget *findWidgetGlobal(std::function<bool(Widget*)> cond);

    virtual std::string wtypename() const;

    template<typename RetClass>
    RetClass *findWidget(const std::string& id, bool inchildren = true)
    {
      Widget* f = findWidget(id, inchildren);
      return f ? RetClass::cast(f) : nullptr;
    }

    template<typename RetClass>
    RetClass *findWidget(WidgetId id, bool inchildren = true)
    {
      return findWidget<RetClass>(std::string(id.value), inchildren);
    }

    template<typename RetClass>
    RetClass *findWidgetGlobal(const std::string& id)
    {
      Widget* f = findWidgetGlobal(id);
      return f ? RetClass::cast(f) : nullptr;
    }

    template<typename WidgetClass>
    std::vector<WidgetClass*> findAll() const
    {
      std::vector<WidgetClass*> ret;
      for (auto& w : mChildren)
      {
        if (auto cw = WidgetClass::cast(w))
          ret.push_back(cw);
      }

      return ret;
    }

    template<typename WidgetClass>
    void forEachChild(std::function<void (WidgetClass*)> f)
    {
      auto widgets = findAll<WidgetClass>();
      for (WidgetClass* w : widgets) f(w);
    }

    void forEachChild(const std::function<void(Widget*)>& f, bool deep = false)
    { for (Widget* w : mChildren) { f(w); if (deep) w->forEachChild(f, deep); }}


    /// Return the size of the widget
    virtual const Vector2i &size() const { return mSize; }
    enum CanTabStop { TabStopSelf=0, TabStopChildren };
    virtual bool tabstop(CanTabStop mode) const;
    /// set the size of the widget
    void setSize(const Vector2i &size) { mSize = size; }
    void setSize(int w, int h) { setSize(Vector2i( w, h )); }

    const Vector2f &relsize() const { return mRelSize; }
    void setRelativeSize(const Vector2f &size) { mRelSize = size; }
    void setRelativeSize(float w, float h) { setRelativeSize(Vector2f(w, h)); }

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
    void setFixedSize(int w, int h) { setFixedSize(Vector2i(w, h)); }

    void setMinSize(const Vector2i &minSize) { mMinSize = minSize; }
    void setMinWidth(int ww) { mMinSize.x() = ww; }

    int minWidth() const { return minSize().x(); }
    int minHeight() const { return minSize().y(); }
    virtual Vector2i minSize() const { return mMinSize; }
    virtual Vector4i getWidgetsArea();

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
    void setVisible(bool visible);

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

    bool bringToFront();
    bool bringChildToFront(Widget* element);

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

    void remove();

    /// Remove a child widget by index
    virtual void removeChild(int index);

    /// Remove a child widget by value
    virtual void removeChild(const Widget *widget);

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

    bool areParentsContain(Widget* w)
    {
      Widget* parentw = mParent;
      while (parentw) {
        if (parentw == w)
          return true;
        parentw = parentw->parent();
      }

      return false;
    }

    template<typename WidgetClass>
    WidgetClass *findParent() {
      Widget *widget = this;
      while (widget) {
        WidgetClass *parentw = WidgetClass::cast(widget);
        if (parentw)
          return parentw;
        widget = widget->parent();
      }
      return nullptr;
    }


    Window *window();
    Screen *screen();

    /// Associate this widget with an ID value (optional)
    void setId(const std::string &id) { mId = id; }
    /// Return the ID value associated with this widget, if any
    const std::string &id() const { return mId; }

    /// Return whether or not this widget is currently enabled
    bool enabled() const { return mEnabled; }
    /// Set whether or not this widget is currently enabled
    virtual void setEnabled(bool enabled) { mEnabled = enabled; }

    /// Return whether or not this widget is currently focused
    bool focused() const { return mFocused; }
    /// Set whether or not this widget is currently focused
    void setFocused(bool focused) { mFocused = focused; }
    /// Request the focus to be moved to this widget
    void requestFocus();

    const std::string &tooltip() const { return mTooltip; }
    void setTooltip(const std::string &tooltip) { mTooltip = tooltip; }

    virtual Widget* getCurrentSelection() const { return nullptr; }

    /// Return current font size. If not set the default of the current theme will be returned
    virtual int fontSize() const;
    /// Set the font size of this widget
    void setFontSize(int fontSize) { mFontSize = fontSize; }
    /// Return whether the font size is explicitly specified for this widget
    bool hasFontSize() const { return mFontSize > 0; }

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

    /// Return a pointer to the cursor of the widget
    Cursor cursor() const { return mCursor; }
    /// Set the cursor of the widget
    void setCursor(Cursor cursor) { mCursor = cursor; }

    /// Check if the widget contains a certain position
    bool contains(const Vector2i &p) const {
        auto d = (p-mPos);
        return (d >= Vector2i::Zero()) && (d < mSize);
    }

    virtual bool prefferContains(const Vector2i&) const { return false; }

    bool isMyChildRecursive(Widget* w)
    {
      if (!w)
        return false;
      do
      {
        if (w->parent())
          w = w->parent();

      } while (w->parent() && w != this);

      return w == this;
    }

    bool isMyChild(Widget* w) const
    {
      for (auto& c : mChildren)
        if (c == w) return true;

      return false;
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
    Vector2i preferredSize();

    /// Invoke the associated layout generator to properly place child widgets, if any
    virtual void performLayout(NVGcontext *ctx);

    /// Draw the widget (and all child widgets)
    virtual void draw(NVGcontext *ctx);
    virtual void afterDraw(NVGcontext *ctx);

    /// Save the state of the widget into the given \ref Serializer instance
    virtual void save(Serializer &s) const;
    virtual void save(Json::value &s) const;

    /// Restore the state of the widget from the given \ref Serializer instance
    virtual bool load(Serializer &s);
    virtual bool load(Json::value &s);

    inline void setSubElement(bool v) { mSubElement = v; }
    inline bool isSubElement() const { return mSubElement; }

    inline void show() { setVisible(true); }
    inline void hide() { setVisible(false); }

    void setDebugDraw(bool en) { mDebugDraw = en; }

    PROPSETTER(FixedHeight, setFixedHeight)
    PROPSETTER(FixedWidth, setFixedWidth)
    PROPSETTER(WidgetId,setId)
    PROPSETTER(Position,setPosition)
    PROPSETTER(WidgetSize,setSize)
    PROPSETTER(MinimumSize,setMinSize)
    PROPSETTER(RelativeSize,setRelativeSize)
    PROPSETTER(FixedSize,setFixedSize)
    PROPSETTER(WidgetLayout,setLayout)
    PROPSETTER(WidgetStretchLayout,setLayout)
    PROPSETTER(WidgetGridLayout,setLayout)
    PROPSETTER(WidgetBoxLayout,setLayout)

    template<typename FF, typename none = void> void set() {}

    template<typename WidgetClass, typename... Args>
    WidgetClass& wdg(const Args&... args) { auto widget = new WidgetClass(this, args...); return *widget; }
    template<typename LayoutClass, typename... Args>
    Widget& withLayout(const Args&... args) { setLayout(new LayoutClass(args...)); return *this; }
    template<typename ThemeClass, typename... Args>
    Widget& withTheme(const Args&... args) { setTheme(new ThemeClass(args...)); return *this; }
    template<typename... Args>Widget& boxlayout(const Args&... args) { return withLayout<BoxLayout>(args...); }
    template<typename... Args>Widget& flexlayout(const Args&... args) { return withLayout<StretchLayout>(args...); }
    template<typename... Args>Widget& hlayer(const Args&... args) { return widget(WidgetStretchLayout{ Orientation::Horizontal }, args...); }
    template<typename... Args>Widget& vlayer(const Args&... args) { return widget(WidgetStretchLayout{ Orientation::Vertical }, args...); }
    template<typename... Args>Widget& hstack(const Args&... args) { return widget(WidgetBoxLayout{ Orientation::Horizontal, Alignment::Fill, 2, 2 }, args...); }
    template<typename... Args>Widget& vstack(const Args&... args) { return widget(WidgetBoxLayout{ Orientation::Vertical, Alignment::Fill, 2, 2 }, args...); }
    template<typename... Args>ToolButton& toolbutton(const Args&... args) { return wdg<ToolButton>(args...); }
    template<typename... Args>PopupButton& popupbutton(const Args&... args) { return wdg<PopupButton>(args...); }
    template<typename... Args>Label& label(const Args&... args) { return wdg<Label>(args...); }
    template<typename... Args>ProgressBar& progressbar(const Args&... args) { return wdg<ProgressBar>(args...); }
    template<typename... Args>ComboBox& combobox(const Args&... args) { return wdg<ComboBox>(args...); }
    template<typename... Args>Button& button(const Args&... args) { return wdg<Button>(args...); }
    template<typename... Args>Widget& widget(const Args&... args) { return wdg<Widget>(args...); }
    template<typename... Args>CheckBox& checkbox(const Args&... args) { return wdg<CheckBox>(args...); }
    template<typename... Args>MessageDialog& msgdialog(const Args&... args) { return wdg<MessageDialog>(args...); }
    template<typename... Args>VScrollPanel& vscrollpanel(const Args&... args) { return wdg<VScrollPanel>(args...); }
    template<typename... Args>ImagePanel& imgpanel(const Args&... args) { return wdg<ImagePanel>(args...); }
    template<typename... Args>Slider& slider(const Args&... args) { return wdg<Slider>(args...); }
    template<typename... Args>TextBox& textbox(const Args&... args) { return wdg<TextBox>(args...); }
    template<typename... Args>SwitchBox& switchbox(const Args&... args) { return wdg<SwitchBox>(args...); }
    template<typename... Args>Listbox& listbox(const Args&... args) { return wdg<Listbox>(args...); }
    template<typename... Args>TabWidget& tabs(const Args&... args) { return wdg<TabWidget>(args...); }
    template<typename... Args>Spinner& spinner(const Args&... args) { return wdg<Spinner>(args...); }
    template<typename Scalar, typename... Args>IntBox<Scalar>& intbox(const Args&... args) { return wdg<IntBox<Scalar>>(args...); }
    template<typename Scalar, typename... Args>FloatBox<Scalar>& floatbox(const Args&... args) { return wdg<FloatBox<Scalar>>(args...); }
    template<typename... Args>ColorPicker& colorpicker(const Args&... args) { return wdg<ColorPicker>(args...); }
    template<typename... Args>Graph& graph(const Args&... args) { return wdg<Graph>(args...); }
    template<typename... Args>TreeView& treeview(const Args&... args) { return wdg<TreeView>(args...); }
    template<typename... Args>ToleranceBar& tolerancebar(const Args&... args) { return wdg<ToleranceBar>(args...); }
    template<typename... Args>LinkButton& link(const Args&... args) { return wdg<LinkButton>(args...); }
    template<typename... Args>TextArea& text(const Args&... args) { return wdg<TextArea>(args...); }
    template<typename... Args>Panel& panel(const Args&... args) { return wdg<Panel>(args...); }
    template<typename... Args>Panel& hgrid2(float split, const Args&... args) { auto& w=wdg<Panel>(args...); w.withLayout<GridLayout>(GridLayoutSplit{ split, 1.f - split }, GridLayoutColAlignment{Alignment::Fill}); return w; }
    template<typename... Args>Widget& linegrid2(float split, const Args&... args) { return widget(args...).withLayout<GridLayout>(GridLayoutSplit{ split, 1.f - split }, GridLayoutColAlignment{ Alignment::Fill }); }
    template<typename Scalar, typename... Args>NumberPicker<Scalar>& numpicker(const Args&... args) { return wdg<NumberPicker<Scalar>>(args...); }

protected:
    /// Free all resources used by the widget and any children
    virtual ~Widget();
    virtual void drawTabstop(NVGcontext *ctx);

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
    Vector2i mPos, mSize, mFixedSize, mMinSize;
    Vector2f mRelSize;
    std::vector<Widget *> mChildren;

    /**
     * Whether or not this Widget is currently visible.  When a Widget is not
     * currently visible, no time is wasted executing its drawing method.
     */
    bool mVisible;
    bool mSubElement = false;
    bool mDebugDraw = false;

    /**
     * Whether or not this Widget is currently enabled.  Various different kinds
     * of derived types use this to determine whether or not user input will be
     * accepted.  For example, when ``mEnabled == false``, the state of a
     * CheckBox cannot be changed, or a TextBox will not allow new input.
     */
    bool mEnabled;
    bool mFocused, mMouseFocus;
    std::string mTooltip;
    int mFontSize;

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

    static std::vector<intptr_t> mFocusChain;
};

NAMESPACE_END(nanogui)
