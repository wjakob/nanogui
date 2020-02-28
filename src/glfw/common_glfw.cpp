#include <nanogui/common.h>

#if NANOGUI_OPENGL_BACKEND

#include <nanogui/screen.h>
#include <nanogui/opengl.h>
#include <GLFW/glfw3.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <map>

#define NANOVG_GL3_IMPLEMENTATION
#include <nanovg_gl.h>

#if defined(_WIN32)
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  undef APIENTRY

#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <windows.h>

#  define GLFW_EXPOSE_NATIVE_WGL
#  define GLFW_EXPOSE_NATIVE_WIN32
#  include <GLFW/glfw3native.h>
#endif

NAMESPACE_BEGIN(nanogui)

bool isMouseButtonLeft(int button) { return button == GLFW_MOUSE_BUTTON_1; }
bool isMouseButtonLeftMod(int button) { return button == (1 << GLFW_MOUSE_BUTTON_1); }
bool isMouseButtonRight(int button) { return button == GLFW_MOUSE_BUTTON_RIGHT; }
bool isMouseActionRelease(int action) { return action == GLFW_RELEASE; }
bool isMouseActionPress(int action) { return action == GLFW_PRESS; }

#if defined(__APPLE__)
  extern void disable_saved_application_state_osx();
#endif

void init() {
#if !defined(_WIN32)
  /* Avoid locale-related number parsing issues */
  setlocale(LC_NUMERIC, "C");
#endif

#if defined(__APPLE__)
  disable_saved_application_state_osx();
#endif

  glfwSetErrorCallback(
    [](int error, const char *descr) {
    if (error == GLFW_NOT_INITIALIZED)
      return; /* Ignore */
    std::cerr << "GLFW error " << error << ": " << descr << std::endl;
  }
  );

  if (!glfwInit())
    throw std::runtime_error("Could not initialize GLFW!");

  glfwSetTime(0);
}

void shutdown() { glfwTerminate(); }

float getTimeFromStart() { return glfwGetTime(); }

bool sample::post_empty_event() { glfwPostEmptyEvent(); return true; }
bool sample::wait_events() { glfwWaitEvents(); return true; }
bool sample::poll_events() { glfwPollEvents(); return true; }
void sample::frame_loop(std::function<void()> &f) { while (is_main_loop_active()) { f(); }; }

void __nanogui_destroy_cursor(intptr_t cursor) { glfwDestroyCursor((GLFWcursor*)cursor); }
intptr_t __nanogui_create_cursor(int shape) { return (intptr_t)glfwCreateStandardCursor(GLFW_ARROW_CURSOR + shape); }

