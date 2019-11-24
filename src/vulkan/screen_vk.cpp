/*
    src/screen_vk.cpp -- Top-level widget and interface between NanoGUI and vulkan

    A significant redesign this code by dalerank.
    Based on https://github.com/danilw/nanovg port

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/screen.h>
#include <nanogui/theme.h>
#include <nanogui/window.h>
#include <nanogui/popup.h>
#include <map>
#include <iostream>

#if defined(_WIN32)
#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  undef APIENTRY

#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <windows.h>
#endif

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <nanovg.h>
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

std::map<GLFWwindow *, Screen *> __nanogui_screens;
void appForEachScreen(std::function<void(Screen*)> f)
{
  for (auto kv : __nanogui_screens)
    f(kv.second);
}

/* Calculate pixel ratio for hi-dpi devices. */
static float get_pixel_ratio(GLFWwindow *window) {
   return 1.f;
}

void Screen::setVisible(bool visible) {
    if (mVisible != visible) {
        mVisible = visible;

        if (visible)
            glfwShowWindow((GLFWwindow*)mHwWindow);
        else
            glfwHideWindow((GLFWwindow*)mHwWindow);
    }
}

Screen::Screen()
    : Widget(nullptr), mHwWindow(nullptr), mNVGContext(nullptr),
      mCursor(Cursor::Arrow), mBackground(0.3f, 0.3f, 0.32f, 1.f),
      mShutdownOnDestruct(false), mFullscreen(false) {
    memset(mCursors, 0, sizeof(GLFWcursor *) * (int) Cursor::CursorCount);
}

void errorcb(int error, const char *desc) {
  printf("GLFW error %d: %s\n", error, desc);
}

void prepareFrame(VkDevice device, VkCommandBuffer cmd_buffer, FrameBuffers *fb) {
  VkResult res;

  // Get the index of the next available swapchain image:
  res = vkAcquireNextImageKHR(device, fb->swap_chain, UINT64_MAX,
    fb->present_complete_semaphore,
    0,
    &fb->current_buffer);
  assert(res == VK_SUCCESS);

  const VkCommandBufferBeginInfo cmd_buf_info = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
  res = vkBeginCommandBuffer(cmd_buffer, &cmd_buf_info);
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
  rp_begin.renderPass = fb->render_pass;
  rp_begin.framebuffer = fb->framebuffers[fb->current_buffer];
  rp_begin.renderArea.offset.x = 0;
  rp_begin.renderArea.offset.y = 0;
  rp_begin.renderArea.extent.width = fb->buffer_size.width;
  rp_begin.renderArea.extent.height = fb->buffer_size.height;
  rp_begin.clearValueCount = 2;
  rp_begin.pClearValues = clear_values;

  vkCmdBeginRenderPass(cmd_buffer, &rp_begin, VK_SUBPASS_CONTENTS_INLINE);

  VkViewport viewport;
  viewport.width = fb->buffer_size.width;
  viewport.height = fb->buffer_size.height;
  viewport.minDepth = (float)0.0f;
  viewport.maxDepth = (float)1.0f;
  viewport.x = rp_begin.renderArea.offset.x;
  viewport.y = rp_begin.renderArea.offset.y;
  vkCmdSetViewport(cmd_buffer, 0, 1, &viewport);

  VkRect2D scissor = rp_begin.renderArea;
  vkCmdSetScissor(cmd_buffer, 0, 1, &scissor);
}
void submitFrame(VkDevice device, VkQueue queue, VkCommandBuffer cmd_buffer, FrameBuffers *fb) {
  VkResult res;

  vkCmdEndRenderPass(cmd_buffer);

  vkEndCommandBuffer(cmd_buffer);

  VkPipelineStageFlags pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

  VkSubmitInfo submit_info = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
  submit_info.pNext = NULL;
  submit_info.waitSemaphoreCount = 1;
  submit_info.pWaitSemaphores = &fb->present_complete_semaphore;
  submit_info.pWaitDstStageMask = &pipe_stage_flags;
  submit_info.commandBufferCount = 1;
  submit_info.pCommandBuffers = &cmd_buffer;
  submit_info.signalSemaphoreCount = 1;
  submit_info.pSignalSemaphores = &fb->render_complete_semaphore;

  /* Queue the command buffer for execution */
  res = vkQueueSubmit(queue, 1, &submit_info, 0);
  assert(res == VK_SUCCESS);

  /* Now present the image in the window */

  VkPresentInfoKHR present = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
  present.pNext = NULL;
  present.swapchainCount = 1;
  present.pSwapchains = &fb->swap_chain;
  present.pImageIndices = &fb->current_buffer;
  present.waitSemaphoreCount = 1;
  present.pWaitSemaphores = &fb->render_complete_semaphore;

  res = vkQueuePresentKHR(queue, &present);
  assert(res == VK_SUCCESS);

  res = vkQueueWaitIdle(queue);
}

