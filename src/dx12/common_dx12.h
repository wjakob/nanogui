/*
MIT License

Copyright(c) 2019 megai2

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/
#pragma once

#include <nanogui/common.h>

#include <windows.h>
#include <windowsx.h>

NAMESPACE_BEGIN(nanogui)

void dx12SetCursorPosCallback(const std::function<void(void *, double, double)>& f);
void dx12SetMouseButtonCallback(const std::function<void(void *, int, int, int)>& f);
void dx12SetKeyCallback(const std::function<void(void *, int, int, int, int)>& f);
void dx12SetCharCallback(const std::function<void(void *, int)>& f);
void dx12SetDropCallback(const std::function<void(void *, int, const char **)> &f);
void dx12SetScrollCallback(const std::function<void(void *w, double x, double y)> &f);
void dx12SetCursorImage(HWND window, HCURSOR cursor);
void dx12SetFramebufferSizeCallback(const std::function<void (void *, int, int)> &f);
void dx12SetClipboardString(HWND window, const char* string);
const char* dx12GetClipboardString(HWND window);
const char* dx12GetClipboardString(HWND window);
void dx12SetWindowFocusCallback(const std::function<void(void *, int)> &f);
void dx12GetWindowSize(HWND window, int* width, int* height);

HRESULT resizeWindow(unsigned int x, unsigned int y);
void UnInitializeDX();
void clearDxContext();
void swapDxContextxBuffers();
BOOL InitializeDX(unsigned int x, unsigned int y);

NAMESPACE_END(nanogui)