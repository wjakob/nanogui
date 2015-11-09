/*
    nanogui/screen_core.h -- Top-level widget
 
    This code has been modified by Giorgio Marcias from the original screen.h
    in order to allow for NanoGUI screens to be associated to custom GLFW windows
    having custom callbacks.

    A significant redesign of this code was contributed by Christian Schueller.

    NanoGUI was developed by Wenzel Jakob <wenzel@inf.ethz.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#pragma once

#include <nanogui/window.h>
#include <nanogui/opengl.h>

NAMESPACE_BEGIN(nanogui)

/**
 * \brief Forms the root element of a hierarchy of nanogui widgets.
 */
class NANOGUI_EXPORT ScreenCore : public Widget {
protected:
    friend class Widget;
    friend class Window;
public:
    /**
     * \brief Default constructor
     *
     * Performs no initialization at all. Use this if the application is
     * responsible for setting up GLFW, OpenGL, etc.
     *
     * In this case, override \ref ScreenCore and call \ref initalize().
     *
     * You will also be responsible in this case to call the event handlers below.
     */
    ScreenCore();

    /// Release all resources
    virtual ~ScreenCore();
    
    /// Release all resources
    void release();
    
    /// Initialize the \ref ScreenCore
    void init(const Vector2i &s, float pRatio);

    /// Set the pixel ratio
    void setPixelRatio(float pRatio);

    /// Default keyboard event handler
    virtual bool keyboardEvent(int key, int scancode, int action, int modifiers);

    /// Text input event handler: codepoint is native endian UTF-32 format
    virtual bool keyboardCharacterEvent(unsigned int codepoint);

    /// Return the last observed mouse position value
    Vector2i mousePos() const { return mMousePos; }

    /// Return a pointer to the underlying nanoVG draw context
    NVGcontext *nvgContext() { return mNVGContext; }

    /// Compute the layout of all widgets
    void performLayout() {
        Widget::performLayout(mNVGContext);
    }

    /* Event handlers */
    bool cursorPosCallbackEvent(double x, double y);
    bool mouseButtonCallbackEvent(int button, int action, int modifiers);
    bool keyCallbackEvent(int key, int scancode, int action, int mods);
    bool charCallbackEvent(unsigned int codepoint);
    bool scrollCallbackEvent(double x, double y);

    /* Internal helper functions */
    void updateFocus(Widget *widget);
    void disposeWindow(Window *window);
    void centerWindow(Window *window);
    void moveWindowToFront(Window *window);
    void drawWidgets();

    void performLayout(NVGcontext *ctx) {
        Widget::performLayout(ctx);
    }

    /// Reimplement this and call glfwSetCursor() with the cursor type given by the parameter
    virtual void setCursorAppearance(int) { }
    /// Reimplement this anc call glfwSetClipboardString() with the string given by the parameter
    virtual void setCliboardString(const std::string &) { }
    /// Reimplement this anc call glfwGetClipboardString()
    virtual std::string getClipboardString() { return std::string(); }

protected:    
    NVGcontext *mNVGContext;
    Cursor mCursor;
    std::vector<Widget *> mFocusPath;
    float mPixelRatio;
    int mMouseState, mModifiers;
    Vector2i mMousePos;
    bool mDragActive;
    Widget *mDragWidget = nullptr;
    double mLastInteraction;
    bool mProcessEvents;
};

NAMESPACE_END(nanogui)
