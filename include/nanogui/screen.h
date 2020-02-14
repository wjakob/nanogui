/*
    nanogui/screen.h -- Top-level widget and interface between NanoGUI and GLFW

    A significant redesign of this code was contributed by Christian Schueller.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/
/** \file */

#pragma once

#include <nanogui/widget.h>

NAMESPACE_BEGIN(nanogui)

class Window;

/**
 * \class Screen screen.h nanogui/screen.h
 *
 * \brief Represents a display surface (i.e. a full-screen or windowed GLFW window)
 * and forms the root element of a hierarchy of nanogui widgets.
 */
class NANOGUI_EXPORT Screen : public Widget {
    friend class Widget;
    friend class Window;
public:
    RTTI_CLASS_UID(Screen)
    RTTI_DECLARE_INFO(Screen)

    /**
     * Create a new Screen instance
     *
     * \param size
     *     Size in pixels at 96 dpi (on high-DPI screens, the actual resolution
     *     in terms of hardware pixels may be larger by an integer factor)
     *
     * \param caption
     *     Window title (in UTF-8 encoding)
     *
     * \param resizable
     *     If creating a window, should it be resizable?
     *
     * \param fullscreen
     *     Specifies whether to create a windowed or full-screen view
     *
     * \param colorBits
     *     Number of bits per pixel dedicated to the R/G/B color components
     *
     * \param alphaBits
     *     Number of bits per pixel dedicated to the alpha channel
     *
     * \param depthBits
     *     Number of bits per pixel dedicated to the Z-buffer
     *
     * \param stencilBits
     *     Number of bits per pixel dedicated to the stencil buffer (recommended
     *     to set this to 8. NanoVG can draw higher-quality strokes using a
     *     stencil buffer)
     *
     * \param nSamples
     *     Number of MSAA samples (set to 0 to disable)
     *
     * \param glMajor
     *     The requested OpenGL Major version number.  Default is 3, if changed
     *     the value must correspond to a forward compatible core profile (for
     *     portability reasons).  For example, set this to 4 and \ref glMinor to 1
     *     for a forward compatible core OpenGL 4.1 profile.  Requesting an
     *     invalid profile will result in no context (and therefore no GUI)
     *     being created.
     *
     * \param glMinor
     *     The requested OpenGL Minor version number.  Default is 3, if changed
     *     the value must correspond to a forward compatible core profile (for
     *     portability reasons).  For example, set this to 1 and \ref glMajor to 4
     *     for a forward compatible core OpenGL 4.1 profile.  Requesting an
     *     invalid profile will result in no context (and therefore no GUI)
     *     being created.
     */
    Screen(const Vector2i &size, const std::string &caption, bool fullscreen);

    /// Release all resources
    virtual ~Screen();

    /// Get the window title bar caption
    const std::string &caption() const { return mCaption; }

    /// Set the window title bar caption
    void setCaption(const std::string &caption);

    /// Return the screen's background color
    const Color &background() const { return mBackground; }

    /// Set the screen's background color
    void setBackground(const Color &background) { mBackground = background; }

    /// Set the top-level window visibility (no effect on full-screen windows)
    void setVisible(bool visible);

    /// Set window size
    void setSize(const Vector2i& size);
    void setSizeFBSize(const Vector2i& size, const Vector2i& fbsize);

    /// Draw the Screen contents
    virtual void drawAll();

    /// Draw the window contents --- put your OpenGL draw calls here
    virtual void drawContents() { /* To be overridden */ }

    /// Return the ratio between pixel and device coordinates (e.g. >= 2 on Mac Retina displays)
    float pixelRatio() const { return mPixelRatio; }
    void setPixelRatio(float ratio) { mPixelRatio = ratio; }

    /// Handle a file drop event
    virtual bool dropEvent(const std::vector<std::string> & /* filenames */) { return false; /* To be overridden */ }

    /// Window resize event handler
    virtual bool resizeEvent(const Vector2i& size);

