/*
    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/**
 * \file nanogui/button.h
 *
 * \brief Defines the [Normal/Toggle/Radio/Popup] \ref nanogui::Button widget.
 */

#pragma once

#include <nanogui/widget.h>
#include <bitset>

NAMESPACE_BEGIN(nanogui)
/**
 * \class Button button.h nanogui/button.h
 *
 * \brief [Normal/Toggle/Radio/Popup] Button widget.
 */
DECLSETTER(ButtonCallback, std::function<void()>)
DECLSETTER(ButtonFlags, int)
DECLSETTER(ButtonStyleTextFlags, int)
DECLSETTER(ButtonDrawFlags, int)
DECLSETTER(ButtonChangeCallback, std::function<void (Button*)>)
DECLSETTER(ButtonDragCallback, std::function<void ()>)
DECLSETTER(ButtonToggleFlag, bool)
DECLSETTER(ButtonPushed, bool)
DECLSETTER(InactiveColor, Color)
DECLSETTER(HoveredTextColor, Color)

class NANOGUI_EXPORT Button : public Widget 
{
public:
    //property(ButtonCallback, std::function<void()>, setCallback)

    RTTI_CLASS_UID(Button)
    RTTI_DECLARE_INFO(Button)

    /// Flags to specify the button behavior (can be combined with binary OR)
    enum Flag {
        NormalButton = 1<<0, ///< A normal Button.
        RadioButton  = 1<<1, ///< A radio Button.
        ToggleButton = 1<<2, ///< A toggle Button.
        PopupButton  = 1<<3  ///< A popup Button.
    };

    /// The available icon positions.
    enum class IconPosition {
        Left,         ///< Button icon on the far left.
        LeftCentered, ///< Button icon on the left, centered (depends on caption text length).
        RightCentered,///< Button icon on the right, centered (depends on caption text length).
        Right         ///< Button icon on the far right.
    };

    enum DrawFlag {
        DrawNone =   0,
        DrawBody =   1<<0,
        DrawCaption =   1<<1,
        DrawBorder = 1<<2,
        DrawIcon =   1<<3,
        DrawAll  =   0xff
    };

    enum StyleTextFlags {
        StyleTextNone      = 0,
        StyleTextUnderline = 1<<0,
        StyleTextShadow    = 1<<1
    };

    /**
     * \brief Creates a button attached to the specified parent.
     *
     * \param parent
     *     The \ref nanogui::Widget this Button will be attached to.
     *
     * \param caption
     *     The name of the button (default ``"Untitled"``).
     *
     * \param icon
     *     The icon to display with this Button.  See \ref nanogui::Button::mIcon.
     */
    explicit Button(Widget *parent, const std::string &caption = "Untitled", int icon = 0);
    explicit Button(Widget *parent, const char* caption)
      : Button(parent, std::string(caption), 0) {}

    using Widget::set;
    template<typename... Args>
    Button(Widget* parent, const Args&... args)
      : Button(parent) { set<Button, Args...>(args...); }

    /// Returns the caption of this Button.
    const std::string &caption() const { return mCaption; }

    /// Sets the caption of this Button.
    void setCaption(const std::string &caption) { mCaption = caption; }

    /// Returns the background color of this Button.
    const Color &backgroundColor() const { return mBackgroundColor; }

    /// Sets the background color of this Button.
    void setBackgroundColor(const Color &color) { mBackgroundColor = color; }
    void setBackgroundHoverColor(const Color &color) { mBackgroundHoverColor = color; }

    bool tabstop(CanTabStop) const override { return true; }

    /// Returns the text color of the caption of this Button.
    const Color &textColor() const { return mTextColor; }

    /// Sets the text color of the caption of this Button.
    void setTextColor(const Color &color) { mTextColor = color; }

    void setHoveredTextColor(const Color& color) { mHoverTextColor = color; }

    /// Returns the icon of this Button.  See \ref nanogui::Button::mIcon.
    int icon() const { return mIcon; }

    /// Sets the icon of this Button.  See \ref nanogui::Button::mIcon.
    void setIcon(int icon) { mIcon = icon; }

    /// The current flags of this Button (see \ref nanogui::Button::Flags for options).
    bool haveFlag(int flag) const { return (mFlags & flag) == flag; }

    /// Sets the flags of this Button (see \ref nanogui::Button::Flags for options).
    void setFlags(int flags) { mFlags = flags; }
    void setFlag(int flags) { mFlags |= flags; }

    void setToggleButton(bool en) { if (en) mFlags |= Button::ToggleButton;
                                    else mFlags &= ~Button::ToggleButton; }

    /// The position of the icon for this Button.
    IconPosition iconPosition() const { return mIconPosition; }

    /// Sets the position of the icon for this Button.
    void setIconPosition(IconPosition iconPosition) { mIconPosition = iconPosition; }

    /// Whether or not this Button is currently pushed.
    bool pushed() const { return mPushed; }

    /// Sets whether or not this Button is currently pushed.
    void setPushed(bool pushed) { mPushed = pushed; }
   
    BoolObservable observable() { return mPushed; }
    void setObservable(BoolObservable value) { mPushed = value; }

    /// The current callback to execute (for any type of button).
    std::function<void()> callback() const { return mCallback; }

    /// Set the push callback (for any type of button).
    void setCallback(const std::function<void()> &callback) { mCallback = callback; }

    void setBorderSize(int v) { mBorderSize = v; }
    void setBorderColor(const Color& c) { mBorderColor = c; }

    /// The current callback to execute (for toggle buttons).
    std::function<void(Button*)> changeCallback() const { return mChangeCallback; }