void sample::clear_frame(const Color& background)
{
  glClearColor(background.r(), background.g(), background.b(), background.a());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void sample::present_frame(void* window) { glfwSwapBuffers((GLFWwindow*)window); }

bool isKeyboardActionRelease(int action) { return action == GLFW_RELEASE; }
bool isKeyboardModifierCtrl(int modifier) { return modifier & SYSTEM_COMMAND_MOD; }
bool isKeyboardModifierShift(int modifier) { return modifier & GLFW_MOD_SHIFT; }
bool isKeyboardActionPress(int action) { return action == GLFW_PRESS; }
bool isKeyboardActionRepeat(int action) { return action == GLFW_REPEAT; }
bool isKeyboardKeyEscape(int key) { return key == GLFW_KEY_ESCAPE; }

uint32_t key2fourcc(int key)
{
  switch (key) {
#define RET_KEYCODE(k,c) case k: return FOURCCS(c);
  RET_KEYCODE(GLFW_KEY_DELETE, "KDEL")
  RET_KEYCODE(GLFW_KEY_A, "KEYA")
  RET_KEYCODE(GLFW_KEY_X, "KEYX")
  RET_KEYCODE(GLFW_KEY_N, "KEYN")
  RET_KEYCODE(GLFW_KEY_B, "KEYB")
  RET_KEYCODE(GLFW_KEY_C, "KEYC")
  RET_KEYCODE(GLFW_KEY_R, "KEYR")
  RET_KEYCODE(GLFW_KEY_P, "KEYP")
  RET_KEYCODE(GLFW_KEY_V, "KEYV")
  RET_KEYCODE(GLFW_KEY_Z, "KEYZ")
  RET_KEYCODE(GLFW_KEY_LEFT, "LEFT")
  RET_KEYCODE(GLFW_KEY_RIGHT, "RGHT")
  RET_KEYCODE(GLFW_KEY_DOWN, "DOWN")
  RET_KEYCODE(GLFW_KEY_UP, "KBUP")
  RET_KEYCODE(GLFW_KEY_HOME, "HOME")
  RET_KEYCODE(GLFW_KEY_END, "KEND")
  RET_KEYCODE(GLFW_KEY_BACKSPACE, "BACK")
  RET_KEYCODE(GLFW_KEY_ENTER, "ENTR")
  RET_KEYCODE(GLFW_KEY_SPACE, "SPCE")
  RET_KEYCODE(GLFW_KEY_ESCAPE, "ESCP")
#undef RET_KEYCODE
  default: return FOURCCS("UNKN");
  }
}

namespace sample 
{

void create_context()
{
  int flags = 0;
  flags |= NVG_STENCIL_STROKES;
  flags |= NVG_ANTIALIAS;
#if !defined(NDEBUG)
  flags |= NVG_DEBUG;
#endif

  __nanogui_context = nvgCreateGL3(flags);
  if (__nanogui_context == nullptr)
    throw std::runtime_error("Could not initialize NanoVG!");
}

void destroy_context()
{
  if (__nanogui_context)
    nvgDeleteGL3(__nanogui_context);
}

#if defined(NANOGUI_GLAD)
bool gladInitialized = false;
#endif

/* Calculate pixel ratio for hi-dpi devices. */
static float get_pixel_ratio(GLFWwindow *window) {
#if defined(_WIN32)
  HWND hWnd = glfwGetWin32Window(window);
  HMONITOR monitor = nullptr;
#if defined(MONITOR_DEFAULTTONEAREST)
  monitor = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
#else
  static HMONITOR(WINAPI *MonitorFromWindow_)(HWND, DWORD) = nullptr;
  static bool MonitorFromWindow_tried = false;
  if (!MonitorFromWindow_tried) {
    auto user32 = LoadLibrary(TEXT("user32"));
    if (user32)
      MonitorFromWindow_ = (decltype(MonitorFromWindow_))GetProcAddress(user32, "MonitorFromWindow");
    MonitorFromWindow_tried = true;
  }
  if (MonitorFromWindow_)
    monitor = MonitorFromWindow_(hWnd, 2);
#endif  // defined(MONITOR_DEFAULTTONEAREST)
  /* The following function only exists on Windows 8.1+, but we don't want to make that a dependency */
  static HRESULT(WINAPI *GetDpiForMonitor_)(HMONITOR, UINT, UINT*, UINT*) = nullptr;
  static bool GetDpiForMonitor_tried = false;

  if (!GetDpiForMonitor_tried) {
    auto shcore = LoadLibrary(TEXT("shcore"));
    if (shcore)
      GetDpiForMonitor_ = (decltype(GetDpiForMonitor_))GetProcAddress(shcore, "GetDpiForMonitor");
    GetDpiForMonitor_tried = true;
  }

  if (GetDpiForMonitor_ && monitor) {
    uint32_t dpiX, dpiY;
    if (GetDpiForMonitor_(monitor, 0 /* effective DPI */, &dpiX, &dpiY) == S_OK)
      return dpiX / 96.0;
  }
  return 1.f;
#elif defined(__linux__)
  (void)window;

  float ratio = 1.0f;
  FILE *fp;
  /* Try to read the pixel ratio from KDEs config */
  auto currentDesktop = std::getenv("XDG_CURRENT_DESKTOP");
  if (currentDesktop && currentDesktop == std::string("KDE")) {
    fp = popen("kreadconfig5 --group KScreen --key ScaleFactor", "r");
    if (!fp)
      return 1;

    if (fscanf(fp, "%f", &ratio) != 1)
      return 1;
  }
  else {
    /* Try to read the pixel ratio from GTK */
    fp = popen("gsettings get org.gnome.desktop.interface scaling-factor", "r");
    if (!fp)
      return 1;

    int ratioInt = 1;
    if (fscanf(fp, "uint32 %i", &ratioInt) != 1)
      return 1;
    ratio = ratioInt;
  }
  if (pclose(fp) != 0)
    return 1;
  return ratio >= 1 ? ratio : 1;

#else
  Vector2i fbSize, size;
  glfwGetFramebufferSize(window, &fbSize[0], &fbSize[1]);
  glfwGetWindowSize(window, &size[0], &size[1]);
  return (float)fbSize[0] / (float)size[0];
#endif
}

void setup_window_params(WindowHandle hw_window, Screen* s)
{
  /* Propagate GLFW events to the appropriate Screen instance */
  static Screen* screen = s;
  glfwSetCursorPosCallback((GLFWwindow*)hw_window,
    [](GLFWwindow *, double x, double y) {
    if (!screen || !screen->canProcessEvents())
      return;
    screen->cursorPosCallbackEvent(x, y);
  });

  glfwSetMouseButtonCallback((GLFWwindow*)hw_window,
    [](GLFWwindow *, int button, int action, int modifiers) {
    if (!screen || !screen->canProcessEvents())
      return;
    screen->mouseButtonCallbackEvent(button, action, modifiers);
  });

  glfwSetKeyCallback((GLFWwindow*)hw_window,
    [](GLFWwindow *, int key, int scancode, int action, int mods) {
    if (!screen || !screen->canProcessEvents())
      return;
    screen->keyCallbackEvent(key, scancode, action, mods);
  });

  glfwSetCharCallback((GLFWwindow*)hw_window,
    [](GLFWwindow *, unsigned int codepoint) {
    if (!screen || !screen->canProcessEvents())
      return;
    screen->charCallbackEvent(codepoint);
  });

  glfwSetDropCallback((GLFWwindow*)hw_window,
    [](GLFWwindow *, int count, const char **filenames) {
    if (!screen || !screen->canProcessEvents())
      return;
    screen->dropCallbackEvent(count, filenames);
  });

  glfwSetScrollCallback((GLFWwindow*)hw_window,
    [](GLFWwindow *, double x, double y) {
    if (!screen || !screen->canProcessEvents())
      return;
    screen->scrollCallbackEvent(x, y);
  });

  /* React to framebuffer size events -- includes window
  size events and also catches things like dragging
  a window from a Retina-capable screen to a normal
  screen on Mac OS X */
  glfwSetFramebufferSizeCallback((GLFWwindow*)hw_window,
    [](GLFWwindow *, int width, int height) {
    if (!screen || !screen->canProcessEvents())
      return;
    screen->resizeCallbackEvent(width, height);
  });

  // notify when the screen has lost focus (e.g. application switch)
  glfwSetWindowFocusCallback((GLFWwindow*)hw_window,
    [](GLFWwindow *, int focused) {
    // focused: 0 when false, 1 when true
    if (!screen)
      screen->focusEvent(focused != 0);
  });

  screen->initClipboardSetHandler([hw_window](std::string text) { glfwSetClipboardString((GLFWwindow*)hw_window, text.c_str()); });
  screen->initClipboardGetHandler([hw_window]() { return glfwGetClipboardString((GLFWwindow*)hw_window); });

  screen->setPrepareFrameHandler([hw_window](Screen* screen) {
    glfwMakeContextCurrent((GLFWwindow*)hw_window);

    Vector2i fbsize, size;
    glfwGetFramebufferSize((GLFWwindow*)hw_window, &fbsize.x(), &fbsize.y());
    glfwGetWindowSize((GLFWwindow*)hw_window, &size.x(), &size.y());

    screen->setSizeFBSize(size, fbsize);

    glViewport(0, 0, fbsize.x(), fbsize.y());
    glBindSampler(0, 0);
  });

  screen->setResizeHwHandler([hw_window](Screen*, Vector2i& size, Vector2i& fbsize) {
    glfwGetFramebufferSize((GLFWwindow*)hw_window, &fbsize.x(), &fbsize.y());
    glfwGetWindowSize((GLFWwindow*)hw_window, &size.x(), &size.y());
  });

  screen->initHwCursorSetter([hw_window](intptr_t cursor) { glfwSetCursor((GLFWwindow*)hw_window, (GLFWcursor*)cursor); });
  screen->setPixelRatio(get_pixel_ratio((GLFWwindow*)hw_window));
}

void destroy_window(WindowHandle wnd)
{
  if (wnd)
    glfwDestroyWindow((GLFWwindow*)wnd);
}

void sample::remove_window_border(WindowHandle wnd)
{
#if defined(_WIN32)
  auto handle = glfwGetWin32Window((GLFWwindow*)wnd);
  long Style = GetWindowLong(handle, GWL_STYLE);
  Style &= ~WS_MAXIMIZEBOX; //this makes it still work when WS_MAXIMIZEBOX is actually already toggled off
  Style &= ~WS_CAPTION;
  Style &= ~WS_SYSMENU;
  Style &= ~WS_BORDER;
  Style &= ~WS_SYSMENU;
  Style &= ~WS_THICKFRAME;
  //Style &= ~WS_GROUP;
  SetWindowLong(handle, GWL_STYLE, Style);
#endif
}

WindowHandle sample::create_window(int w, int h, const std::string& caption, bool resizable, bool fullscreen)
{
  uint32_t glMajor = 3;
  unsigned int glMinor = 3;

  int colorBits = 8;
  int alphaBits = 8; 
  int depthBits = 24;
  int stencilBits = 8;
  int nSamples = 0;
  /* Request a forward compatible OpenGL glMajor.glMinor core profile context.
  Default value is an OpenGL 3.3 core profile context. */
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, glMajor);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, glMinor);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwWindowHint(GLFW_SAMPLES, nSamples);
  glfwWindowHint(GLFW_RED_BITS, colorBits);
  glfwWindowHint(GLFW_GREEN_BITS, colorBits);
  glfwWindowHint(GLFW_BLUE_BITS, colorBits);
  glfwWindowHint(GLFW_ALPHA_BITS, alphaBits);
  glfwWindowHint(GLFW_STENCIL_BITS, stencilBits);
  glfwWindowHint(GLFW_DEPTH_BITS, depthBits);
  glfwWindowHint(GLFW_VISIBLE, GL_TRUE);
  glfwWindowHint(GLFW_RESIZABLE, resizable ? GL_TRUE : GL_FALSE);

  WindowHandle hw_window = 0;
  if (fullscreen) {
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    hw_window = (WindowHandle)glfwCreateWindow(mode->width, mode->height, (caption + " (OpenGL)").c_str(), monitor, nullptr);
  }
  else {
    hw_window = (WindowHandle)glfwCreateWindow(w, h, (caption + " (OpenGL)").c_str(), nullptr, nullptr);
  }

  if (!hw_window)
    throw std::runtime_error("Could not create an OpenGL " +
                              std::to_string(glMajor) + "." +
                              std::to_string(glMinor) + " context!");

  glfwMakeContextCurrent((GLFWwindow*)hw_window);

#if defined(NANOGUI_GLAD)
  if (!gladInitialized) {
    gladInitialized = true;
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
      throw std::runtime_error("Could not initialize GLAD!");
    glGetError(); // pull and ignore unhandled errors like GL_INVALID_ENUM
  }
#endif

  int fbw, fbh;
  glfwGetFramebufferSize((GLFWwindow*)hw_window, &fbw, &fbh);
  glViewport(0, 0, fbw, fbh);
  //glClearColor(mBackground.r(), mBackground.g(), mBackground.b(), mBackground.a());
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glfwSwapInterval(0);
  glfwSwapBuffers((GLFWwindow*)hw_window);

#if defined(__APPLE__)
  /* Poll for events once before starting a potentially
  lengthy loading process. This is needed to be
  classified as "interactive" by other software such
  as iTerm2 */

  glfwPollEvents();
#endif

  glfwGetWindowSize((GLFWwindow*)hw_window, &w, &h);
  glfwGetFramebufferSize((GLFWwindow*)hw_window, &fbw, &fbh);

  float pixelRatio = get_pixel_ratio((GLFWwindow*)hw_window);

#if defined(_WIN32) || defined(__linux__)
  if (pixelRatio != 1 && !fullscreen)
    glfwSetWindowSize((GLFWwindow*)hw_window, w * pixelRatio, h * pixelRatio);
#endif

#if defined(NANOGUI_GLAD)
  if (!gladInitialized) {
    gladInitialized = true;
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
      throw std::runtime_error("Could not initialize GLAD!");
    glGetError(); // pull and ignore unhandled errors like GL_INVALID_ENUM
  }
#endif

  /* Detect framebuffer properties and set up compatible NanoVG context */
  GLint nStencilBits = 0;
  glGetFramebufferAttachmentParameteriv(GL_DRAW_FRAMEBUFFER,
    GL_STENCIL, GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &nStencilBits);
  glGetIntegerv(GL_SAMPLES, &nSamples);

  return hw_window;
}

}//end namespace sample

NAMESPACE_END(nanogui)

#endif