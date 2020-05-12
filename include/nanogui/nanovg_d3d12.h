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
#ifndef NANOVG_D3D12_H
#define NANOVG_D3D12_H

#if NANOGUI_DX12_BACKEND

// Hide nameless struct/union warning for D3D headers
#pragma warning (disable : 4201)
#include "dx12_subset.h"
#pragma warning (default : 4201)

#ifdef __cplusplus
extern "C" {
#endif

// Flag indicating if geoemtry based anti-aliasing is used (may not be needed when using MSAA).
#define NVG_ANTIALIAS 1

// Flag indicating if strokes should be drawn using stencil buffer. The rendering will be a little
// slower, but path overlaps (i.e. self-intersecting or sharp turns) will be drawn just once.
#define NVG_STENCIL_STROKES 2

//struct NVGcontext* nvgCreateD3D12(ID3D11Device* pDevice, int edgeaa);
void nvgDeleteD3D12(struct NVGcontext* ctx);

// These are additional flags on top of NVGimageFlags.
enum NVGimageFlagsD3D11 {
  NVG_IMAGE_NODELETE      = 1<<16,  // Do not delete texture object.
};

// Not done yet.  Simple enough to do though...
#ifdef IMPLEMENTED_IMAGE_FUNCS
int nvd3dCreateImageFromHandle(struct NVGcontext* ctx, void* texture, int w, int h, int flags);
unsigned int nvd3dImageHandle(struct NVGcontext* ctx, int image);
void nvd3dImageFlags(struct NVGcontext* ctx, int image, int flags);
#endif

#ifdef __cplusplus
}
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "nanovg.h"
#include "vert_dx12.h"
#include "frag_aa_dx12.h"
#include "frag_dx12.h"

#pragma pack(push)
#pragma pack(16)
typedef struct D3DNVGfragUniforms
{
    float scissorMat[16];
    float scissorExt[4];
    float scissorScale[4];
    float paintMat[16];
    float extent[4];
    float radius[4];
    float feather[4];
    struct NVGcolor innerCol;
    struct NVGcolor outerCol;
    float strokeMult[4];
    int texType;
    int type;
} D3DNVGfragUniforms;
#pragma pack(pop)

typedef struct VS_CONSTANTS
{
    float dummy[16];
    float viewSize[2];
} VS_CONSTANTS;

enum D3DNVGshaderType {
    NSVG_SHADER_FILLGRAD,
    NSVG_SHADER_FILLIMG,
    NSVG_SHADER_SIMPLE,
    NSVG_SHADER_IMG
};

typedef struct D3DNVGtexture {
    int id;
    dx12_subset_resource* tex;
    int width, height;
    int type;
    int flags;
} D3DNVGtexture;

enum D3DNVGcallType {
    D3DNVG_NONE = 0,
    D3DNVG_FILL,
    D3DNVG_CONVEXFILL,
    D3DNVG_STROKE,
    D3DNVG_TRIANGLES
};

typedef struct D3DNVGcall {
  int type;
  int image;
  int pathOffset;
  int pathCount;
  int triangleOffset;
  int triangleCount;
  int uniformOffset;
} D3DNVGcall;

typedef struct D3DNVGpath {
  int fillOffset;
  int fillCount;
  int strokeOffset;
  int strokeCount;
} D3DNVGpath;

typedef struct D3DNVGBuffer {
    unsigned int MaxBufferEntries;
    unsigned int CurrentBufferEntry;
    dx12_subset_resource* pBuffer;
} D3DNVGBuffer;

typedef struct D3DNVGcontext {
    struct D3DNVGtexture* textures;
    float view[2];
    int ntextures;
    int ctextures;
    int textureId;
    int pSamplerState[4];

    int fragSize;
    int flags;
  int psoId;

    // Per frame buffers
  struct D3DNVGcall* calls;
  int ccalls;
  int ncalls;
  struct D3DNVGpath* paths;
  int cpaths;
  int npaths;
  struct NVGvertex* verts;
  int cverts;
  int nverts;
  unsigned char* uniforms;
  int cuniforms;
  int nuniforms;

    // D3D
    // Geometry
    D3DNVGBuffer VertexBuffer;
  dx12_subset_resource* pFanIndexBuffer;

    // State
    dx12_subset_resource* pVSConstants;
  dx12_subset_resource* pPSConstants;

    dx12_subset* dx12;
} D3DNVGcontext;

//util functions
int D3Dnvg__maxi(int a, int b);
void D3Dnvg_copyMatrix3to4(float* pDest, const float* pSource);
void D3Dnvg__xformToMat3x3(float* m3, float* t);
struct NVGcolor D3Dnvg__premulColor(struct NVGcolor c);
int D3Dnvg__checkError(HRESULT hr, const char* str);

//ctor/dtor

struct NVGcontext* nvgCreateD3D12(dx12_subset* pDX12subset, int flags);
void nvgDeleteD3D12(struct NVGcontext* ctx);

//actual nanovg interface

 int D3Dnvg__renderCreate(D3DNVGcontext* D3D);
