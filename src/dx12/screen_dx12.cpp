/*

Port to dx12 by megai2

*/
#include <nanogui/screen.h>
#include <nanogui/theme.h>
#include <nanogui/window.h>
#include <nanogui/popup.h>
#include <nanovg.h>
#include <map>
#include <iostream>

#include "common_dx12.h"
#include "dx12_subset.h"

#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  undef APIENTRY

#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif

#define NANOVG_D3D12_IMPLEMENTATION
#define INITGUID
#include <nanogui/nanovg_d3d12.h>
#include <windows.h>
#include <windowsx.h>

#include "nanovg.h"

NAMESPACE_BEGIN(nanogui)

HINSTANCE hInstance;
HWND hWndmain;
dx12_subset pDX12;
bool dxPremultMode = false;
const char* pszWindowClass = "WindowClass";

int dx12mouseposx = 0;
int dx12mouseposy = 0;
int xWin = 0;
int yWin = 0;


std::map<HWND , Screen *> __nanogui_screens;
void appForEachScreen(std::function<void(Screen*)> f)
{
  for (auto kv : __nanogui_screens)
    f(kv.second);
}

// Setup the device and the rendering targets
BOOL InitializeDX(void* window, unsigned int x, unsigned int y)
{
  hWndmain = (HWND)window;
  pDX12.init((HWND)window, x, y);
  resizeWindow(x, y);

  return true;
}

// Frees everything
void UnInitializeDX()
{
  pDX12.deinit();
}

void clearDxContext()
{
  pDX12.fr_start();
}

void swapDxContextxBuffers()
{
  pDX12.fr_end();
}

HRESULT resizeWindow(unsigned int x, unsigned int y)
{
  HRESULT hr = S_OK;
  pDX12.set_size(x, y);
  return hr;
}


/* Calculate pixel ratio for hi-dpi devices. */
static float get_pixel_ratio(HWND window) {
    HMONITOR monitor = nullptr;
    static HMONITOR (WINAPI *MonitorFromWindow_)(HWND, DWORD);
    static bool MonitorFromWindow_tried = false;
    if (!MonitorFromWindow_tried) {
            auto user32 = LoadLibrary(TEXT("user32"));
            if (user32)
                MonitorFromWindow_ = (decltype(MonitorFromWindow_)) GetProcAddress(user32, "MonitorFromWindow");
            MonitorFromWindow_tried = true;
    }
    if (MonitorFromWindow_)
        monitor = MonitorFromWindow_(hWndmain, 2);
    /* The following function only exists on Windows 8.1+, but we don't want to make that a dependency */
    static HRESULT (WINAPI *GetDpiForMonitor_)(HMONITOR, UINT, UINT*, UINT*) = nullptr;
    static bool GetDpiForMonitor_tried = false;

    if (!GetDpiForMonitor_tried) {
        auto shcore = LoadLibrary(TEXT("shcore"));
        if (shcore)
            GetDpiForMonitor_ = (decltype(GetDpiForMonitor_)) GetProcAddress(shcore, "GetDpiForMonitor");
        GetDpiForMonitor_tried = true;
    }

    if (GetDpiForMonitor_ && monitor) {
        uint32_t dpiX, dpiY;
        if (GetDpiForMonitor_(monitor, 0 /* effective DPI */, &dpiX, &dpiY) == S_OK)
            return dpiX / 96.0;
    }
    return 1.f;
}

Screen::Screen()
    : Widget(nullptr), mHwWindow(nullptr), mNVGContext(nullptr),
      mCursor(Cursor::Arrow), mBackground(0.3f, 0.3f, 0.32f, 1.f),
      mShutdownOnDestruct(false), mFullscreen(false) {
    memset(mCursors, 0, sizeof(GLFWcursor *) * (int) Cursor::CursorCount);
}


