/*
    src/screen.cpp -- Top-level widget and interface between NanoGUI and GLFW

    A significant redesign of this code was contributed by Christian Schueller.

    NanoGUI was developed by Wenzel Jakob <wenzel@inf.ethz.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/screen.h>
#include <nanogui/opengl.h>
#include <iostream>
#include <map>

NAMESPACE_BEGIN(nanogui)

std::map<GLFWwindow *, Screen *> __nanogui_screens;

#if defined(WIN32)
static bool glewInitialized = false;
#endif

Screen::Screen()
    : ScreenCore(), mGLFWWindow(nullptr), mShutdownGLFWOnDestruct(false) {
    memset(mCursors, 0, sizeof(GLFWcursor *) * (int) Cursor::CursorCount);
}

Screen::Screen(const Vector2i &size, const std::string &caption,
               bool resizable, bool fullscreen)
    : ScreenCore(), mGLFWWindow(nullptr), mCaption(caption), mShutdownGLFWOnDestruct(false) {
    memset(mCursors, 0, sizeof(GLFWcursor *) * (int) Cursor::CursorCount);
    
    /* Request a forward compatible OpenGL 3.3 core profile context */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Request a RGBA8 buffer without MSAA */
    glfwWindowHint(GLFW_SAMPLES, 0);
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
        mGLFWWindow = glfwCreateWindow(size.x(), size.y(), caption.c_str(),
                                       nullptr, nullptr);
    }

    if (!mGLFWWindow)
        throw std::runtime_error("Could not create an OpenGL 3.3 context!");

    glfwMakeContextCurrent(mGLFWWindow);

#if defined(WIN32)
    if (!glewInitialized) {
        glewExperimental = GL_TRUE;
        glewInitialized = true;
        if (glewInit() != GLEW_NO_ERROR)
            throw std::runtime_error("Could not initialize GLEW!");
        glGetError(); // pull and ignore unhandled errors like GL_INVALID_ENUM
    }
#endif

    glfwGetFramebufferSize(mGLFWWindow, &mFBSize[0], &mFBSize[1]);
    mSize = size;
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
    init(mSize, (float)mFBSize[0] / (float)mSize[0]);

    mVisible = glfwGetWindowAttrib(window, GLFW_VISIBLE) != 0;
    mBackground = Vector3f(0.3f, 0.3f, 0.32f);
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
    glfwMakeContextCurrent(mGLFWWindow);
    glfwGetFramebufferSize(mGLFWWindow, &mFBSize[0], &mFBSize[1]);
    glfwGetWindowSize(mGLFWWindow, &mSize[0], &mSize[1]);
    glViewport(0, 0, mFBSize[0], mFBSize[1]);
    setPixelRatio((float) mFBSize[0] / (float) mSize[0]);

    drawContents();
    
    if (mVisible)
        drawWidgets();

    glfwSwapBuffers(mGLFWWindow);
}

bool Screen::dropCallbackEvent(int count, const char **filenames) {
    std::vector<std::string> arg(count);
    for (int i = 0; i < count; ++i)
        arg[i] = filenames[i];
    return dropEvent(arg);
}

bool Screen::resizeCallbackEvent(int, int) {
    glfwGetWindowSize(mGLFWWindow, &mSize[0], &mSize[1]);
    glfwGetFramebufferSize(mGLFWWindow, &mFBSize[0], &mFBSize[1]);
    try {
        return resizeEvent(mSize);
    } catch (const std::exception &e) {
        std::cerr << "Caught exception in event handler: " << e.what()
                  << std::endl;
        abort();
    }
}

void Screen::setCursorAppearance(int c) {
    glfwSetCursor(mGLFWWindow, mCursors[c]);
}

/// Reimplement this anc call glfwSetClipboardString() with the string given by the parameter
void Screen::setCliboardString(const std::string &str) {
    glfwSetClipboardString(mGLFWWindow, str.c_str());
}

/// Reimplement this anc call glfwGetClipboardString()
std::string Screen::getClipboardString() {
    return std::string(glfwGetClipboardString(mGLFWWindow));
}

NAMESPACE_END(nanogui)
