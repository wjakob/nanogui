//
// Copyright (c) 2009-2013 Mikko Mononen memon@inside.org
// Port of _gl.h to _d3d11.h by Chris Maughan
// Port of _d3d11.h to _d3d12.h by megai2
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//
#include "../../include/nanogui/nanovg_d3d12.h"

int D3Dnvg__maxi(int a, int b)
{ 
  return a > b ? a : b; 
}

void D3Dnvg_copyMatrix3to4(float* pDest, const float* pSource)
{
  unsigned int i;
  for (i = 0; i < 4; i++)
  {
    memcpy(&pDest[i * 4], &pSource[i * 3], sizeof(float) * 3);
  }
}

void D3Dnvg__xformToMat3x3(float* m3, float* t)
{
  m3[0] = t[0];
  m3[1] = t[1];
  m3[2] = 0.0f;
  m3[3] = t[2];
  m3[4] = t[3];
  m3[5] = 0.0f;
  m3[6] = t[4];
  m3[7] = t[5];
  m3[8] = 1.0f;
}

struct NVGcolor D3Dnvg__premulColor(struct NVGcolor c)
{
  c.r *= c.a;
  c.g *= c.a;
  c.b *= c.a;
  return c;
}

int D3Dnvg__checkError(HRESULT hr, const char* str)
{
  if (!SUCCEEDED(hr))
  {
    printf("nanovg dx12: Error %08x after %s\n", hr, str);
    return 1;
  }
  return 0;
}