Screen::Screen(const Vector2i &size, const std::string &caption, bool resizable,
               bool fullscreen, int colorBits, int alphaBits, int depthBits,
               int stencilBits, int nSamples,
               unsigned int glMajor, unsigned int glMinor)
    : Widget(nullptr), mHwWindow(nullptr), mNVGContext(nullptr),
      mCursor(Cursor::Arrow), mBackground(0.3f, 0.3f, 0.32f, 1.f), mCaption(caption),
      mShutdownOnDestruct(false), mFullscreen(fullscreen)
{
    //memset(mCursors, 0, sizeof(GLFWcursor *) * (int) Cursor::CursorCount);
    RECT rcWin;

    if (fullscreen) {
        //???
    } else {
        rcWin.left = 0;
        rcWin.right = size.x();
        rcWin.top = 0;
        rcWin.bottom = size.y();

        AdjustWindowRectEx(&rcWin, WS_OVERLAPPEDWINDOW, FALSE, 0);

        rcWin.right += -rcWin.left;
        rcWin.bottom += -rcWin.top;

        mHwWindow = (void*)CreateWindowEx(0, pszWindowClass, (caption + " (DX12)").c_str(), WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, (int)rcWin.right, (int)rcWin.bottom, NULL, NULL, hInstance, NULL);
    }

    if (!mHwWindow)
        throw std::runtime_error("Could not create an Window ");

    if (FAILED(InitializeDX(mHwWindow, rcWin.right, rcWin.bottom)))
        throw std::runtime_error("Could not create an dx12 context ");


    /* Propagate GLFW events to the appropriate Screen instance */
    dx12SetCursorPosCallback([](void *w, double x, double y) {
            auto it = __nanogui_screens.find((HWND)w);
            if (it == __nanogui_screens.end())
                return;
            dx12mouseposx = x;
            dx12mouseposy = y;
            Screen *s = it->second;
            if (!s->mProcessEvents)
                return;
            s->cursorPosCallbackEvent(x, y);
        }
    );

    dx12SetMouseButtonCallback([](void *w, int button, int action, int modifiers) {
            auto it = __nanogui_screens.find((HWND)w);
            if (it == __nanogui_screens.end())
                return;
            Screen *s = it->second;
            if (!s->mProcessEvents)
                return;
            s->mouseButtonCallbackEvent(button, action, modifiers);
        }
    );

    dx12SetKeyCallback([](void *w, int key, int scancode, int action, int mods) {
            auto it = __nanogui_screens.find((HWND)w);
            if (it == __nanogui_screens.end())
                return;
            Screen *s = it->second;
            if (!s->mProcessEvents)
                return;
            s->keyCallbackEvent(key, scancode, action, mods);
        }
    );

    dx12SetCharCallback([](void *w, unsigned int codepoint) {
            auto it = __nanogui_screens.find((HWND)w);
            if (it == __nanogui_screens.end())
                return;
            Screen *s = it->second;
            if (!s->mProcessEvents)
                return;
            s->charCallbackEvent(codepoint);
        }
    );

    dx12SetDropCallback([](void *w, int count, const char **filenames) {
            auto it = __nanogui_screens.find((HWND)w);
            if (it == __nanogui_screens.end())
                return;
            Screen *s = it->second;
            if (!s->mProcessEvents)
                return;
            s->dropCallbackEvent(count, filenames);
        }
    );

    dx12SetScrollCallback([](void *w, double x, double y) {
            auto it = __nanogui_screens.find((HWND)w);
            if (it == __nanogui_screens.end())
                return;
            Screen *s = it->second;
            if (!s->mProcessEvents)
                return;
            s->scrollCallbackEvent(x, y);
        }
    );

    dx12SetFramebufferSizeCallback([](void *w, int width, int height) {
            auto it = __nanogui_screens.find((HWND)w);
            if (it == __nanogui_screens.end())
                return;
            Screen *s = it->second;

            if (!s->mProcessEvents)
                return;

            s->resizeCallbackEvent(width, height);
        }
    );

    // notify when the screen has lost focus (e.g. application switch)
    dx12SetWindowFocusCallback([](void *w, int focused) {
            auto it = __nanogui_screens.find((HWND)w);
            if (it == __nanogui_screens.end())
                return;

            Screen *s = it->second;
            // focused: 0 when false, 1 when true
            s->focusEvent(focused != 0);
        }
    );

    initialize(mHwWindow, true);
}

LPSTR translateCursorShape(int shape)
{
    switch (shape)
    {
        case 0: return IDC_ARROW;
        case 1: return IDC_IBEAM;
        case 2: return IDC_CROSS;
        case 3: return IDC_HAND;
        case 4: return IDC_SIZEWE;
        case 5: return IDC_SIZENS;
    }

    return nullptr;
}

intptr_t Screen::createStandardCursor(int shape)
{
  return (intptr_t)CopyCursor(LoadCursorA(NULL, translateCursorShape(shape)));
}