void Screen::setCaption(const std::string &caption) {
    if (caption != mCaption) {
        glfwSetWindowTitle((GLFWwindow*)mHwWindow, caption.c_str());
        mCaption = caption;
    }
}

void Screen::setSize(const Vector2i &size) {
    Widget::setSize(size);

#if defined(_WIN32) || defined(__linux__)
    glfwSetWindowSize((GLFWwindow*)mHwWindow, size.x() * mPixelRatio, size.y() * mPixelRatio);
#else
    glfwSetWindowSize((GLFWwindow*)mHwWindow, size.x(), size.y());
#endif
}


Screen::Screen(const Vector2i &size, const std::string &caption, bool resizable,
               bool fullscreen, int colorBits, int alphaBits, int depthBits,
               int stencilBits, int nSamples,
               unsigned int glMajor, unsigned int glMinor)
    : Widget(nullptr), mHwWindow(nullptr), mNVGContext(nullptr),
      mCursor(Cursor::Arrow), mBackground(0.3f, 0.3f, 0.32f, 1.f), mCaption(caption),
      mShutdownOnDestruct(false), mFullscreen(fullscreen)
{
    memset(mCursors, 0, sizeof(GLFWcursor *) * (int) Cursor::CursorCount);

    glfwSetErrorCallback(errorcb);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    if (fullscreen) {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        mHwWindow = glfwCreateWindow(mode->width, mode->height,
                                       caption.c_str(), monitor, nullptr);
    } else {
        mHwWindow = glfwCreateWindow(size.x(), size.y(),
                                       caption.c_str(), nullptr, nullptr);
    }

    if (!mHwWindow)
        throw std::runtime_error("Could not create Vulkan context");

#ifdef NDEBUG
  internal::instance = createVkInstance(false);
#else
  internal::instance = createVkInstance(true);
  internal::debug_callback = CreateDebugReport(internal::instance);
#endif

  VkResult res = glfwCreateWindowSurface(internal::instance, (GLFWwindow*)mHwWindow, 0, &internal::surface);
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

#if defined(__APPLE__)
    /* Poll for events once before starting a potentially
       lengthy loading process. This is needed to be
       classified as "interactive" by other software such
       as iTerm2 */

    glfwPollEvents();
#endif

    /* Propagate GLFW events to the appropriate Screen instance */
    glfwSetCursorPosCallback((GLFWwindow*)mHwWindow,
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

    glfwSetMouseButtonCallback((GLFWwindow*)mHwWindow,
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

    glfwSetKeyCallback((GLFWwindow*)mHwWindow,
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

    glfwSetCharCallback((GLFWwindow*)mHwWindow,
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

    glfwSetDropCallback((GLFWwindow*)mHwWindow,
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

    glfwSetScrollCallback((GLFWwindow*)mHwWindow,
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
    glfwSetFramebufferSizeCallback((GLFWwindow*)mHwWindow,
        [](GLFWwindow *w, int width, int height) {
            auto it = __nanogui_screens.find(w);
            if (it == __nanogui_screens.end())
                return;
            Screen *s = it->second;

            if (!s->mProcessEvents)
                return;

            s->resizeCallbackEvent(width, height);
        }
    );

    // notify when the screen has lost focus (e.g. application switch)
    glfwSetWindowFocusCallback((GLFWwindow*)mHwWindow,
        [](GLFWwindow *w, int focused) {
            auto it = __nanogui_screens.find(w);
            if (it == __nanogui_screens.end())
                return;

            Screen *s = it->second;
            // focused: 0 when false, 1 when true
            s->focusEvent(focused != 0);
        }
    );

    initialize((GLFWwindow*)mHwWindow, true);
}

void Screen::initialize(void *window, bool shutdownGLFWOnDestruct) {
    mHwWindow = window;
    mShutdownOnDestruct = shutdownGLFWOnDestruct;
    glfwGetWindowSize((GLFWwindow*)mHwWindow, &mSize[0], &mSize[1]);

    mPixelRatio = get_pixel_ratio((GLFWwindow*)window);

#if defined(_WIN32) || defined(__linux__)
    if (mPixelRatio != 1 && !mFullscreen)
        glfwSetWindowSize((GLFWwindow*)window, mSize.x() * mPixelRatio, mSize.y() * mPixelRatio);
#endif

    vkGetDeviceQueue(internal::device->device, internal::device->graphicsQueueFamilyIndex, 0, &internal::queue);
    internal::fb = createFrameBuffers(internal::device, internal::surface, internal::queue, mSize.x(), mSize.y(), 0);

    internal::cmd_buffer = createCmdBuffer(internal::device->device, internal::device->commandPool);
    VKNVGCreateInfo create_info = {0};
    create_info.device = internal::device->device;
    create_info.gpu = internal::device->gpu;
    create_info.renderpass = internal::fb.render_pass;
    create_info.cmdBuffer = internal::cmd_buffer;

    int flags = 0;
#if !defined(NDEBUG)
    flags |= NVG_DEBUG;
#endif

    mNVGContext = nvgCreateVk(create_info, NVG_ANTIALIAS | NVG_STENCIL_STROKES);
    if (mNVGContext == nullptr)
        throw std::runtime_error("Could not initialize NanoVG!");

    mVisible = glfwGetWindowAttrib((GLFWwindow*)window, GLFW_VISIBLE) != 0;
    setTheme(new Theme(mNVGContext));
    mMousePos = Vector2i::Zero();
    mMouseState = mModifiers = 0;
    mDragActive = false;
    mLastInteraction = glfwGetTime();
    mProcessEvents = true;
    __nanogui_screens[(GLFWwindow*)mHwWindow] = this;

    for (int i=0; i < (int) Cursor::CursorCount; ++i)
        mCursors[i] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR + i);

    /// Fixes retina display-related font rendering issue (#185)
    nvgBeginFrame(mNVGContext, mSize[0], mSize[1], mPixelRatio);
    nvgEndFrame(mNVGContext);
}

Screen::~Screen() 
{
    __nanogui_screens.erase((GLFWwindow*)mHwWindow);
    for (int i=0; i < (int) Cursor::CursorCount; ++i) {
        if (mCursors[i])
            glfwDestroyCursor((GLFWcursor*)mCursors[i]);
    }
    
    if (mNVGContext)
    {
        nvgDeleteVk(mNVGContext);

        destroyFrameBuffers(internal::device, &internal::fb);
        destroyVulkanDevice(internal::device);

#ifndef NDEBUG
        _vkDestroyDebugReportCallbackEXT(internal::instance, internal::debug_callback, 0);
#endif
        vkDestroyInstance(internal::instance, NULL);
    }

    if (mHwWindow && mShutdownOnDestruct)
        glfwDestroyWindow((GLFWwindow*)mHwWindow);
}

void Screen::setClipboardString(const std::string & text)
{
  glfwSetClipboardString((GLFWwindow*)mHwWindow, text.c_str());

}

std::string Screen::getClipboardString()
{
  return glfwGetClipboardString((GLFWwindow*)mHwWindow);
}

void Screen::_drawWidgetsBefore()
{
    int cwinWidth, cwinHeight;
    glfwGetWindowSize((GLFWwindow*)mHwWindow, &cwinWidth, &cwinHeight);
    if (mSize.x() != cwinWidth || mSize.y() != cwinHeight)
    {
      mSize = { cwinWidth, cwinHeight };
      destroyFrameBuffers(internal::device, &internal::fb);
      internal::fb = createFrameBuffers(internal::device, internal::surface, internal::queue, mSize.x(), mSize.y(), 0);
    }
}

void Screen::drawAll() 
{
    prepareFrame(internal::device->device, internal::cmd_buffer, &internal::fb);

    drawContents();
    drawWidgets();

    submitFrame(internal::device->device, internal::queue, internal::cmd_buffer, &internal::fb);
}

void Screen::_internalSetCursor(int cursor)
{
    glfwSetCursor((GLFWwindow*)mHwWindow, (GLFWcursor*)mCursors[(int) cursor]);
}


bool Screen::resizeCallbackEvent(int, int) {
    Vector2i fbSize, size;
    glfwGetFramebufferSize((GLFWwindow*)mHwWindow, &fbSize[0], &fbSize[1]);
    glfwGetWindowSize((GLFWwindow*)mHwWindow, &size[0], &size[1]);

#if defined(_WIN32) || defined(__linux__)
    size = (size.cast<float>() / mPixelRatio).cast<int>();
#endif

    if (fbSize == Vector2i(0, 0) || size == Vector2i(0, 0))
        return false;

    mFBSize = fbSize; mSize = size;
    mLastInteraction = glfwGetTime();

    try {
        return resizeEvent(mSize);
    } catch (const std::exception &e) {
        std::cerr << "Caught exception in event handler: " << e.what()
                  << std::endl;
        return false;
    }
}

NAMESPACE_END(nanogui)
