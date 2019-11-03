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
#include "dx12_subset.h"
#include "dx12_subset_resource.h"
#include "dx12_subset_dheap.h"
#include <wrl.h>
#include <stdio.h>

using namespace Microsoft::WRL;

dx12_subset::dx12_subset()
{
	dxgi_handlers[dxgi_state_setup] = &dx12_subset::dxgi_setup;
	dxgi_handlers[dxgi_state_present] = &dx12_subset::dxgi_present;
	dxgi_handlers[dxgi_state_error] = &dx12_subset::dxgi_error;
	dxgi_handlers[dxgi_state_resize] = &dx12_subset::dxgi_resize;
	dxgi_handlers[dxgi_state_present_w7] = &dx12_subset::dxgi_present_w7;
	uploadBuffer[0] = 0;	
	uploadBufferSize[0] = 0;
	uploadBuffer[1] = 0;	
	uploadBufferSize[1] = 0;
	uploadBufferPos = 0;
	isRunning = 0;
}

dx12_subset::~dx12_subset()
{
}

void dx12_subset::init(HWND window, int w, int h)
{
	if (!imports.Init())
		LOG_ERR_THROW2(-1, "imports.init");

	dev = SelectSutiableGPU().Get();

	//que

	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;

	LOG_ERR_THROW2(dev->CreateCommandQueue(&desc, IID_PPV_ARGS(&cmdQue)), "can't create command queue on dx12 device");

	//initial ul buffer	
	for (int i = 0; i != 2; ++i)
	{
		uploadBufferSize[i] = 65535;
		uploadBuffer[i] = new dx12_subset_resource(this);
		uploadBuffer[i]->buffer(uploadBufferSize[i], D3D12_HEAP_TYPE_UPLOAD);
	}

	//command lists

	cl_stack = &cl_stack_buffer[0];
	cl_buffer_seek = &cl_buffer[0];
	cl_cleanup_stack = &cl_cleanup_stack_buffer[0];
	InitCL();
	InitCL();

	//fence

	LOG_ERR_THROW2(dev->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)), "can't create fence");
	fenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	fenceId = 0;
	gpuWorkIssued = 0;

	//initial dheaps

	for (int i = 0; i!=4;++i)
		dHeaps[i] = new dx12_subset_dheap(this, i);

	//ppso & rsig lib

	InitRootSigs();
	InitPPSOLib();

	//dxgi

	dxgiBackBuffer[0] = NULL;
	dxgiBackBuffer[1] = NULL;
	dxgi_sc = NULL;

	dxgi_win = window;
	dxgi_state = dxgi_state_setup;
	winH = h;
	winW = w;

	if (!FAILED(cmdQue->QueryInterface<ID3D12CommandQueueDownlevel>(&w7_cq)))
	{
		LOG_INFO_DTDM("downleveled queue is present, running w7 swap path");
		dxgi_state = dxgi_state_present_w7;

		InitCL();
		InitCL();
	}

	//fr resources

	rt = 0;
	ds = 0;
	frCl = cl_stack[-1];
	ResizeFrResources();

	isRunning = 1;
}

void dx12_subset::deinit()
{
	isRunning = 0;

	WaitForGPU();

	dxgi_release_buffers();

	if (dxgi_sc)
		dxgi_sc->Release();

	for (int i = 0; i != 2; ++i)
	{
		uploadBuffer[i]->ModRef(-1);
	}

	ds->ModRef(-1);
	rt->ModRef(-1);
	fence->Release();
	CloseHandle(fenceEvent);

	ReleaseAfterFrameObjects();
	ReleaseAfterFrameObjects();

	for (int i = 0; i != 4; ++i)
		delete dHeaps[i];

	for (int i = 0; i != DX12_SUBSET_RSIG_COUNT; ++i)
		if (RootSigArr[i])
			RootSigArr[i]->Release();

	for (int i = 0; i != DX12_SUBSET_PPSO_COUNT; ++i)
		if (PPSOArr[i])
			PPSOArr[i]->Release();

	while (cl_stack != cl_stack_buffer)
	{
		--cl_stack;
		cl_stack[0]->cl->Release();
		cl_stack[0]->alc->Release();
	}

	cmdQue->Release();
	dev->Release();
}

void dx12_subset::set_size(int w, int h)
{
	if (w == 0 || h == 0)
		return;

	winH = h;
	winW = w;
	dxgi_resize_query = 1;	
}

