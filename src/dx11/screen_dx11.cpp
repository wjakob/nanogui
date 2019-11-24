#include <nanogui/screen.h>
#include <nanogui/theme.h>
#include <nanogui/window.h>
#include <nanogui/popup.h>
#include <nanovg.h>
#include <map>
#include <iostream>

#include "common_dx11.h"

#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  undef APIENTRY

#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif

#define NANOVG_D3D11_IMPLEMENTATION
#define INITGUID
#include <nanogui/nanovg_d3d11.h>
#include <windows.h>
#include <windowsx.h>

#include "nanovg.h"

NAMESPACE_BEGIN(nanogui)

HINSTANCE hInstance;
HWND hWndmain;
ID3D11Device* pDevice = nullptr;
ID3D11DeviceContext* pDeviceContext;
IDXGISwapChain* pSwapChain;
DXGI_SWAP_CHAIN_DESC swapDesc;
ID3D11RenderTargetView* pRenderTargetView;
ID3D11Texture2D* pDepthStencil;
ID3D11DepthStencilView* pDepthStencilView;
D3D_FEATURE_LEVEL FeatureLevel;
bool dxPremultMode = false;
const char* pszWindowClass = "WindowClass";

int dx11mouseposx = 0;
int dx11mouseposy = 0;
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
  HRESULT hr = S_OK;
  IDXGIDevice *pDXGIDevice = NULL;
  IDXGIAdapter *pAdapter = NULL;
  IDXGIFactory *pDXGIFactory = NULL;
  UINT deviceFlags = 0;
  UINT driver = 0;
  hWndmain = (HWND)window;

  static const D3D_DRIVER_TYPE driverAttempts[] =
  {
    D3D_DRIVER_TYPE_HARDWARE,
    D3D_DRIVER_TYPE_WARP,
    D3D_DRIVER_TYPE_REFERENCE,
  };

  static const D3D_FEATURE_LEVEL levelAttempts[] =
  {
    D3D_FEATURE_LEVEL_11_0,  // Direct3D 11.0 SM 5
    D3D_FEATURE_LEVEL_10_1,  // Direct3D 10.1 SM 4
    D3D_FEATURE_LEVEL_10_0,  // Direct3D 10.0 SM 4
    D3D_FEATURE_LEVEL_9_3,   // Direct3D 9.3  SM 3
    D3D_FEATURE_LEVEL_9_2,   // Direct3D 9.2  SM 2
    D3D_FEATURE_LEVEL_9_1,   // Direct3D 9.1  SM 2
  };

  for (driver = 0; driver < ARRAYSIZE(driverAttempts); driver++)
  {
    hr = D3D11CreateDevice(
      NULL,
      driverAttempts[driver],
      NULL,
      deviceFlags,
      levelAttempts,
      ARRAYSIZE(levelAttempts),
      D3D11_SDK_VERSION,
      &pDevice,
      &FeatureLevel,
      &pDeviceContext
    );

    if (SUCCEEDED(hr))
    {
      break;
    }

  }


  if (SUCCEEDED(hr))
  {
    hr = pDevice->QueryInterface(&pDXGIDevice);
  }
  if (SUCCEEDED(hr))
  {
    hr = pDXGIDevice->GetAdapter(&pAdapter);
  }
  if (SUCCEEDED(hr))
  {
    hr = pAdapter->GetParent(IID_IDXGIFactory, (void**)&pDXGIFactory);
  }
  if (SUCCEEDED(hr))
  {
#ifdef DEMO_MSAA
    UINT i;
#endif
    ZeroMemory(&swapDesc, sizeof(swapDesc));

    swapDesc.SampleDesc.Count = 1;        //The Number of Multisamples per Level
    swapDesc.SampleDesc.Quality = 0;      //between 0(lowest Quality) and one lesser than pDevice->CheckMultisampleQualityLevels

                                          // Enable if you want to use multisample AA for the rendertarget
#ifdef DEMO_MSAA
    for (i = 1; i <= D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; i++)
    {
      UINT Quality;
      if SUCCEEDED(pDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM, i, &Quality))
      {
        if (Quality > 0)
        {
          DXGI_SAMPLE_DESC Desc;
          Desc.Count = i;
          Desc.Quality = Quality - 1;
          swapDesc.SampleDesc = Desc;
        }
      }
    }
#endif

    swapDesc.BufferDesc.Width = x;
    swapDesc.BufferDesc.Height = y;
    swapDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swapDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.BufferCount = 1;
    swapDesc.OutputWindow = hWndmain;
    swapDesc.Windowed = TRUE;
    hr = pDXGIFactory->CreateSwapChain((IUnknown*)pDevice, &swapDesc, &pSwapChain);

    // Fallback to single sample
#ifdef DEMO_MSAA
    if (FAILED(hr))
    {
      swapDesc.SampleDesc.Count = 1;
      hr = pDXGIFactory->CreateSwapChain((IUnknown*)pDevice, &swapDesc, &pSwapChain);
    }
