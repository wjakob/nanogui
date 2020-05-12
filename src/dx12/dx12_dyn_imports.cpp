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
#include "dx12_dyn_imports.h"

#if NANOGUI_DX12_BACKEND

dx12_dynamic_imports::dx12_dynamic_imports()
{
}

dx12_dynamic_imports::~dx12_dynamic_imports()
{
}

int dx12_dynamic_imports::Init()
{
  UINT dynLibSet = LoadDynLib(DYNIMP_DX12, DYNIMP_LIB_SET_WIN7) ? DYNIMP_LIB_SET_WIN7 : DYNIMP_LIB_SET_WIN10;

  //megai2: tricky cycle start
  for (int i = dynLibSet; i != DYNIMP_COUNT; ++i)
  {
    if (!LoadDynLib((dx12_dynamic_import_lib)i, dynLibSet))
      return 0;
  }

  UINT funIdx = 0;

  while (dx12_dynamic_import_funcs[funIdx].fn)
  {
    dx12_dynamic_import_lib libId = dx12_dynamic_import_funcs[funIdx].libId;

    import_fun_ptrs[funIdx] = GetProcAddress(libHandles[libId], dx12_dynamic_import_funcs[funIdx].fn);

    if (!import_fun_ptrs[funIdx])
      return 0;

    ++funIdx;
  }

  return 1;
}

UINT dx12_dynamic_imports::LoadDynLib(dx12_dynamic_import_lib lib, UINT set)
{
  if (dx12_dynamic_import_libs[set].dllName[lib])
  {
    wchar_t buf[1024];
    wsprintfW(buf, L"%s%s", dx12_dynamic_import_libs[set].pathPrefix[lib], dx12_dynamic_import_libs[set].dllName[lib]);

    libHandles[lib] = LoadLibraryW(buf);

    return libHandles[lib] != 0;
  }
  else
    libHandles[lib] = NULL;

  return 1;
}

#endif //NANOGUI_DX12_BACKEND