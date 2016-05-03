/*
    src/screen.cpp -- Top-level widget and interface between NanoGUI and GLFW

    A significant redesign of this code was contributed by Christian Schueller.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/screen.h>
#include <nanogui/theme.h>
#include <nanogui/opengl.h>
#include <nanogui/window.h>
#include <nanogui/popup.h>
#include <iostream>
#include <map>

#if defined(_WIN32)
#define NOMINMAX
#undef APIENTRY
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#endif

/* Allow enforcing the GL2 implementation of NanoVG */
#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>

NAMESPACE_BEGIN(nanogui)

std::map<GLFWwindow *, Screen *> __nanogui_screens;

#if defined(_WIN32)
static bool gladInitialized = false;
#endif

/* Calculate pixel ratio for hi-dpi devices. */
static float get_pixel_ratio(GLFWwindow *window) {
#if defined(_WIN32)
    HWND hWnd = glfwGetWin32Window(window);
    HMONITOR monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
    /* The following function only exists on Windows 8.1+, but we don't want to make that a dependency */
    static HRESULT (WINAPI *GetDpiForMonitor_)(HMONITOR, UINT, UINT*, UINT*) = nullptr;
    static bool GetDpiForMonitor_tried = false;

    if (!GetDpiForMonitor_tried) {
        auto shcore = LoadLibrary(TEXT("shcore"));
        if (shcore)
            GetDpiForMonitor_ = (decltype(GetDpiForMonitor_)) GetProcAddress(shcore, "GetDpiForMonitor");
        GetDpiForMonitor_tried = true;
    }

    if (GetDpiForMonitor_) {
        uint32_t dpiX, dpiY;
        if (GetDpiForMonitor_(monitor, 0 /* effective DPI */, &dpiX, &dpiY) == S_OK)
            return std::round(dpiX / 96.0);
    }
    return 1.f;
#else
    Vector2i fbSize, size;
    glfwGetFramebufferSize(window, &fbSize[0], &fbSize[1]);
    glfwGetWindowSize(window, &size[0], &size[1]);
    return (float)fbSize[0] / (float)size[0];
#endif
}
Screen::Screen()
    : Widget(nullptr), mGLFWWindow(nullptr), mNVGContext(nullptr),
      mCursor(Cursor::Arrow), mBackground(0.3f, 0.3f, 0.32f),
      mShutdownGLFWOnDestruct(false), mFullscreen(false) {
    memset(mCursors, 0, sizeof(GLFWcursor *) * (int) Cursor::CursorCount);
}

Screen::Screen(const Vector2i &size, const std::string &caption,
               bool resizable, bool fullscreen)
    : Widget(nullptr), mGLFWWindow(nullptr), mNVGContext(nullptr),
      mCursor(Cursor::Arrow), mBackground(0.3f, 0.3f, 0.32f), mCaption(caption),
      mShutdownGLFWOnDestruct(false), mFullscreen(fullscreen) {
    memset(mCursors, 0, sizeof(GLFWcursor *) * (int) Cursor::CursorCount);

    /* Request a forward compatible OpenGL 3.3 core profile context */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Request a RGBA8 buffer without MSAA */
    glfwWindowHint(GLFW_SAMPLES, 16);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, resizable ? GL_TRUE : GL_FALSE);

    if (fullscreen) {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        mGLFWWindow = glfwCreateWindow(mode->width, mode->height,
                                       caption.c_str(), monitor, nullptr);
    } else {
        mGLFWWindow = glfwCreateWindow(size.x(), size.y(),
                                       caption.c_str(), nullptr, nullptr);
    }

    if (!mGLFWWindow)
        throw std::runtime_error("Could not create an OpenGL 3.3 context!");

    glfwMakeContextCurrent(mGLFWWindow);

#if defined(_WIN32)
    if (!gladInitialized) {
        gladInitialized = true;
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            throw std::runtime_error("Could not initialize GLAD!");
        glGetError(); // pull and ignore unhandled errors like GL_INVALID_ENUM
    }
