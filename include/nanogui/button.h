#if !defined(__NANOGUI_BUTTON_H)
#define __NANOGUI_BUTTON_H

#include <nanogui/widget.h>

NANOGUI_NAMESPACE_BEGIN

class Button : public Widget {
public:
	/// Flags to specify the button behavior (can be combined with binary OR)
	enum ButtonFlags {
		NormalButton = 1,
		RadioButton  = 2,
		ToggleButton = 4,
		PopupButton  = 8
	};

	enum IconPosition {
		Left,
		LeftCentered,
		RightCentered,
		Right
	};

	Button(Widget *parent, const std::string &caption = "Untitled", int icon = 0);

	inline const std::string &caption() const { return mCaption; }
	inline void setCaption(const std::string &caption) { mCaption = caption; }

	inline const Color &backgroundColor() const { return mBackgroundColor; }
	inline void setBackgroundColor(const Color &backgroundColor) { mBackgroundColor = backgroundColor; }

	inline const Color &textColor() const { return mTextColor; }
	inline void setTextColor(const Color &textColor) { mTextColor = textColor; }

	inline int icon() const { return mIcon; }
	inline void setIcon(int icon) { mIcon = icon; }

	inline int buttonFlags() const { return mButtonFlags; }
	inline void setButtonFlags(int buttonFlags) { mButtonFlags = buttonFlags; }

	inline int fontSize() const { return mFontSize; }
	inline void setFontSize(int fontSize) { mFontSize = fontSize; }

	inline IconPosition iconPosition() const { return mIconPosition; }
	inline void setIconPosition(IconPosition iconPosition) { mIconPosition = iconPosition; }

	inline bool pushed() const { return mPushed; }
	inline void setPushed(bool pushed) { mPushed = pushed; }

	/// Set the push callback (for any type of button)
	inline std::function<void()> callback() const { return mCallback; }
	inline void setCallback(std::function<void()> callback) { mCallback = callback; }

	/// Set the change callback (for toggle buttons)
	inline std::function<void(bool)> changeCallback() const { return mChangeCallback; }
	inline void setChangeCallback(std::function<void(bool)> callback) { mChangeCallback = callback; }

	/// Set the button group (for radio buttons)
	inline void setButtonGroup(const std::vector<Button *> &buttonGroup) { mButtonGroup = buttonGroup; }
	inline const std::vector<Button *> &buttonGroup() const { return mButtonGroup; }

	virtual Vector2i preferredSize(NVGcontext *ctx) const;
	virtual bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers);
	virtual void draw(NVGcontext *ctx);
protected:
	std::string mCaption;
	int mIcon;
	IconPosition mIconPosition;
	bool mPushed;
	int mButtonFlags;
	Color mBackgroundColor;
	Color mTextColor;
	std::function<void()> mCallback;
	std::function<void(bool)> mChangeCallback;
	std::vector<Button *> mButtonGroup;
	int mFontSize;
};

NANOGUI_NAMESPACE_END

#endif /* __NANOGUI_BUTTON_H */
