#include "common_dx11.h"

#if NANOGUI_DX11_BACKEND

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
#include <nanogui/screen.h>

#include <windows.h>
#include <windowsx.h>
#include <winerror.h>

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


enum {
  KB_RELEASE = 0, MOUSE_RELEASE = 0, MOUSE_BUTTON_LEFT = 0, KB_MOD_SHIFT = 1,
  MOUSE_BUTTON_RIGHT = 1, KB_PRESS = 1, MOUSE_PRESS = 1, MOUSE_BUTTON_MIDDLE = 2,
  KB_MOD_CONTROL = 2, KB_MOD_ALT = 4, KB_MOD_SUPER = 8,
  KB_LEFT_SHIFT = 340, KB_RIGHT_SHIFT = 344
};

bool isMouseButtonLeft(int button) { return button == MOUSE_BUTTON_LEFT; }
bool isMouseButtonLeftMod(int buttons) { return buttons & (1 << MOUSE_BUTTON_LEFT); }
bool isMouseButtonRight(int button) { return button == MOUSE_BUTTON_RIGHT; }
bool isMouseActionRelease(int action) { return action == MOUSE_RELEASE; }
bool isMouseActionPress(int action) { return action == MOUSE_PRESS; }

std::function<void(void *, double, double)> cursorPosCallback;
std::function<void(void *, int, int, int)> mouseButtonCallback;
std::function<void(void *, int, int, int, int)> keyCallback;
std::function<void(void *, int)> charCallback;
std::function<void(void *, double x, double y)> scrollCallback;
std::function<void(void *, int, int)> resizeCallback;
std::function<void(void *, int)> focusCallback;
std::function<void()> drawCallback;

void dx11SetCursorPosCallback(const std::function<void (void *, double, double)>& f) { cursorPosCallback = f; }
void dx11SetMouseButtonCallback(const std::function<void(void *, int, int, int)>& f) { mouseButtonCallback = f; }
void dx11SetKeyCallback(const std::function<void(void *, int, int, int, int)>& f) { keyCallback = f; }
void dx11SetCharCallback(const std::function<void(void *, int)>& f) { charCallback = f; }
void dx11SetDropCallback(const std::function<void(void *, int , const char **)> &f) {};
void dx11SetScrollCallback(const std::function<void(void *, double , double )> &f) { scrollCallback = f; }
void dx11SetFramebufferSizeCallback(const std::function<void(void *, int, int)> &f) { resizeCallback = f; }
void dx11SetWindowFocusCallback(const std::function<void(void *, int )> &f) { focusCallback = f; }
void dx11SetDrawCallback(const std::function<void()> &f) { drawCallback = f; }

void sample::set_window_topmost(WindowHandle w, bool topalways)
{
  HWND realw = (HWND)w;
  
  ShowWindow(realw, TRUE);
  SetForegroundWindow(realw);
  RECT rect;

  // get the current window size and position
  GetWindowRect(realw, &rect);

  // now change the size, position, and Z order
  // of the window.
  ::SetWindowPos(realw,       // handle to window
    topalways ? HWND_TOPMOST : HWND_BOTTOM,  // placement-order handle
    rect.left,     // horizontal position
    rect.top,      // vertical position
    rect.right - rect.left,  // width
    rect.bottom - rect.top, // height
    SWP_SHOWWINDOW);
}

Vector2i sample::get_window_pos(WindowHandle* w)
{
  POINT pos = { 0, 0 };
  ClientToScreen((HWND)w, &pos);
  return { pos.x, pos.y };
}

