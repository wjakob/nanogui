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
#include "dx12_subset_dheap.h"

static const D3D12_DESCRIPTOR_HEAP_DESC dx12_heap_config[DX12_SUBSET_DHEAP_MAX] = {
	{ D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 16, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0 },
	{ D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 16, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 0 },
	{ D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, 16, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 0 },
	{ D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 1024*64, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 0 }
};

dx12_subset_dheap::dx12_subset_dheap(dx12_subset* dev, UINT idx) 
{
	dx12 = dev;
	const D3D12_DESCRIPTOR_HEAP_DESC* desc = &dx12_heap_config[idx];

	LOG_ERR_THROW3(dx12->dev->CreateDescriptorHeap(desc, IID_PPV_ARGS(&heap)), "descriptor heap");

	handleSz = dx12->dev->GetDescriptorHandleIncrementSize(desc->Type);

	cpuBase = heap->GetCPUDescriptorHandleForHeapStart();
	gpuBase = heap->GetGPUDescriptorHandleForHeapStart();

	LOG_INFO_DTDM2("DHeap %u limit: %u", idx, desc->NumDescriptors);

	for (int i = 0; DX12_SUBSET_DHEAP_STACK_COUNT != i; ++i)
		stacks[i] = new dx12_subset_dheap_slot_stack(desc->NumDescriptors);

	for (int i = 0; i != desc->NumDescriptors; ++i)
		stacks[DX12_SUBSET_DHEAP_STACK_FREE]->Push((desc->NumDescriptors - 1) - i);

	m_desc = desc;

	selfIID = idx;

	LOG_DBG_DTDM2("type %u cnt %u id %u", desc->Type, desc->NumDescriptors, selfIID);
}

dx12_subset_dheap::~dx12_subset_dheap()
{
	for (int i = 0; DX12_SUBSET_DHEAP_STACK_COUNT != i; ++i)
		delete stacks[i];

	heap->Release();
}

UINT dx12_subset_dheap::OccupySlot()
{
	FRAME_METRIC_DHEAP(selfIID, stacks[DX12_SUBSET_DHEAP_STACK_FREE]->Count());

	UINT ret = stacks[DX12_SUBSET_DHEAP_STACK_FREE]->Pop();

	LOG_DBG_DTDM2("dheap[%u] used slot %u", selfIID, ret);

	return ret;
}

void dx12_subset_dheap::FreeSlot(UINT slot)
{
	LOG_DBG_DTDM2("dheap[%u] freed slot %u", selfIID, slot);

	stacks[DX12_SUBSET_DHEAP_STACK_CLEANUP]->Push(slot);
}

void dx12_subset_dheap::FreeSlotByPtr(D3D12_CPU_DESCRIPTOR_HANDLE cptr)
{
	FreeSlot((UINT32)((cptr.ptr - cpuBase.ptr) / handleSz));
}

void dx12_subset_dheap::CleanupSlots(UINT maxCount)
{
	UINT limit = 0;

	while (stacks[DX12_SUBSET_DHEAP_STACK_CLEANUP]->Count())
	{
		stacks[DX12_SUBSET_DHEAP_STACK_FREE]->Push(stacks[DX12_SUBSET_DHEAP_STACK_CLEANUP]->Pop());

		if (limit >= maxCount)
			break;
		else
			++limit;
	}

	FRAME_METRIC_DHEAP(selfIID, stacks[DX12_SUBSET_DHEAP_STACK_FREE]->Count());
}

D3D12_CPU_DESCRIPTOR_HANDLE dx12_subset_dheap::GetDHeapHandle(UINT slot)
{
	D3D12_CPU_DESCRIPTOR_HANDLE ret;
	ret.ptr = cpuBase.ptr + slot * handleSz;

	return ret;
}

D3D12_GPU_DESCRIPTOR_HANDLE dx12_subset_dheap::GetGPUDHeapHandle(UINT slot)
{
	D3D12_GPU_DESCRIPTOR_HANDLE ret;
	ret.ptr = gpuBase.ptr + slot * handleSz;

	return ret;
}

UINT dx12_subset_dheap::CreateSRV(ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC* dsc)
{
	UINT ret = OccupySlot();

	if (dsc)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC constDsc;
		constDsc = *dsc;
		dx12->dev->CreateShaderResourceView(resource, &constDsc, GetDHeapHandle(ret));
	}
	else {
		dx12->dev->CreateShaderResourceView(resource, NULL, GetDHeapHandle(ret));
	}

	LOG_DBG_DTDM2("new SRV @%u = %u", selfIID, ret);

	return ret;
}