void dx12_subset::fr_start()
{
	if (!isRunning)
		return;

	frCl = GetUnusedCL();

	OM_RTDS(rt, ds);

	D3D12_VIEWPORT viewport;
	viewport.Height = (float)winH;
	viewport.Width = (float)winW;
	viewport.MaxDepth = 1.0f;
	viewport.MinDepth = 0.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	frCl->cl->RSSetViewports(1, &viewport);

	D3D12_RECT scissor;
	scissor.bottom = winH;
	scissor.top = 0;	
	scissor.right = winW;
	scissor.left = 0;

	frCl->cl->RSSetScissorRects(1, &scissor);
	
	const ID3D12DescriptorHeap* dheapPtrArray[2];
	
	for (int i = 0; i!=2;++i)
		dheapPtrArray[i] = dHeaps[i+2]->GetHeapObj();

	frCl->cl->SetDescriptorHeaps(2, (ID3D12DescriptorHeap* const *)dheapPtrArray);

	static float clearColor[4] = {
		0.3f,
		0.3f,
		0.32f,
		1.0f
	};

	rt->ClearAsRTV(clearColor, 0);
	ds->ClearAsDSV(0, 0, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 0);

	Use_RSIG(0);
}

void dx12_subset::fr_end()
{
	CopyFrameToDXGI();

	SwapUploadBuffer();

	WaitForGPU();
	ReleaseAfterFrameObjects();	

	ExecuteCL(frCl);

	(this->*dxgi_handlers[dxgi_state])();
	
	const UINT64 fenceVal = fenceId;
	LOG_ERR_THROW2(cmdQue->Signal(fence, fenceVal), "can't set signal on fence");
}

dx12_subset_resource * dx12_subset::GetUploadBuffer(unsigned int space, UINT64 * offset, UINT32 align)
{
	if (align && (uploadBufferPos & (align - 1)))
	{
		uploadBufferPos = (uploadBufferPos & ~(align - 1)) + align;		
	}

	if (space >= (uploadBufferSize[uploadBufferIdx] - uploadBufferPos))
	{
		uploadBufferPos = 0;
		uploadBufferSize[uploadBufferIdx] = (space > uploadBufferSize[uploadBufferIdx]) ? space * 2 : uploadBufferSize[uploadBufferIdx];

		uploadBuffer[uploadBufferIdx]->ModRef(-1);

		uploadBuffer[uploadBufferIdx] = new dx12_subset_resource(this);
		uploadBuffer[uploadBufferIdx]->buffer(uploadBufferSize[uploadBufferIdx], D3D12_HEAP_TYPE_UPLOAD);
	}

	*offset = uploadBufferPos;
	uploadBufferPos += space;

	return uploadBuffer[uploadBufferIdx];
}

void dx12_subset::ReleaseAfterFrameEnd(IUnknown * obj)
{
	frameEndCleanupQue[frameEndCleanupQueId].push(obj);
}

dx12_subset_dheap * dx12_subset::CurrentDHeap(int idx)
{
	return dHeaps[idx];
}

void dx12_subset::UseSamplerAtRSIG(int rsigIdx, int samplerDHslot)
{
	frCl->cl->SetGraphicsRootDescriptorTable(rsigIdx, dHeaps[DX12_SUBSET_DHEAP_SPL]->GetGPUDHeapHandle(samplerDHslot));
}

ComPtr<ID3D12Device> dx12_subset::SelectSutiableGPU()
{
	ComPtr<IDXGIFactory1> dxgiFactory;

#ifdef _DEBUG_DX12
	UINT createFactoryFlags = 0;

#pragma comment(lib, "d3d12.lib")
	ComPtr<ID3D12Debug> debugInterface;
	LOG_ERR_THROW2(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)), "dbg layer manipulation error");
	debugInterface->EnableDebugLayer();

	createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;

	LOG_ERR_THROW2(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory)), "DXGI factory 2 @ GetAdapter");

#else

	LOG_ERR_THROW2(CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)), "DXGI factory @ GetAdapter");

