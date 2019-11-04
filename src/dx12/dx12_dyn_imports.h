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
#ifndef DX12_DYN_IMPORTS
#define DX12_DYN_IMPORTS

#include <d3d12.h>
#include <dxgi.h>
#include <windows.h>
#include <d3dcompiler.h>

typedef enum dx12_dynamic_import_lib {
	DYNIMP_DX12,
	DYNIMP_D3DCOMPILER,
	DYNIMP_DXILCONV7,
	DYNIMP_COUNT
} dx12_dynamic_import_lib;

typedef struct dx12_dynamic_import_lib_array {
	const wchar_t* dllName[DYNIMP_COUNT];
	const wchar_t* pathPrefix[DYNIMP_COUNT];
} dx12_dynamic_import_lib_array;

#define DYNIMP_LIB_SET_WIN10 0
#define DYNIMP_LIB_SET_WIN7 1

static const dx12_dynamic_import_lib_array dx12_dynamic_import_libs[] = {
	{
		{
			L"d3d12.dll",
			L"d3dcompiler_47.dll",
			NULL
		},
		{
			L"",
			L"",
			L""
		}
	}, 
	{
		{
			L"d3d12.dll",
			L"d3dcompiler_47_v10",
			L"dxilconv7.dll"
		},
		{
			L"12on7/",
			L"12on7/",
			L"12on7/"
		}
	}
};

typedef struct dx12_dynamic_import_entry {
	const char* fn;
	dx12_dynamic_import_lib libId;
} dx12_dynamic_import_entry;

static const dx12_dynamic_import_entry dx12_dynamic_import_funcs[] = {
	{ "D3D12CreateDevice", DYNIMP_DX12 },
	{ "D3D12SerializeRootSignature", DYNIMP_DX12 },
	{ "D3DCompile", DYNIMP_D3DCOMPILER },
	{ "D3DCompileFromFile", DYNIMP_D3DCOMPILER },
	{ NULL, DYNIMP_DX12 }
};

typedef HRESULT(WINAPI *pD3DCompile_47)
(LPCVOID                         pSrcData,
	SIZE_T                          SrcDataSize,
	LPCSTR                          pFileName,
	CONST D3D_SHADER_MACRO*         pDefines,
	ID3DInclude*                    pInclude,
	LPCSTR                          pEntrypoint,
	LPCSTR                          pTarget,
	UINT                            Flags1,
	UINT                            Flags2,
	ID3DBlob**                      ppCode,
	ID3DBlob**                      ppErrorMsgs);

typedef HRESULT(WINAPI *pD3DCompileFromFile_47)(
	LPCWSTR pFileName,
	CONST D3D_SHADER_MACRO* pDefines,
	ID3DInclude* pInclude,
	LPCSTR pEntrypoint,
	LPCSTR pTarget,
	UINT Flags1,
	UINT Flags2,
	ID3DBlob** ppCode,
	ID3DBlob** ppErrorMsgs
	);

class dx12_dynamic_imports 
{
public:
	dx12_dynamic_imports();
	~dx12_dynamic_imports();

	int Init();

	union {
		struct {
			struct
			{
				PFN_D3D12_CREATE_DEVICE CreateDevice;
				PFN_D3D12_SERIALIZE_ROOT_SIGNATURE SerializeRootSignature;
			} dx12;

			struct {
				pD3DCompile_47 Compile;
				pD3DCompileFromFile_47 CompileFromFile;
			} d3d_compiler;
		} fun;
		void* import_fun_ptrs[4];
	};

private:

	UINT LoadDynLib(dx12_dynamic_import_lib lib, UINT set);
	HMODULE libHandles[DYNIMP_COUNT];

};

#endif