void sample::set_window_pos(WindowHandle w, const Vector2i& pos)
{
    RECT rect = { pos.x(), pos.y(), pos.x(), pos.y() };
    AdjustWindowRectEx(&rect, getWindowStyle(window),
                       FALSE, getWindowExStyle(window));
    SetWindowPos((HWND)w, NULL, rect.left, rect.top, 0, 0,
                 SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
}

Vector2i sample::get_cursor_pos()
{
  POINT p;
  if (GetCursorPos(&p))
    return{ p.x, p.y };
}

/* Calculate pixel ratio for hi-dpi devices. */
static float get_pixel_ratio(HWND window) {
  HMONITOR monitor = nullptr;
  static HMONITOR(WINAPI *MonitorFromWindow_)(HWND, DWORD);
  static bool MonitorFromWindow_tried = false;
  if (!MonitorFromWindow_tried) {
    auto user32 = LoadLibrary(TEXT("user32"));
    if (user32)
      MonitorFromWindow_ = (decltype(MonitorFromWindow_))GetProcAddress(user32, "MonitorFromWindow");
    MonitorFromWindow_tried = true;
  }
  if (MonitorFromWindow_)
    monitor = MonitorFromWindow_(hWndmain, 2);
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
}


void dx11SetCursorImage(HWND window, HCURSOR cursor)
{
   if (cursor)
      SetCursor(cursor);
    else
      SetCursor(LoadCursorA(NULL, IDC_ARROW));
}

void dx11GetWindowSize(HWND window, int* width, int* height)
{
  RECT area;
  GetClientRect(window, &area);

  if (width)
    *width = area.right;
  if (height)
    *height = area.bottom;
}

WCHAR* dx11CreateWideStringFromUTF8Win32(const char* source)
{
  WCHAR* target;
  int length;

  length = MultiByteToWideChar(CP_UTF8, 0, source, -1, NULL, 0);
  if (!length)
    return NULL;

  target = (WCHAR*)calloc(length, sizeof(WCHAR));

  if (!MultiByteToWideChar(CP_UTF8, 0, source, -1, target, length))
  {
    free(target);
    return NULL;
  }

  return target;
}

void dx11SetClipboardString(HWND window, const char* string)
{
  WCHAR* wideString;
  HANDLE stringHandle;
  size_t wideSize;

  wideString = dx11CreateWideStringFromUTF8Win32(string);
  if (!wideString)
  {
    std::runtime_error("Win32: Failed to convert string to UTF-16");
    return;
  }

  wideSize = (wcslen(wideString) + 1) * sizeof(WCHAR);

  stringHandle = GlobalAlloc(GMEM_MOVEABLE, wideSize);
  if (!stringHandle)
  {
    free(wideString);
    std::runtime_error("Win32: Failed to allocate global handle for clipboard");
    return;
  }

  memcpy(GlobalLock(stringHandle), wideString, wideSize);
  GlobalUnlock(stringHandle);

  if (!OpenClipboard(window))
  {
    GlobalFree(stringHandle);
    free(wideString);

    std::runtime_error("Win32: Failed to open clipboard");
    return;
  }

  EmptyClipboard();
  SetClipboardData(CF_UNICODETEXT, stringHandle);
  CloseClipboard();

  free(wideString);
}

void dx11FocusWindow(HWND window)
{
  BringWindowToTop(window);
  SetForegroundWindow(window);
  SetFocus(window);
}

char* dx11CreateUTF8FromWideStringWin32(const WCHAR* source)
{
  char* target;
  int length;

  length = WideCharToMultiByte(CP_UTF8, 0, source, -1, NULL, 0, NULL, NULL);
  if (!length)
    return NULL;

  target = (char*)calloc(length, sizeof(char));

  if (!WideCharToMultiByte(CP_UTF8, 0, source, -1, target, length, NULL, NULL))
  {
    free(target);
    return NULL;
  }

  return target;
}

const char* dx11GetClipboardString(HWND window)
{
  HANDLE stringHandle;

  if (!OpenClipboard(window))
  {
    std::runtime_error("Win32: Failed to open clipboard");
    return NULL;
  }

  stringHandle = GetClipboardData(CF_UNICODETEXT);
  if (!stringHandle)
  {
    CloseClipboard();

    std::runtime_error("Win32: Failed to convert clipboard to string");
    return NULL;
  }

  static const char* clipboardString = nullptr;
  if (clipboardString)
    free((void*)clipboardString);
  clipboardString = dx11CreateUTF8FromWideStringWin32((const WCHAR*)GlobalLock(stringHandle));

  GlobalUnlock(stringHandle);
  CloseClipboard();

  if (!clipboardString)
  {
    std::runtime_error("Win32: Failed to convert wide string to UTF-8");
    return NULL;
  }

  return clipboardString;
}

bool sample::post_empty_event() { PostMessage(hWndmain, WM_NULL, 0, 0); return false; }
bool sample::wait_events() { poll_events(); return false; }
bool sample::poll_events() { return false; }

#define VK_KEY_A                  65
#define VK_KEY_X                  88
#define VK_KEY_N                  78
#define VK_KEY_B                  66
#define VK_KEY_C                  67
#define VK_KEY_R                  82
#define VK_KEY_P                  80
#define VK_KEY_V                  86
#define VK_KEY_Z                  90

#define VK_BACKSPACE          259

uint32_t key2fourcc(int key)
{
  switch (key) {
#define RET_KEYCODE(k,c) case k: return FOURCCS(c);
    RET_KEYCODE(VK_DELETE, "KDEL")
    RET_KEYCODE(VK_ACCEPT, "KEYA")
    RET_KEYCODE(VK_KEY_X,  "KEYX")
    RET_KEYCODE(VK_KEY_N,  "KEYN")
    RET_KEYCODE(VK_KEY_B,  "KEYB")
    RET_KEYCODE(VK_KEY_C,  "KEYC")
    RET_KEYCODE(VK_KEY_R,  "KEYR")
    RET_KEYCODE(VK_KEY_P,  "KEYP")
    RET_KEYCODE(VK_KEY_V,  "KEYV")
    RET_KEYCODE(VK_KEY_Z,  "KEYZ")
    RET_KEYCODE(VK_LEFT,   "LEFT")
    RET_KEYCODE(VK_RIGHT,  "RGHT")
    RET_KEYCODE(VK_DOWN,   "DOWN")
    RET_KEYCODE(VK_UP,     "KBUP")
    RET_KEYCODE(VK_HOME,   "HOME")
    RET_KEYCODE(VK_END,    "KEND")
    RET_KEYCODE(VK_BACKSPACE, "BACK")
    RET_KEYCODE(VK_EXECUTE,   "ENTR")
    RET_KEYCODE(VK_ESCAPE,    "ESCP")
    RET_KEYCODE(VK_SPACE,  "SPCE")
#undef RET_KEYCODE
  default: return FOURCCS("UNKN");
  }}

bool isKeyboardModifierCtrl(int modifier) { return modifier & KB_MOD_CONTROL; }
bool isKeyboardModifierShift(int modifier) { return modifier & KB_MOD_SHIFT; }
bool isKeyboardActionPress(int action) { return action == KB_PRESS; }
bool isKeyboardActionRelease(int action) { return action == KB_RELEASE; }
bool isKeyboardKeyEscape(int key) { return isKeyboardKey(key, "ESCP"); }
bool isKeyboardActionRepeat(int action) { return false; }


static int getKeyMods(void)
{
  int mods = 0;

  if (GetKeyState(VK_SHIFT) & (1 << 31))
    mods |= KB_MOD_SHIFT;
  if (GetKeyState(VK_CONTROL) & (1 << 31))
    mods |= KB_MOD_CONTROL;
  if (GetKeyState(VK_MENU) & (1 << 31))
    mods |= KB_MOD_ALT;
  if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & (1 << 31))
    mods |= KB_MOD_SUPER;

  return mods;
}

