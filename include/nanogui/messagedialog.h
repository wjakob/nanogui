#if !defined(__NANOGUI_MESSAGEDIALOG_H)
#define __NANOGUI_MESSAGEDIALOG_H

#include <nanogui/window.h>

NANOGUI_NAMESPACE_BEGIN

class MessageDialog : public Window {
public:
	enum Type {
		Information,
		Question,
		Warning
	};

	MessageDialog(Widget *parent, Type type, const std::string &title = "Untitled",
			      const std::string &message = "Message",
			      const std::string &buttonText = "OK",
			      const std::string &altButtonText = "Cancel", bool altButton = false);

	inline std::function<void(int)> callback() const { return mCallback; }
	inline void setCallback(std::function<void(int)> callback) { mCallback = callback; }
protected:
	std::function<void(int)> mCallback;
};

NANOGUI_NAMESPACE_END

#endif /* __NANOGUI_MESSAGEDIALOG_H */
