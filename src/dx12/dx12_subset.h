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
#ifndef DX12_SUBSET_H
#define DX12_SUBSET_H

#include "dx12_dyn_imports.h"
#include <d3d12.h>
#include <dxgi1_4.h>
#include <Windows.h>
#include <wrl.h>
#include "../../ext/D3D12Downlevel.h"
#include <queue>

using namespace Microsoft::WRL;

class dx12_subset;
class dx12_subset_resource;

#define dxgi_state_setup 0
#define dxgi_state_present 1
#define dxgi_state_error 2
#define dxgi_state_resize 3
#define dxgi_state_present_w7 4
#define dxgi_state_count 5

typedef struct dx12_cmd_list {
	ID3D12GraphicsCommandList* cl;
	ID3D12CommandAllocator* alc;
} dx12_cmd_list;

//#define _DEBUG_DX12
//#define _ENABLE_DX12_LOGGING

#ifdef _DEBUG_DX12
	#define LOG_DBG_DTDM2(...) printf(__VA_ARGS__); printf("\r\n")
#else
	#define LOG_DBG_DTDM2(...) 
#endif

#ifdef _ENABLE_DX12_LOGGING
	#define LOG_WARN_DTDM(...) printf(__VA_ARGS__); printf("\r\n")
	#define LOG_INFO_DTDM(...) printf(__VA_ARGS__); printf("\r\n")
	#define LOG_INFO_DTDM2(...) printf(__VA_ARGS__); printf("\r\n")
#else
	#define LOG_WARN_DTDM(...) 
	#define LOG_INFO_DTDM(...) 
	#define LOG_INFO_DTDM2(...)
#endif

#define LOG_ERR_THROW2(hr, hr2) ThrowError(hr, #hr2 )
#define LOG_ERR_THROW3(hr, hr2) dx12->ThrowError(hr, #hr2 )

#define DX12_SUBSET_PPSO_DEF 0
#define DX12_SUBSET_PPSO_DEF_Def_Blend_Cull 1
#define DX12_SUBSET_PPSO_DEF_DrawShapes_NoWrite_NoCull 2
#define DX12_SUBSET_PPSO_DEF_DrawShapes_Blend_Cull 3
#define DX12_SUBSET_PPSO_DEF_Fill_Blend_NoCull 4
#define DX12_SUBSET_PPSO_DEF_DrawAA_Blend_Cull 5
#define DX12_SUBSET_PPSO_DEF_Fill_Blend_Cull 6
#define DX12_SUBSET_PPSO_AA_OFFSET 7
#define DX12_SUBSET_PPSO_AA_Def_Blend_Cull 8
#define DX12_SUBSET_PPSO_AA_DrawShapes_NoWrite_NoCull 9
#define DX12_SUBSET_PPSO_AA_DrawShapes_Blend_Cull 10
#define DX12_SUBSET_PPSO_AA_Fill_Blend_NoCull 11
#define DX12_SUBSET_PPSO_AA_DrawAA_Blend_Cull 12
#define DX12_SUBSET_PPSO_AA_Fill_Blend_Cull 13
#define DX12_SUBSET_PPSO_COUNT 14

#define DX12_SUBSET_RSIG_DEFAULT 0
#define DX12_SUBSET_RSIG_COUNT 1

#define DX12_SUBSET_RSIG_IDX_DEFAULT_TEXBIND 0
#define DX12_SUBSET_RSIG_IDX_DEFAULT_SPL0 1
#define DX12_SUBSET_RSIG_IDX_DEFAULT_VS_CBUF 2
#define DX12_SUBSET_RSIG_IDX_DEFAULT_PS_CBUF 3

class dx12_subset_dheap;

class dx12_subset {

public:
	dx12_subset();
	~dx12_subset();

	void init(HWND window, int w, int h);
	void deinit();
	
	void set_size(int w, int h);

	void fr_start();
	void fr_end();	

	void ThrowError(HRESULT hr, const char * msg);

	void Use_PPSO(int id);
	void Use_RSIG(int id);
	void OM_RTDS(dx12_subset_resource* rtSurf, dx12_subset_resource* dsSurf);

	D3D12_HEAP_PROPERTIES GetResourceHeap(D3D12_HEAP_TYPE Type);

	dx12_cmd_list* FrameCL2() { return frCl; };
	ID3D12GraphicsCommandList* FrameCL() { return frCl->cl; };
	ID3D12Device* dev;
	UINT32 isRunning;

	dx12_subset_resource* GetUploadBuffer(unsigned int space, UINT64* offset, UINT32 align);
	
	void ReleaseAfterFrameEnd(IUnknown* obj);

	dx12_subset_dheap* CurrentDHeap(int idx);

	void UseSamplerAtRSIG(int rsigIdx, int samplerDHslot);
		   
private:
	ComPtr<ID3D12Device> SelectSutiableGPU();
	void InitCL();
	dx12_cmd_list* GetUnusedCL();
	void RecycleCL(dx12_cmd_list* cl);
	void ExecuteCL(dx12_cmd_list* cl);
	void ResizeFrResources();
	void WaitForGPU();
	void ReleaseAfterFrameObjects();
	
	//dxgi
	void dxgi_setup();
	void dxgi_present();
	void dxgi_present_w7();
	void dxgi_setup_w7();
	void dxgi_error();
	void dxgi_resize();

	void dxgi_release_buffers();

	void (dx12_subset::*dxgi_handlers[dxgi_state_count])();

	void CopyFrameToDXGI();
		
	IDXGISwapChain3* dxgi_sc;
	HWND dxgi_win;
	int dxgi_state;
	int dxgi_resize_query;
	ID3D12Resource* dxgiBackBuffer[2];

	//fr resources
	dx12_subset_resource* rt;
	dx12_subset_resource* ds;
	dx12_cmd_list* frCl;
	int winH;
	int winW;

	std::queue<IUnknown*> frameEndCleanupQue[2];
	int frameEndCleanupQueId;

	//upload buffer managment
	void SwapUploadBuffer();
	dx12_subset_resource* uploadBuffer[2];	
	unsigned int uploadBufferPos;
	unsigned int uploadBufferSize[2];
	int uploadBufferIdx = 0;
	

	//dx12 objects	
	ID3D12CommandQueue* cmdQue;	
	ID3D12Fence* fence;
	HANDLE fenceEvent;
	UINT64 fenceId;
	UINT32 gpuWorkIssued;	

	dx12_subset_dheap* dHeaps[4];

	dx12_cmd_list cl_buffer[10];
	dx12_cmd_list* cl_buffer_seek;

	dx12_cmd_list* cl_stack_buffer[10];
	dx12_cmd_list** cl_stack;	

	dx12_cmd_list* cl_cleanup_stack_buffer[10];
	dx12_cmd_list** cl_cleanup_stack;

	dx12_dynamic_imports imports;

	//w7 port
	ID3D12CommandQueueDownlevel* w7_cq;	
	UINT32 w7_cl_idx;

	//ppso & rsig lib

	ID3D12RootSignature* RootSigArr[DX12_SUBSET_RSIG_COUNT];
	ID3D12PipelineState* PPSOArr[DX12_SUBSET_PPSO_COUNT];

	void InitPPSOLib();
	void InitRootSigs();
	ID3D12RootSignature * ConstructRootSignature(D3D12_ROOT_SIGNATURE_DESC* rootSignatureDesc);
};

#endif