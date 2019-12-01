/*

Port to dx12 by megai2

*/
#include "common_dx12.h"

#if NANOGUI_DX12_BACKEND

#include <windowsx.h>
#include <winerror.h>

#  ifndef NOMINMAX
#    define NOMINMAX
#  endif
#  undef APIENTRY

#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif

NAMESPACE_BEGIN(nanogui)

extern HINSTANCE hInstance;
extern HWND hWndmain;
extern const char* pszWindowClass;


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

void dx12SetCursorPosCallback(const std::function<void (void *, double, double)>& f) { cursorPosCallback = f; }
void dx12SetMouseButtonCallback(const std::function<void(void *, int, int, int)>& f) { mouseButtonCallback = f; }
void dx12SetKeyCallback(const std::function<void(void *, int, int, int, int)>& f) { keyCallback = f; }
void dx12SetCharCallback(const std::function<void(void *, int)>& f) { charCallback = f; }
void dx12SetDropCallback(const std::function<void(void *, int , const char **)> &f) {};
void dx12SetScrollCallback(const std::function<void(void *, double , double )> &f) { scrollCallback = f; }
void dx12SetFramebufferSizeCallback(const std::function<void(void *, int, int)> &f) { resizeCallback = f; }
void dx12SetWindowFocusCallback(const std::function<void(void *, int )> &f) { focusCallback = f; }

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

bool appPostEmptyEvent(void)
{
  PostMessage(hWndmain, WM_NULL, 0, 0); return false;
}

bool appWaitEvents(void)
{
  WaitMessage();

  appPollEvents();
  return false;
}

static bool dx12showCloseScreen = false;
bool appPollEvents(void)
{
  MSG msg;
  HWND handle;

  //while
  (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE));
  {
    if (msg.message == WM_QUIT)
    {
      // Treat WM_QUIT as a close on all windows
      // While GLFW does not itself post WM_QUIT, other processes may post
      // it to this one, for example Task Manager

      dx12showCloseScreen = true;
    }
    else
    {
      TranslateMessage(&msg);
      DispatchMessageW(&msg);
    }
  }
  return false;
}
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
            //Draw(hWnd);
            //ValidateRect(hWnd, NULL);
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
void init() {
  hInstance = GetModuleHandle(NULL);
  if (!MyRegisterClass(hInstance))
    throw std::runtime_error("Could not initialize window!");
  QueryPerformanceCounter((LARGE_INTEGER*)&dx12startTime);
}

void shutdown() {}

bool appIsShouldCloseScreen(Screen* screen) { return dx12showCloseScreen; }

float getTimeFromStart(void)
{
    unsigned __int64 time;
    QueryPerformanceCounter((LARGE_INTEGER*)&time);
    return (time - dx12startTime)/1000000.f;
}

NAMESPACE_END(nanogui)

#endif //NANOGUI_DX12_BACKEND