#endif

	ComPtr<IDXGIAdapter1> dxgiAdapter1;
	ComPtr<IDXGIAdapter2> dxgiAdapter4;
	ComPtr<IDXGIAdapter2> gpu = nullptr;

	SIZE_T maxVidmem = 0;
	D3D_FEATURE_LEVEL usingFeatures = D3D_FEATURE_LEVEL_12_1;

	const D3D_FEATURE_LEVEL featureToCreate[] = {
		D3D_FEATURE_LEVEL_9_1,//megai2: should never happen
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	LOG_INFO_DTDM("Enum DXGI adapters");
	{
		for (UINT i = 0; dxgiFactory->EnumAdapters1(i, &dxgiAdapter1) != DXGI_ERROR_NOT_FOUND; ++i)
		{
			LOG_ERR_THROW2(dxgiAdapter1.As(&dxgiAdapter4), "dxgiAdapter 1->4 as");

			DXGI_ADAPTER_DESC2 dxgiAdapterDesc2;
			dxgiAdapter4->GetDesc2(&dxgiAdapterDesc2);

			UINT operational = (dxgiAdapterDesc2.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) == 0;

			const char* flText[] = {
				"not supported           ",
				"FL_12_1 should work 100%",
				"FL_12_0 should work  99%",
				"FL_11_1 should work  80%",
				"FL_11_0 expect problems ",
				"FL_10_1 expect problems ",
				"FL_10_0 expect problems ",
				"FL_9_3 expect problems ",
				"FL_9_2 expect problems ",
				"FL_9_1 expect problems "
			};

			UINT flTestMask = 0;

			if (operational)
			{
				for (int j = 1; j != 10; ++j)
				{
					flTestMask |= SUCCEEDED(imports.fun.dx12.CreateDevice(dxgiAdapter1.Get(), featureToCreate[j], __uuidof(ID3D12Device), nullptr)) << (j - 1);
				}

				while ((flTestMask & 1) == 0)
				{
					flTestMask = flTestMask >> 1;
					++operational;

					if (operational > 9)
					{
						operational = 0;
						break;
					}
				}
			}

			LOG_INFO_DTDM("%u: VRAM: %06u Mb | FL: %s (%u) | %S",
				i,
				(DWORD)(dxgiAdapterDesc2.DedicatedVideoMemory >> 20llu),
				flText[operational], flTestMask,
				dxgiAdapterDesc2.Description
			);

			if (operational && (maxVidmem < dxgiAdapterDesc2.DedicatedVideoMemory))
			{
				if ((maxVidmem > 0) && (dxgiAdapterDesc2.VendorId == 0x8086))
				{
					LOG_WARN_DTDM("Under prioritized Intel GPU have more VRAM than other non-Intel GPU. Odd, skipping Intel GPU.");
				}
				else {
					maxVidmem = dxgiAdapterDesc2.DedicatedVideoMemory;
					gpu = dxgiAdapter4;

					usingFeatures = featureToCreate[operational];
				}
			}
		}
	}

	LOG_INFO_DTDM("Selecting DXGI adapter by vidmem size");

	if (gpu == nullptr)
	{
		LOG_ERR_THROW2(-1, "No suitable GPU found. Exiting.");
	}

	DXGI_ADAPTER_DESC2 pDesc;
	LOG_ERR_THROW2(gpu->GetDesc2(&pDesc), "desc2 are not available");

	UINT64 gpu_totalVidmemMB = (DWORD)(pDesc.DedicatedVideoMemory >> 20llu);

	LOG_INFO_DTDM("GPU name: %S vidmem: %llu Mb", pDesc.Description, gpu_totalVidmemMB);

	//megai2: create device actually

	ComPtr<ID3D12Device> ret;
	LOG_ERR_THROW2(imports.fun.dx12.CreateDevice(gpu.Get(), usingFeatures, IID_PPV_ARGS(&ret)), "dx12CreateDevice");

	// Enable debug messages in debug mode.
#ifdef _DEBUG_DX12
	ComPtr<ID3D12InfoQueue> pInfoQueue;
	if (SUCCEEDED(ret.As(&pInfoQueue)))
	{
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, TRUE);
		pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
		//pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, TRUE);
		// Suppress whole categories of messages
		//D3D12_MESSAGE_CATEGORY Categories[] = {};

		// Suppress messages based on their severity level
		D3D12_MESSAGE_SEVERITY Severities[] =
		{
			D3D12_MESSAGE_SEVERITY_INFO
		};

		// Suppress individual messages by their ID
		D3D12_MESSAGE_ID DenyIds[] = {
			D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE,   // I'm really not sure how to avoid this message.
			D3D12_MESSAGE_ID_MAP_INVALID_NULLRANGE,                         // This warning occurs when using capture frame while graphics debugging.
			D3D12_MESSAGE_ID_UNMAP_INVALID_NULLRANGE,                       // This warning occurs when using capture frame while graphics debugging.
		};

		D3D12_INFO_QUEUE_FILTER NewFilter = {};
		//NewFilter.DenyList.NumCategories = _countof(Categories);
		//NewFilter.DenyList.pCategoryList = Categories;
		NewFilter.DenyList.NumSeverities = _countof(Severities);
		NewFilter.DenyList.pSeverityList = Severities;
		NewFilter.DenyList.NumIDs = _countof(DenyIds);
		NewFilter.DenyList.pIDList = DenyIds;		
	}