void D3Dnvg__renderDelete(D3DNVGcontext* D3D);
 int D3Dnvg__renderCreateTexture(D3DNVGcontext* D3D, int type, int w, int h, int imageFlags, const unsigned char* data);
 int D3Dnvg__renderDeleteTexture(D3DNVGcontext* D3D, int image);
 int D3Dnvg__renderUpdateTexture(D3DNVGcontext* D3D, int image, int x, int y, int w, int h, const unsigned char* data);
 int D3Dnvg__renderGetTextureSize(D3DNVGcontext* D3D, int image, int* w, int* h);
void D3Dnvg__renderCancel(D3DNVGcontext* D3D);
void D3Dnvg__renderFlush(D3DNVGcontext* D3D);
void D3Dnvg__renderFill(D3DNVGcontext* D3D, struct NVGpaint* paint, NVGcompositeOperationState compositeOperation, struct NVGscissor* scissor, float fringe,
  const float* bounds, const struct NVGpath* paths, int npaths);
void D3Dnvg__renderStroke(D3DNVGcontext* D3D, struct NVGpaint* paint, NVGcompositeOperationState compositeOperation, struct NVGscissor* scissor, float fringe,
  float strokeWidth, const struct NVGpath* paths, int npaths);
void D3Dnvg__renderTriangles(D3DNVGcontext* D3D, struct NVGpaint* paint, NVGcompositeOperationState compositeOperation, struct NVGscissor* scissor,
  const struct NVGvertex* verts, int nverts);
void D3Dnvg__renderViewport(D3DNVGcontext* D3D, float width, float height, float pixelRatio = 1);

//dymem funcs

struct D3DNVGtexture* D3Dnvg__allocTexture(D3DNVGcontext* D3D);
struct D3DNVGcall* D3Dnvg__allocCall(D3DNVGcontext* D3D);
int D3Dnvg__allocPaths(D3DNVGcontext* D3D, int n);
int D3Dnvg__allocVerts(D3DNVGcontext* D3D, int n);
int D3Dnvg__allocFragUniforms(D3DNVGcontext* D3D, int n);

//vbuffers
struct NVGvertex* D3Dnvg_beginVertexBuffer(D3DNVGcontext* D3D, unsigned int maxCount, unsigned int* baseOffset);
void D3Dnvg_endVertexBuffer(D3DNVGcontext* D3D);
void D3Dnvg__copyVerts(NVGvertex* pDest, const struct NVGvertex* pSource, unsigned int num);
void D3Dnvg__vset(struct NVGvertex* vtx, float x, float y, float u, float v);
unsigned int D3Dnvg_updateVertexBuffer(D3DNVGBuffer* buffer, const struct NVGvertex* verts, unsigned int nverts);
void D3Dnvg_setBuffers(D3DNVGcontext* D3D, unsigned int dynamicOffset);

void D3Dnvg_buildFanIndices(D3DNVGcontext* D3D);

//tex & uniforms
D3DNVGfragUniforms* nvg__fragUniformPtr(D3DNVGcontext* D3D, int i);
void D3Dnvg__setUniforms(D3DNVGcontext* D3D, int uniformOffset, int image);

struct D3DNVGtexture* D3Dnvg__findTexture(D3DNVGcontext* D3D, int id);
int D3Dnvg__deleteTexture(D3DNVGcontext* D3D, int id);

//inner drawers
int D3Dnvg__convertPaint(D3DNVGcontext* D3D, struct D3DNVGfragUniforms* frag,
  struct NVGpaint* paint, struct NVGscissor* scissor,
  float width, float fringe, float strokeThr);
void D3Dnvg__fill(D3DNVGcontext* D3D, struct D3DNVGcall* call);
void D3Dnvg__convexFill(D3DNVGcontext* D3D, struct D3DNVGcall* call);
void D3Dnvg__stroke(D3DNVGcontext* D3D, struct D3DNVGcall* call);
void D3Dnvg__triangles(D3DNVGcontext* D3D, struct D3DNVGcall* call);

int D3Dnvg__maxVertCount(const struct NVGpath* paths, int npaths);


#ifdef IMPLEMENTED_IMAGE_FUNCS
int nvd3dCreateImageFromHandle(struct NVGcontext* ctx, void* textureId, int w, int h, int flags)
{

    /*struct D3DNVGcontext* gl = (struct D3DNVGcontext*)nvgInternalParams(ctx)->userPtr;
  struct D3DNVGtexture* tex = D3Dnvg__allocTexture(gl);

  if (tex == NULL) return 0;

  tex->type = NVG_TEXTURE_RGBA;
  tex->tex = textureId;
  tex->flags = flags;
  tex->width = w;
  tex->height = h;

  return tex->id;
*/
    return 0;
}

unsigned int nvd3dImageHandle(struct NVGcontext* ctx, int image)
{
    /*
  struct D3DNVGcontext* gl = (struct D3DNVGcontext*)nvgInternalParams(ctx)->userPtr;
  struct D3DNVGtexture* tex = D3Dnvg__findTexture(gl, image);
  return tex->tex;*/
    return 0;
}

void nvd3dImageFlags(struct NVGcontext* ctx, int image, int flags)
{
    /*
  struct D3DNVGcontext* gl = (struct D3DNVGcontext*)nvgInternalParams(ctx)->userPtr;
  struct D3DNVGtexture* tex = D3Dnvg__findTexture(gl, image);
  tex->flags = flags;
    */
}
#endif

#endif //NANOGUI_DX12_BACKEND

#endif //NANOVG_D3D12_H