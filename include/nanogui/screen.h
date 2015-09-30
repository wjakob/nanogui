#pragma once

#include <nanogui/widget.h>

NANOGUI_NAMESPACE_BEGIN

/**
 * \brief Represents a display surface (i.e. a full-screen or windowed GLFW window)
 * and forms the root element of a hierarchy of nanogui widgets
 */
class NANOGUI_EXPORT Screen : public Widget {
    friend class Widget;
    friend class Window;
public:
    /// Create a new screen
    Screen(const Vector2i &size, const std::string &caption,
           bool resizable = true, bool fullscreen = false);

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
    virtual void dropEvent(const std::vector<std::string> &filenames) { /* To be overridden */ }

    /// Basic default keyboard event handler: hide the window when the 'Escape' key is pressed
    virtual bool keyboardEvent(int key, int scancode, bool press, int modifiers);

    /// Return the last observed mouse position value
    Vector2i mousePos() const { return mMousePos; }

    /// Return a pointer to the underlying GLFW window data structure
    GLFWwindow *glfwWindow() { return mGLFWWindow; }
    
    /// Return a pointer to the underlying nanoVG draw context
    NVGcontext *nvgContext() { return mNVGContext; }

protected:
    /* Internal helper functions */
    void updateFocus(Widget *widget);
    void disposeWindow(Window *window);
    void centerWindow(Window *window);
    void moveWindowToFront(Window *window);

protected:
    GLFWwindow *mGLFWWindow;
    NVGcontext *mNVGContext;
    std::vector<Widget *> mFocusPath;
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
