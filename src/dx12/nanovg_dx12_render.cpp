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
#include "dx12_subset_resource.h"

int D3Dnvg__renderCreate(D3DNVGcontext * D3D)
{
  HRESULT hr;

  if (D3D->flags & NVG_ANTIALIAS)
    D3D->psoId = DX12_SUBSET_PPSO_AA_OFFSET;
  else
    D3D->psoId = DX12_SUBSET_PPSO_DEF;

  D3D->VertexBuffer.pBuffer = new dx12_subset_resource(D3D->dx12);
  D3D->pFanIndexBuffer = new dx12_subset_resource(D3D->dx12);
  D3D->pVSConstants = new dx12_subset_resource(D3D->dx12);
  D3D->pPSConstants = new dx12_subset_resource(D3D->dx12);

  // Todo: Need to find a good value for this, and
  // Use the dynamic buffer fill technnique to handle overflow
  D3D->VertexBuffer.MaxBufferEntries = 1000000;
  D3D->VertexBuffer.CurrentBufferEntry = 0;

  D3Dnvg__checkError(
    D3D->VertexBuffer.pBuffer->vbuffer(sizeof(struct NVGvertex)* D3D->VertexBuffer.MaxBufferEntries, 0),
    "Create Vertex Buffer"
  );

  D3Dnvg__checkError(
    D3D->pFanIndexBuffer->vbuffer(sizeof(UINT32)* D3D->VertexBuffer.MaxBufferEntries, 1),
    "Create Vertex Buffer Static"
  );

  D3Dnvg__checkError(
    D3D->pVSConstants->vbuffer(sizeof(struct VS_CONSTANTS), 0),
    "Create VS Constant Buffer"
  );

  D3Dnvg__checkError(
    D3D->pPSConstants->vbuffer(sizeof(struct D3DNVGfragUniforms), 0),
    "Create PS Constant Buffer"
  );

  D3Dnvg_buildFanIndices(D3D);

  unsigned int alignedBufSz = sizeof(struct D3DNVGfragUniforms);
  if ((alignedBufSz % 16) != 0)
  {
    alignedBufSz += 16 - (alignedBufSz % 16);
  }
  D3D->fragSize = alignedBufSz;

  D3D12_SAMPLER_DESC sampDesc;
  dx12_subset_dheap* srvDH = D3D->dx12->CurrentDHeap(DX12_SUBSET_DHEAP_SPL);

  ZeroMemory(&sampDesc, sizeof(sampDesc));
  sampDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
  sampDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
  sampDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
  sampDesc.MinLOD = 0;
  sampDesc.MaxLOD = D3D12_FLOAT32_MAX;
  sampDesc.MipLODBias = 0.0f;//-1.0f;

  sampDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
  sampDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
  D3D->pSamplerState[0] = srvDH->CreateSampler(&sampDesc);

  sampDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
  sampDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
  D3D->pSamplerState[1] = srvDH->CreateSampler(&sampDesc);

  sampDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
  sampDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
  D3D->pSamplerState[2] = srvDH->CreateSampler(&sampDesc);

  sampDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
  sampDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
  D3D->pSamplerState[3] = srvDH->CreateSampler(&sampDesc);

  return 1;
}

void D3Dnvg__renderDelete(D3DNVGcontext * D3D)
{
  int i;
  if (D3D == NULL)
  {
    return;
  }

  for (i = 0; i < D3D->ntextures; i++)
  {
    if (D3D->textures[i].tex != 0 && (D3D->textures[i].flags & NVG_IMAGE_NODELETE) == 0)
      D3D->textures[i].tex->ModRef(-1);
  }

  D3D->VertexBuffer.pBuffer->ModRef(-1);

  D3D->pVSConstants->ModRef(-1);
  D3D->pPSConstants->ModRef(-1);

  D3D->pFanIndexBuffer->ModRef(-1);

  free(D3D->textures);

  free(D3D->paths);
  free(D3D->verts);
  free(D3D->uniforms);
  free(D3D->calls);

  free(D3D);
}

int D3Dnvg__renderCreateTexture(D3DNVGcontext * D3D, int type, int w, int h, int imageFlags, const unsigned char * data)
{
  struct D3DNVGtexture* tex = D3Dnvg__allocTexture(D3D);

  int pixelWidthBytes;
  HRESULT hr;

  if (tex == NULL)
  {
    return 0;
  }

  tex->tex = new dx12_subset_resource(D3D->dx12);
  tex->width = w;
  tex->height = h;
  tex->type = type;
  tex->flags = imageFlags;

  UINT16 levels = 1;

  if (type == NVG_TEXTURE_RGBA)
  {
    if (imageFlags & NVG_IMAGE_GENERATE_MIPMAPS)
      levels = 0;

    hr = tex->tex->tex2d(w, h, DXGI_FORMAT_R8G8B8A8_UNORM, &levels, 1);

    pixelWidthBytes = 4;
  }
  else
  {
    hr = tex->tex->tex2d(w, h, DXGI_FORMAT_R8_UNORM, &levels, 1);
    pixelWidthBytes = 1;
  }

  if (D3Dnvg__checkError(hr, "create tex"))
  {
    return 0;
  }

  if (data != NULL)
  {
    tex->tex->MapWriteTex2D((void*)data);
  }


  if (data != NULL && levels != 1)
  {
    //TODO
  }

  return tex->id;
}