    /// Set the change callback (for toggle buttons).
    void setChangeCallback(const std::function<void(Button*)> &callback) { mChangeCallback = callback; }
    
    /// Set the button group (for radio buttons).
    void setButtonGroup(const std::vector<Button *> &buttonGroup) { mButtonGroup = buttonGroup; }

    /// The current button group (for radio buttons).
    const std::vector<Button *> &buttonGroup() const { return mButtonGroup; }

    std::string wtypename() const override { return "button"; }
    void setFontName(const std::string& font) { mFontName = font; }

    /// The preferred size of this Button.
    Vector2i preferredSize(NVGcontext *ctx) const override;

    /// The callback that is called when any type of mouse button event is issued to this Button.
    bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) override;

    /// Responsible for drawing the Button.
    void draw(NVGcontext *ctx) override;

    /// Saves the state of this Button provided the given Serializer.
    void save(Json::value &s) const override;

    /// Sets the state of this Button provided the given Serializer.
    bool load(Json::value &s) override;

    void setDrawFlags(int flags) { mDrawFlags = flags; }
    bool haveDrawFlag(int flag) { return (mDrawFlags & flag)==flag; }

    void setTextStyleFlags(int flags) { mTextStyleFlags = flags; }
    bool haveTextStyleFlag(int flag) { return (mTextStyleFlags & flag) == flag; }

    void setIconColor(const Color& c) { mIconColor = c; }
    void setPushedIconColor(const Color& c) { mPushedIconColor = c; }

protected:
    virtual Color getTextColor() const;
    virtual Color getIconColor() const;

    virtual void beforeDoCallback() {}
    virtual void beforeDoChangeCallback(bool) {}
    Vector2f Button::getCaptionSize(NVGcontext *ctx);
    /// The caption of this Button.
    std::string mCaption;
    Vector2f mCaptionSize;

    int mBorderSize;
    Color mBorderColor;
    /**
     * \brief The icon of this Button (``0`` means no icon).
     *
     * \rst
     * The icon to display with this Button.  If not ``0``, may either be a
     * picture icon, or one of the icons enumerated in
     * :ref:`file_nanogui_entypo.h`.  The kind of icon (image or Entypo)
     * is determined by the functions :func:`nanogui::nvgIsImageIcon` and its
     * reciprocal counterpart :func:`nanogui::nvgIsFontIcon`.
     * \endrst
     */
    int mIcon;
    Color mIconColor, mPushedIconColor;

    /// The position to draw the icon at.
    IconPosition mIconPosition;

    /// Whether or not this Button is currently pushed.
    BoolObservable mPushed;

    /// The current flags of this button (see \ref nanogui::Button::Flags for options).
    int mFlags;
    int mDrawFlags;
    int mTextStyleFlags;

    std::string mFontName;

    /// The background color of this Button.
    Color mBackgroundColor, mBackgroundHoverColor;

    /// The color of the caption text of this Button.
    Color mTextColor, mHoverTextColor, mPressedTextColor;

    /// The callback issued for all types of buttons.
    std::function<void()> mCallback;

    /// The callback issued for toggle buttons.
    std::function<void(Button*)> mChangeCallback;

    /// The button group for radio buttons.
    std::vector<Button *> mButtonGroup;

public:
    PROPSETTER(ButtonCallback, setCallback)
    PROPSETTER(Caption, setCaption)
    PROPSETTER(CaptionFont, setFontName)
    PROPSETTER(TooltipText, setTooltip)
    PROPSETTER(Icon, setIcon)
    PROPSETTER(IconColor, setIconColor)
    PROPSETTER(IconPushedColor, setPushedIconColor)
    PROPSETTER(BackgroundColor,setBackgroundColor)
    PROPSETTER(BackgroundHoverColor, setBackgroundHoverColor)
    PROPSETTER(ButtonFlags,setFlags)
    PROPSETTER(ButtonDrawFlags, setDrawFlags)
    PROPSETTER(ButtonStyleTextFlags, setTextStyleFlags)
    PROPSETTER(ButtonChangeCallback,setChangeCallback)
    PROPSETTER(ButtonDragCallback,setDragCallback)
    PROPSETTER(ButtonToggleFlag,setToggleButton)
    PROPSETTER(ButtonPushed,setPushed)
    PROPSETTER(BoolObservable, setObservable)
    PROPSETTER(TextColor, setTextColor)
    PROPSETTER(HoveredTextColor, setHoveredTextColor)
    PROPSETTER(BorderSize, setBorderSize)
    PROPSETTER(BorderColor, setBorderColor)
};

class NANOGUI_EXPORT LedButton : public Button
{
public:
  RTTI_CLASS_UID(LedButton)
  RTTI_DECLARE_INFO(LedButton)

  enum Mode { circleCustom=0, rectCustom, triangleCustom, roundrectCustom,
              circleBlack, circleBlue, circleGreen, circleGray, circleOrange, circleRed, circleYellow, circlePurple
  };
  LedButton(Widget* parent, Mode mode = circleBlack, int w = 40, int h = 40);
  void draw(NVGcontext* ctx) override;

  void setMode(Mode mode) { mMode = mode; }

private:
  Mode mMode = circleBlack;
};

class NANOGUI_EXPORT LinkButton : public Button
{
public:
  RTTI_CLASS_UID(LinkButton)
  RTTI_DECLARE_INFO(LinkButton)

  LinkButton(Widget* parent);

  using Button::set;
  template<typename... Args>
  LinkButton(Widget* parent, const Args&... args)
    : LinkButton(parent) { set<LinkButton, Args...>(args...); }

  void draw(NVGcontext* ctx) override;
  Color getTextColor() const override;
};

NAMESPACE_END(nanogui)