#endif
  }

  D3D_API_RELEASE(pDXGIDevice);
  D3D_API_RELEASE(pAdapter);
  D3D_API_RELEASE(pDXGIFactory);

  if (SUCCEEDED(hr))
  {
    hr = resizeWindow(x, y);
    if (FAILED(hr))
    {
      return FALSE;
    }
  }
  else
  {
    // Fail
    UnInitializeDX();
    return FALSE;
  }

  return TRUE;
}

// Frees everything
void UnInitializeDX()
{
  // Detach RTs
  if (pDeviceContext)
  {
    ID3D11RenderTargetView *viewList[1] = { NULL };
    pDeviceContext->OMSetRenderTargets(1, viewList, NULL);
  }

  D3D_API_RELEASE(pDeviceContext);
  D3D_API_RELEASE(pDevice);
  D3D_API_RELEASE(pSwapChain);
  D3D_API_RELEASE(pRenderTargetView);
  D3D_API_RELEASE(pDepthStencil);
  D3D_API_RELEASE(pDepthStencilView);
}

void clearDxContext()
{
  int n;
  int i;
  float gpuTimes[3];
  double dt;
  double t;
  float clearColor[4];
  D3D11_VIEWPORT viewport;
  float pxRatio;

  if (!pDeviceContext)
  {
    return;
  }

  if (dxPremultMode)
  {
    clearColor[0] = 0.0f;
    clearColor[1] = 0.0f;
    clearColor[2] = 0.0f;
    clearColor[3] = 0.0f;
  }
  else
  {

    clearColor[0] = 0.3f;
    clearColor[1] = 0.3f;
    clearColor[2] = 0.32f;
    clearColor[3] = 1.0f;
  }

  pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);

  viewport.Height = (float)yWin;
  viewport.Width = (float)xWin;
  viewport.MaxDepth = 1.0f;
  viewport.MinDepth = 0.0f;
  viewport.TopLeftX = 0.0f;
  viewport.TopLeftY = 0.0f;
  pDeviceContext->RSSetViewports(1, &viewport);

  pDeviceContext->ClearRenderTargetView(pRenderTargetView, clearColor);
  pDeviceContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 0.0f, (UINT8)0);
}

void swapDxContextxBuffers()
{
  pSwapChain->Present(0, 0);
}