#endif

    glfwGetFramebufferSize(mGLFWWindow, &mFBSize[0], &mFBSize[1]);
    glViewport(0, 0, mFBSize[0], mFBSize[1]);
    glClearColor(mBackground[0], mBackground[1], mBackground[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glfwSwapInterval(0);
    glfwSwapBuffers(mGLFWWindow);

#if defined(__APPLE__)
    /* Poll for events once before starting a potentially
       lengthy loading process. This is needed to be
       classified as "interactive" by other software such
       as iTerm2 */

    glfwPollEvents();
#endif

    /* Propagate GLFW events to the appropriate Screen instance */
    glfwSetCursorPosCallback(mGLFWWindow,
        [](GLFWwindow *w, double x, double y) {
            auto it = __nanogui_screens.find(w);
            if (it == __nanogui_screens.end())
                return;
            Screen *s = it->second;
            if (!s->mProcessEvents)
                return;
            s->cursorPosCallbackEvent(x, y);
        }
    );

    glfwSetMouseButtonCallback(mGLFWWindow,
        [](GLFWwindow *w, int button, int action, int modifiers) {
            auto it = __nanogui_screens.find(w);
            if (it == __nanogui_screens.end())
                return;
            Screen *s = it->second;
            if (!s->mProcessEvents)
                return;
            s->mouseButtonCallbackEvent(button, action, modifiers);
        }
    );

    glfwSetKeyCallback(mGLFWWindow,
        [](GLFWwindow *w, int key, int scancode, int action, int mods) {
            auto it = __nanogui_screens.find(w);
            if (it == __nanogui_screens.end())
                return;
            Screen *s = it->second;
            if (!s->mProcessEvents)
                return;
            s->keyCallbackEvent(key, scancode, action, mods);
        }
    );

    glfwSetCharCallback(mGLFWWindow,
        [](GLFWwindow *w, unsigned int codepoint) {
            auto it = __nanogui_screens.find(w);
            if (it == __nanogui_screens.end())
                return;
            Screen *s = it->second;
            if (!s->mProcessEvents)
                return;
            s->charCallbackEvent(codepoint);
        }
    );

    glfwSetDropCallback(mGLFWWindow,
        [](GLFWwindow *w, int count, const char **filenames) {
            auto it = __nanogui_screens.find(w);
            if (it == __nanogui_screens.end())
                return;
            Screen *s = it->second;
            if (!s->mProcessEvents)
                return;
            s->dropCallbackEvent(count, filenames);
        }
    );

    glfwSetScrollCallback(mGLFWWindow,
        [](GLFWwindow *w, double x, double y) {
            auto it = __nanogui_screens.find(w);
            if (it == __nanogui_screens.end())
                return;
            Screen *s = it->second;
            if (!s->mProcessEvents)
                return;
            s->scrollCallbackEvent(x, y);
        }
    );

    /* React to framebuffer size events -- includes window
       size events and also catches things like dragging
       a window from a Retina-capable screen to a normal
       screen on Mac OS X */
    glfwSetFramebufferSizeCallback(mGLFWWindow,
        [](GLFWwindow* w, int width, int height) {
            auto it = __nanogui_screens.find(w);
            if (it == __nanogui_screens.end())
                return;
            Screen* s = it->second;

            if (!s->mProcessEvents)
                return;

            s->resizeCallbackEvent(width, height);
        }
    );

    initialize(mGLFWWindow, true);
}

void Screen::initialize(GLFWwindow *window, bool shutdownGLFWOnDestruct) {
    mGLFWWindow = window;
    mShutdownGLFWOnDestruct = shutdownGLFWOnDestruct;
    glfwGetWindowSize(mGLFWWindow, &mSize[0], &mSize[1]);
    glfwGetFramebufferSize(mGLFWWindow, &mFBSize[0], &mFBSize[1]);

    mPixelRatio = get_pixel_ratio(window);

#if defined(_WIN32)
    if (mPixelRatio != 1 && !mFullscreen)
        glfwSetWindowSize(window, mSize.x() * mPixelRatio, mSize.y() * mPixelRatio);
#endif

#ifdef NDEBUG
    mNVGContext = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_ANTIALIAS);
#else
    mNVGContext = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_ANTIALIAS | NVG_DEBUG);