void setWindowSize(HWND window, int w, int h)
{
  RECT rect = { 0, 0, w, h };
  AdjustWindowRectEx(&rect, WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
    FALSE, WS_EX_APPWINDOW);
  SetWindowPos((HWND)window, HWND_TOP,
    0, 0, rect.right - rect.left, rect.bottom - rect.top,
    SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER);
}

void Screen::initialize(void *window, bool shutdownOnDestruct) {

    mHwWindow = window;
    mShutdownOnDestruct = shutdownOnDestruct;

    mPixelRatio = get_pixel_ratio((HWND)window);

    if (mPixelRatio != 1 && !mFullscreen)
    {
        int ww = mSize.x() * mPixelRatio;
        int hh = mSize.y() * mPixelRatio;
        setWindowSize((HWND)window, ww, hh);
    }

    int flags = 0;
   // if (nStencilBits >= 8)
   //    flags |= NVG_STENCIL_STROKES;
   // if (nSamples <= 1)
   //    flags |= NVG_ANTIALIAS;
#if !defined(NDEBUG)
    //flags |= NVG_DEBUG;
#endif

    mNVGContext = nvgCreateD3D12(&pDX12, NVG_ANTIALIAS | NVG_STENCIL_STROKES);
    if (mNVGContext == nullptr)
        throw std::runtime_error("Could not initialize NanoVG!");

    __nanogui_screens[(HWND)mHwWindow] = this;
    _setupStartParams();
}

void Screen::setVisible(bool visible) {
  if (mVisible != visible) {
    mVisible = visible;
  }

  if (visible)
    ShowWindow((HWND)mHwWindow, SW_SHOW);
  else
    ShowWindow((HWND)mHwWindow, SW_HIDE);
}

Screen::~Screen() {
    __nanogui_screens.erase((HWND)mHwWindow);
    for (int i=0; i < (int) Cursor::CursorCount; ++i) {
        if (mCursors[i])
            DestroyIcon((HICON) mCursors[i]);
    }
    if (mNVGContext)
    nvgDeleteD3D12(mNVGContext);

  nanogui::UnInitializeDX();

    //if (mHwWindow && mShutdownOnDestruct)
    //    glfwDestroyWindow((GLFWwindow*)mHwWindow);
}

void Screen::setCaption(const std::string &caption) {
    if (caption != mCaption) {
        SetWindowTextA((HWND)mHwWindow, caption.c_str());
        mCaption = caption;
    }
}

void Screen::setSize(const Vector2i &size) {
    Widget::setSize(size);

    setWindowSize((HWND)mHwWindow, size.x() * mPixelRatio, size.y() * mPixelRatio);
}

void Screen::drawAll() {
  if (!pDX12.isRunning)
    return;

    clearDxContext();

    drawContents();
    drawWidgets();

    swapDxContextxBuffers();
}

void Screen::setClipboardString(const std::string & text)
{
  dx12SetClipboardString((HWND)mHwWindow, text.c_str());
}

std::string Screen::getClipboardString()
{
  return dx12GetClipboardString((HWND)mHwWindow);
}

void Screen::_drawWidgetsBefore()
{
  dx12GetWindowSize((HWND)mHwWindow, &mSize[0], &mSize[1]);

#if defined(_WIN32) || defined(__linux__)
    mSize = (mSize.cast<float>() / mPixelRatio).cast<int>();
    mFBSize = (mSize.cast<float>() * mPixelRatio).cast<int>();
#else
    /* Recompute pixel ratio on OSX */
    if (mSize[0])
        mPixelRatio = (float) mFBSize[0] / (float) mSize[0];
#endif
}

void Screen::_internalSetCursor(int cursor)
{
  dx12SetCursorImage((HWND)mHwWindow, (HCURSOR)mCursors[cursor]);
}

bool Screen::resizeCallbackEvent(int, int) {
    Vector2i fbSize, size;

    dx12GetWindowSize((HWND)mHwWindow, &fbSize[0], &fbSize[1]);
    dx12GetWindowSize((HWND)mHwWindow, &size[0], &size[1]);

#if defined(_WIN32) || defined(__linux__)
    size = (size.cast<float>() / mPixelRatio).cast<int>();
#endif

    if (fbSize == Vector2i(0, 0) || size == Vector2i(0, 0))
        return false;

    mFBSize = fbSize; mSize = size;
    mLastInteraction = getTimeFromStart();

    try {
        return resizeEvent(mSize);
    } catch (const std::exception &e) {
        std::cerr << "Caught exception in event handler: " << e.what()
                  << std::endl;
        return false;
    }
}

NAMESPACE_END(nanogui)
