#include <nanogui/common.h>

#if NANOGUI_VULKAN_BACKEND

#include <nanogui/screen.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstdio>
#include <iostream>
#include <map>

#define NANOVG_VULKAN_IMPLEMENTATION
#include <nanogui/nanovg_vk.h>
#include "vulkan_util.h"

#if defined(_WIN32)
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <windows.h>
#  define GLFW_EXPOSE_NATIVE_WIN32
#  include <GLFW/glfw3native.h>
#endif

NAMESPACE_BEGIN(nanogui)

namespace internal
{
  VkInstance instance;
  VkSurfaceKHR surface;
  VulkanDevice *device;
  FrameBuffers fb;
  VkDebugReportCallbackEXT debug_callback;
  VkQueue queue;
  VkCommandBuffer cmd_buffer;
}

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

bool isKeyboardActionRelease(int action) { return action == GLFW_RELEASE; }
bool isKeyboardModifierCtrl(int modifier) { return modifier & SYSTEM_COMMAND_MOD; }
bool isKeyboardModifierShift(int modifier) { return modifier & GLFW_MOD_SHIFT; }
bool isKeyboardActionPress(int action) { return action == GLFW_PRESS; }
bool isKeyboardActionRepeat(int action) { return action == GLFW_REPEAT; }
bool isKeyboardKeyEscape(int key) { return key == GLFW_KEY_ESCAPE; }

uint32_t key2fourcc(int key)
{
  switch (key) {
#define RET_KEYCODE(k,c) case k: return FOURCC(c);
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
  default: return FOURCC("UNKN");
  }
}

void sample::clear_frame(const Color& background)
{
  VkResult res;

  // Get the index of the next available swapchain image:
  res = vkAcquireNextImageKHR(internal::device->device, internal::fb.swap_chain, UINT64_MAX,
    internal::fb.present_complete_semaphore,
    0,
    &internal::fb.current_buffer);
  assert(res == VK_SUCCESS);

  const VkCommandBufferBeginInfo cmd_buf_info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
  res = vkBeginCommandBuffer(internal::cmd_buffer, &cmd_buf_info);
  assert(res == VK_SUCCESS);

  VkClearValue clear_values[2];
  clear_values[0].color.float32[0] = 0.3f;
  clear_values[0].color.float32[1] = 0.3f;
  clear_values[0].color.float32[2] = 0.32f;
  clear_values[0].color.float32[3] = 1.0f;
  clear_values[1].depthStencil.depth = 1.0f;
  clear_values[1].depthStencil.stencil = 0;

  VkRenderPassBeginInfo rp_begin;
  rp_begin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  rp_begin.pNext = NULL;
  rp_begin.renderPass = internal::fb.render_pass;
  rp_begin.framebuffer = internal::fb.framebuffers[internal::fb.current_buffer];
  rp_begin.renderArea.offset.x = 0;
  rp_begin.renderArea.offset.y = 0;
  rp_begin.renderArea.extent.width = internal::fb.buffer_size.width;
  rp_begin.renderArea.extent.height = internal::fb.buffer_size.height;
  rp_begin.clearValueCount = 2;
  rp_begin.pClearValues = clear_values;

  vkCmdBeginRenderPass(internal::cmd_buffer, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport;
  viewport.width = internal::fb.buffer_size.width;
  viewport.height = internal::fb.buffer_size.height;
  viewport.minDepth = (float)0.0f;
  viewport.maxDepth = (float)1.0f;
  viewport.x = rp_begin.renderArea.offset.x;
  viewport.y = rp_begin.renderArea.offset.y;
  vkCmdSetViewport(internal::cmd_buffer, 0, 1, &viewport);

  VkRect2D scissor = rp_begin.renderArea;
  vkCmdSetScissor(internal::cmd_buffer, 0, 1, &scissor);
}

void sample::present_frame(void* window) 
{
  VkResult res;

  vkCmdEndRenderPass(internal::cmd_buffer);

  vkEndCommandBuffer(internal::cmd_buffer);

  VkPipelineStageFlags pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

  VkSubmitInfo submit_info = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
  submit_info.pNext = NULL;
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = &internal::fb.present_complete_semaphore;
  submit_info.pWaitDstStageMask = &pipe_stage_flags;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &internal::cmd_buffer;
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = &internal::fb.render_complete_semaphore;

  /* Queue the command buffer for execution */
  res = vkQueueSubmit(internal::queue, 1, &submit_info, 0);
  assert(res == VK_SUCCESS);

  /* Now present the image in the window */

  VkPresentInfoKHR present = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
  present.pNext = NULL;
  present.swapchainCount = 1;
  present.pSwapchains = &internal::fb.swap_chain;
  present.pImageIndices = &internal::fb.current_buffer;
  present.waitSemaphoreCount = 1;
  present.pWaitSemaphores = &internal::fb.render_complete_semaphore;

  res = vkQueuePresentKHR(internal::queue, &present);
  assert(res == VK_SUCCESS);

  res = vkQueueWaitIdle(internal::queue);
}

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
void sample::setup_window_params(WindowHandle hw_window, Screen* s)
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

  glfwSetWindowCloseCallback((GLFWwindow*)hw_window,
    [](GLFWwindow*) {
      sample::stop_frame_loop();
  });


  screen->initClipboardSetHandler([hw_window](std::string text) { glfwSetClipboardString((GLFWwindow*)hw_window, text.c_str()); });
  screen->initClipboardGetHandler([hw_window]() { return glfwGetClipboardString((GLFWwindow*)hw_window); });

  //FIXME: make this handlers properly

  screen->setPrepareFrameHandler([hw_window](Screen* screen) {
   // Vector2i fbsize, size;
   // glfwGetFramebufferSize((GLFWwindow*)hw_window, &fbsize.x(), &fbsize.y());
   // glfwGetWindowSize((GLFWwindow*)hw_window, &size.x(), &size.y());
   // if (size.x() != screen->width() || size.y() != screen->height())
   // {
   //   destroyFrameBuffers(internal::device, &internal::fb);
   //   internal::fb = createFrameBuffers(internal::device, internal::surface, internal::queue, size.x(), size.y(), 0);
   //   screen->setSizeFBSize(size, fbsize);
   // }
  });

  screen->setResizeHwHandler([hw_window](Screen*, Vector2i& size, Vector2i& fbsize) {
    glfwGetFramebufferSize((GLFWwindow*)hw_window, &fbsize.x(), &fbsize.y());
    glfwGetWindowSize((GLFWwindow*)hw_window, &size.x(), &size.y());
    if (size.x() != screen->width() || size.y() != screen->height())
    {
        destroyFrameBuffers(internal::device, &internal::fb);
        internal::fb = createFrameBuffers(internal::device, internal::surface, internal::queue, size.x(), size.y(), 0);
        screen->setSizeFBSize(size, fbsize);
    }
  });

  screen->initHwCursorSetter([hw_window](intptr_t cursor) { glfwSetCursor((GLFWwindow*)hw_window, (GLFWcursor*)cursor); });
  screen->setPixelRatio(get_pixel_ratio((GLFWwindow*)hw_window));
}