int D3Dnvg__renderDeleteTexture(D3DNVGcontext * D3D, int image)
{
  return D3Dnvg__deleteTexture(D3D, image);
}

int D3Dnvg__renderUpdateTexture(D3DNVGcontext * D3D, int image, int x, int y, int w, int h, const unsigned char * data)
{
  struct D3DNVGtexture* tex = D3Dnvg__findTexture(D3D, image);
  /*D3D11_BOX box;
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
  D3D_API_6(D3D->pDeviceContext, UpdateSubresource, (ID3D11Resource*)tex->tex, 0, &box, pData, tex->width, tex->width * tex->height);*/

  tex->tex->MapWriteTex2D((void*)data);

  return 1;
}

int D3Dnvg__renderGetTextureSize(D3DNVGcontext * D3D, int image, int * w, int * h)
{
  struct D3DNVGtexture* tex = D3Dnvg__findTexture(D3D, image);
  if (tex == NULL)
  {
    return 0;
  }
  *w = tex->width;
  *h = tex->height;
  return 1;
}

void D3Dnvg__renderCancel(D3DNVGcontext * D3D)
{
  D3D->nverts = 0;
  D3D->npaths = 0;
  D3D->ncalls = 0;
  D3D->nuniforms = 0;
}

void D3Dnvg__renderFlush(D3DNVGcontext * D3D)
{
  int i;

  if (D3D->ncalls > 0)
  {
    unsigned int buffer0Offset = D3Dnvg_updateVertexBuffer(&D3D->VertexBuffer, D3D->verts, D3D->nverts);
    D3Dnvg_setBuffers(D3D, buffer0Offset);

    // Ensure valid state
    D3D->pPSConstants->UseAsCBVatRSIG(DX12_SUBSET_RSIG_IDX_DEFAULT_PS_CBUF, 0);
    D3D->pVSConstants->UseAsCBVatRSIG(DX12_SUBSET_RSIG_IDX_DEFAULT_VS_CBUF, 0);

    D3D->dx12->Use_PPSO(D3D->psoId + DX12_SUBSET_PPSO_DEF_Def_Blend_Cull);

    for (i = 0; i < D3D->ncalls; i++) {
      struct D3DNVGcall* call = &D3D->calls[i];

      if (call->image != 0)
      {
        struct D3DNVGtexture* tex = D3Dnvg__findTexture(D3D, call->image);
        if (tex != NULL)
        {
          D3D->dx12->UseSamplerAtRSIG(DX12_SUBSET_RSIG_IDX_DEFAULT_SPL0, D3D->pSamplerState[(tex->flags & NVG_IMAGE_REPEATX ? 1 : 0) + (tex->flags & NVG_IMAGE_REPEATY ? 2 : 0)]);
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

void D3Dnvg__renderFill(D3DNVGcontext * D3D, NVGpaint * paint, NVGcompositeOperationState compositeOperation, NVGscissor * scissor, float fringe, const float * bounds, const NVGpath * paths, int npaths)
{
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
  }
  else {
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

void D3Dnvg__renderStroke(D3DNVGcontext * D3D, NVGpaint * paint, NVGcompositeOperationState compositeOperation, NVGscissor * scissor, float fringe, float strokeWidth, const NVGpath * paths, int npaths)
{
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
    D3Dnvg__convertPaint(D3D, nvg__fragUniformPtr(D3D, call->uniformOffset + D3D->fragSize), paint, scissor, strokeWidth, fringe, 1.0f - 0.5f / 255.0f);

  }
  else {
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

void D3Dnvg__renderTriangles(D3DNVGcontext * D3D, NVGpaint * paint, NVGcompositeOperationState compositeOperation, NVGscissor * scissor, const NVGvertex * verts, int nverts)
{
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

void D3Dnvg__renderViewport(D3DNVGcontext * D3D, float width, float height, float pixelRatio)
{
  //D3D->alphaMode = alphaBlend;
  VS_CONSTANTS* vc = (VS_CONSTANTS*)D3D->pVSConstants->MapWrite(0, sizeof(VS_CONSTANTS));
  vc->viewSize[0] = (float)width;
  vc->viewSize[1] = (float)height;
}