    /// Set the resize callback
    std::function<void(Vector2i)> resizeCallback() const { return mResizeCallback; }
    void setResizeHwHandler(std::function<void(Screen*, Vector2i&, Vector2i&)> func) { mResizeHwHandler = func; }
    void setResizeCallback(const std::function<void(Vector2i)> &callback) { mResizeCallback = callback; }

    /// Return the last observed mouse position value
    Vector2i mousePos() const { return mMousePos; }

    /// Return a pointer to the underlying nanoVG draw context
    NVGcontext *nvgContext();

    void setShutdownOnDestruct(bool v) { mShutdownOnDestruct = v; }
    bool shutdownOnDestruct() { return mShutdownOnDestruct; }

    void addChild(int index, Widget * widget) override;

    using Widget::performLayout;

    /// Compute the layout of all widgets
    void performLayout() { Widget::performLayout(nvgContext()); }

    void setClipboardString(const std::string& text) { if (mClipboardSetFunc) mClipboardSetFunc(text); }
    void initClipboardSetHandler(std::function<void(std::string)> handler) { mClipboardSetFunc = handler; }
    void initHwCursorSetter(std::function<void(intptr_t)> func) { mHwCursorSetter = func; }

    std::string getClipboardString() { if (mClipboardGetFunc) return mClipboardGetFunc(); return ""; }
    void initClipboardGetHandler(std::function<std::string ()> handler) { mClipboardGetFunc = handler; }

    void needPerformLayout(Widget* w);

    template<typename T> void setTheme() { Widget::setTheme(new T(nvgContext())); }
    template<typename... Args>Window& window(const Args&... args) { return wdg<Window>(args...); }

public:
    /********* API for applications which manage GLFW themselves *********/

    /**
     * \brief Default constructor
     *
     * Performs no initialization at all. Use this if the application is
     * responsible for setting up GLFW, OpenGL, etc.
     *
     * In this case, override \ref Screen and call \ref initalize() with a
     * pointer to an existing \c GLFWwindow instance
     *
     * You will also be responsible in this case to deliver GLFW callbacks
     * to the appropriate callback event handlers below
     */
    Screen();

    /* Event handlers */
    bool cursorPosCallbackEvent(double x, double y);
    bool mouseButtonCallbackEvent(int button, int action, int modifiers);
    bool keyCallbackEvent(int key, int scancode, int action, int mods);
    bool charCallbackEvent(unsigned int codepoint);
    bool dropCallbackEvent(int count, const char **filenames);
    bool scrollCallbackEvent(double x, double y);
    bool resizeCallbackEvent(int width, int height);

    /* Internal helper functions */
    void updateFocus(Widget *widget);
    void disposeWindow(Window *window);
    void centerWindow(Window *window);
    void moveWindowToFront(Window *window);
    void drawWidgets();
    bool canProcessEvents() const { return mProcessEvents; }
    void setPrepareFrameHandler(std::function<void(Screen*)> handler) { mPrepareFrameFunc = handler; }

    Widget* getCurrentSelection() const override;

    intptr_t createStandardCursor(int shape);

protected:
    void _setupStartParams();

    intptr_t mCursors[(int)Cursor::CursorCount];
    Cursor mCursor;
    Widget* mSelectedWidget = nullptr;
    std::vector<Widget *> mFocusPath;
    Vector2i mFBSize;
    float mPixelRatio;
    int mMouseState, mModifiers;
    Vector2i mMousePos;
    bool mDragActive;
    Widget *mDragWidget = nullptr;
    double mLastInteraction;
    bool mProcessEvents;
    Color mBackground;
    std::string mCaption;
    bool mShutdownOnDestruct;
    bool mFullscreen;
    bool mWidgetsNeedUpdateGlobal = false;
    std::vector<Widget*> mWidgetsNeedUpdate;
    std::function<void(Vector2i)> mResizeCallback;
    std::function<void(std::string)> mClipboardSetFunc;
    std::function<std::string()> mClipboardGetFunc;
    std::function<void(Screen*)> mPrepareFrameFunc;
    std::function<void(Screen*, Vector2i&, Vector2i&)> mResizeHwHandler;
    std::function<void(intptr_t)> mHwCursorSetter;
};

NAMESPACE_END(nanogui)