void errorcb(int error, const char *desc) { printf("GLFW error %d: %s\n", error, desc); }

void sample::set_window_topmost(WindowHandle w, bool top, bool always)
{
#ifdef _WIN32
  HWND realw = glfwGetWin32Window((GLFWwindow*)w);
  
  ShowWindow(realw, TRUE);
  SetForegroundWindow(realw);
  RECT rect;

  // get the current window size and position
  GetWindowRect(realw, &rect);

  // now change the size, position, and Z order
  // of the window.
  ::SetWindowPos(realw,       // handle to window
    (top && always) ? HWND_TOPMOST : (top ? HWND_TOP : HWND_BOTTOM),  // placement-order handle
    rect.left,     // horizontal position
    rect.top,      // vertical position
    rect.right - rect.left,  // width
    rect.bottom - rect.top, // height
    SWP_SHOWWINDOW);
#else

#endif
}

Vector2i sample::get_window_pos(WindowHandle w) 
{
  Vector2i pos;
  glfwGetWindowPos((GLFWwindow*)w, &pos.x(), &pos.y());
  return pos;
}

Vector2i sample::get_cursor_pos()
{
#if WIN32
  POINT p;
  if (GetCursorPos(&p))
    return{ p.x, p.y };
#else
  return{ 0, 0 };
#endif
}

void sample::set_window_pos(WindowHandle w, const Vector2i& pos)
{
  glfwSetWindowPos((GLFWwindow*)w, pos.x(), pos.y());
}

