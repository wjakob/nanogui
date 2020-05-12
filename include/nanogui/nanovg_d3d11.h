//
// Copyright (c) 2009-2013 Mikko Mononen memon@inside.org
// Port of _gl.h to _d3d11.h by Chris Maughan
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
#ifndef NANOVG_D3D11_H
#define NANOVG_D3D11_H

// Hide nameless struct/union warning for D3D headers
#pragma warning (disable : 4201)
#include <d3d11.h>
#include <dxgi.h>
#pragma warning (default : 4201)

#ifdef __cplusplus
extern "C" {
#endif

// Flag indicating if geoemtry based anti-aliasing is used (may not be needed when using MSAA).
#define NVG_ANTIALIAS 1

// Flag indicating if strokes should be drawn using stencil buffer. The rendering will be a little
// slower, but path overlaps (i.e. self-intersecting or sharp turns) will be drawn just once.
#define NVG_STENCIL_STROKES 2

//struct NVGcontext* nvgCreateD3D11(ID3D11Device* pDevice, int edgeaa);
void nvgDeleteD3D11(struct NVGcontext* ctx);

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

#ifdef NANOVG_D3D11_IMPLEMENTATION
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "nanovg.h"
#include <d3d11.h>

#include "vert_dx11.h"
#include "frag_aa_dx11.h"
#include "frag_dx11.h"

// The cpp calling is much simpler.
// For 'c' calling of DX, we need to do pPtr->lpVtbl->Func(pPtr, ...)
// There's probably a better way... (but note we can't use the IInterace_Method() helpers because
// They won't work when compiled for cpp)
#ifdef __cplusplus
#define D3D_API(p, name, arg1) p->name()
#define D3D_API_1(p, name, arg1) p->name(arg1)
#define D3D_API_2(p, name, arg1, arg2) p->name(arg1, arg2)
#define D3D_API_3(p, name, arg1, arg2, arg3) p->name(arg1, arg2, arg3)
#define D3D_API_4(p, name, arg1, arg2, arg3, arg4) p->name(arg1, arg2, arg3, arg4)
#define D3D_API_5(p, name, arg1, arg2, arg3, arg4, arg5) p->name(arg1, arg2, arg3, arg4, arg5)
#define D3D_API_6(p, name, arg1, arg2, arg3, arg4, arg5, arg6) p->name(arg1, arg2, arg3, arg4, arg5, arg6)
#define D3D_API_RELEASE(p) { if ( (p) ) { (p)->Release(); (p) = NULL; } }
#else
#define D3D_API(p, name) p->lpVtbl->name(p)
#define D3D_API_1(p, name, arg1) p->lpVtbl->name(p, arg1)
#define D3D_API_2(p, name, arg1, arg2) p->lpVtbl->name(p, arg1, arg2)
#define D3D_API_3(p, name, arg1, arg2, arg3) p->lpVtbl->name(p, arg1, arg2, arg3)
#define D3D_API_4(p, name, arg1, arg2, arg3, arg4) p->lpVtbl->name(p, arg1, arg2, arg3, arg4)
#define D3D_API_5(p, name, arg1, arg2, arg3, arg4, arg5) p->lpVtbl->name(p, arg1, arg2, arg3, arg4, arg5)
#define D3D_API_6(p, name, arg1, arg2, arg3, arg4, arg5, arg6) p->lpVtbl->name(p, arg1, arg2, arg3, arg4, arg5, arg6)
#define D3D_API_RELEASE(p) { if ( (p) ) { (p)->lpVtbl->Release((p)); (p) = NULL; } }
#endif

#pragma pack(push)
#pragma pack(16)
struct D3DNVGfragUniforms
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
};
#pragma pack(pop)

struct VS_CONSTANTS
{
    float dummy[16];
    float viewSize[2];
};

enum D3DNVGshaderType {
    NSVG_SHADER_FILLGRAD,
    NSVG_SHADER_FILLIMG,
    NSVG_SHADER_SIMPLE,
    NSVG_SHADER_IMG
};

struct D3DNVGshader {
    ID3D11PixelShader* frag;
    ID3D11VertexShader* vert;
    struct VS_CONSTANTS vc;
};

struct D3DNVGtexture {
    int id;
    ID3D11Texture2D* tex;
    ID3D11ShaderResourceView* resourceView;
    int width, height;
    int type;
    int flags;
};

enum D3DNVGcallType {
    D3DNVG_NONE = 0,
    D3DNVG_FILL,
    D3DNVG_CONVEXFILL,
    D3DNVG_STROKE,
    D3DNVG_TRIANGLES
};

struct D3DNVGcall {
  int type;
  int image;
  int pathOffset;
  int pathCount;
  int triangleOffset;
  int triangleCount;
  int uniformOffset;
};

struct D3DNVGpath {
  int fillOffset;
  int fillCount;
  int strokeOffset;
  int strokeCount;
};

struct D3DNVGBuffer {
    unsigned int MaxBufferEntries;
    unsigned int CurrentBufferEntry;
    ID3D11Buffer* pBuffer;
};

struct D3DNVGcontext {

    struct D3DNVGshader shader;
    struct D3DNVGtexture* textures;
    float view[2];
    int ntextures;
    int ctextures;
    int textureId;
    ID3D11SamplerState* pSamplerState[4];

    int fragSize;
    int flags;

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
    struct D3DNVGBuffer VertexBuffer;
    ID3D11Buffer* pFanIndexBuffer;
    ID3D11InputLayout* pLayoutRenderTriangles;

    // State
    ID3D11Buffer* pVSConstants;
    ID3D11Buffer* pPSConstants;

    ID3D11Device* pDevice;
    ID3D11DeviceContext* pDeviceContext;

    ID3D11BlendState* pBSBlend;
    ID3D11BlendState* pBSNoWrite;

    ID3D11RasterizerState* pRSNoCull;
    ID3D11RasterizerState* pRSCull;

    ID3D11DepthStencilState* pDepthStencilDrawShapes;
    ID3D11DepthStencilState* pDepthStencilDrawAA;
    ID3D11DepthStencilState* pDepthStencilFill;
    ID3D11DepthStencilState* pDepthStencilDefault;
};

static int D3Dnvg__maxi(int a, int b) { return a > b ? a : b; }

static struct D3DNVGtexture* D3Dnvg__allocTexture(struct D3DNVGcontext* D3D)
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
            int ctextures = D3Dnvg__maxi(D3D->ntextures+1, 4) +  D3D->ctextures/2; // 1.5x Overallocate
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

static struct D3DNVGtexture* D3Dnvg__findTexture(struct D3DNVGcontext* D3D, int id)
{
    int i;
    for (i = 0; i < D3D->ntextures; i++)
        if (D3D->textures[i].id == id)
            return &D3D->textures[i];
    return NULL;
}