#endif

	ret.Get()->AddRef();

	return ret;
}

void dx12_subset::InitCL()
{
	dx12_cmd_list* 	cl = cl_buffer_seek;
	++cl_buffer_seek;

	cl_stack[0] = cl;
	++cl_stack;

	LOG_ERR_THROW2(dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cl->alc)), "gpu cmd list allocator error");
	LOG_ERR_THROW2(dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, cl->alc, NULL, IID_PPV_ARGS(&cl->cl)), "gpu cmd list error");
}

dx12_cmd_list * dx12_subset::GetUnusedCL()
{
	--cl_stack;
	dx12_cmd_list* ret = cl_stack[0];	
	return ret;
}

void dx12_subset::RecycleCL(dx12_cmd_list * cl)
{
	cl->alc->Reset();
	cl->cl->Reset(cl->alc, 0);	

	cl_stack[0] = cl;
	++cl_stack;
}

void dx12_subset::ExecuteCL(dx12_cmd_list * cl)
{
	gpuWorkIssued |= 1;

	cl->cl->Close();
	cmdQue->ExecuteCommandLists(1, (ID3D12CommandList* const *) &cl->cl);

	*cl_cleanup_stack = cl;
	++cl_cleanup_stack;
}

void dx12_subset::ResizeFrResources()
{
	if (rt)
		rt->ModRef(-1);

	if (ds)
		ds->ModRef(-1);

	rt = new dx12_subset_resource(this);
	ds = new dx12_subset_resource(this);

	float clearVal[4] = { 0,0,0,0 };

	rt->rtgt(DXGI_FORMAT_B8G8R8A8_UNORM, clearVal, winW, winH);
	ds->zbuf(DXGI_FORMAT_R24G8_TYPELESS, 0, winW, winH, DXGI_FORMAT_D24_UNORM_S8_UINT);
}

void dx12_subset::WaitForGPU()
{
	if (!gpuWorkIssued)
		return;

	if (fence->GetCompletedValue() < fenceId)
	{
		LOG_ERR_THROW2(fence->SetEventOnCompletion(fenceId, fenceEvent), "can't set wait event on fence");
		::WaitForSingleObject(fenceEvent, INFINITE);
	}
	++fenceId;

	while (cl_cleanup_stack != cl_cleanup_stack_buffer)
	{
		--cl_cleanup_stack;
		dx12_cmd_list* tmp = cl_cleanup_stack[0];

		RecycleCL(tmp);
	}

	gpuWorkIssued = 0;
}

void dx12_subset::ReleaseAfterFrameObjects()
{
	std::queue<IUnknown*>* que = &frameEndCleanupQue[!frameEndCleanupQueId];
	while (!que->empty())
	{
		que->front()->Release();
		que->pop();
	}

	frameEndCleanupQueId = !frameEndCleanupQueId;

	for (int i = 0; i != 4; ++i)
		dHeaps[i]->CleanupSlots(10);
}

void dx12_subset::dxgi_setup()
{
	ComPtr<IDXGIFactory2> dxgiFactory4;
#ifdef _DEBUG_DX12
	UINT createFactoryFlags = 0;
	createFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
	LOG_ERR_THROW2(CreateDXGIFactory2(createFactoryFlags, IID_PPV_ARGS(&dxgiFactory4)), "DXGI factory 2 @ InitDXGISwapChain");
#else
	LOG_ERR_THROW2(CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory4)), "DXGI factory 1 @ InitDXGISwapChain");
#endif

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
	swapChainDesc.Width = winW;
	swapChainDesc.Height = winH;
	swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc = { 1, 0 };
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;

	swapChainDesc.Flags = 0;

	IDXGISwapChain1* dxgi_sc1 = 0;
	
	HRESULT swapRet = dxgiFactory4->CreateSwapChainForHwnd(
		cmdQue,
		dxgi_win,
		&swapChainDesc,
		nullptr,//currentPP.Windowed ? nullptr : &swapChainFullscreenDsc,
		nullptr,
		&dxgi_sc1
	);

	LOG_ERR_THROW2(swapRet, "dxgiFactory4->CreateSwapChainForHwnd");

	swapRet = dxgi_sc1->QueryInterface<IDXGISwapChain3>(&dxgi_sc);

	dxgi_sc1->Release();

	if (FAILED(swapRet))
	{
		dxgi_state = dxgi_state_error;
	}
	else {
		dxgi_state = dxgi_state_present;
		dxgi_sc->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer[0]));
		dxgi_sc->GetBuffer(1, IID_PPV_ARGS(&dxgiBackBuffer[1]));
	}
}

