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
#include "dx12_subset.h"
#include "dx12_subset_resource.h"
#include "dx12_subset_dheap.h"
#include <wrl.h>
#include <stdio.h>

#include "frag_aa_dx12.h"
#include "frag_dx12.h"
#include "vert_dx12.h"

void dx12_subset::InitPPSOLib()
{
  const D3D12_DEPTH_STENCILOP_DESC frontOp = { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_INCR, D3D12_COMPARISON_FUNC_ALWAYS };
  const D3D12_DEPTH_STENCILOP_DESC backOp = { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_DECR, D3D12_COMPARISON_FUNC_ALWAYS };
  const D3D12_DEPTH_STENCILOP_DESC aaOp = { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_EQUAL };
  const D3D12_DEPTH_STENCILOP_DESC fillOp = { D3D12_STENCIL_OP_ZERO, D3D12_STENCIL_OP_ZERO, D3D12_STENCIL_OP_ZERO, D3D12_COMPARISON_FUNC_NOT_EQUAL };
  const D3D12_DEPTH_STENCILOP_DESC defaultOp = { D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };

  const D3D12_RASTERIZER_DESC RSNoCull = { D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_NONE, TRUE, 0.0f, 0.0f, TRUE, 0, 0, 0, D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF };
  const D3D12_RASTERIZER_DESC RSCull = { D3D12_FILL_MODE_SOLID, D3D12_CULL_MODE_BACK, TRUE, 0.0f, 0.0f, TRUE, 0, 0, 0, D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF };

  D3D12_RENDER_TARGET_BLEND_DESC rtBSnull = { 0 };
  D3D12_RENDER_TARGET_BLEND_DESC rtBSBlend =
    {
      TRUE, FALSE,
      D3D12_BLEND_ONE, D3D12_BLEND_INV_SRC_ALPHA, D3D12_BLEND_OP_ADD,
      D3D12_BLEND_ONE, D3D12_BLEND_INV_SRC_ALPHA, D3D12_BLEND_OP_ADD,
      D3D12_LOGIC_OP_NOOP, D3D12_COLOR_WRITE_ENABLE_ALL
    };
  D3D12_RENDER_TARGET_BLEND_DESC rtBSNoWrite =
    {
      FALSE, FALSE,
      D3D12_BLEND_ONE, D3D12_BLEND_INV_SRC_ALPHA, D3D12_BLEND_OP_ADD,
      D3D12_BLEND_ONE, D3D12_BLEND_INV_SRC_ALPHA, D3D12_BLEND_OP_ADD,
      D3D12_LOGIC_OP_NOOP, 0
    };

  D3D12_BLEND_DESC BSBlend1 = { FALSE, FALSE, {rtBSBlend, rtBSnull, rtBSnull, rtBSnull, rtBSnull, rtBSnull, rtBSnull, rtBSnull} };
  D3D12_BLEND_DESC BSNoWrite1 = { FALSE, FALSE, {rtBSNoWrite, rtBSnull, rtBSnull, rtBSnull, rtBSnull, rtBSnull, rtBSnull, rtBSnull} };

  D3D12_DEPTH_STENCIL_DESC DSDrawShapes = {
    FALSE, D3D12_DEPTH_WRITE_MASK_ZERO, D3D12_COMPARISON_FUNC_LESS_EQUAL,
    TRUE, D3D12_DEFAULT_STENCIL_READ_MASK, D3D12_DEFAULT_STENCIL_WRITE_MASK,
    frontOp, backOp
  };

  D3D12_DEPTH_STENCIL_DESC DSDrawAA = {
    FALSE, D3D12_DEPTH_WRITE_MASK_ZERO, D3D12_COMPARISON_FUNC_LESS_EQUAL,
    TRUE, D3D12_DEFAULT_STENCIL_READ_MASK, D3D12_DEFAULT_STENCIL_WRITE_MASK,
    aaOp, aaOp
  };

  D3D12_DEPTH_STENCIL_DESC DSFill = {
    FALSE, D3D12_DEPTH_WRITE_MASK_ZERO, D3D12_COMPARISON_FUNC_LESS_EQUAL,
    TRUE, D3D12_DEFAULT_STENCIL_READ_MASK, D3D12_DEFAULT_STENCIL_WRITE_MASK,
    fillOp, fillOp
  };

  D3D12_DEPTH_STENCIL_DESC DSDefault = {
    FALSE, D3D12_DEPTH_WRITE_MASK_ZERO, D3D12_COMPARISON_FUNC_LESS_EQUAL,
    FALSE, D3D12_DEFAULT_STENCIL_READ_MASK, D3D12_DEFAULT_STENCIL_WRITE_MASK,
    defaultOp, defaultOp
  };

  D3D12_INPUT_ELEMENT_DESC LayoutRenderTriangles[] =
  {
    { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
  };

  ////////


/*
#define DX12_SUBSET_PPSO_AA_OFFSET 7
#define DX12_SUBSET_PPSO_AA_Def_Blend_Cull 8
#define DX12_SUBSET_PPSO_AA_DrawShapes_NoWrite_NoCull 9
#define DX12_SUBSET_PPSO_AA_DrawShapes_Blend_Cull 10
#define DX12_SUBSET_PPSO_AA_Fill_Blend_NoCull 11
#define DX12_SUBSET_PPSO_AA_DrawAA_Blend_Cull 12
#define DX12_SUBSET_PPSO_AA_Fill_Blend_Cull 13
*/

  D3D12_GRAPHICS_PIPELINE_STATE_DESC cDscBase;
  ZeroMemory(&cDscBase, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

  {
    cDscBase.pRootSignature = RootSigArr[0];
    cDscBase.NumRenderTargets = 1;
    cDscBase.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
    cDscBase.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    cDscBase.SampleDesc.Count = 1;
    cDscBase.SampleDesc.Quality = 0;
    cDscBase.SampleMask = 0xFFFFFFFF;

    cDscBase.GS.pShaderBytecode = NULL;
    cDscBase.DS.pShaderBytecode = NULL;
    cDscBase.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    cDscBase.PS.BytecodeLength = sizeof(g_D3D12PixelShader_Main);
    cDscBase.PS.pShaderBytecode = g_D3D12PixelShader_Main;

    cDscBase.VS.BytecodeLength = sizeof(g_D3D12VertexShader_Main);
    cDscBase.VS.pShaderBytecode = g_D3D12VertexShader_Main;

    cDscBase.InputLayout.NumElements = 2;
    cDscBase.InputLayout.pInputElementDescs = LayoutRenderTriangles;

    //dev->CreateGraphicsPipelineState(&cDscBase, IID_PPV_ARGS(&PPSOArr[0]));

    //DX12_SUBSET_PPSO_DEF_Def_Blend_Cull
    {
      cDscBase.DepthStencilState = DSDefault;
      cDscBase.BlendState = BSBlend1;
      cDscBase.RasterizerState = RSCull;

      dev->CreateGraphicsPipelineState(&cDscBase, IID_PPV_ARGS(&PPSOArr[DX12_SUBSET_PPSO_DEF_Def_Blend_Cull]));
    }
    //DX12_SUBSET_PPSO_DEF_DrawShapes_NoWrite_NoCull
    {
      cDscBase.DepthStencilState = DSDrawShapes;
      cDscBase.BlendState = BSNoWrite1;
      cDscBase.RasterizerState = RSNoCull;

      dev->CreateGraphicsPipelineState(&cDscBase, IID_PPV_ARGS(&PPSOArr[DX12_SUBSET_PPSO_DEF_DrawShapes_NoWrite_NoCull]));
    }
    //DX12_SUBSET_PPSO_DEF_DrawShapes_Blend_Cull
    {
      cDscBase.DepthStencilState = DSDrawShapes;
      cDscBase.BlendState = BSBlend1;
      cDscBase.RasterizerState = RSCull;

      dev->CreateGraphicsPipelineState(&cDscBase, IID_PPV_ARGS(&PPSOArr[DX12_SUBSET_PPSO_DEF_DrawShapes_Blend_Cull]));
    }
    //DX12_SUBSET_PPSO_DEF_Fill_Blend_NoCull
    {
      cDscBase.DepthStencilState = DSFill;
      cDscBase.BlendState = BSBlend1;
      cDscBase.RasterizerState = RSNoCull;

      dev->CreateGraphicsPipelineState(&cDscBase, IID_PPV_ARGS(&PPSOArr[DX12_SUBSET_PPSO_DEF_Fill_Blend_NoCull]));
    }
    //DX12_SUBSET_PPSO_DEF_DrawAA_Blend_Cull
    {
      cDscBase.DepthStencilState = DSDrawAA;
      cDscBase.BlendState = BSBlend1;
      cDscBase.RasterizerState = RSCull;

      dev->CreateGraphicsPipelineState(&cDscBase, IID_PPV_ARGS(&PPSOArr[DX12_SUBSET_PPSO_DEF_DrawAA_Blend_Cull]));
    }
    //DX12_SUBSET_PPSO_DEF_Fill_Blend_Cull
    {
      cDscBase.DepthStencilState = DSFill;
      cDscBase.BlendState = BSBlend1;
      cDscBase.RasterizerState = RSCull;

      dev->CreateGraphicsPipelineState(&cDscBase, IID_PPV_ARGS(&PPSOArr[DX12_SUBSET_PPSO_DEF_Fill_Blend_Cull]));
    }
  }

  {
    cDscBase.pRootSignature = RootSigArr[0];
    cDscBase.NumRenderTargets = 1;
    cDscBase.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
    cDscBase.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    cDscBase.SampleDesc.Count = 1;
    cDscBase.SampleDesc.Quality = 0;
    cDscBase.SampleMask = 0xFFFFFFFF;

    cDscBase.GS.pShaderBytecode = NULL;
    cDscBase.DS.pShaderBytecode = NULL;
    cDscBase.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

    cDscBase.PS.BytecodeLength = sizeof(g_D3D12PixelShaderAA_Main);
    cDscBase.PS.pShaderBytecode = g_D3D12PixelShaderAA_Main;

    cDscBase.VS.BytecodeLength = sizeof(g_D3D12VertexShader_Main);
    cDscBase.VS.pShaderBytecode = g_D3D12VertexShader_Main;

    cDscBase.InputLayout.NumElements = 2;
    cDscBase.InputLayout.pInputElementDescs = LayoutRenderTriangles;

    //dev->CreateGraphicsPipelineState(&cDscBase, IID_PPV_ARGS(&PPSOArr[DX12_SUBSET_PPSO_AA_OFFSET]));

    //DX12_SUBSET_PPSO_AA_Def_Blend_Cull
    {
      cDscBase.DepthStencilState = DSDefault;
      cDscBase.BlendState = BSBlend1;
      cDscBase.RasterizerState = RSCull;

      dev->CreateGraphicsPipelineState(&cDscBase, IID_PPV_ARGS(&PPSOArr[DX12_SUBSET_PPSO_AA_Def_Blend_Cull]));
    }
    //DX12_SUBSET_PPSO_AA_DrawShapes_NoWrite_NoCull
    {
      cDscBase.DepthStencilState = DSDrawShapes;
      cDscBase.BlendState = BSNoWrite1;
      cDscBase.RasterizerState = RSNoCull;

      dev->CreateGraphicsPipelineState(&cDscBase, IID_PPV_ARGS(&PPSOArr[DX12_SUBSET_PPSO_AA_DrawShapes_NoWrite_NoCull]));
    }
    //DX12_SUBSET_PPSO_AA_DrawShapes_Blend_Cull
    {
      cDscBase.DepthStencilState = DSDrawShapes;
      cDscBase.BlendState = BSBlend1;
      cDscBase.RasterizerState = RSCull;

      dev->CreateGraphicsPipelineState(&cDscBase, IID_PPV_ARGS(&PPSOArr[DX12_SUBSET_PPSO_AA_DrawShapes_Blend_Cull]));
    }
    //DX12_SUBSET_PPSO_AA_Fill_Blend_NoCull
    {
      cDscBase.DepthStencilState = DSFill;
      cDscBase.BlendState = BSBlend1;
      cDscBase.RasterizerState = RSNoCull;

      dev->CreateGraphicsPipelineState(&cDscBase, IID_PPV_ARGS(&PPSOArr[DX12_SUBSET_PPSO_AA_Fill_Blend_NoCull]));
    }
    //DX12_SUBSET_PPSO_AA_DrawAA_Blend_Cull
    {
      cDscBase.DepthStencilState = DSDrawAA;
      cDscBase.BlendState = BSBlend1;
      cDscBase.RasterizerState = RSCull;

      dev->CreateGraphicsPipelineState(&cDscBase, IID_PPV_ARGS(&PPSOArr[DX12_SUBSET_PPSO_AA_DrawAA_Blend_Cull]));
    }
    //DX12_SUBSET_PPSO_AA_Fill_Blend_Cull
    {
      cDscBase.DepthStencilState = DSFill;
      cDscBase.BlendState = BSBlend1;
      cDscBase.RasterizerState = RSCull;

      dev->CreateGraphicsPipelineState(&cDscBase, IID_PPV_ARGS(&PPSOArr[DX12_SUBSET_PPSO_AA_Fill_Blend_Cull]));
    }
  }
}



void dx12_subset::InitRootSigs()
{
  //rsig 0 - default
  {
    D3D12_DESCRIPTOR_RANGE ranges[2];

    ranges[0].BaseShaderRegister = 0;
    ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
    ranges[0].NumDescriptors = 1;
    ranges[0].OffsetInDescriptorsFromTableStart = 0;
    ranges[0].RegisterSpace = 0;

    ranges[1].BaseShaderRegister = 0;
    ranges[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
    ranges[1].NumDescriptors = 1;
    ranges[1].OffsetInDescriptorsFromTableStart = 0;
    ranges[1].RegisterSpace = 0;

    D3D12_ROOT_PARAMETER rootParameters[4];

    for (int i = 0; i != 2; ++i)
    {
      rootParameters[i].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
      rootParameters[i].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
      rootParameters[i].DescriptorTable.NumDescriptorRanges = 1;
      rootParameters[i].DescriptorTable.pDescriptorRanges = &ranges[i];
    }

    rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[2].Descriptor.RegisterSpace = 0;
    rootParameters[2].Descriptor.ShaderRegister = 0;
    rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

    rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
    rootParameters[3].Descriptor.RegisterSpace = 0;
    rootParameters[3].Descriptor.ShaderRegister = 0;
    rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
    rootSignatureDesc.NumParameters = 4;
    rootSignatureDesc.NumStaticSamplers = 0;
    rootSignatureDesc.pParameters = rootParameters;
    rootSignatureDesc.pStaticSamplers = 0;

    RootSigArr[0] = ConstructRootSignature(&rootSignatureDesc);
  }
}

ID3D12RootSignature * dx12_subset::ConstructRootSignature(D3D12_ROOT_SIGNATURE_DESC * rootSignatureDesc)
{
  ComPtr<ID3DBlob> signature;
  ComPtr<ID3DBlob> error;

  ID3D12RootSignature* rsObj;

  HRESULT ret;

  ret = imports.fun.dx12.SerializeRootSignature(rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);

  if (FAILED(ret))
  {
    if ((error != NULL) && (error->GetBufferSize()))
    {
      LOG_INFO_DTDM("error: %S", error->GetBufferPointer());
    }
    LOG_ERR_THROW2(ret, "SerializeRootSignature failed");
  }

  LOG_ERR_THROW2(dev->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rsObj)), "CreateRootSignature failed");

  return rsObj;
}