static int D3Dnvg__deleteTexture(struct D3DNVGcontext* D3D, int id)
{
    int i;
    for (i = 0; i < D3D->ntextures; i++) {
        if (D3D->textures[i].id == id) {
            if (D3D->textures[i].tex != 0 && (D3D->textures[i].flags & NVG_IMAGE_NODELETE) == 0)
            {
                D3D_API_RELEASE(D3D->textures[i].tex);
                D3D_API_RELEASE(D3D->textures[i].resourceView);
            }
            memset(&D3D->textures[i], 0, sizeof(D3D->textures[i]));
            return 1;
        }
    }
    return 0;
}

// BEGIN D3D specific

static void D3Dnvg_copyMatrix3to4(float* pDest, const float* pSource)
{
    unsigned int i;
    for (i = 0; i < 4; i++)
    {
        memcpy(&pDest[i * 4], &pSource[i * 3], sizeof(float) * 3);
    }
}
// END D3D specific

static int D3Dnvg__checkError(HRESULT hr, const char* str)
{
    if (!SUCCEEDED(hr))
    {
        printf("Error %08x after %s\n", hr, str);
        return 1;
    }
    return 0;
}

static int D3Dnvg__createShader(struct D3DNVGcontext* D3D, struct D3DNVGshader* shader, const void* vshader, unsigned int vshader_size, const void* fshader, unsigned int fshader_size)
{
    ID3D11VertexShader* vert = NULL;
    ID3D11PixelShader* frag = NULL;
    HRESULT hr;

    memset(shader, 0, sizeof(*shader));

    // Shader byte code is created at compile time from the .hlsl files.
    // No need for error checks; shader errors can be fixed in the IDE.
    hr = D3D_API_4(D3D->pDevice, CreateVertexShader, vshader, vshader_size, NULL, &vert);
    hr = D3D_API_4(D3D->pDevice, CreatePixelShader, fshader, fshader_size, NULL, &frag);

    shader->vert = vert;
    shader->frag = frag;

    return 1;
}

static void D3Dnvg__deleteShader(struct D3DNVGshader* shader)
{
    D3D_API_RELEASE(shader->vert);
    D3D_API_RELEASE(shader->frag);
}

