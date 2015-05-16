#include <nanogui/screen.h>
#include <nanogui/theme.h>
#include <nanogui/opengl.h>
#include <nanogui/window.h>
#include <nanogui/popup.h>
#include <iostream>
#include <map>

#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>

NANOGUI_NAMESPACE_BEGIN

std::map<GLFWwindow *, Screen *> __nanogui_screens;

#if defined(WIN32)
static bool glewInitialized = false;
#endif

Screen::Screen(const Vector2i &size, const std::string &caption,
               bool resizable, bool fullscreen)
    : Widget(nullptr) {

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
        mGLFWWindow = glfwCreateWindow(mode->width, mode->height, caption.c_str(), monitor, nullptr);
    } else {
        mGLFWWindow = glfwCreateWindow(size.x(), size.y(), caption.c_str(), nullptr, nullptr);
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

#ifdef NDEBUG
    mNVGContext = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_ANTIALIAS);
#else
    mNVGContext = nvgCreateGL3(NVG_STENCIL_STROKES | NVG_ANTIALIAS | NVG_DEBUG);
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
            Vector2i p((int) x, (int) y);
            bool ret = false;
            s->mLastInteraction = glfwGetTime();
            try {
                if (s->mDragActive)
                    ret = s->mDragWidget->mouseDragEvent(
                        p - s->mDragWidget->parent()->absolutePosition(), p - s->mMousePos,
                        s->mMouseState, s->mModifiers);
                else
                    s->mouseMotionEvent(p, p - s->mMousePos, s->mMouseState, s->mModifiers);
                s->mMousePos = p;
            } catch (const std::exception &e) {
                std::cerr << "Caught exception in event handler: " << e.what() << std::endl;
                abort();
            }
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
            s->mModifiers = modifiers;
            s->mLastInteraction = glfwGetTime();
            try {
                if (s->mFocusPath.size() > 1) {
                    const Window *window = dynamic_cast<Window *>(s->mFocusPath[s->mFocusPath.size()-2]);
                    if (window && window->modal()) {
                        if (!window->contains(s->mMousePos))
                            return;
                    }
                }

                if (action == GLFW_PRESS)
                    s->mMouseState |= 1 << button;
                else
                    s->mMouseState &= ~(1 << button);

                if (s->mDragActive && action == GLFW_RELEASE &&
                    s->findWidget(s->mMousePos) != s->mDragWidget)
                    s->mDragWidget->mouseButtonEvent(
                        s->mMousePos - s->mDragWidget->parent()->absolutePosition(),
                        button, false, s->mModifiers);

                if (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_1) {
                    s->mDragWidget = s->findWidget(s->mMousePos);
                    if (s->mDragWidget == s)
                        s->mDragWidget = nullptr;
                    s->mDragActive = s->mDragWidget != nullptr;
                    if (!s->mDragActive)
                        s->updateFocus(nullptr);
                } else {
                    s->mDragActive = false;
                    s->mDragWidget = nullptr;
                }

                s->mouseButtonEvent(s->mMousePos, button, action == GLFW_PRESS, s->mModifiers);
            } catch (const std::exception &e) {
                std::cerr << "Caught exception in event handler: " << e.what() << std::endl;
                abort();
            }
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
            s->mLastInteraction = glfwGetTime();
            try {
                s->keyboardEvent(key, scancode, action == GLFW_PRESS, mods);
            } catch (const std::exception &e) {
                std::cerr << "Caught exception in event handler: " << e.what() << std::endl;
                abort();
            }
        }
    );

    glfwSetDropCallback(mGLFWWindow,
        [](GLFWwindow *w, int count, const char **filenames) {
            auto it = __nanogui_screens.find(w);
            if (it == __nanogui_screens.end() || count <= 0)
                return;
            Screen *s = it->second;
            std::vector<std::string> arg(count);
            for (int i=0; i<count; ++i)
                arg[i] = filenames[i];
            s->dropEvent(arg);
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
            s->mLastInteraction = glfwGetTime();
            try {
                if (s->mFocusPath.size() > 1) {
                    const Window *window = dynamic_cast<Window *>(s->mFocusPath[s->mFocusPath.size()-2]);
                    if (window && window->modal()) {
                        if (!window->contains(s->mMousePos))
                            return;
                    }
                }
                s->scrollEvent(s->mMousePos, Vector2f(x, y));
            } catch (const std::exception &e) {
                std::cerr << "Caught exception in event handler: " << e.what() << std::endl;
                abort();
            }
        }
    );

    glfwGetWindowSize(mGLFWWindow, &mSize[0], &mSize[1]);
    glfwGetFramebufferSize(mGLFWWindow, &mFBSize[0], &mFBSize[1]);
    mTheme = new Theme(mNVGContext);
    mMousePos = Vector2i::Zero();
    mMouseState = mModifiers = 0;
    mDragActive = false;
    mLastInteraction = glfwGetTime();
    mProcessEvents = true;
    mBackground = Vector3f(0.3f, 0.3f, 0.32f);
    mVisible = false;
    __nanogui_screens[mGLFWWindow] = this;
}

