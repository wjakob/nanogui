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
#include <nanogui/nanovg_d3d12.h>

#if NANOGUI_DX12_BACKEND

D3DNVGtexture * D3Dnvg__allocTexture(D3DNVGcontext * D3D)
{
  struct D3DNVGtexture* tex = NULL;
  int i;

  for (i = 0; i < D3D->ntextures; i++) {
    if (D3D->textures[i].id == 0) {
      tex = &D3D->textures[i];
      break;
    }
  }
  if (tex == NULL) {
    if (D3D->ntextures + 1 > D3D->ctextures) {
      struct D3DNVGtexture* textures;
      int ctextures = D3Dnvg__maxi(D3D->ntextures + 1, 4) + D3D->ctextures / 2; // 1.5x Overallocate
      textures = (struct D3DNVGtexture*)realloc(D3D->textures, sizeof(struct D3DNVGtexture)*ctextures);
      if (textures == NULL) return NULL;
      D3D->textures = textures;
      D3D->ctextures = ctextures;
    }
    tex = &D3D->textures[D3D->ntextures++];
  }

  memset(tex, 0, sizeof(*tex));
  tex->id = ++D3D->textureId;

  return tex;
}

D3DNVGcall * D3Dnvg__allocCall(D3DNVGcontext * D3D)
{
  struct D3DNVGcall* ret = NULL;
  if (D3D->ncalls + 1 > D3D->ccalls) {
    struct D3DNVGcall* calls;
    int ccalls = D3Dnvg__maxi(D3D->ncalls + 1, 128) + D3D->ccalls / 2; // 1.5x Overallocate
    calls = (struct D3DNVGcall*)realloc(D3D->calls, sizeof(struct D3DNVGcall) * ccalls);
    if (calls == NULL) return NULL;
    D3D->calls = calls;
    D3D->ccalls = ccalls;
  }
  ret = &D3D->calls[D3D->ncalls++];
  memset(ret, 0, sizeof(struct D3DNVGcall));
  return ret;
}

int D3Dnvg__allocPaths(D3DNVGcontext * D3D, int n)
{
  int ret = 0;
  if (D3D->npaths + n > D3D->cpaths) {
    struct D3DNVGpath* paths;
    int cpaths = D3Dnvg__maxi(D3D->npaths + n, 128) + D3D->cpaths / 2; // 1.5x Overallocate
    paths = (struct D3DNVGpath*)realloc(D3D->paths, sizeof(struct D3DNVGpath) * cpaths);
    if (paths == NULL) return -1;
    D3D->paths = paths;
    D3D->cpaths = cpaths;
  }
  ret = D3D->npaths;
  D3D->npaths += n;
  return ret;
}

int D3Dnvg__allocVerts(D3DNVGcontext * D3D, int n)
{
  int ret = 0;
  if (D3D->nverts + n > D3D->cverts) {
    struct NVGvertex* verts;
    int cverts = D3Dnvg__maxi(D3D->nverts + n, 4096) + D3D->cverts / 2; // 1.5x Overallocate
    verts = (struct NVGvertex*)realloc(D3D->verts, sizeof(struct NVGvertex) * cverts);
    if (verts == NULL) return -1;
    D3D->verts = verts;
    D3D->cverts = cverts;
  }
  ret = D3D->nverts;
  D3D->nverts += n;
  return ret;
}

int D3Dnvg__allocFragUniforms(D3DNVGcontext * D3D, int n)
{
  int ret = 0, structSize = D3D->fragSize;
  if (D3D->nuniforms + n > D3D->cuniforms) {
    unsigned char* uniforms;
    int cuniforms = D3Dnvg__maxi(D3D->nuniforms + n, 128) + D3D->cuniforms / 2; // 1.5x Overallocate
    uniforms = (unsigned char*)realloc(D3D->uniforms, structSize * cuniforms);
    if (uniforms == NULL) return -1;
    D3D->uniforms = uniforms;
    D3D->cuniforms = cuniforms;
  }
  ret = D3D->nuniforms * structSize;
  D3D->nuniforms += n;
  return ret;
}

D3DNVGfragUniforms* nvg__fragUniformPtr(struct D3DNVGcontext* D3D, int i)
{
  return (struct D3DNVGfragUniforms*)&D3D->uniforms[i];
}

D3DNVGtexture* D3Dnvg__findTexture(D3DNVGcontext* D3D, int id)
{
  int i;
  for (i = 0; i < D3D->ntextures; i++)
    if (D3D->textures[i].id == id)
      return &D3D->textures[i];
  return NULL;
}

#endif //NANOGUI_DX12_BACKEND