/*
MIT License

Copyright(c) 2018-2019 megai2

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
#ifndef DX12_SUBSET_DHEAP_H
#define DX12_SUBSET_DHEAP_H
#include "dx12_subset.h"

#define dx12_subset_dheap_slot_type UINT32

class dx12_subset_dheap_slot_stack {

public:
	dx12_subset_dheap_slot_stack(UINT32 size);
	~dx12_subset_dheap_slot_stack();

	void Push(dx12_subset_dheap_slot_type val);
	dx12_subset_dheap_slot_type Pop();

	LONG Count() { return top; };

private:

	dx12_subset_dheap_slot_type* data;
	LONG top;	
};

#define DX12_SUBSET_DHEAP_STACK_FREE 0
#define DX12_SUBSET_DHEAP_STACK_CLEANUP 1
#define DX12_SUBSET_DHEAP_STACK_COUNT 2

#define DX12_SUBSET_DHEAP_MAX 4
#define DX12_SUBSET_DHEAP_RTV 0
#define DX12_SUBSET_DHEAP_DSV 1
#define DX12_SUBSET_DHEAP_SPL 2
#define DX12_SUBSET_DHEAP_SRV 3

#define FRAME_METRIC_DHEAP(...)

class dx12_subset_dheap 
{
public:
	dx12_subset_dheap(dx12_subset* dev, UINT idx);
	~dx12_subset_dheap();

	UINT OccupySlot();
	void FreeSlot(UINT slot);
	void FreeSlotByPtr(D3D12_CPU_DESCRIPTOR_HANDLE cptr);

	void CleanupSlots(UINT maxCount);

	D3D12_CPU_DESCRIPTOR_HANDLE GetDHeapHandle(UINT slot);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDHeapHandle(UINT slot);

	UINT CreateRTV(ID3D12Resource* resource, D3D12_RENDER_TARGET_VIEW_DESC* dsc);
	UINT CreateDSV(ID3D12Resource* resource, D3D12_DEPTH_STENCIL_VIEW_DESC* dsc);
	UINT CreateSRV_at(ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC* dsc, UINT32 slot);
	UINT CreateSRV(ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC* dsc);
	UINT CreateCBV(D3D12_CONSTANT_BUFFER_VIEW_DESC* dsc);
	UINT CreateUAV(D3D12_UNORDERED_ACCESS_VIEW_DESC* dsc, ID3D12Resource* iRes);
	UINT CreateSampler(D3D12_SAMPLER_DESC* dsc);

	ID3D12DescriptorHeap* GetHeapObj() {
		return heap;
	};

	const D3D12_DESCRIPTOR_HEAP_DESC* GetDesc() {
		return m_desc;
	}

private:
	UINT selfIID;

	//megai2: slot status filled in stacks
	dx12_subset_dheap_slot_stack* stacks[DX12_SUBSET_DHEAP_STACK_COUNT];

	ID3D12DescriptorHeap* heap;

	const D3D12_DESCRIPTOR_HEAP_DESC* m_desc;
	UINT handleSz;

	D3D12_CPU_DESCRIPTOR_HANDLE cpuBase;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuBase;

	dx12_subset* dx12;
};

#endif