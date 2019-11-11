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
#include "dx12_subset_resource.h"

dx12_subset_resource::dx12_subset_resource(dx12_subset * parent)
{
  dx12 = parent;
  mappedPtr = 0;
  refs = 1;

  srvHeap = NULL;
  dheapHH.ptr = NULL;
}

dx12_subset_resource::~dx12_subset_resource()
{
  if (srvHeap)
  {
    srvHeap->FreeSlot(srvId);
  }

  if (dheapHH.ptr != 0)
  {
    dx12_subset_dheap* tdh;
    if (res->GetDesc().Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
    {
      tdh = dx12->CurrentDHeap(DX12_SUBSET_DHEAP_DSV);
    }
    else {
      tdh = dx12->CurrentDHeap(DX12_SUBSET_DHEAP_RTV);
    }

    tdh->FreeSlotByPtr(dheapHH);
  }
}

void dx12_subset_resource::ModRef(int v)
{
  refs += v;

  if (!refs)
  {
    dx12->ReleaseAfterFrameEnd(res);
    delete this;
  }
}

HRESULT dx12_subset_resource::zbuf(DXGI_FORMAT fmt, float clearV, UINT width, UINT height, DXGI_FORMAT clearVFmt)
{
  D3D12_CLEAR_VALUE optimizedClearValue = { clearVFmt, { clearV, 0 } };

  D3D12_HEAP_PROPERTIES rhCfg = dx12->GetResourceHeap(D3D12_HEAP_TYPE_DEFAULT);

  D3D12_RESOURCE_DESC rsDesc = {
    D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0,
    width, height, 1, 1,
    fmt, {1, 0},
    D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
  };

  stateCache = D3D12_RESOURCE_STATE_DEPTH_WRITE;

  HRESULT hr = dx12->dev->CreateCommittedResource(
    &rhCfg,
    D3D12_HEAP_FLAG_NONE,
    &rsDesc,
    D3D12_RESOURCE_STATE_DEPTH_WRITE,
    &optimizedClearValue,
    IID_PPV_ARGS(&res)
  );

  if (FAILED(hr))
    return hr;

  dx12_subset_dheap* tdh = dx12->CurrentDHeap(DX12_SUBSET_DHEAP_DSV);

  //megai2: fmt is hardcoded here
  D3D12_DEPTH_STENCIL_VIEW_DESC dsvDsc = { DXGI_FORMAT_D24_UNORM_S8_UINT, D3D12_DSV_DIMENSION_TEXTURE2D , D3D12_DSV_FLAG_NONE };
  dsvDsc.Texture2D.MipSlice = 0;

  UINT32 slot = tdh->CreateDSV(res, &dsvDsc);
  dheapHH = tdh->GetDHeapHandle(slot);

  /*srvHeap = dx12->CurrentDHeap(DX12_SUBSET_DHEAP_SRV);
  srvId = srvHeap->CreateSRV(res, 0);*/

  return hr;
}

HRESULT dx12_subset_resource::rtgt(DXGI_FORMAT fmt, float * clearV, UINT width, UINT height)
{
  D3D12_CLEAR_VALUE optimizedClearValue = { fmt, {clearV[0], clearV[1], clearV[2], clearV[3]} };

  D3D12_HEAP_PROPERTIES rhCfg = dx12->GetResourceHeap(D3D12_HEAP_TYPE_DEFAULT);

  D3D12_RESOURCE_DESC rsDesc = {
    D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0,
    width, height, 1, 1,
    fmt, {1, 0},
    D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
  };

  stateCache = D3D12_RESOURCE_STATE_RENDER_TARGET;

  HRESULT hr = dx12->dev->CreateCommittedResource(
    &rhCfg,
    D3D12_HEAP_FLAG_NONE,
    &rsDesc,
    D3D12_RESOURCE_STATE_RENDER_TARGET,
    &optimizedClearValue,
    IID_PPV_ARGS(&res)
  );

  if (FAILED(hr))
    return hr;

  dx12_subset_dheap* tdh = dx12->CurrentDHeap(DX12_SUBSET_DHEAP_RTV);
  UINT32 slot = tdh->CreateRTV(res, 0);
  dheapHH = tdh->GetDHeapHandle(slot);

  srvHeap = dx12->CurrentDHeap(DX12_SUBSET_DHEAP_SRV);
  srvId = srvHeap->CreateSRV(res, 0);

  return hr;
}

HRESULT dx12_subset_resource::buffer(size_t size, D3D12_HEAP_TYPE heap)
{
  if ((size & 0xFF) != 0)
  {
    size = (size  & ~0xFF) + 0x100;
  }

  D3D12_HEAP_PROPERTIES rhCfg = dx12->GetResourceHeap(heap);

  D3D12_RESOURCE_DESC rsDesc = {
    D3D12_RESOURCE_DIMENSION_BUFFER, 0,
    size, 1, 1, 1,
    DXGI_FORMAT_UNKNOWN, {1, 0},
    D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_NONE
  };

  stateCache = D3D12_RESOURCE_STATE_GENERIC_READ;

  return dx12->dev->CreateCommittedResource(
    &rhCfg,
    D3D12_HEAP_FLAG_NONE,
    &rsDesc,
    D3D12_RESOURCE_STATE_GENERIC_READ,
    0,
    IID_PPV_ARGS(&res)
  );
}

HRESULT dx12_subset_resource::vbuffer(size_t size, int isIB)
{
  HRESULT hr = buffer(size, D3D12_HEAP_TYPE_DEFAULT);

  if (FAILED(hr))
    return hr;

  if (isIB)
  {
    bindData.i.Format = DXGI_FORMAT_R32_UINT;
    bindData.i.SizeInBytes = size;
  }
  else {
    bindData.v.SizeInBytes = size;
    bindData.v.StrideInBytes = 0;
  }

  bindData.i.BufferLocation = res->GetGPUVirtualAddress();

  return hr;
}

HRESULT dx12_subset_resource::tex2d(UINT width, UINT height, DXGI_FORMAT fmt, UINT16 * levels, UINT arrSz)
{
  D3D12_HEAP_PROPERTIES rhCfg = dx12->GetResourceHeap(D3D12_HEAP_TYPE_DEFAULT);

  D3D12_RESOURCE_DESC rsDesc = {
    D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0,
    width, height, (UINT16)arrSz, *levels,
    fmt, {1, 0},
    D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_NONE
  };

  stateCache = D3D12_RESOURCE_STATE_COMMON;

  HRESULT hr = dx12->dev->CreateCommittedResource(
    &rhCfg,
    D3D12_HEAP_FLAG_NONE,
    &rsDesc,
    D3D12_RESOURCE_STATE_COMMON,
    NULL,
    IID_PPV_ARGS(&res)
  );

  srvHeap = dx12->CurrentDHeap(DX12_SUBSET_DHEAP_SRV);
  srvId = srvHeap->CreateSRV(res, 0);

  return hr;
}

void dx12_subset_resource::ClearAsRTV(FLOAT * color4f, D3D12_RECT * clearRect)
{
  const float* cc4f;
  cc4f = color4f;

  dx12->FrameCL()->ClearRenderTargetView(dheapHH, cc4f, clearRect != 0 ? 1 : 0, (const D3D12_RECT*)clearRect);
}

void dx12_subset_resource::ClearAsDSV(FLOAT Depth, UINT8 Stencil, D3D12_CLEAR_FLAGS flag, D3D12_RECT * clearRect)
{
  dx12->FrameCL()->ClearDepthStencilView(dheapHH, flag, Depth, Stencil, clearRect != 0 ? 1 : 0, (const D3D12_RECT*)clearRect);
}

void dx12_subset_resource::SetIB()
{
  dx12->FrameCL()->IASetIndexBuffer(&bindData.i);
}

void dx12_subset_resource::SetVB(int stream, int offset, int stride)
{
  D3D12_VERTEX_BUFFER_VIEW bindDataLocal;

  bindDataLocal = bindData.v;
  bindDataLocal.StrideInBytes = stride;

  if (offset != 0)
  {
    bindDataLocal.SizeInBytes -= offset;
    bindDataLocal.BufferLocation += offset;
  }

  dx12->FrameCL()->IASetVertexBuffers(stream, 1, &bindDataLocal);
}

void dx12_subset_resource::UseAsSRVatRSIG(int rsigIdx)
{
  BTransit4(D3D12_RESOURCE_STATE_GENERIC_READ);

  dx12->FrameCL()->SetGraphicsRootDescriptorTable(rsigIdx, srvHeap->GetGPUDHeapHandle(srvId));
}

void dx12_subset_resource::UseAsCBVatRSIG(int rsigIdx, int offset)
{
  dx12->FrameCL()->SetGraphicsRootConstantBufferView(rsigIdx, bindData.i.BufferLocation + offset);
}

D3D12_CPU_DESCRIPTOR_HANDLE * dx12_subset_resource::GetHHforRTDS(D3D12_RESOURCE_STATES state)
{
  BTransit4(state);

  return &dheapHH;
}

UINT64 AlignValueByPow2(UINT64 val, UINT64 pow2val)
{
  UINT64 mask = (pow2val - 1);
  if (val & mask)
    return (val & ~mask) + pow2val;
  else
    return val;
}

void dx12_subset_resource::MapWriteTex2D(void * data)
{
  D3D12_RESOURCE_DESC dsc = res->GetDesc();

  UINT wPitch = ((dsc.Format == DXGI_FORMAT_R8G8B8A8_UNORM)*3 + 1) * dsc.Width;

  D3D12_PLACED_SUBRESOURCE_FOOTPRINT subresFootprints;

  dx12->dev->GetCopyableFootprints(
    &dsc,
    0,
    1,
    0,
    &subresFootprints,
    NULL,
    NULL,
    NULL
  );

  UINT64 ul_memory_space = AlignValueByPow2(subresFootprints.Footprint.RowPitch*dsc.Height, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);

  BTransit4(D3D12_RESOURCE_STATE_COPY_DEST);

  UINT64 ulOffset = 0;
  dx12_subset_resource* ul = dx12->GetUploadBuffer(ul_memory_space, &ulOffset, 512);
  mappedPtr = ul->UploadTexFR(this, ulOffset, 0);

  BTransit4(D3D12_RESOURCE_STATE_GENERIC_READ);

  for (UINT i = 0; i != dsc.Height; ++i)
  {
    memcpy((void*)((intptr_t)mappedPtr + i*subresFootprints.Footprint.RowPitch), (void*)((intptr_t)data + i*wPitch), wPitch);
  }
}

void dx12_subset_resource::BTransitOOC(UINT subres, D3D12_RESOURCE_STATES to, D3D12_RESOURCE_STATES from, dx12_cmd_list * cl, ID3D12Resource * res)
{
  D3D12_RESOURCE_BARRIER bar = {
        D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, D3D12_RESOURCE_BARRIER_FLAG_NONE,
        {res, subres, from, to}
  };

  cl->cl->ResourceBarrier(1, &bar);
}

void dx12_subset_resource::BTransit(UINT subres, D3D12_RESOURCE_STATES to, D3D12_RESOURCE_STATES from, dx12_cmd_list * cl)
{
  BTransitOOC(subres, to, from, cl, res);
}

void dx12_subset_resource::BTransit2(D3D12_RESOURCE_STATES to, D3D12_RESOURCE_STATES from, dx12_cmd_list * cl)
{
  BTransit(D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, to, from, cl);
}

void dx12_subset_resource::BTransit3(D3D12_RESOURCE_STATES to, D3D12_RESOURCE_STATES from)
{
  BTransit2(to, from, dx12->FrameCL2());
}

void dx12_subset_resource::BTransit4(D3D12_RESOURCE_STATES to)
{
  if (stateCache != to)
  {
    BTransit3(to, stateCache);
    stateCache = to;
  }
}

void * dx12_subset_resource::MapWrite(unsigned int offset, unsigned int size)
{
  BTransit4(D3D12_RESOURCE_STATE_COPY_DEST);

  UINT64 ulOffset = 0;
  dx12_subset_resource* ul = dx12->GetUploadBuffer(size, &ulOffset, 0);
  void* ret = ul->UploadBlockFR(this, offset, ulOffset, size);

  BTransit4(D3D12_RESOURCE_STATE_GENERIC_READ);

  return ret;
}

void* dx12_subset_resource::UploadBlockFR(dx12_subset_resource * dst, UINT64 block_offset, UINT64 upload_offset, UINT64 sz)
{
  dx12->FrameCL()->CopyBufferRegion(dst->GetD12Obj(), block_offset, res, upload_offset, sz);

  return (void*)DPtrOffset(upload_offset);
}

void * dx12_subset_resource::UploadTexFR(dx12_subset_resource * dst, UINT64 upload_offset, UINT32 lv)
{
  D3D12_TEXTURE_COPY_LOCATION dstR = { dst->GetD12Obj(), D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX, lv };
  D3D12_TEXTURE_COPY_LOCATION srcR = { res, D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT, 0 };

  D3D12_RESOURCE_DESC dstDsc = dst->GetD12Obj()->GetDesc();

  dx12->dev->GetCopyableFootprints(&dstDsc, lv, 1, 0, &srcR.PlacedFootprint, 0, 0, 0);

  srcR.PlacedFootprint.Offset = upload_offset;

  dx12->FrameCL()->CopyTextureRegion(&dstR, 0, 0, 0, &srcR, NULL);

  return (void*)DPtrOffset(upload_offset);
}

intptr_t dx12_subset_resource::DPtrOffset(UINT64 offset)
{
  if (!mappedPtr)
    res->Map(0, 0, &mappedPtr);

  return (intptr_t)mappedPtr + offset;
}