void __nanogui_destroy_cursor(intptr_t) {}
void sample::destroy_window(sample::WindowHandle) {}

void sample::frame_loop(std::function<void()> &f)
{
  dx11SetDrawCallback(f);

  MSG msg;
  HWND handle;

  while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
  {
    TranslateMessage(&msg);
    DispatchMessageW(&msg);
  }
}

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

void UnInitializeDX()
{
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

void setWindowSize(HWND window, int w, int h)
{
  RECT rect = { 0, 0, w, h };
  AdjustWindowRectEx(&rect, WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
    FALSE, WS_EX_APPWINDOW);
  SetWindowPos((HWND)window, HWND_TOP,
    0, 0, rect.right - rect.left, rect.bottom - rect.top,
    SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER);
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


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch (message)
  {
        // Keyboard handling
        case WM_KEYDOWN :
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
          const int key = wParam;
          const int scancode = (lParam >> 16) & 0x1ff;
          const int action = ((lParam >> 31) & 1) ? 0 : 1;
          const int mods = getKeyMods();

          if (!keyCallback)
            return 0;

          if (key == -2)
            break;

          if (action == KB_RELEASE && wParam == VK_SHIFT)
          {
            // Release both Shift keys on Shift up event, as only one event
            // is sent even if both keys are released
            keyCallback(hWnd, KB_LEFT_SHIFT, scancode, action, mods);
            keyCallback(hWnd, KB_RIGHT_SHIFT, scancode, action, mods);
          }
          else
            keyCallback(hWnd, key, scancode, action, mods);

          break;
        }

        case WM_CHAR:
        case WM_SYSCHAR:
        case WM_UNICHAR:
        {
          const bool plain = (message != WM_SYSCHAR);

          if (message == WM_UNICHAR && wParam == UNICODE_NOCHAR)
          {
            // WM_UNICHAR is not sent by Windows, but is sent by some
            // third-party input method engine
            // Returning TRUE here announces support for this message
            return TRUE;
          }
          if (charCallback)
            charCallback(hWnd, (unsigned int)wParam);

          return 0;
        }
        break;

        // Mouse pos
        case WM_MOUSEMOVE:
        {
            int xm = GET_X_LPARAM(lParam);
            int ym = GET_Y_LPARAM(lParam);
            if (cursorPosCallback)
              cursorPosCallback(hWnd, xm, ym);
        }
break;

        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_XBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
        {
          int button, action;

          if (message == WM_LBUTTONDOWN || message == WM_LBUTTONUP)
            button = MOUSE_BUTTON_LEFT;
          else if (message == WM_RBUTTONDOWN || message == WM_RBUTTONUP)
            button = MOUSE_BUTTON_RIGHT;
          else if (message == WM_MBUTTONDOWN || message == WM_MBUTTONUP)
            button = MOUSE_BUTTON_MIDDLE;

          if (message == WM_LBUTTONDOWN || message == WM_RBUTTONDOWN ||
            message == WM_MBUTTONDOWN || message == WM_XBUTTONDOWN)
          {
            action = MOUSE_PRESS;
            SetCapture(hWnd);
          }
          else
          {
            action = MOUSE_RELEASE;
            ReleaseCapture();
          }

          if (mouseButtonCallback)
            mouseButtonCallback(hWnd, button, action, getKeyMods());

          if (message == WM_XBUTTONDOWN || message == WM_XBUTTONUP)
            return TRUE;

          return 0;
        }
        break;

        // Painting
        case WM_PAINT:
        {
          if (drawCallback)
            drawCallback();
        }
        break;

        // Sizing
        case WM_SIZE:
        {
          nanogui::resizeWindow(LOWORD(lParam), HIWORD(lParam));
        }
        break;

        case WM_SETFOCUS:
        {
          if (focusCallback)
            focusCallback(hWnd, 1);

          return 0;
        }

        case WM_MOUSEWHEEL:
        {
          scrollCallback(hWnd, 0.0, (SHORT)HIWORD(wParam) / (double)WHEEL_DELTA);
          return 0;
        }

        case WM_MOUSEHWHEEL:
        {
          // This message is only sent on Windows Vista and later
          // NOTE: The X-axis is inverted for consistency with OS X and X11.
          scrollCallback(hWnd, -((SHORT)HIWORD(wParam) / (double)WHEEL_DELTA), 0.0);
          return 0;
        }

        case WM_ERASEBKGND:
        {
          // No need to erase background
          return 1;
        }
        break;

        case WM_DESTROY:
        {
          nanogui::UnInitializeDX();

          // Quit the app
          PostQuitMessage(0);
        }
        break;

        default: return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
  WNDCLASSEX wcex;

  wcex.cbSize = sizeof(WNDCLASSEX);

  wcex.style      = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  wcex.lpfnWndProc  = WndProc;
  wcex.cbClsExtra    = 0;
  wcex.cbWndExtra    = 0;
  wcex.hInstance    = hInstance;
  wcex.hIcon      = NULL;
  wcex.hCursor    = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
  wcex.lpszMenuName  = "";
  wcex.lpszClassName  = pszWindowClass;
  wcex.hIconSm    = NULL;

  return RegisterClassEx(&wcex);
}

static unsigned __int64 dx11startTime = 0;
void init() {
  hInstance = GetModuleHandle(NULL);
  if (!MyRegisterClass(hInstance))
    throw std::runtime_error("Could not initialize window!");
  QueryPerformanceCounter((LARGE_INTEGER*)&dx11startTime);
}

void shutdown() {}

bool appIsShouldCloseScreen(Screen* screen) { return false; }

float getTimeFromStart(void)
{
    unsigned __int64 time;
    QueryPerformanceCounter((LARGE_INTEGER*)&time);
    return (time - dx11startTime)/1000000.f;
}

void sample::clear_frame(Color background)
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

void sample::present_frame(void* window) { pSwapChain->Present(0, 0); }
void sample::create_context() {
  __nanogui_context = nvgCreateD3D11(pDevice, NVG_ANTIALIAS | NVG_STENCIL_STROKES);
  if (__nanogui_context == nullptr)
    throw std::runtime_error("Could not initialize NanoVG!");
}

void sample::remove_window_border(WindowHandle wnd)
{
  auto handle = (HWND)wnd;
  long Style = GetWindowLong(handle, GWL_STYLE);
  Style &= ~WS_MAXIMIZEBOX; //this makes it still work when WS_MAXIMIZEBOX is actually already toggled off
  Style &= ~WS_CAPTION;
  Style &= ~WS_SYSMENU;
  Style &= ~WS_BORDER;
  Style &= ~WS_SYSMENU;
  Style &= ~WS_THICKFRAME;
  //Style &= ~WS_GROUP;
  SetWindowLong(handle, GWL_STYLE, Style);
}

sample::WindowHandle sample::create_window(int w, int h, const std::string& caption, bool resizable, bool fullscreen)
{
  RECT rcWin;
  sample::WindowHandle hw_window;

  if (fullscreen) {
    //???
  }
  else 
  {
    rcWin.left = 0;
    rcWin.right = w;
    rcWin.top = 0;
    rcWin.bottom = h;

    AdjustWindowRectEx(&rcWin, WS_OVERLAPPEDWINDOW, FALSE, 0);

    rcWin.right += -rcWin.left;
    rcWin.bottom += -rcWin.top;

    hw_window = (void*)CreateWindowEx(0, pszWindowClass, (caption + " (DX11)").c_str(), WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, (int)rcWin.right, (int)rcWin.bottom, NULL, NULL, hInstance, NULL);
  }

  if (!hw_window)
    throw std::runtime_error("Could not create an Window ");

  float pixelRatio = get_pixel_ratio((HWND)hw_window);
  int ww = w * pixelRatio;
  int hh = h * pixelRatio;
  if (pixelRatio != 1 && !fullscreen)
    setWindowSize((HWND)hw_window, ww, hh);

  ShowWindow((HWND)hw_window, SW_SHOW);

  if (FAILED(InitializeDX(hw_window, ww, hh)))
    throw std::runtime_error("Could not create an dx11 context ");

  return hw_window;
}

void sample::setup_window_params(WindowHandle hw_window, Screen* screen)
{
  /* Propagate GLFW events to the appropriate Screen instance */
  static Screen* s = screen;
  dx11SetCursorPosCallback([](void *w, double x, double y) {
    dx11mouseposx = x;
    dx11mouseposy = y;
    if (!s->canProcessEvents())
      return;
    s->cursorPosCallbackEvent(x, y);
  });

  dx11SetMouseButtonCallback([](void *w, int button, int action, int modifiers) {
    if (!s->canProcessEvents())
      return;
    s->mouseButtonCallbackEvent(button, action, modifiers);
  });

  dx11SetKeyCallback([](void *w, int key, int scancode, int action, int mods) {
    if (!s->canProcessEvents())
      return;
    s->keyCallbackEvent(key, scancode, action, mods);
  });

  dx11SetCharCallback([](void *w, unsigned int codepoint) {
    if (!s->canProcessEvents())
      return;
    s->charCallbackEvent(codepoint);
  });

  dx11SetDropCallback([](void *w, int count, const char **filenames) {
    if (!s->canProcessEvents())
      return;
    s->dropCallbackEvent(count, filenames);
  });

  dx11SetScrollCallback([](void *w, double x, double y) {
    if (!s->canProcessEvents())
      return;
    s->scrollCallbackEvent(x, y);
  });

  dx11SetFramebufferSizeCallback([](void *w, int width, int height) {
    if (!s->canProcessEvents())
      return;
    s->resizeCallbackEvent(width, height);
  });

  // notify when the screen has lost focus (e.g. application switch)
  dx11SetWindowFocusCallback([](void*, int focused) {
    // focused: 0 when false, 1 when true
    s->focusEvent(focused != 0);
  });

  s->initClipboardSetHandler([hw_window](std::string text) { dx11SetClipboardString((HWND)hw_window, text.c_str()); });
  s->initClipboardGetHandler([hw_window]() { return dx11GetClipboardString((HWND)hw_window); });

  s->setPrepareFrameHandler([hw_window](Screen* screen) {
    Vector2i fbsize, size;
    dx11GetWindowSize((HWND)hw_window, &size[0], &size[1]);
    screen->setSizeFBSize(size, size);
  });

  s->setResizeHwHandler([hw_window](Screen*, Vector2i& size, Vector2i& fbsize) {
    dx11GetWindowSize((HWND)hw_window, &size[0], &size[1]);
    fbsize = size;
  });

  s->initHwCursorSetter([hw_window](intptr_t cursor) { dx11SetCursorImage((HWND)hw_window, (HCURSOR)cursor); });
  s->setPixelRatio(get_pixel_ratio((HWND)hw_window));
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
intptr_t __nanogui_create_cursor(int shape) { return (intptr_t)CopyCursor(LoadCursorA(NULL, translateCursorShape(shape))); }

NAMESPACE_END(nanogui)

#endif //NANOGUI_DX11_BACKEND