Screen::~Screen() {
    delete mTheme;
    __nanogui_screens.erase(mGLFWWindow);
    nvgDeleteGL3(mNVGContext);
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

void Screen::setSize(const Vector2i& size) {
    Widget::setSize(size);
    glfwSetWindowSize(mGLFWWindow, size.x(), size.y());
}

void Screen::drawAll() {
    Vector2i oldFBSize(mFBSize);
    glfwMakeContextCurrent(mGLFWWindow);
    glfwGetFramebufferSize(mGLFWWindow, &mFBSize[0], &mFBSize[1]);
    glfwGetWindowSize(mGLFWWindow, &mSize[0], &mSize[1]);
    glViewport(0, 0, mFBSize[0], mFBSize[1]);
    if (oldFBSize != mFBSize)
        framebufferSizeChanged();

    glClearColor(mBackground[0], mBackground[1], mBackground[2], 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
            GL_STENCIL_BUFFER_BIT);
    drawContents();

    /* Calculate pixel ratio for hi-dpi devices. */
    mPixelRatio = (float)mFBSize[0] / (float)mSize[0];
    nvgBeginFrame(mNVGContext, mSize[0], mSize[1], mPixelRatio);

    nvgTranslate(mNVGContext, -2, -2);
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
            Vector2i pos =
                widget->absolutePosition() +
                Vector2i(widget->width() / 2, widget->height() + 10);

            nvgTextBoxBounds(mNVGContext, pos.x(), pos.y(), tooltipWidth,
                             widget->tooltip().c_str(), nullptr, bounds);

            nvgGlobalAlpha(mNVGContext,
                           std::min(1.0, 2 * (elapsed - 0.5f)) * 0.8);

            nvgBeginPath(mNVGContext);
            nvgFillColor(mNVGContext, Color(0, 255));
            int h = (bounds[2] - bounds[0]) / 2;
            nvgRoundedRect(mNVGContext, bounds[0] - 4 - h, bounds[1] - 4,
                           (int)(bounds[2] - bounds[0]) + 8,
                           (int)(bounds[3] - bounds[1]) + 8, 3);

            int px = (int)((bounds[2]+bounds[0])/2) - h;
            nvgMoveTo(mNVGContext, px,bounds[1] - 10);
            nvgLineTo(mNVGContext, px+7,bounds[1]+1);
            nvgLineTo(mNVGContext, px-7,bounds[1]+1);
            nvgFill(mNVGContext);

            nvgFillColor(mNVGContext, Color(255, 255));
            nvgFontBlur(mNVGContext, 0.0f);
            nvgTextBox(mNVGContext, pos.x() - h, pos.y(), tooltipWidth,
                       widget->tooltip().c_str(), nullptr);
        }
    }

    nvgEndFrame(mNVGContext);

    glfwSwapBuffers(mGLFWWindow);
}

bool Screen::keyboardEvent(int key, int scancode, bool press, int modifiers) {
    if (Widget::keyboardEvent(key, scancode, press, modifiers))
        return true;

    if (key == GLFW_KEY_ESCAPE && press) {
        setVisible(false);
        return true;
    }

    return false;
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
    mChildren.erase(std::remove(mChildren.begin(), mChildren.end(), window), mChildren.end());
    if (std::find(mFocusPath.begin(), mFocusPath.end(), window) != mFocusPath.end())
        mFocusPath.clear();
    if (mDragWidget == window)
        mDragWidget = nullptr;
    delete window;
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

NANOGUI_NAMESPACE_END