#endif
    if (mNVGContext == nullptr)
        throw std::runtime_error("Could not initialize NanoVG!");

    mVisible = glfwGetWindowAttrib(window, GLFW_VISIBLE) != 0;
    setTheme(new Theme(mNVGContext));
    mMousePos = Vector2i::Zero();
    mMouseState = mModifiers = 0;
    mDragActive = false;
    mLastInteraction = glfwGetTime();
    mProcessEvents = true;
    __nanogui_screens[mGLFWWindow] = this;

    for (int i=0; i < (int) Cursor::CursorCount; ++i)
        mCursors[i] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR + i);
}

Screen::~Screen() {
    __nanogui_screens.erase(mGLFWWindow);
    for (int i=0; i < (int) Cursor::CursorCount; ++i) {
        if (mCursors[i])
            glfwDestroyCursor(mCursors[i]);
    }
    if (mNVGContext)
        nvgDeleteGL3(mNVGContext);
    if (mGLFWWindow && mShutdownGLFWOnDestruct)
        glfwDestroyWindow(mGLFWWindow);
}

void Screen::setVisible(bool visible) {
    if (mVisible != visible) {
        mVisible = visible;

        if (visible)
            glfwShowWindow(mGLFWWindow);
        else
            glfwHideWindow(mGLFWWindow);
    }
}

void Screen::setCaption(const std::string &caption) {
    if (caption != mCaption) {
        glfwSetWindowTitle(mGLFWWindow, caption.c_str());
        mCaption = caption;
    }
}

void Screen::setSize(const Vector2i &size) {
    Widget::setSize(size);
    glfwSetWindowSize(mGLFWWindow, size.x(), size.y());
}

