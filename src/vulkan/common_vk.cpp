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

void sample::clear_frame(Color background)
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

static float get_pixel_ratio(GLFWwindow *window) { return 1.f; }

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

  screen->initClipboardSetHandler([hw_window](std::string text) { glfwSetClipboardString((GLFWwindow*)hw_window, text.c_str()); });
  screen->initClipboardGetHandler([hw_window]() { return glfwGetClipboardString((GLFWwindow*)hw_window); });

  screen->setPrepareFrameHandler([hw_window](Screen* screen) {
    Vector2i fbsize, size;
    glfwGetFramebufferSize((GLFWwindow*)hw_window, &fbsize.x(), &fbsize.y());
    glfwGetWindowSize((GLFWwindow*)hw_window, &size.x(), &size.y());
    if (size.x() != screen->width() || size.y() != screen->height())
    {
      destroyFrameBuffers(internal::device, &internal::fb);
      internal::fb = createFrameBuffers(internal::device, internal::surface, internal::queue, size.x(), size.y(), 0);
      screen->setSizeFBSize(size, fbsize);
    }
  });

  screen->setResizeHwHandler([hw_window](Screen*, Vector2i& size, Vector2i& fbsize) {
    glfwGetFramebufferSize((GLFWwindow*)hw_window, &fbsize.x(), &fbsize.y());
    glfwGetWindowSize((GLFWwindow*)hw_window, &size.x(), &size.y());
  });

  screen->initHwCursorSetter([hw_window](intptr_t cursor) { glfwSetCursor((GLFWwindow*)hw_window, (GLFWcursor*)cursor); });
  screen->setPixelRatio(get_pixel_ratio((GLFWwindow*)hw_window));
}

void errorcb(int error, const char *desc) { printf("GLFW error %d: %s\n", error, desc); }

sample::WindowHandle sample::create_window(int w, int h, const std::string& caption, bool resizable, bool fullscreen)
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

  Vector2i size;
  glfwGetWindowSize((GLFWwindow*)hw_window, &size.x(), &size.y());

  vkGetDeviceQueue(internal::device->device, internal::device->graphicsQueueFamilyIndex, 0, &internal::queue);
  internal::fb = createFrameBuffers(internal::device, internal::surface, internal::queue, size.x(), size.y(), 0);
  internal::cmd_buffer = createCmdBuffer(internal::device->device, internal::device->commandPool);

  return hw_window;
}

void sample::destroy_window(WindowHandle wnd)
{
  if (wnd)
    glfwDestroyWindow((GLFWwindow*)wnd);
}

void __nanogui_destroy_cursor(intptr_t cursor) { glfwDestroyCursor((GLFWcursor*)cursor); }
intptr_t __nanogui_create_cursor(int shape) { return (intptr_t)glfwCreateStandardCursor(GLFW_ARROW_CURSOR + shape); }

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