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

#if NANOGUI_DX12_BACKEND

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

void dx12SetCursorPosCallback(const std::function<void (void *, double, double)>& f) { cursorPosCallback = f; }
void dx12SetMouseButtonCallback(const std::function<void(void *, int, int, int)>& f) { mouseButtonCallback = f; }
void dx12SetKeyCallback(const std::function<void(void *, int, int, int, int)>& f) { keyCallback = f; }
void dx12SetCharCallback(const std::function<void(void *, int)>& f) { charCallback = f; }
void dx12SetDropCallback(const std::function<void(void *, int , const char **)> &f) {};
void dx12SetScrollCallback(const std::function<void(void *, double , double )> &f) { scrollCallback = f; }
void dx12SetFramebufferSizeCallback(const std::function<void(void *, int, int)> &f) { resizeCallback = f; }
void dx12SetWindowFocusCallback(const std::function<void(void *, int )> &f) { focusCallback = f; }
void dx12SetDrawCallback(const std::function<void()>& f) { drawCallback = f; }

// Frees everything
void UnInitializeDX()
{
  pDX12.deinit();
}

HRESULT resizeWindow(unsigned int x, unsigned int y)
{
  HRESULT hr = S_OK;
  pDX12.set_size(x, y);
  return hr;
}

void dx12SetCursorImage(HWND window, HCURSOR cursor)
{
   if (cursor)
      SetCursor(cursor);
    else
      SetCursor(LoadCursorA(NULL, IDC_ARROW));
}

void dx12GetWindowSize(HWND window, int* width, int* height)
{
  RECT area;
  GetClientRect(window, &area);

  if (width)
    *width = area.right;
  if (height)
    *height = area.bottom;
}

WCHAR* dx12CreateWideStringFromUTF8Win32(const char* source)
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

void dx12SetClipboardString(HWND window, const char* string)
{
  WCHAR* wideString;
  HANDLE stringHandle;
  size_t wideSize;

  wideString = dx12CreateWideStringFromUTF8Win32(string);
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

void dx12FocusWindow(HWND window)
{
  BringWindowToTop(window);
  SetForegroundWindow(window);
  SetFocus(window);
}

char* dx12CreateUTF8FromWideStringWin32(const WCHAR* source)
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

const char* dx12GetClipboardString(HWND window)
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
  clipboardString = dx12CreateUTF8FromWideStringWin32((const WCHAR*)GlobalLock(stringHandle));

  GlobalUnlock(stringHandle);
  CloseClipboard();

  if (!clipboardString)
  {
    std::runtime_error("Win32: Failed to convert wide string to UTF-8");
    return NULL;
  }

  return clipboardString;
}

void sample::set_window_topmost(WindowHandle w, bool top, bool always)
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
    (top && always) ? HWND_TOPMOST : (top ? HWND_TOP : HWND_BOTTOM),  // placement-order handle
    rect.left,     // horizontal position
    rect.top,      // vertical position
    rect.right - rect.left,  // width
    rect.bottom - rect.top, // height
    SWP_SHOWWINDOW);
}

Vector2i sample::get_window_pos(WindowHandle w)
{
  POINT pos = { 0, 0 };
  ClientToScreen((HWND)w, &pos);
  return { pos.x, pos.y };
}

Vector2i sample::get_cursor_pos()
{
  POINT p;
  if (GetCursorPos(&p))
    return{ p.x, p.y };
}