void Screen::drawAll() {
    glClearColor(mBackground[0], mBackground[1], mBackground[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    drawContents();
    drawWidgets();

    glfwSwapBuffers(mGLFWWindow);
}

void Screen::drawWidgets() {
    if (!mVisible)
        return;

    glfwMakeContextCurrent(mGLFWWindow);
    float newPixelRatio = get_pixel_ratio(mGLFWWindow);

#if defined(_WIN32)
    if (mPixelRatio != newPixelRatio && !mFullscreen)
        glfwSetWindowSize(mGLFWWindow, mSize.x() * newPixelRatio / mPixelRatio, mSize.y() * newPixelRatio / mPixelRatio);
#endif

    mPixelRatio = newPixelRatio;
    glfwGetFramebufferSize(mGLFWWindow, &mFBSize[0], &mFBSize[1]);
    glfwGetWindowSize(mGLFWWindow, &mSize[0], &mSize[1]);

#if defined(_WIN32)
    mSize /= mPixelRatio;
#endif

    glViewport(0, 0, mFBSize[0], mFBSize[1]);

    /* Calculate pixel ratio for hi-dpi devices. */
    mPixelRatio = (float) mFBSize[0] / (float) mSize[0];
    nvgBeginFrame(mNVGContext, mSize[0], mSize[1], mPixelRatio);

    draw(mNVGContext);

    double elapsed = glfwGetTime() - mLastInteraction;

    if (elapsed > 0.5f) {
        /* Draw tooltips */
        const Widget *widget = findWidget(mMousePos);
        if (widget && !widget->tooltip().empty()) {
            int tooltipWidth = 150;

            float bounds[4];
            nvgFontFace(mNVGContext, "sans");
            nvgFontSize(mNVGContext, 15.0f);
            nvgTextAlign(mNVGContext, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
            nvgTextLineHeight(mNVGContext, 1.1f);
            Vector2i pos = widget->absolutePosition() +
                           Vector2i(widget->width() / 2, widget->height() + 10);

            nvgTextBoxBounds(mNVGContext, pos.x(), pos.y(), tooltipWidth,
                             widget->tooltip().c_str(), nullptr, bounds);

            nvgGlobalAlpha(mNVGContext,
                           std::min(1.0, 2 * (elapsed - 0.5f)) * 0.8);

            nvgBeginPath(mNVGContext);
            nvgFillColor(mNVGContext, Color(0, 255));
            int h = (bounds[2] - bounds[0]) / 2;
            nvgRoundedRect(mNVGContext, bounds[0] - 4 - h, bounds[1] - 4,
                           (int) (bounds[2] - bounds[0]) + 8,
                           (int) (bounds[3] - bounds[1]) + 8, 3);

            int px = (int) ((bounds[2] + bounds[0]) / 2) - h;
            nvgMoveTo(mNVGContext, px, bounds[1] - 10);
            nvgLineTo(mNVGContext, px + 7, bounds[1] + 1);
            nvgLineTo(mNVGContext, px - 7, bounds[1] + 1);
            nvgFill(mNVGContext);

            nvgFillColor(mNVGContext, Color(255, 255));
            nvgFontBlur(mNVGContext, 0.0f);
            nvgTextBox(mNVGContext, pos.x() - h, pos.y(), tooltipWidth,
                       widget->tooltip().c_str(), nullptr);
        }
    }

    nvgEndFrame(mNVGContext);
}

bool Screen::keyboardEvent(int key, int scancode, int action, int modifiers) {
    if (mFocusPath.size() > 0) {
        for (auto it = mFocusPath.rbegin() + 1; it != mFocusPath.rend(); ++it)
            if ((*it)->focused() && (*it)->keyboardEvent(key, scancode, action, modifiers))
                return true;
    }

    return false;
}

bool Screen::keyboardCharacterEvent(unsigned int codepoint) {
    if (mFocusPath.size() > 0) {
        for (auto it = mFocusPath.rbegin() + 1; it != mFocusPath.rend(); ++it)
            if ((*it)->focused() && (*it)->keyboardCharacterEvent(codepoint))
                return true;
    }
    return false;
}

bool Screen::cursorPosCallbackEvent(double x, double y) {
    Vector2i p((int) x, (int) y);
#if defined(_WIN32)
    p /= mPixelRatio;
#endif
    bool ret = false;
    mLastInteraction = glfwGetTime();
    try {
        p -= Vector2i(1, 2);

        if (!mDragActive) {
            Widget *widget = findWidget(p);
            if (widget != nullptr && widget->cursor() != mCursor) {
                mCursor = widget->cursor();
                glfwSetCursor(mGLFWWindow, mCursors[(int) mCursor]);
            }
        } else {
            ret = mDragWidget->mouseDragEvent(
                p - mDragWidget->parent()->absolutePosition(), p - mMousePos,
                mMouseState, mModifiers);
        }

        if (!ret)
            ret = mouseMotionEvent(p, p - mMousePos, mMouseState, mModifiers);

        mMousePos = p;

        return ret;
    } catch (const std::exception &e) {
        std::cerr << "Caught exception in event handler: " << e.what() << std::endl;
        abort();
    }
}

bool Screen::mouseButtonCallbackEvent(int button, int action, int modifiers) {
    mModifiers = modifiers;
    mLastInteraction = glfwGetTime();
    try {
        if (mFocusPath.size() > 1) {
            const Window *window =
                dynamic_cast<Window *>(mFocusPath[mFocusPath.size() - 2]);
            if (window && window->modal()) {
                if (!window->contains(mMousePos))
                    return false;
            }
        }

        if (action == GLFW_PRESS)
            mMouseState |= 1 << button;
        else
            mMouseState &= ~(1 << button);

        auto dropWidget = findWidget(mMousePos);
        if (mDragActive && action == GLFW_RELEASE &&
            dropWidget != mDragWidget)
            mDragWidget->mouseButtonEvent(
                mMousePos - mDragWidget->parent()->absolutePosition(), button,
                false, mModifiers);

        if (dropWidget != nullptr && dropWidget->cursor() != mCursor) {
            mCursor = dropWidget->cursor();
            glfwSetCursor(mGLFWWindow, mCursors[(int) mCursor]);
        }

        if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_1) {
            mDragWidget = findWidget(mMousePos);
            if (mDragWidget == this)
                mDragWidget = nullptr;
            mDragActive = mDragWidget != nullptr;
            if (!mDragActive)
                updateFocus(nullptr);
        } else {
            mDragActive = false;
            mDragWidget = nullptr;
        }

        return mouseButtonEvent(mMousePos, button, action == GLFW_PRESS,
                                mModifiers);
    } catch (const std::exception &e) {
        std::cerr << "Caught exception in event handler: " << e.what() << std::endl;
        abort();
    }
}

bool Screen::keyCallbackEvent(int key, int scancode, int action, int mods) {
    mLastInteraction = glfwGetTime();
    try {
        return keyboardEvent(key, scancode, action, mods);
    } catch (const std::exception &e) {
        std::cerr << "Caught exception in event handler: " << e.what() << std::endl;
        abort();
    }
}

bool Screen::charCallbackEvent(unsigned int codepoint) {
    mLastInteraction = glfwGetTime();
    try {
        return keyboardCharacterEvent(codepoint);
    } catch (const std::exception &e) {
        std::cerr << "Caught exception in event handler: " << e.what()
                  << std::endl;
        abort();
    }
}

bool Screen::dropCallbackEvent(int count, const char **filenames) {
    std::vector<std::string> arg(count);
    for (int i = 0; i < count; ++i)
        arg[i] = filenames[i];
    return dropEvent(arg);
}

bool Screen::scrollCallbackEvent(double x, double y) {
    mLastInteraction = glfwGetTime();
    try {
        if (mFocusPath.size() > 1) {
            const Window *window =
                dynamic_cast<Window *>(mFocusPath[mFocusPath.size() - 2]);
            if (window && window->modal()) {
                if (!window->contains(mMousePos))
                    return false;
            }
        }
        return scrollEvent(mMousePos, Vector2f(x, y));
    } catch (const std::exception &e) {
        std::cerr << "Caught exception in event handler: " << e.what()
                  << std::endl;
        abort();
    }
}

bool Screen::resizeCallbackEvent(int, int) {
    Vector2i fbSize, size;
    glfwGetFramebufferSize(mGLFWWindow, &fbSize[0], &fbSize[1]);
    glfwGetWindowSize(mGLFWWindow, &size[0], &size[1]);
#if defined(_WIN32)
    size /= mPixelRatio;
#endif

    if (mFBSize == Vector2i(0, 0) || size == Vector2i(0, 0))
        return false;

    mFBSize = fbSize; mSize = size;
    mLastInteraction = glfwGetTime();

    try {
        return resizeEvent(mSize);
    } catch (const std::exception &e) {
        std::cerr << "Caught exception in event handler: " << e.what()
                  << std::endl;
        abort();
    }
}

void Screen::updateFocus(Widget *widget) {
    for (auto w: mFocusPath) {
        if (!w->focused())
            continue;
        w->focusEvent(false);
    }
    mFocusPath.clear();
    Widget *window = nullptr;
    while (widget) {
        mFocusPath.push_back(widget);
        if (dynamic_cast<Window *>(widget))
            window = widget;
        widget = widget->parent();
    }
    for (auto it = mFocusPath.rbegin(); it != mFocusPath.rend(); ++it)
        (*it)->focusEvent(true);

    if (window)
        moveWindowToFront((Window *) window);
}

void Screen::disposeWindow(Window *window) {
    if (std::find(mFocusPath.begin(), mFocusPath.end(), window) != mFocusPath.end())
        mFocusPath.clear();
    if (mDragWidget == window)
        mDragWidget = nullptr;
    removeChild(window);
}

void Screen::centerWindow(Window *window) {
    if (window->size() == Vector2i::Zero()) {
        window->setSize(window->preferredSize(mNVGContext));
        window->performLayout(mNVGContext);
    }
    window->setPosition((mSize - window->size()) / 2);
}

void Screen::moveWindowToFront(Window *window) {
    mChildren.erase(std::remove(mChildren.begin(), mChildren.end(), window), mChildren.end());
    mChildren.push_back(window);
    /* Brute force topological sort (no problem for a few windows..) */
    bool changed = false;
    do {
        size_t baseIndex = 0;
        for (size_t index = 0; index < mChildren.size(); ++index)
            if (mChildren[index] == window)
                baseIndex = index;
        changed = false;
        for (size_t index = 0; index < mChildren.size(); ++index) {
            Popup *pw = dynamic_cast<Popup *>(mChildren[index]);
            if (pw && pw->parentWindow() == window && index < baseIndex) {
                moveWindowToFront(pw);
                changed = true;
                break;
            }
        }
    } while (changed);
}

NAMESPACE_END(nanogui)