HRESULT resizeWindow(unsigned int x, unsigned int y)
{
  D3D11_RENDER_TARGET_VIEW_DESC renderDesc;
  ID3D11RenderTargetView *viewList[1] = { NULL };
  HRESULT hr = S_OK;
  ID3D11Resource *pBackBufferResource = NULL;
  D3D11_TEXTURE2D_DESC texDesc;
  D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc;

  xWin = x;
  yWin = y;

  if (!pDevice || !pDeviceContext)
    return E_FAIL;

  //pDeviceContext->ClearState();
  pDeviceContext->OMSetRenderTargets(1, viewList, NULL);

  // Ensure that nobody is holding onto one of the old resources
  D3D_API_RELEASE(pRenderTargetView);
  D3D_API_RELEASE(pDepthStencilView);

  // Resize render target buffers
  hr = pSwapChain->ResizeBuffers(1, x, y, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
  if (FAILED(hr))
  {
    return hr;
  }

  // Create the render target view and set it on the device
  hr = pSwapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&pBackBufferResource);
  if (FAILED(hr))
  {
    return hr;
  }

  renderDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  renderDesc.ViewDimension = (swapDesc.SampleDesc.Count > 1) ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;
  renderDesc.Texture2D.MipSlice = 0;

  hr = pDevice->CreateRenderTargetView(pBackBufferResource, &renderDesc, &pRenderTargetView);
  D3D_API_RELEASE(pBackBufferResource);
  if (FAILED(hr))
  {
    return hr;
  }

  texDesc.ArraySize = 1;
  texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  texDesc.CPUAccessFlags = 0;
  texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  texDesc.Height = (UINT)y;
  texDesc.Width = (UINT)x;
  texDesc.MipLevels = 1;
  texDesc.MiscFlags = 0;
  texDesc.SampleDesc.Count = swapDesc.SampleDesc.Count;
  texDesc.SampleDesc.Quality = swapDesc.SampleDesc.Quality;
  texDesc.Usage = D3D11_USAGE_DEFAULT;

  D3D_API_RELEASE(pDepthStencil);
  hr = pDevice->CreateTexture2D(&texDesc, NULL, &pDepthStencil);
  if (FAILED(hr))
  {
    return hr;
  }

  depthViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depthViewDesc.ViewDimension = (swapDesc.SampleDesc.Count > 1) ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
  depthViewDesc.Flags = 0;
  depthViewDesc.Texture2D.MipSlice = 0;

  hr = pDevice->CreateDepthStencilView((ID3D11Resource*)pDepthStencil, &depthViewDesc, &pDepthStencilView);
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

        mHwWindow = (void*)CreateWindowEx(0, pszWindowClass, (mCaption + " (DX11)").c_str(), WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, (int)rcWin.right, (int)rcWin.bottom, NULL, NULL, hInstance, NULL);
    }

    if (!mHwWindow)
        throw std::runtime_error("Could not create an Window ");

    if (FAILED(InitializeDX(mHwWindow, rcWin.right, rcWin.bottom)))
        throw std::runtime_error("Could not create an dx11 context ");


    /* Propagate GLFW events to the appropriate Screen instance */
    dx11SetCursorPosCallback([](void *w, double x, double y) {
            auto it = __nanogui_screens.find((HWND)w);
            if (it == __nanogui_screens.end())
                return;
            dx11mouseposx = x;
            dx11mouseposy = y;
            Screen *s = it->second;
            if (!s->mProcessEvents)
                return;
            s->cursorPosCallbackEvent(x, y);
        }
    );

    dx11SetMouseButtonCallback([](void *w, int button, int action, int modifiers) {
            auto it = __nanogui_screens.find((HWND)w);
            if (it == __nanogui_screens.end())
                return;
            Screen *s = it->second;
            if (!s->mProcessEvents)
                return;
            s->mouseButtonCallbackEvent(button, action, modifiers);
        }
    );

    dx11SetKeyCallback([](void *w, int key, int scancode, int action, int mods) {
            auto it = __nanogui_screens.find((HWND)w);
            if (it == __nanogui_screens.end())
                return;
            Screen *s = it->second;
            if (!s->mProcessEvents)
                return;
            s->keyCallbackEvent(key, scancode, action, mods);
        }
    );

    dx11SetCharCallback([](void *w, unsigned int codepoint) {
            auto it = __nanogui_screens.find((HWND)w);
            if (it == __nanogui_screens.end())
                return;
            Screen *s = it->second;
            if (!s->mProcessEvents)
                return;
            s->charCallbackEvent(codepoint);
        }
    );

    dx11SetDropCallback([](void *w, int count, const char **filenames) {
            auto it = __nanogui_screens.find((HWND)w);
            if (it == __nanogui_screens.end())
                return;
            Screen *s = it->second;
            if (!s->mProcessEvents)
                return;
            s->dropCallbackEvent(count, filenames);
        }
    );

    dx11SetScrollCallback([](void *w, double x, double y) {
            auto it = __nanogui_screens.find((HWND)w);
            if (it == __nanogui_screens.end())
                return;
            Screen *s = it->second;
            if (!s->mProcessEvents)
                return;
            s->scrollCallbackEvent(x, y);
        }
    );

    dx11SetFramebufferSizeCallback([](void *w, int width, int height) {
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
    dx11SetWindowFocusCallback([](void *w, int focused) {
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

HICON createStandardCursor(HWND window, int shape)
{
  return CopyCursor(LoadCursorA(NULL, translateCursorShape(shape)));
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

    mNVGContext = nvgCreateD3D11(pDevice, NVG_ANTIALIAS | NVG_STENCIL_STROKES);
    if (mNVGContext == nullptr)
        throw std::runtime_error("Could not initialize NanoVG!");

    mVisible = true;
    setTheme(new Theme(mNVGContext));
    mMousePos = Vector2i::Zero();
    mMouseState = mModifiers = 0;
    mDragActive = false;
    mLastInteraction = getTimeFromStart();
    mProcessEvents = true;
    __nanogui_screens[(HWND)mHwWindow] = this;

    for (int i=0; i < (int) Cursor::CursorCount; ++i)
        mCursors[i] = (void*)createStandardCursor((HWND)mHwWindow, i);

    /// Fixes retina display-related font rendering issue (#185)
    nvgBeginFrame(mNVGContext, mSize[0], mSize[1], mPixelRatio);
    nvgEndFrame(mNVGContext);
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
    //if (mNVGContext)
    //    nvgDelete(mNVGContext);
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
    clearDxContext();

    drawContents();
    drawWidgets();

    swapDxContextxBuffers();
}

void Screen::setClipboardString(const std::string & text)
{
  dx11SetClipboardString((HWND)mHwWindow, text.c_str());
}

std::string Screen::getClipboardString()
{
  return dx11GetClipboardString((HWND)mHwWindow);
}

void Screen::_drawWidgetsBefore()
{
  dx11GetWindowSize((HWND)mHwWindow, &mSize[0], &mSize[1]);

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
  dx11SetCursorImage((HWND)mHwWindow, (HCURSOR)mCursors[cursor]);
}

bool Screen::resizeCallbackEvent(int, int) {
    Vector2i fbSize, size;

    dx11GetWindowSize((HWND)mHwWindow, &fbSize[0], &fbSize[1]);
    dx11GetWindowSize((HWND)mHwWindow, &size[0], &size[1]);

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