void sample::set_window_pos(WindowHandle w, const Vector2i& pos)
{
    RECT rect = { pos.x(), pos.y(), pos.x(), pos.y() };
    AdjustWindowRectEx(&rect, GetWindowStyle((HWND)w),
                       FALSE, GetWindowExStyle((HWND)w));
    SetWindowPos((HWND)w, NULL, rect.left, rect.top, 0, 0,
                 SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
}

bool sample::post_empty_event(void) { PostMessage(hWndmain, WM_NULL, 0, 0); return false; }
bool sample::wait_events(void) { sample::poll_events(); return false; }

void sample::frame_loop(std::function<void()> &f)
{
  dx12SetDrawCallback(f);

  MSG msg;
  HWND handle;

  while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
  {
    TranslateMessage(&msg);
    DispatchMessageW(&msg);
  }
}

bool sample::poll_events(void) { return false; }

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
#define RET_KEYCODE(k,c) case k: return FOURCC(c);
    RET_KEYCODE(VK_DELETE, "KDEL")
    RET_KEYCODE(VK_ACCEPT , "KEYA")
    RET_KEYCODE(VK_KEY_X, "KEYX")
    RET_KEYCODE(VK_KEY_N, "KEYN")
    RET_KEYCODE(VK_KEY_B, "KEYB")
    RET_KEYCODE(VK_KEY_C, "KEYC")
    RET_KEYCODE(VK_KEY_R, "KEYR")
    RET_KEYCODE(VK_KEY_P, "KEYP")
    RET_KEYCODE(VK_KEY_V, "KEYV")
    RET_KEYCODE(VK_KEY_Z, "KEYZ")
    RET_KEYCODE(VK_LEFT, "LEFT")
    RET_KEYCODE(VK_RIGHT, "RGHT")
    RET_KEYCODE(VK_DOWN, "DOWN")
    RET_KEYCODE(VK_UP, "KBUP")
    RET_KEYCODE(VK_HOME, "HOME")
    RET_KEYCODE(VK_END, "KEND")
    RET_KEYCODE(VK_BACKSPACE, "BACK")
    RET_KEYCODE(VK_EXECUTE, "ENTR")
    RET_KEYCODE(VK_ESCAPE, "ESCP")
    RET_KEYCODE(VK_SPACE,  "SPCE")    
#undef RET_KEYCODE
  default: return FOURCC("UNKN");
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
          else
              button = MOUSE_BUTTON_LEFT;

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
            // Quit the app
            PostQuitMessage(0);
            sample::stop_frame_loop();
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

static unsigned __int64 dx12startTime = 0;
static unsigned __int64 dx12timeFreq = 0;

void init() {
  hInstance = GetModuleHandle(NULL);
  if (!MyRegisterClass(hInstance))
    throw std::runtime_error("Could not initialize window!");
  QueryPerformanceCounter((LARGE_INTEGER*)&dx12startTime);  
  QueryPerformanceFrequency((LARGE_INTEGER*)&dx12timeFreq);
}

void shutdown() {}

bool appIsShouldCloseScreen(Screen* screen) { return false; }

float getTimeFromStart(void)
{
    unsigned __int64 time;
    QueryPerformanceCounter((LARGE_INTEGER*)&time);
    return (time - dx12startTime) / (float)dx12timeFreq;
}

// Setup the device and the rendering targets
BOOL InitializeDX(void* window, unsigned int x, unsigned int y)
{
  hWndmain = (HWND)window;
  pDX12.init((HWND)window, x, y);
  resizeWindow(x, y);

  return true;
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

/* Calculate pixel ratio for hi-dpi devices. */
static float get_pixel_ratio(HWND window) {
    HMONITOR monitor = nullptr;
    static HMONITOR(WINAPI * MonitorFromWindow_)(HWND, DWORD);
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
    static HRESULT(WINAPI * GetDpiForMonitor_)(HMONITOR, UINT, UINT*, UINT*) = nullptr;
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

sample::WindowHandle sample::create_window(int& w, int& h, const std::string& caption, bool resizable, bool fullscreen, bool header)
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

    hw_window = (void*)CreateWindowEx(0, pszWindowClass, (caption + " (DX12)").c_str(), WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, (int)rcWin.right, (int)rcWin.bottom, NULL, NULL, hInstance, NULL);
  }

  if (!hw_window)
    throw std::runtime_error("Could not create an Window ");

  if (!header)
  {
    long Style = GetWindowLong((HWND)hw_window, GWL_STYLE);
    Style &= ~WS_MAXIMIZEBOX; //this makes it still work when WS_MAXIMIZEBOX is actually already toggled off
    Style &= ~WS_CAPTION;
    Style &= ~WS_SYSMENU;
    Style &= ~WS_BORDER;
    Style &= ~WS_SYSMENU;
    Style &= ~WS_THICKFRAME;
    //Style &= ~WS_GROUP;
    SetWindowLong(handle, GWL_STYLE, Style);
  }

  float pixelRatio = get_pixel_ratio((HWND)hw_window);
  int ww = w * pixelRatio;
  int hh = h * pixelRatio;
  if (pixelRatio != 1 && !fullscreen)
    setWindowSize((HWND)hw_window, ww, hh);

  ShowWindow((HWND)hw_window, SW_SHOW);

  if (FAILED(InitializeDX(hw_window, ww, hh)))
    throw std::runtime_error("Could not create an dx11 context ");

  if (!header)
  {
    RECT crect; GetClientRect((HWND)hw_window, &crect);

    w = (crect.right - crect.left) / pixelRatio;
    h = (crect.bottom - crect.top) / pixelRatio;
  }

  return hw_window;
}

void sample::present_frame(void* window)
{ 
    pDX12.fr_end();
}

void sample::clear_frame(const Color& background) 
{
  if (!pDX12.isRunning)
    return;

  pDX12.fr_start(); 
}

void sample::create_context() {
  __nanogui_context = nvgCreateD3D12(&pDX12, NVG_ANTIALIAS | NVG_STENCIL_STROKES);
  if (__nanogui_context == nullptr)
    throw std::runtime_error("Could not initialize NanoVG!");
}

void sample::destroy_window(sample::WindowHandle) { UnInitializeDX(); }

void sample::setup_window_params(WindowHandle hw_window, Screen* screen)
{
  /* Propagate GLFW events to the appropriate Screen instance */
  static Screen* s = screen;
  dx12SetCursorPosCallback([](void *w, double x, double y) {
    dx12mouseposx = x;
    dx12mouseposy = y;
    if (!s->canProcessEvents())
      return;
    s->cursorPosCallbackEvent(x, y);
  });

  dx12SetMouseButtonCallback([](void *w, int button, int action, int modifiers) {
    if (!s->canProcessEvents())
      return;
    s->mouseButtonCallbackEvent(button, action, modifiers);
  });

  dx12SetKeyCallback([](void *w, int key, int scancode, int action, int mods) {
    if (!s->canProcessEvents())
      return;
    s->keyCallbackEvent(key, scancode, action, mods);
  });

  dx12SetCharCallback([](void *w, unsigned int codepoint) {
    if (!s->canProcessEvents())
      return;
    s->charCallbackEvent(codepoint);
  });

  dx12SetDropCallback([](void *w, int count, const char **filenames) {
    if (!s->canProcessEvents())
      return;
    s->dropCallbackEvent(count, filenames);
  });

  dx12SetScrollCallback([](void *w, double x, double y) {
    if (!s->canProcessEvents())
      return;
    s->scrollCallbackEvent(x, y);
  });

  dx12SetFramebufferSizeCallback([](void *w, int width, int height) {
    if (!s->canProcessEvents())
      return;
    s->resizeCallbackEvent(width, height);
  });

  // notify when the screen has lost focus (e.g. application switch)
  dx12SetWindowFocusCallback([](void *w, int focused) {
    // focused: 0 when false, 1 when true
    s->focusEvent(focused != 0);
  });

  s->initClipboardSetHandler([hw_window](std::string text) { dx12SetClipboardString((HWND)hw_window, text.c_str()); });
  s->initClipboardGetHandler([hw_window]() { return dx12GetClipboardString((HWND)hw_window); });

  s->setPrepareFrameHandler([hw_window](Screen* screen) {
    Vector2i fbsize, size;
    dx12GetWindowSize((HWND)hw_window, &size[0], &size[1]);
    screen->setSizeFBSize(size, size);
  });

  s->setResizeHwHandler([hw_window](Screen*, Vector2i& size, Vector2i& fbsize) {
    dx12GetWindowSize((HWND)hw_window, &size[0], &size[1]);
    fbsize = size;
  });

  s->initHwCursorSetter([hw_window](intptr_t cursor) { dx12SetCursorImage((HWND)hw_window, (HCURSOR)cursor); });
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

void __nanogui_destroy_cursor(intptr_t cursor) { DestroyCursor((HCURSOR)cursor); }
intptr_t __nanogui_create_cursor(int shape) { return (intptr_t)CopyCursor(LoadCursorA(NULL, translateCursorShape(shape))); }


NAMESPACE_END(nanogui)

#endif //NANOGUI_DX12_BACKEND