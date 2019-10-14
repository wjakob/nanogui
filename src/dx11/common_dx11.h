#pragma once

#include <nanogui/common.h>

#include <windows.h>
#include <windowsx.h>

NAMESPACE_BEGIN(nanogui)

void dx11SetCursorPosCallback(const std::function<void(void *, double, double)>& f);
void dx11SetMouseButtonCallback(const std::function<void(void *, int, int, int)>& f);
void dx11SetKeyCallback(const std::function<void(void *, int, int, int, int)>& f);
void dx11SetCharCallback(const std::function<void(void *, int)>& f);
void dx11SetDropCallback(const std::function<void(void *, int, const char **)> &f);
void dx11SetScrollCallback(const std::function<void(void *w, double x, double y)> &f);
void dx11SetCursorImage(HWND window, HCURSOR cursor);
void dx11SetFramebufferSizeCallback(const std::function<void (void *, int, int)> &f);
void dx11SetClipboardString(HWND window, const char* string);
const char* dx11GetClipboardString(HWND window);
const char* dx11GetClipboardString(HWND window);
void dx11SetWindowFocusCallback(const std::function<void(void *, int)> &f);
void dx11GetWindowSize(HWND window, int* width, int* height);
HRESULT resizeWindow(unsigned int x, unsigned int y);
void UnInitializeDX();

void clearDxContext();
void swapDxContextxBuffers();

BOOL InitializeDX(unsigned int x, unsigned int y);

NAMESPACE_END(nanogui)