void dx12_subset::dxgi_present()
{
	HRESULT ret = dxgi_sc->Present(0, 0);
	
	if (!((ret == DXGI_ERROR_WAS_STILL_DRAWING) || (ret == S_OK) || (ret == DXGI_STATUS_OCCLUDED)))
	{
		dxgi_state = dxgi_state_error;
	}

	if (dxgi_resize_query)
	{
		dxgi_state = dxgi_state_resize;
	}
}

void dx12_subset::dxgi_present_w7()
{
}

void dx12_subset::dxgi_setup_w7()
{
}

void dx12_subset::dxgi_error()
{
	dxgi_release_buffers();

	if (dxgi_sc)
	{
		dxgi_sc->Release();
		dxgi_sc = 0;
	}

	dxgi_state = dxgi_state_setup;
}

void dx12_subset::dxgi_resize()
{
	dxgi_resize_query = 0;

	ResizeFrResources();
	dxgi_release_buffers();

	if (FAILED(dxgi_sc->ResizeBuffers(2, winW, winH, DXGI_FORMAT_B8G8R8A8_UNORM, 0)))
	{
		dxgi_state = dxgi_state_error;
	}
	else {
		dxgi_state = dxgi_state_present;
		dxgi_sc->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer[0]));
		dxgi_sc->GetBuffer(1, IID_PPV_ARGS(&dxgiBackBuffer[1]));
	}
}

void dx12_subset::dxgi_release_buffers()
{
	for (int i = 0; i!=2;++i)
		if (dxgiBackBuffer[i])
		{
			dxgiBackBuffer[i]->Release();
			dxgiBackBuffer[i] = NULL;
		}
}

void dx12_subset::CopyFrameToDXGI()
{
	if ((dxgi_state != dxgi_state_present) || dxgi_resize_query)
		return;

	UINT dxgiIdx = dxgi_sc->GetCurrentBackBufferIndex();

	ID3D12Resource* dxgiBuf = dxgiBackBuffer[dxgiIdx];

	dx12_subset_resource::BTransitOOC(D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PRESENT, frCl, dxgiBuf);
	rt->BTransit4(D3D12_RESOURCE_STATE_COPY_SOURCE);

	frCl->cl->CopyResource(dxgiBuf, rt->GetD12Obj());

	//rt->BTransit4(D3D12_RESOURCE_STATE_RENDER_TARGET);
	dx12_subset_resource::BTransitOOC(D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES, D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_COPY_DEST, frCl, dxgiBuf);
}

void dx12_subset::SwapUploadBuffer()
{
	uploadBufferPos = 0;
	uploadBufferIdx = !uploadBufferIdx;
}

void dx12_subset::ThrowError(HRESULT hr, const char * msg)
{
	if (!FAILED(hr))
		return;
	
	printf("dx12 backend critical error: %s", msg);
	//MessageBoxA(0, msg, "nanogui dx12 backend", MB_ICONERROR);

	abort();
}

void dx12_subset::Use_PPSO(int id)
{
	frCl->cl->SetPipelineState(PPSOArr[id]);
}

void dx12_subset::Use_RSIG(int id)
{
	frCl->cl->SetGraphicsRootSignature(RootSigArr[id]);
}

void dx12_subset::OM_RTDS(dx12_subset_resource * rtSurf, dx12_subset_resource * dsSurf)
{
	frCl->cl->OMSetRenderTargets(1, rtSurf->GetHHforRTDS(D3D12_RESOURCE_STATE_RENDER_TARGET), 0, dsSurf->GetHHforRTDS(D3D12_RESOURCE_STATE_DEPTH_WRITE));
}

D3D12_HEAP_PROPERTIES dx12_subset::GetResourceHeap(D3D12_HEAP_TYPE Type)
{
	D3D12_HEAP_PROPERTIES ret;

	ret.Type = Type;
	ret.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	ret.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	ret.CreationNodeMask = 1;
	ret.VisibleNodeMask = 1;

	return ret;
}
