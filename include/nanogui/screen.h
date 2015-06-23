#if !defined(__NANOGUI_SCREEN_H)
#define __NANOGUI_SCREEN_H

#include <nanogui/widget.h>

struct GLFWcursor;

NANOGUI_NAMESPACE_BEGIN

// define command key for windows/mac/linux
#ifdef __APPLE__
#define SYSTEM_COMMAND_MOD GLFW_MOD_SUPER
#else
#define SYSTEM_COMMAND_MOD GLFW_MOD_CONTROL
#endif

enum class Cursor 
{
  Arrow = 0,
  IBeam,
  Crossair,
  Hand,
  HResize,
  Vresize,
  None
};

/**
 * \brief Represents a display surface (i.e. a full-screen or windowed GLFW window)
 * and forms the root element of a hierarchy of nanogui widgets
 */
class Screen : public Widget {
    friend class Widget;
    friend class Window;

public:

    /// Create a new screen
    Screen(const Vector2i &size,const std::string &caption,bool resizable = true,bool fullscreen = false);

    /// Release all resources
    virtual ~Screen();

    /// Draw the Screen contents
    virtual void drawAll();

    /// Return the screen's background color
    const Vector3f &background() const { return mBackground; }
    /// Set the screen's background color
    void setBackground(const Vector3f &background) { mBackground = background; }

    /// Set the top-level window visibility (no effect on full-screen windows)
    void setVisible(bool visible);

    /// Set window size
    void setSize(const Vector2i& size);

    /// Draw the window contents -- put your OpenGL draw calls here
    virtual void drawContents() { /* To be overridden */ }

    /// Invoked when the window is resized
    virtual void framebufferSizeChanged() { /* To be overridden */ }
    
    /// Handle a file drop event
    virtual bool dropEvent(const std::vector<std::string> &filenames) { return false; /* To be overridden */ }

    /// Basic default keyboard event handler: hide the window when the 'Escape' key is pressed
    virtual bool keyboardEvent(int key, int scancode, int action, int modifiers);

    /// Text input event handler: codepoint is native endian UTF-32 format
    virtual bool keyboardEvent(unsigned int codepoint);

    /// Return the last observed mouse position value
    inline Vector2i mousePos() const { return mMousePos; }

    /// Return a pointer to the underlying GLFW window data structure
    GLFWwindow *glfwWindow() { return mGLFWWindow; }
    
    /// Return a pointer to the underlying nanoVG draw context
    NVGcontext *nvgContext() { return mNVGContext; }

protected:

    // Default constructor
    Screen();

    void initScreen(GLFWwindow *window);
    void drawWidgets();

    /* event functions */
    bool cursorPosCallbackEvent(GLFWwindow *w,double x,double y);
    bool mouseButtonCallbackEvent(GLFWwindow *w,int button,int action,int modifiers);
    bool keyCallbackEvent(GLFWwindow *w,int key,int scancode,int action,int mods);
    bool charCallbackEvent(GLFWwindow* w,unsigned int codepoint);
    bool dropCallbackEvent(GLFWwindow *w,int count,const char **filenames);
    bool scrollCallbackEvent(GLFWwindow *w,double x,double y);

    /* Internal helper functions */
    void updateFocus(Widget *widget);
    void updateMouseFocus(Widget *widget);
    void disposeWindow(Window *window);
    void centerWindow(Window *window);
    void moveWindowToFront(Window *window);

protected:
    bool mIsMainWindow;
    GLFWwindow *mGLFWWindow;
    NVGcontext *mNVGContext;
    Cursor mCursorType;
    GLFWcursor* mCursor;
    std::vector<Widget *> mFocusPath;
    std::vector<Widget *> mMouseFocusedWidgets;
    Vector2i mFBSize;
    float mPixelRatio;
    int mMouseState, mModifiers;
    Vector2i mMousePos;
    bool mDragActive;
    Widget *mDragWidget = nullptr;
    double mLastInteraction;
    bool mProcessEvents;
    Vector3f mBackground;
};

NANOGUI_NAMESPACE_END

#endif /* __NANOGUI_SCREEN_H */