UINT dx12_subset_dheap::CreateCBV(D3D12_CONSTANT_BUFFER_VIEW_DESC * dsc)
{
	UINT ret = OccupySlot();

	D3D12_CONSTANT_BUFFER_VIEW_DESC constDsc;
	constDsc = *dsc;
	dx12->dev->CreateConstantBufferView(&constDsc, GetDHeapHandle(ret));

	LOG_DBG_DTDM2("new CBV @%u = %u", selfIID, ret);

	return ret;
}

UINT dx12_subset_dheap::CreateUAV(D3D12_UNORDERED_ACCESS_VIEW_DESC * dsc, ID3D12Resource* iRes)
{
	UINT ret = OccupySlot();

	D3D12_UNORDERED_ACCESS_VIEW_DESC constDsc;

	constDsc = *dsc;

	dx12->dev->CreateUnorderedAccessView(
		iRes,
		0,
		&constDsc,
		GetDHeapHandle(ret)
	);

	LOG_DBG_DTDM2("new UAV @%u = %u", selfIID, ret);

	return ret;
}

UINT dx12_subset_dheap::CreateSampler(D3D12_SAMPLER_DESC * dsc)
{
	UINT ret = OccupySlot();

	D3D12_SAMPLER_DESC constDsc;
	constDsc = *dsc;
	dx12->dev->CreateSampler(&constDsc, GetDHeapHandle(ret));

	LOG_DBG_DTDM2("new SPL @%u = %u", selfIID, ret);

	return ret;
}

UINT dx12_subset_dheap::CreateRTV(ID3D12Resource* resource, D3D12_RENDER_TARGET_VIEW_DESC* dsc)
{
	UINT ret = OccupySlot();

	if (dsc)
	{
		D3D12_RENDER_TARGET_VIEW_DESC constDsc;
		constDsc = *dsc;
		dx12->dev->CreateRenderTargetView(resource, &constDsc, GetDHeapHandle(ret));
	}
	else {
		dx12->dev->CreateRenderTargetView(resource, NULL, GetDHeapHandle(ret));
	}

	LOG_DBG_DTDM2("new RTV @%u = %u", selfIID, ret);

	return ret;
}

UINT dx12_subset_dheap::CreateDSV(ID3D12Resource* resource, D3D12_DEPTH_STENCIL_VIEW_DESC * dsc)
{
	UINT ret = OccupySlot();

	if (dsc)
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC constDsc;
		constDsc = *dsc;
		dx12->dev->CreateDepthStencilView(resource, &constDsc, GetDHeapHandle(ret));
	}
	else {
		dx12->dev->CreateDepthStencilView(resource, NULL, GetDHeapHandle(ret));
	}

	LOG_DBG_DTDM2("new DSV @%u = %u", selfIID, ret);

	return ret;
}

UINT dx12_subset_dheap::CreateSRV_at(ID3D12Resource* resource, D3D12_SHADER_RESOURCE_VIEW_DESC * dsc, UINT32 slot)
{
	UINT ret = slot;

	if (dsc)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC constDsc;
		constDsc = *dsc;
		dx12->dev->CreateShaderResourceView(resource, &constDsc, GetDHeapHandle(ret));
	}
	else {
		dx12->dev->CreateShaderResourceView(resource, NULL, GetDHeapHandle(ret));
	}

	LOG_DBG_DTDM2("reusing SRV @%u = %u", selfIID, ret);

	return ret;
}

dx12_subset_dheap_slot_stack::dx12_subset_dheap_slot_stack(UINT32 size)
{
	data = (dx12_subset_dheap_slot_type*)malloc(size * sizeof(dx12_subset_dheap_slot_type));	
	top = 0;
}

dx12_subset_dheap_slot_stack::~dx12_subset_dheap_slot_stack()
{
	free(data);
}

void dx12_subset_dheap_slot_stack::Push(dx12_subset_dheap_slot_type val)
{
	data[top] = val;
	++top;
}

dx12_subset_dheap_slot_type dx12_subset_dheap_slot_stack::Pop()
{
	dx12_subset_dheap_slot_type ret = 0;

	LONG idx = --top;

	if (idx < 0)
	{		
		printf("dx12 backend: out of dheap slots");
		abort();
		return 0;
	}
	else
		ret = data[idx];

	return ret;
}
