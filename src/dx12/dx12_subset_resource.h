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
#ifndef DX12_SUBSET_RESOURCE_H
#define DX12_SUBSET_RESOURCE_H

#include "dx12_subset.h"
#include "dx12_subset_dheap.h"

class dx12_subset_resource 
{
public:
  dx12_subset_resource(dx12_subset* parent);
  ~dx12_subset_resource();

  void ModRef(int v);

  HRESULT  zbuf(DXGI_FORMAT fmt, float clearV, UINT width, UINT height, DXGI_FORMAT clearVFmt);
  HRESULT  rtgt(DXGI_FORMAT fmt, float* clearV, UINT width, UINT height);  
  HRESULT  buffer(size_t size, D3D12_HEAP_TYPE heap);    
  HRESULT vbuffer(size_t size, int isIB);
  HRESULT tex2d(UINT width, UINT height, DXGI_FORMAT fmt, UINT16* levels, UINT arrSz);  

  static void BTransitOOC(UINT subres, D3D12_RESOURCE_STATES to, D3D12_RESOURCE_STATES from, dx12_cmd_list* cl, ID3D12Resource* res);
  void BTransit(UINT subres, D3D12_RESOURCE_STATES to, D3D12_RESOURCE_STATES from, dx12_cmd_list* cl);
  void BTransit2(D3D12_RESOURCE_STATES to, D3D12_RESOURCE_STATES from, dx12_cmd_list* cl);
  void BTransit3(D3D12_RESOURCE_STATES to, D3D12_RESOURCE_STATES from);
  void BTransit4(D3D12_RESOURCE_STATES to);

  ID3D12Resource* GetD12Obj() { return res; };

  void* MapWrite(unsigned int offset, unsigned int size);
  void MapWriteTex2D(void* data);

  void* UploadBlockFR(dx12_subset_resource* dst, UINT64 block_offset, UINT64 upload_offset, UINT64 sz);
  void* UploadTexFR(dx12_subset_resource* dst, UINT64 upload_offset, UINT32 lv);
  
  intptr_t DPtrOffset(UINT64 offset);

  //cl&res related calls
  void ClearAsRTV(FLOAT * color4f, D3D12_RECT* clearRect);
  void ClearAsDSV(FLOAT Depth, UINT8 Stencil, D3D12_CLEAR_FLAGS flag, D3D12_RECT* clearRect);
  void SetIB();
  void SetVB(int stream, int offset, int stride);
  void UseAsSRVatRSIG(int rsigIdx);
  void UseAsCBVatRSIG(int rsigIdx, int offset);

  D3D12_CPU_DESCRIPTOR_HANDLE* GetHHforRTDS(D3D12_RESOURCE_STATES state);
  
private:
  dx12_subset* dx12;
  D3D12_RESOURCE_STATES stateCache;

  D3D12_CPU_DESCRIPTOR_HANDLE dheapHH;
  dx12_subset_dheap* srvHeap;
  UINT32 srvId;

  ID3D12Resource* res;

  void* mappedPtr;

  union bindData {
    D3D12_VERTEX_BUFFER_VIEW v;
    D3D12_INDEX_BUFFER_VIEW i;
  } bindData;

  int refs;
};

#endif