void D3Dnvg_buildFanIndices(struct D3DNVGcontext* D3D)
{
    UINT32 index0 = 0;
    UINT32 index1 = 1;
    UINT32 index2 = 2;
    UINT32 current = 0;
  D3D11_MAPPED_SUBRESOURCE resource;
  UINT32* pIndices = NULL;

    D3D_API_5(D3D->pDeviceContext, Map, (ID3D11Resource*)D3D->pFanIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
    pIndices = (UINT32*)resource.pData;

    while (current < (D3D->VertexBuffer.MaxBufferEntries - 3))
    {
        pIndices[current++] = index0;
        pIndices[current++] = index1++;
        pIndices[current++] = index2++;
    }
    D3D_API_2(D3D->pDeviceContext, Unmap, (ID3D11Resource*)D3D->pFanIndexBuffer, 0);
}

struct NVGvertex* D3Dnvg_beginVertexBuffer(struct D3DNVGcontext* D3D, unsigned int maxCount, unsigned int* baseOffset)
{
    D3D11_MAPPED_SUBRESOURCE resource;
    if (maxCount >= D3D->VertexBuffer.MaxBufferEntries)
    {
        D3Dnvg__checkError(E_FAIL, "Vertex buffer too small!");
        return NULL;
    }
    if ((D3D->VertexBuffer.CurrentBufferEntry + maxCount) >= D3D->VertexBuffer.MaxBufferEntries)
    {
        *baseOffset = 0;
        D3D->VertexBuffer.CurrentBufferEntry = maxCount;
        D3D_API_5(D3D->pDeviceContext, Map, (ID3D11Resource*)D3D->VertexBuffer.pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
    }
    else
    {
        *baseOffset = D3D->VertexBuffer.CurrentBufferEntry;
        D3D->VertexBuffer.CurrentBufferEntry = *baseOffset + maxCount;
        D3D_API_5(D3D->pDeviceContext, Map, (ID3D11Resource*)D3D->VertexBuffer.pBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &resource);
    }
    return ((struct NVGvertex*)resource.pData + *baseOffset);
}

void D3Dnvg_endVertexBuffer(struct D3DNVGcontext* D3D)
{
    D3D_API_2(D3D->pDeviceContext, Unmap, (ID3D11Resource*)D3D->VertexBuffer.pBuffer, 0);
}

static void D3Dnvg__copyVerts(struct NVGvertex* pDest, const struct NVGvertex* pSource, unsigned int num)
{
    unsigned int i;
    for (i = 0; i < num; i++)
    {
        pDest[i].x = pSource[i].x;
        pDest[i].y = pSource[i].y;
        pDest[i].u = pSource[i].u;
        pDest[i].v = pSource[i].v;
    }
}

static unsigned int D3Dnvg_updateVertexBuffer(ID3D11DeviceContext* pContext, struct D3DNVGBuffer* buffer, const struct NVGvertex* verts, unsigned int nverts)
{
    D3D11_MAPPED_SUBRESOURCE resource;
    unsigned int retEntry;

    if (nverts > buffer->MaxBufferEntries)
    {
        D3Dnvg__checkError(E_FAIL, "Vertex buffer too small!");
        return 0;
    }
    if ((buffer->CurrentBufferEntry + nverts) >= buffer->MaxBufferEntries)
    {
        buffer->CurrentBufferEntry = 0;
        D3D_API_5(pContext, Map, (ID3D11Resource*)buffer->pBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
    }
    else
    {
        D3D_API_5(pContext, Map, (ID3D11Resource*)buffer->pBuffer, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &resource);
    }

    D3Dnvg__copyVerts((((struct NVGvertex*)resource.pData) + buffer->CurrentBufferEntry), (const struct NVGvertex*)verts, nverts);

    D3D_API_2(pContext, Unmap, (ID3D11Resource*)buffer->pBuffer, 0);
    retEntry = buffer->CurrentBufferEntry;
    buffer->CurrentBufferEntry += nverts;
    return retEntry;
}

static void D3Dnvg_setBuffers(struct D3DNVGcontext* D3D, unsigned int dynamicOffset)
{
    ID3D11Buffer* pBuffers[1];
    unsigned int strides[1];
    unsigned int offsets[1];

    pBuffers[0] = D3D->VertexBuffer.pBuffer;
    strides[0] = sizeof(struct NVGvertex);
    offsets[0] = dynamicOffset * sizeof(struct NVGvertex);

    D3D_API_3(D3D->pDeviceContext, IASetIndexBuffer, D3D->pFanIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    D3D_API_5(D3D->pDeviceContext, IASetVertexBuffers, 0, 1, pBuffers, strides, offsets);
    D3D_API_1(D3D->pDeviceContext, IASetInputLayout, D3D->pLayoutRenderTriangles);
}

static int D3Dnvg__renderCreate(void* uptr)
{
    HRESULT hr;
    D3D11_BUFFER_DESC bufferDesc;
    D3D11_RASTERIZER_DESC rasterDesc;
    D3D11_BLEND_DESC blendDesc;
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    D3D11_SAMPLER_DESC sampDesc;

    struct D3DNVGcontext* D3D = (struct D3DNVGcontext*)uptr;

    const D3D11_DEPTH_STENCILOP_DESC frontOp = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_INCR, D3D11_COMPARISON_ALWAYS };
    const D3D11_DEPTH_STENCILOP_DESC backOp = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_DECR, D3D11_COMPARISON_ALWAYS };

    const D3D11_DEPTH_STENCILOP_DESC aaOp = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_EQUAL };
    const D3D11_DEPTH_STENCILOP_DESC fillOp = { D3D11_STENCIL_OP_ZERO, D3D11_STENCIL_OP_ZERO, D3D11_STENCIL_OP_ZERO, D3D11_COMPARISON_NOT_EQUAL };

    const D3D11_DEPTH_STENCILOP_DESC defaultOp = { D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS };

    D3D11_INPUT_ELEMENT_DESC LayoutRenderTriangles[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    if (D3D->flags & NVG_ANTIALIAS) {
        if (D3Dnvg__createShader(D3D, &D3D->shader, g_D3D11VertexShader_Main, sizeof(g_D3D11VertexShader_Main), g_D3D11PixelShaderAA_Main, sizeof(g_D3D11PixelShaderAA_Main)) == 0)
            return 0;
    }
    else {
        if (D3Dnvg__createShader(D3D, &D3D->shader, g_D3D11VertexShader_Main, sizeof(g_D3D11VertexShader_Main), g_D3D11PixelShader_Main, sizeof(g_D3D11PixelShader_Main)) == 0)
            return 0;
    }

    // Todo: Need to find a good value for this, and
    // Use the dynamic buffer fill technnique to handle overflow
    D3D->VertexBuffer.MaxBufferEntries = 1000000;
    D3D->VertexBuffer.CurrentBufferEntry = 0;

    memset(&bufferDesc, 0, sizeof(bufferDesc));
    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = 0;

    // Create the vertex buffer.
    bufferDesc.ByteWidth = sizeof(struct NVGvertex)* D3D->VertexBuffer.MaxBufferEntries;
    hr = D3D_API_3(D3D->pDevice, CreateBuffer, &bufferDesc, NULL, &D3D->VertexBuffer.pBuffer);
    D3Dnvg__checkError(hr, "Create Vertex Buffer");

    bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bufferDesc.ByteWidth = sizeof(UINT32)* D3D->VertexBuffer.MaxBufferEntries;
    hr = D3D_API_3(D3D->pDevice, CreateBuffer, &bufferDesc, NULL, &D3D->pFanIndexBuffer);
    D3Dnvg__checkError(hr, "Create Vertex Buffer Static");

    D3Dnvg_buildFanIndices(D3D);

    hr = D3D_API_5(D3D->pDevice, CreateInputLayout, LayoutRenderTriangles, 2, g_D3D11VertexShader_Main, sizeof(g_D3D11VertexShader_Main), &D3D->pLayoutRenderTriangles);
    D3Dnvg__checkError(hr, "Create Input Layout");

    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bufferDesc.MiscFlags = 0;
    bufferDesc.ByteWidth = sizeof(struct VS_CONSTANTS);
    if ((bufferDesc.ByteWidth % 16) != 0)
    {
        bufferDesc.ByteWidth += 16 - (bufferDesc.ByteWidth % 16);
    }

    hr = D3D_API_3(D3D->pDevice, CreateBuffer, &bufferDesc, NULL, &D3D->pVSConstants);
    D3Dnvg__checkError(hr, "Create Constant Buffer");

    bufferDesc.ByteWidth = sizeof(struct D3DNVGfragUniforms);
    if ((bufferDesc.ByteWidth % 16) != 0)
    {
        bufferDesc.ByteWidth += 16 - (bufferDesc.ByteWidth % 16);
    }
    D3D->fragSize = bufferDesc.ByteWidth;

    hr = D3D_API_3(D3D->pDevice, CreateBuffer, &bufferDesc, NULL, &D3D->pPSConstants);
    D3Dnvg__checkError(hr, "Create Constant Buffer");

    ZeroMemory(&rasterDesc, sizeof(rasterDesc));
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.CullMode = D3D11_CULL_NONE;
    rasterDesc.DepthClipEnable = TRUE;
    rasterDesc.FrontCounterClockwise = TRUE;
    hr = D3D_API_2(D3D->pDevice, CreateRasterizerState, &rasterDesc, &D3D->pRSNoCull);

    rasterDesc.CullMode = D3D11_CULL_BACK;
    hr = D3D_API_2(D3D->pDevice, CreateRasterizerState, &rasterDesc, &D3D->pRSCull);

    ZeroMemory(&blendDesc, sizeof(blendDesc));
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.IndependentBlendEnable = FALSE;
    hr = D3D_API_2(D3D->pDevice, CreateBlendState, &blendDesc, &D3D->pBSBlend);


    blendDesc.RenderTarget[0].BlendEnable = FALSE;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = 0;
    hr = D3D_API_2(D3D->pDevice, CreateBlendState, &blendDesc, &D3D->pBSNoWrite);

    // Stencil A Draw shapes
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
    depthStencilDesc.DepthEnable = FALSE;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    depthStencilDesc.StencilEnable = TRUE;
    depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
    depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

    depthStencilDesc.FrontFace = frontOp;
    depthStencilDesc.BackFace = backOp;

    // No color write
    hr = D3D_API_2(D3D->pDevice, CreateDepthStencilState, &depthStencilDesc, &D3D->pDepthStencilDrawShapes);

    // Draw AA
    depthStencilDesc.FrontFace = aaOp;
    depthStencilDesc.BackFace = aaOp;

    hr = D3D_API_2(D3D->pDevice, CreateDepthStencilState, &depthStencilDesc, &D3D->pDepthStencilDrawAA);

    // Stencil Fill
    depthStencilDesc.FrontFace = fillOp;
    depthStencilDesc.BackFace = fillOp;
    hr = D3D_API_2(D3D->pDevice, CreateDepthStencilState, &depthStencilDesc, &D3D->pDepthStencilFill);

    depthStencilDesc.FrontFace = defaultOp;
    depthStencilDesc.BackFace = defaultOp;
    depthStencilDesc.StencilEnable = FALSE;
    hr = D3D_API_2(D3D->pDevice, CreateDepthStencilState, &depthStencilDesc, &D3D->pDepthStencilDefault);

    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    sampDesc.MipLODBias = 0.0f;//-1.0f;

    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    D3D_API_2(D3D->pDevice, CreateSamplerState, &sampDesc, &D3D->pSamplerState[0]);

    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
  D3D_API_2(D3D->pDevice, CreateSamplerState, &sampDesc, &D3D->pSamplerState[1]);

    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  D3D_API_2(D3D->pDevice, CreateSamplerState, &sampDesc, &D3D->pSamplerState[2]);

    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  D3D_API_2(D3D->pDevice, CreateSamplerState, &sampDesc, &D3D->pSamplerState[3]);

    return 1;
}

static int D3Dnvg__renderCreateTexture(void* uptr, int type, int w, int h, int imageFlags, const unsigned char* data)
{
    struct D3DNVGcontext* D3D = (struct D3DNVGcontext*)uptr;
    struct D3DNVGtexture* tex = D3Dnvg__allocTexture(D3D);
    D3D11_TEXTURE2D_DESC texDesc;
    int pixelWidthBytes;
    HRESULT hr;
    D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;

    if (tex == NULL)
    {
        return 0;
    }

    tex->width = w;
    tex->height = h;
    tex->type = type;
  tex->flags = imageFlags;

    memset(&texDesc, 0, sizeof(texDesc));
    texDesc.ArraySize = 1;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.MipLevels = 1;
    if (type == NVG_TEXTURE_RGBA)
    {
        texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        pixelWidthBytes = 4;

        // Mip maps
        if (imageFlags & NVG_IMAGE_GENERATE_MIPMAPS)
        {
            texDesc.MipLevels = 0;
            texDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
            texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
        }
    }
    else
    {
        texDesc.Format = DXGI_FORMAT_R8_UNORM;
        pixelWidthBytes = 1;
        texDesc.MipLevels = 1;
    }
    texDesc.Height = tex->height;
    texDesc.Width = tex->width;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;

    hr = D3D_API_3(D3D->pDevice, CreateTexture2D, &texDesc, NULL, &tex->tex);
    if (FAILED(hr))
    {
        return 0;
    }

    if (data != NULL)
    {
        D3D_API_6(D3D->pDeviceContext, UpdateSubresource, (ID3D11Resource*)tex->tex, 0, NULL, data, tex->width * pixelWidthBytes, (tex->width * tex->height) * pixelWidthBytes);
    }

    viewDesc.Format = texDesc.Format;
    viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    viewDesc.Texture2D.MipLevels = (UINT)-1;
    viewDesc.Texture2D.MostDetailedMip = 0;

    D3D_API_3(D3D->pDevice, CreateShaderResourceView, (ID3D11Resource*)tex->tex, &viewDesc, &tex->resourceView);

    if (data != NULL && texDesc.MipLevels == 0)
    {
        D3D_API_1(D3D->pDeviceContext, GenerateMips, tex->resourceView);
    }

    if (D3Dnvg__checkError(hr, "create tex"))
        return 0;

    return tex->id;
}

static int D3Dnvg__renderDeleteTexture(void* uptr, int image)
{
    struct D3DNVGcontext* D3D = (struct D3DNVGcontext*)uptr;
    return D3Dnvg__deleteTexture(D3D, image);
}

static int D3Dnvg__renderUpdateTexture(void* uptr, int image, int x, int y, int w, int h, const unsigned char* data)
{
    struct D3DNVGcontext* D3D = (struct D3DNVGcontext*)uptr;
    struct D3DNVGtexture* tex = D3Dnvg__findTexture(D3D, image);
    D3D11_BOX box;
    unsigned int pixelWidthBytes;
    unsigned char* pData;

    if (tex == NULL)
    {
        return 0;
    }

    box.left = x;
    box.right = (x + w);
    box.top = y;
    box.bottom = (y + h);
    box.front = 0;
    box.back = 1;

    if (tex->type == NVG_TEXTURE_RGBA)
    {
        pixelWidthBytes = 4;
    }
    else
    {
        pixelWidthBytes = 1;
    }

    pData = (unsigned char*)data + (y * (tex->width * pixelWidthBytes)) + (x * pixelWidthBytes);
    D3D_API_6(D3D->pDeviceContext, UpdateSubresource, (ID3D11Resource*)tex->tex, 0, &box, pData, tex->width, tex->width * tex->height);

    return 1;
}

static int D3Dnvg__renderGetTextureSize(void* uptr, int image, int* w, int* h)
{
    struct D3DNVGcontext* D3D = (struct D3DNVGcontext*)uptr;
    struct D3DNVGtexture* tex = D3Dnvg__findTexture(D3D, image);
    if (tex == NULL)
    {
        return 0;
    }
    *w = tex->width;
    *h = tex->height;
    return 1;
}

static void D3Dnvg__xformToMat3x3(float* m3, float* t)
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

static struct NVGcolor D3Dnvg__premulColor(struct NVGcolor c)
{
  c.r *= c.a;
  c.g *= c.a;
  c.b *= c.a;
  return c;
}

static int D3Dnvg__convertPaint(struct D3DNVGcontext* D3D, struct D3DNVGfragUniforms* frag,
    struct NVGpaint* paint, struct NVGscissor* scissor,
    float width, float fringe, float strokeThr)
{
    struct D3DNVGtexture* tex = NULL;
    float invxform[6], paintMat[9], scissorMat[9];

    memset(frag, 0, sizeof(*frag));

    frag->innerCol = D3Dnvg__premulColor(paint->innerColor);
    frag->outerCol = D3Dnvg__premulColor(paint->outerColor);

    if (scissor->extent[0] < -0.5f || scissor->extent[1] < -0.5f)
    {
        memset(scissorMat, 0, sizeof(scissorMat));
        frag->scissorExt[0] = 1.0f;
        frag->scissorExt[1] = 1.0f;
        frag->scissorScale[0] = 1.0f;
        frag->scissorScale[1] = 1.0f;
    }
    else
    {
        nvgTransformInverse(invxform, scissor->xform);
        D3Dnvg__xformToMat3x3(scissorMat, invxform);
        frag->scissorExt[0] = scissor->extent[0];
        frag->scissorExt[1] = scissor->extent[1];
        frag->scissorScale[0] = sqrtf(scissor->xform[0] * scissor->xform[0] + scissor->xform[2] * scissor->xform[2]) / fringe;
        frag->scissorScale[1] = sqrtf(scissor->xform[1] * scissor->xform[1] + scissor->xform[3] * scissor->xform[3]) / fringe;
    }
    D3Dnvg_copyMatrix3to4(frag->scissorMat, scissorMat);


    frag->extent[0] = paint->extent[0];
    frag->extent[1] = paint->extent[1];

    frag->strokeMult[0] = (width*0.5f + fringe*0.5f) / fringe;
    frag->strokeMult[1] = strokeThr;

    if (paint->image != 0)
    {
        tex = D3Dnvg__findTexture(D3D, paint->image);
        if (tex == NULL)
        {
            return 0;
        }

        if ((tex->flags & NVG_IMAGE_FLIPY) != 0)
        {
      float flipped[6];
      nvgTransformScale(flipped, 1.0f, -1.0f);
      nvgTransformMultiply(flipped, paint->xform);
      nvgTransformInverse(invxform, flipped);
    }
        else
        {
      nvgTransformInverse(invxform, paint->xform);
    }
    frag->type = NSVG_SHADER_FILLIMG;

    if (tex->type == NVG_TEXTURE_RGBA)
        {
      frag->texType = (tex->flags & NVG_IMAGE_PREMULTIPLIED) ? 0 : 1;
        }
        else
        {
      frag->texType = 2;
        }
    }
    else
    {
        frag->type = NSVG_SHADER_FILLGRAD;
        frag->radius[0] = paint->radius;
        frag->feather[0] = paint->feather;
        nvgTransformInverse(invxform, paint->xform);
    }

    D3Dnvg__xformToMat3x3(paintMat, invxform);
    D3Dnvg_copyMatrix3to4(frag->paintMat, paintMat);

    //D3Dnvg_updateShaders(D3D);

    return 1;
}

static struct D3DNVGfragUniforms* nvg__fragUniformPtr(struct D3DNVGcontext* D3D, int i);

static void D3Dnvg__setUniforms(struct D3DNVGcontext* D3D, int uniformOffset, int image)
{
    struct D3DNVGfragUniforms* frag = nvg__fragUniformPtr(D3D, uniformOffset);

    D3D11_MAPPED_SUBRESOURCE MappedResource;

    // Pixel shader constants
    D3D_API_5(D3D->pDeviceContext, Map, (ID3D11Resource*)D3D->pPSConstants, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
    memcpy(MappedResource.pData, frag, sizeof(struct D3DNVGfragUniforms));
    D3D_API_2(D3D->pDeviceContext, Unmap, (ID3D11Resource*)D3D->pPSConstants, 0);

    if (image != 0)
    {
    struct D3DNVGtexture* tex = D3Dnvg__findTexture(D3D, image);
        if (tex != NULL)
        {
          D3D_API_3(D3D->pDeviceContext, PSSetShaderResources,0, 1, &tex->resourceView);
        }
        else
        {
           // D3D_API_3(D3D->pDeviceContext, PSSetShaderResources,0, 1, NULL);
        }
    }
    else
    {
       // D3D_API_3(D3D->pDeviceContext, PSSetShaderResources,0, 1, NULL);
  }

}

static void D3Dnvg__renderViewport(void* uptr, float width, float height, float pixelRatio=1)
{
    struct D3DNVGcontext* D3D = (struct D3DNVGcontext*)uptr;
    D3D11_MAPPED_SUBRESOURCE mappedResource;

    //D3D->alphaMode = alphaBlend;
    D3D->shader.vc.viewSize[0] = (float)width;
    D3D->shader.vc.viewSize[1] = (float)height;

    // Vertex parameters only change when viewport size changes
    D3D_API_5(D3D->pDeviceContext, Map, (ID3D11Resource*)D3D->pVSConstants, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    memcpy(mappedResource.pData, &D3D->shader.vc, sizeof(struct VS_CONSTANTS));
    D3D_API_2(D3D->pDeviceContext, Unmap, (ID3D11Resource*)D3D->pVSConstants, 0);
}

static void D3Dnvg__fill(struct D3DNVGcontext* D3D, struct D3DNVGcall* call)
{
  struct D3DNVGpath* paths = &D3D->paths[call->pathOffset];
  int i, npaths = call->pathCount;

  // Draw shapes
    D3D_API_2(D3D->pDeviceContext, OMSetDepthStencilState, D3D->pDepthStencilDrawShapes, 0);
    D3D_API_3(D3D->pDeviceContext, OMSetBlendState, D3D->pBSNoWrite, NULL, 0xFFFFFFFF);
  D3D_API_1(D3D->pDeviceContext, RSSetState, D3D->pRSNoCull);

  // set bindpoint for solid loc
  D3Dnvg__setUniforms(D3D, call->uniformOffset, 0);

    D3D_API_1(D3D->pDeviceContext, IASetPrimitiveTopology, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  for (i = 0; i < npaths; i++)
    {
        unsigned int numIndices = ((paths[i].fillCount - 2) * 3);
        assert(numIndices < D3D->VertexBuffer.MaxBufferEntries);
        if (numIndices < D3D->VertexBuffer.MaxBufferEntries)
        {
            D3D_API_3(D3D->pDeviceContext, DrawIndexed, numIndices, 0, paths[i].fillOffset);
        }
    }

    // Draw anti-aliased pixels
    D3D_API_1(D3D->pDeviceContext, RSSetState, D3D->pRSCull);
    D3D_API_1(D3D->pDeviceContext, IASetPrimitiveTopology, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    D3D_API_3(D3D->pDeviceContext, OMSetBlendState, D3D->pBSBlend, NULL, 0xFFFFFFFF);

  D3Dnvg__setUniforms(D3D, call->uniformOffset + D3D->fragSize, call->image);

  if (D3D->flags & NVG_ANTIALIAS)
    {
        D3D_API_2(D3D->pDeviceContext, OMSetDepthStencilState, D3D->pDepthStencilDrawAA, 0);
    // Draw fringes
    for (i = 0; i < npaths; i++)
        {
            D3D_API_2(D3D->pDeviceContext, Draw, paths[i].strokeCount, paths[i].strokeOffset);
        }
  }

  // Draw fill
    D3D_API_1(D3D->pDeviceContext, RSSetState, D3D->pRSNoCull);

    D3D_API_2(D3D->pDeviceContext, OMSetDepthStencilState, D3D->pDepthStencilFill, 0);
  D3D_API_2(D3D->pDeviceContext, Draw, call->triangleCount, call->triangleOffset);

    D3D_API_2(D3D->pDeviceContext, OMSetDepthStencilState, D3D->pDepthStencilDefault, 0);

}

static void D3Dnvg__convexFill(struct D3DNVGcontext* D3D, struct D3DNVGcall* call)
{
  struct D3DNVGpath* paths = &D3D->paths[call->pathOffset];
  int i, npaths = call->pathCount;

    D3Dnvg__setUniforms(D3D, call->uniformOffset, call->image);

    D3D_API_1(D3D->pDeviceContext, IASetPrimitiveTopology, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  for (i = 0; i < npaths; i++)
    {
        // Draws a fan using indices to fake it up, since there isn't a fan primitive in D3D11.
    if (paths[i].fillCount > 2)
    {
      unsigned int numIndices = ((paths[i].fillCount - 2) * 3);
      assert(numIndices < D3D->VertexBuffer.MaxBufferEntries);
      if (numIndices < D3D->VertexBuffer.MaxBufferEntries)
      {
        D3D_API_3(D3D->pDeviceContext, DrawIndexed, numIndices, 0, paths[i].fillOffset);
      }
    }
    }

    D3D_API_1(D3D->pDeviceContext, IASetPrimitiveTopology, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    if (D3D->flags & NVG_ANTIALIAS)
    {
    // Draw fringes
    for (i = 0; i < npaths; i++)
        {
            D3D_API_2(D3D->pDeviceContext, Draw, paths[i].strokeCount, paths[i].strokeOffset);
        }
  }
}

static void D3Dnvg__stroke(struct D3DNVGcontext* D3D, struct D3DNVGcall* call)
{
  struct D3DNVGpath* paths = &D3D->paths[call->pathOffset];
  int npaths = call->pathCount, i;

    D3D_API_1(D3D->pDeviceContext, IASetPrimitiveTopology, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

  if (D3D->flags & NVG_STENCIL_STROKES)
    {
        // Fill the stroke base without overlap
        D3D_API_2(D3D->pDeviceContext, OMSetDepthStencilState, D3D->pDepthStencilDefault, 0);

        D3Dnvg__setUniforms(D3D, call->uniformOffset + D3D->fragSize, call->image);
    for (i = 0; i < npaths; i++)
        {
            D3D_API_2(D3D->pDeviceContext, Draw, paths[i].strokeCount, paths[i].strokeOffset);
        }

    // Draw anti-aliased pixels.
    D3Dnvg__setUniforms(D3D, call->uniformOffset, call->image);
    D3D_API_2(D3D->pDeviceContext, OMSetDepthStencilState, D3D->pDepthStencilDrawAA, 0);
      for (i = 0; i < npaths; i++)
        {
            D3D_API_2(D3D->pDeviceContext, Draw, paths[i].strokeCount, paths[i].strokeOffset);
        }

    // Clear stencil buffer.
        D3D_API_2(D3D->pDeviceContext, OMSetDepthStencilState, D3D->pDepthStencilFill, 0);
    for (i = 0; i < npaths; i++)
        {
            D3D_API_2(D3D->pDeviceContext, Draw, paths[i].strokeCount, paths[i].strokeOffset);
        }

        D3D_API_2(D3D->pDeviceContext, OMSetDepthStencilState, D3D->pDepthStencilDefault, 0);
  }
    else
    {
        D3Dnvg__setUniforms(D3D, call->uniformOffset, call->image);
    // Draw Strokes
    for (i = 0; i < npaths; i++)
        {
            D3D_API_2(D3D->pDeviceContext, Draw, paths[i].strokeCount, paths[i].strokeOffset);
        }
  }
}

static void D3Dnvg__triangles(struct D3DNVGcontext* D3D, struct D3DNVGcall* call)
{
    D3D_API_1(D3D->pDeviceContext, IASetPrimitiveTopology, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3Dnvg__setUniforms(D3D, call->uniformOffset, call->image);

    D3D_API_2(D3D->pDeviceContext, Draw, call->triangleCount, call->triangleOffset);
}

static void D3Dnvg__renderCancel(void* uptr)
{
  struct D3DNVGcontext* D3D = (struct D3DNVGcontext*)uptr;
    D3D->nverts = 0;
  D3D->npaths = 0;
  D3D->ncalls = 0;
  D3D->nuniforms = 0;
}

static void D3Dnvg__renderFlush(void* uptr)
{
  struct D3DNVGcontext* D3D = (struct D3DNVGcontext*)uptr;
  int i;

  if (D3D->ncalls > 0)
    {
        unsigned int buffer0Offset = D3Dnvg_updateVertexBuffer(D3D->pDeviceContext, &D3D->VertexBuffer, D3D->verts, D3D->nverts);
        D3Dnvg_setBuffers(D3D, buffer0Offset);

        // Ensure valid state
        D3D_API_3(D3D->pDeviceContext, PSSetConstantBuffers, 0, 1, &D3D->pPSConstants);
        D3D_API_3(D3D->pDeviceContext, VSSetConstantBuffers, 0, 1, &D3D->pVSConstants);

        D3D_API_3(D3D->pDeviceContext, PSSetShader, D3D->shader.frag, NULL, 0);
        D3D_API_3(D3D->pDeviceContext, VSSetShader, D3D->shader.vert, NULL, 0);

         // Draw shapes
        D3D_API_2(D3D->pDeviceContext, OMSetDepthStencilState, D3D->pDepthStencilDefault, 0);
        D3D_API_3(D3D->pDeviceContext, OMSetBlendState, D3D->pBSBlend, NULL, 0xFFFFFFFF);

        D3D_API_1(D3D->pDeviceContext, RSSetState, D3D->pRSCull);

    for (i = 0; i < D3D->ncalls; i++) {
      struct D3DNVGcall* call = &D3D->calls[i];

      if (call->image != 0)
      {
        struct D3DNVGtexture* tex = D3Dnvg__findTexture(D3D, call->image);
        if (tex != NULL)
        {
          D3D_API_3(D3D->pDeviceContext, PSSetSamplers, 0, 1, &D3D->pSamplerState[(tex->flags & NVG_IMAGE_REPEATX ? 1 : 0) + (tex->flags & NVG_IMAGE_REPEATY ? 2 : 0)]);
        }
      }

      if (call->type == D3DNVG_FILL)
        D3Dnvg__fill(D3D, call);
      else if (call->type == D3DNVG_CONVEXFILL)
        D3Dnvg__convexFill(D3D, call);
      else if (call->type == D3DNVG_STROKE)
        D3Dnvg__stroke(D3D, call);
      else if (call->type == D3DNVG_TRIANGLES)
        D3Dnvg__triangles(D3D, call);
    }
    }

  // Reset calls
  D3D->nverts = 0;
  D3D->npaths = 0;
  D3D->ncalls = 0;
    D3D->nuniforms = 0;
}

static int D3Dnvg__maxVertCount(const struct NVGpath* paths, int npaths)
{
  int i, count = 0;
  for (i = 0; i < npaths; i++) {
    count += paths[i].nfill;
    count += paths[i].nstroke;
  }
  return count;
}

static struct D3DNVGcall* D3Dnvg__allocCall(struct D3DNVGcontext* D3D)
{
  struct D3DNVGcall* ret = NULL;
  if (D3D->ncalls+1 > D3D->ccalls) {
    struct D3DNVGcall* calls;
    int ccalls = D3Dnvg__maxi(D3D->ncalls+1, 128) + D3D->ccalls/2; // 1.5x Overallocate
    calls = (struct D3DNVGcall*)realloc(D3D->calls, sizeof(struct D3DNVGcall) * ccalls);
    if (calls == NULL) return NULL;
    D3D->calls = calls;
    D3D->ccalls = ccalls;
  }
  ret = &D3D->calls[D3D->ncalls++];
  memset(ret, 0, sizeof(struct D3DNVGcall));
  return ret;
}

static int D3Dnvg__allocPaths(struct D3DNVGcontext* D3D, int n)
{
  int ret = 0;
  if (D3D->npaths+n > D3D->cpaths) {
    struct D3DNVGpath* paths;
    int cpaths = D3Dnvg__maxi(D3D->npaths + n, 128) + D3D->cpaths/2; // 1.5x Overallocate
    paths = (struct D3DNVGpath*)realloc(D3D->paths, sizeof(struct D3DNVGpath) * cpaths);
    if (paths == NULL) return -1;
    D3D->paths = paths;
    D3D->cpaths = cpaths;
  }
  ret = D3D->npaths;
  D3D->npaths += n;
  return ret;
}

static int D3Dnvg__allocVerts(struct D3DNVGcontext* D3D, int n)
{
  int ret = 0;
  if (D3D->nverts+n > D3D->cverts) {
    struct NVGvertex* verts;
    int cverts = D3Dnvg__maxi(D3D->nverts + n, 4096) + D3D->cverts/2; // 1.5x Overallocate
    verts = (struct NVGvertex*)realloc(D3D->verts, sizeof(struct NVGvertex) * cverts);
    if (verts == NULL) return -1;
    D3D->verts = verts;
    D3D->cverts = cverts;
  }
  ret = D3D->nverts;
  D3D->nverts += n;
  return ret;
}

static int D3Dnvg__allocFragUniforms(struct D3DNVGcontext* D3D, int n)
{
  int ret = 0, structSize = D3D->fragSize;
  if (D3D->nuniforms+n > D3D->cuniforms) {
    unsigned char* uniforms;
    int cuniforms = D3Dnvg__maxi(D3D->nuniforms+n, 128) + D3D->cuniforms/2; // 1.5x Overallocate
    uniforms = (unsigned char*)realloc(D3D->uniforms, structSize * cuniforms);
    if (uniforms == NULL) return -1;
    D3D->uniforms = uniforms;
    D3D->cuniforms = cuniforms;
  }
  ret = D3D->nuniforms * structSize;
  D3D->nuniforms += n;
  return ret;
}

static struct D3DNVGfragUniforms* nvg__fragUniformPtr(struct D3DNVGcontext* D3D, int i)
{
  return (struct D3DNVGfragUniforms*)&D3D->uniforms[i];
}

static void D3Dnvg__vset(struct NVGvertex* vtx, float x, float y, float u, float v)
{
  vtx->x = x;
  vtx->y = y;
  vtx->u = u;
  vtx->v = v;
}

static void D3Dnvg__renderFill(void* uptr, struct NVGpaint* paint, NVGcompositeOperationState compositeOperation, struct NVGscissor* scissor, float fringe,
                const float* bounds, const struct NVGpath* paths, int npaths)
{
  struct D3DNVGcontext* D3D = (struct D3DNVGcontext*)uptr;
  struct D3DNVGcall* call = D3Dnvg__allocCall(D3D);
  struct NVGvertex* quad;
  struct D3DNVGfragUniforms* frag;
  int i, maxverts, offset;

  if (call == NULL) return;

  call->type = D3DNVG_FILL;
  call->pathOffset = D3Dnvg__allocPaths(D3D, npaths);
  if (call->pathOffset == -1) goto error;
  call->pathCount = npaths;
  call->image = paint->image;

  if (npaths == 1 && paths[0].convex)
    call->type = D3DNVG_CONVEXFILL;

  // Allocate vertices for all the paths.
  maxverts = D3Dnvg__maxVertCount(paths, npaths) + 6;
  offset = D3Dnvg__allocVerts(D3D, maxverts);
  if (offset == -1) goto error;

  for (i = 0; i < npaths; i++) {
    struct D3DNVGpath* copy = &D3D->paths[call->pathOffset + i];
    const struct NVGpath* path = &paths[i];
    memset(copy, 0, sizeof(struct D3DNVGpath));
    if (path->nfill > 0) {
      copy->fillOffset = offset;
      copy->fillCount = path->nfill;
      memcpy(&D3D->verts[offset], path->fill, sizeof(struct NVGvertex) * path->nfill);
      offset += path->nfill;
    }
    if (path->nstroke > 0) {
      copy->strokeOffset = offset;
      copy->strokeCount = path->nstroke;
      memcpy(&D3D->verts[offset], path->stroke, sizeof(struct NVGvertex) * path->nstroke);
      offset += path->nstroke;
    }
  }

  // Quad
  call->triangleOffset = offset;
  call->triangleCount = 6;
  quad = &D3D->verts[call->triangleOffset];
  D3Dnvg__vset(&quad[0], bounds[0], bounds[3], 0.5f, 1.0f);
  D3Dnvg__vset(&quad[1], bounds[2], bounds[3], 0.5f, 1.0f);
  D3Dnvg__vset(&quad[2], bounds[2], bounds[1], 0.5f, 1.0f);

  D3Dnvg__vset(&quad[3], bounds[0], bounds[3], 0.5f, 1.0f);
  D3Dnvg__vset(&quad[4], bounds[2], bounds[1], 0.5f, 1.0f);
  D3Dnvg__vset(&quad[5], bounds[0], bounds[1], 0.5f, 1.0f);

  // Setup uniforms for draw calls
  if (call->type == D3DNVG_FILL) {
    call->uniformOffset = D3Dnvg__allocFragUniforms(D3D, 2);
    if (call->uniformOffset == -1) goto error;
    // Simple shader for stencil
    frag = nvg__fragUniformPtr(D3D, call->uniformOffset);
    memset(frag, 0, sizeof(*frag));
    frag->strokeMult[1] = -1.0f;
    frag->type = NSVG_SHADER_SIMPLE;
    // Fill shader
    D3Dnvg__convertPaint(D3D, nvg__fragUniformPtr(D3D, call->uniformOffset + D3D->fragSize), paint, scissor, fringe, fringe, -1.0f);
  } else {
    call->uniformOffset = D3Dnvg__allocFragUniforms(D3D, 1);
    if (call->uniformOffset == -1) goto error;
    // Fill shader
    D3Dnvg__convertPaint(D3D, nvg__fragUniformPtr(D3D, call->uniformOffset), paint, scissor, fringe, fringe, -1.0f);
  }

  return;

error:
  // We get here if call alloc was ok, but something else is not.
  // Roll back the last call to prevent drawing it.
  if (D3D->ncalls > 0) D3D->ncalls--;
}

static void D3Dnvg__renderStroke(void* uptr, struct NVGpaint* paint, NVGcompositeOperationState compositeOperation, struct NVGscissor* scissor, float fringe,
                float strokeWidth, const struct NVGpath* paths, int npaths)
{
  struct D3DNVGcontext* D3D = (struct D3DNVGcontext*)uptr;
  struct D3DNVGcall* call = D3Dnvg__allocCall(D3D);
  int i, maxverts, offset;

  if (call == NULL) return;

  call->type = D3DNVG_STROKE;
  call->pathOffset = D3Dnvg__allocPaths(D3D, npaths);
  if (call->pathOffset == -1) goto error;
  call->pathCount = npaths;
  call->image = paint->image;

  // Allocate vertices for all the paths.
  maxverts = D3Dnvg__maxVertCount(paths, npaths);
  offset = D3Dnvg__allocVerts(D3D, maxverts);
  if (offset == -1) goto error;

  for (i = 0; i < npaths; i++) {
    struct D3DNVGpath* copy = &D3D->paths[call->pathOffset + i];
    const struct NVGpath* path = &paths[i];
    memset(copy, 0, sizeof(struct D3DNVGpath));
    if (path->nstroke) {
      copy->strokeOffset = offset;
      copy->strokeCount = path->nstroke;
      memcpy(&D3D->verts[offset], path->stroke, sizeof(struct NVGvertex) * path->nstroke);
      offset += path->nstroke;
    }
  }

  if (D3D->flags & NVG_STENCIL_STROKES) {
    // Fill shader
    call->uniformOffset = D3Dnvg__allocFragUniforms(D3D, 2);
    if (call->uniformOffset == -1) goto error;

    D3Dnvg__convertPaint(D3D, nvg__fragUniformPtr(D3D, call->uniformOffset), paint, scissor, strokeWidth, fringe, -1.0f);
    D3Dnvg__convertPaint(D3D, nvg__fragUniformPtr(D3D, call->uniformOffset + D3D->fragSize), paint, scissor, strokeWidth, fringe, 1.0f - 0.5f/255.0f);

  } else {
    // Fill shader
    call->uniformOffset = D3Dnvg__allocFragUniforms(D3D, 1);
    if (call->uniformOffset == -1) goto error;
    D3Dnvg__convertPaint(D3D, nvg__fragUniformPtr(D3D, call->uniformOffset), paint, scissor, strokeWidth, fringe, -1.0f);
  }

  return;

error:
  // We get here if call alloc was ok, but something else is not.
  // Roll back the last call to prevent drawing it.
  if (D3D->ncalls > 0) D3D->ncalls--;
}

static void D3Dnvg__renderTriangles(void* uptr, struct NVGpaint* paint, NVGcompositeOperationState compositeOperation, struct NVGscissor* scissor,
                   const struct NVGvertex* verts, int nverts)
{
  struct D3DNVGcontext* D3D = (struct D3DNVGcontext*)uptr;
  struct D3DNVGcall* call = D3Dnvg__allocCall(D3D);
  struct D3DNVGfragUniforms* frag;

  if (call == NULL) return;

  call->type = D3DNVG_TRIANGLES;
  call->image = paint->image;

  // Allocate vertices for all the paths.
  call->triangleOffset = D3Dnvg__allocVerts(D3D, nverts);
  if (call->triangleOffset == -1) goto error;
  call->triangleCount = nverts;

  memcpy(&D3D->verts[call->triangleOffset], verts, sizeof(struct NVGvertex) * nverts);

  // Fill shader
  call->uniformOffset = D3Dnvg__allocFragUniforms(D3D, 1);
  if (call->uniformOffset == -1) goto error;
  frag = nvg__fragUniformPtr(D3D, call->uniformOffset);
  D3Dnvg__convertPaint(D3D, frag, paint, scissor, 1.0f, 1.0f, -1.0f);
  frag->type = NSVG_SHADER_IMG;

  return;

error:
  // We get here if call alloc was ok, but something else is not.
  // Roll back the last call to prevent drawing it.
  if (D3D->ncalls > 0) D3D->ncalls--;
}

static void D3Dnvg__renderDelete(void* uptr)
{
    struct D3DNVGcontext* D3D = (struct D3DNVGcontext*)uptr;
    int i;
    if (D3D == NULL)
    {
        return;
    }

    D3Dnvg__deleteShader(&D3D->shader);

    for (i = 0; i < D3D->ntextures; i++)
    {
        if (D3D->textures[i].tex != 0 && (D3D->textures[i].flags & NVG_IMAGE_NODELETE) == 0)
        {
            D3D_API_RELEASE(D3D->textures[i].tex);
            D3D_API_RELEASE(D3D->textures[i].resourceView);
        }
    }
    for (i = 0; i < 4; i++)
  {
    D3D_API_RELEASE(D3D->pSamplerState[i]);
  }

    D3D_API_RELEASE(D3D->VertexBuffer.pBuffer);

    D3D_API_RELEASE(D3D->pVSConstants);
    D3D_API_RELEASE(D3D->pPSConstants);

    D3D_API_RELEASE(D3D->pFanIndexBuffer);
    D3D_API_RELEASE(D3D->pLayoutRenderTriangles);

    D3D_API_RELEASE(D3D->pBSBlend);
    D3D_API_RELEASE(D3D->pBSNoWrite);
    D3D_API_RELEASE(D3D->pRSCull);
    D3D_API_RELEASE(D3D->pRSNoCull);
    D3D_API_RELEASE(D3D->pDepthStencilDrawShapes);
    D3D_API_RELEASE(D3D->pDepthStencilDrawAA);
    D3D_API_RELEASE(D3D->pDepthStencilFill);
    D3D_API_RELEASE(D3D->pDepthStencilDefault);

    // We took a reference to this
    // Don't delete the device though.
    D3D_API_RELEASE(D3D->pDeviceContext);

    free(D3D->textures);

    free(D3D->paths);
    free(D3D->verts);
    free(D3D->uniforms);
    free(D3D->calls);

    free(D3D);
}


struct NVGcontext* nvgCreateD3D11(ID3D11Device* pDevice, int flags)
{
    struct NVGparams params;
    struct NVGcontext* ctx = NULL;
    struct D3DNVGcontext* D3D = (struct D3DNVGcontext*)malloc(sizeof(struct D3DNVGcontext));
    if (D3D == NULL)
    {
        goto error;
    }
    memset(D3D, 0, sizeof(struct D3DNVGcontext));
    D3D->pDevice = pDevice;
    D3D_API_1(pDevice, GetImmediateContext, &D3D->pDeviceContext);

    memset(&params, 0, sizeof(params));
    params.renderCreate = D3Dnvg__renderCreate;
    params.renderCreateTexture = D3Dnvg__renderCreateTexture;
    params.renderDeleteTexture = D3Dnvg__renderDeleteTexture;
    params.renderUpdateTexture = D3Dnvg__renderUpdateTexture;
    params.renderGetTextureSize = D3Dnvg__renderGetTextureSize;
    params.renderViewport = D3Dnvg__renderViewport;
    params.renderCancel = D3Dnvg__renderCancel;
    params.renderFlush = D3Dnvg__renderFlush;
    params.renderFill = D3Dnvg__renderFill;
    params.renderStroke = D3Dnvg__renderStroke;
    params.renderTriangles = D3Dnvg__renderTriangles;
    params.renderDelete = D3Dnvg__renderDelete;
    params.userPtr = D3D;
    params.edgeAntiAlias = flags & NVG_ANTIALIAS ? 1 : 0;

    D3D->flags = flags;

    ctx = nvgCreateInternal(&params);
    if (ctx == NULL) goto error;

    return ctx;

error:
    // 'D3D' is freed by nvgDeleteInternal.
    if (ctx != NULL) nvgDeleteInternal(ctx);
    return NULL;
}

void nvgDeleteD3D11(struct NVGcontext* ctx)
{
    nvgDeleteInternal(ctx);
}

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

#endif //NANOVG_D3D11_IMPLEMENTATION
#endif //NANOVG_D3D11_H