sample::WindowHandle sample::create_window(int& w, int& h, const std::string& caption, bool resizable, bool fullscreen, bool header)
{
  WindowHandle hw_window = nullptr;
  glfwSetErrorCallback(errorcb);
  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  if (fullscreen) {
    GLFWmonitor *monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(monitor);
    hw_window = glfwCreateWindow(mode->width, mode->height, (caption + "(Vulkan)").c_str(), monitor, nullptr);
  }
  else {
    hw_window = glfwCreateWindow(w, h, (caption + "(Vulkan)").c_str(), nullptr, nullptr);
  }

  if (!header)
  {
#if _WIN32
    auto handle = glfwGetWin32Window((GLFWwindow*)hw_window);
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

#ifdef NDEBUG
  internal::instance = createVkInstance(false);
#else
  internal::instance = createVkInstance(true);
  internal::debug_callback = CreateDebugReport(internal::instance);
#endif

  VkResult res = glfwCreateWindowSurface(internal::instance, (GLFWwindow*)hw_window, 0, &internal::surface);
  if (VK_SUCCESS != res) {
    printf("glfwCreateWindowSurface failed\n");
    exit(-1);
  }

  VkPhysicalDevice gpu;
  uint32_t gpu_count = 1;
  res = vkEnumeratePhysicalDevices(internal::instance, &gpu_count, &gpu);
  if (res != VK_SUCCESS && res != VK_INCOMPLETE) {
    printf("vkEnumeratePhysicalDevices failed %d \n", res);
    exit(-1);
  }
  internal::device = createVulkanDevice(gpu);

  float pixelRatio = get_pixel_ratio((GLFWwindow*)hw_window);
#if defined(_WIN32) || defined(__linux__)
  if (pixelRatio != 1 && !fullscreen)
    glfwSetWindowSize((GLFWwindow*)hw_window, w * pixelRatio, h * pixelRatio);
#endif

  Vector2i size;
  glfwGetWindowSize((GLFWwindow*)hw_window, &size.x(), &size.y());

  vkGetDeviceQueue(internal::device->device, internal::device->graphicsQueueFamilyIndex, 0, &internal::queue);
  internal::fb = createFrameBuffers(internal::device, internal::surface, internal::queue, size.x(), size.y(), 0);
  internal::cmd_buffer = createCmdBuffer(internal::device->device, internal::device->commandPool);

  if (!header)
  {
#if _WIN32
    auto handle = glfwGetWin32Window((GLFWwindow*)hw_window);
    RECT crect; GetClientRect(handle, &crect);

    w = (crect.right - crect.left) / pixelRatio;
    h = (crect.bottom - crect.top) / pixelRatio;
#endif
  }

  return hw_window;
}

void sample::destroy_window(WindowHandle wnd)
{
  if (wnd)
    glfwDestroyWindow((GLFWwindow*)wnd);
}

void __nanogui_destroy_cursor(intptr_t cursor) { glfwDestroyCursor((GLFWcursor*)cursor); }
intptr_t __nanogui_create_cursor(int shape) { return (intptr_t)glfwCreateStandardCursor(GLFW_ARROW_CURSOR + shape); }

void sample::frame_loop(std::function<void()> &f) {
  while (is_main_loop_active())
  { 
    f();
  }; 
}

void sample::create_context()
{
  VKNVGCreateInfo create_info = { 0 };
  create_info.device = internal::device->device;
  create_info.gpu = internal::device->gpu;
  create_info.renderpass = internal::fb.render_pass;
  create_info.cmdBuffer = internal::cmd_buffer;
  create_info.graphicsQueue = internal::queue;
  create_info.cmdPool = internal::device->commandPool;

  int flags = 0;
#if !defined(NDEBUG)
  flags |= NVG_DEBUG;
#endif

  __nanogui_context = nvgCreateVk(create_info, NVG_ANTIALIAS | NVG_STENCIL_STROKES);
  if (__nanogui_context == nullptr)
    throw std::runtime_error("Could not initialize NanoVG!");
}

void destroy_context()
{
  if (__nanogui_context)
  {
    nvgDeleteVk(__nanogui_context);

    destroyFrameBuffers(internal::device, &internal::fb);
    destroyVulkanDevice(internal::device);

#ifndef NDEBUG
    _vkDestroyDebugReportCallbackEXT(internal::instance, internal::debug_callback, 0);
#endif
    vkDestroyInstance(internal::instance, NULL);
  }
}

NAMESPACE_END(nanogui)

#endif //NANOGUI_VULKAN_BACKEND