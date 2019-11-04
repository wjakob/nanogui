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

NVGcontext * nvgCreateD3D12(dx12_subset * pDX12subset, int flags)
{
	struct NVGparams params;
	struct NVGcontext* ctx = NULL;
	struct D3DNVGcontext* D3D = (struct D3DNVGcontext*)malloc(sizeof(struct D3DNVGcontext));
	if (D3D == NULL)
	{
		goto error;
	}

	memset(D3D, 0, sizeof(struct D3DNVGcontext));
	D3D->dx12 = pDX12subset;
	
	memset(&params, 0, sizeof(params));

	void** fnArr = (void**)&params.renderCreate;
	
	fnArr[0] = D3Dnvg__renderCreate;
	fnArr[1] = D3Dnvg__renderCreateTexture;
	fnArr[2] = D3Dnvg__renderDeleteTexture;
	fnArr[3] = D3Dnvg__renderUpdateTexture;
	fnArr[4] = D3Dnvg__renderGetTextureSize;
	fnArr[5] = D3Dnvg__renderViewport;
	fnArr[6] = D3Dnvg__renderCancel;
	fnArr[7] = D3Dnvg__renderFlush;
	fnArr[8] = D3Dnvg__renderFill;
	fnArr[9] = D3Dnvg__renderStroke;
	fnArr[10] = D3Dnvg__renderTriangles;
	fnArr[11] = D3Dnvg__renderDelete;
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

void nvgDeleteD3D12(NVGcontext * ctx)
{
	nvgDeleteInternal(ctx);
}

int D3Dnvg__convertPaint(D3DNVGcontext* D3D, struct D3DNVGfragUniforms* frag,
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

	frag->strokeMult[0] = (width*0.5f + fringe * 0.5f) / fringe;
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

void D3Dnvg__fill(D3DNVGcontext* D3D, struct D3DNVGcall* call)
{
	struct D3DNVGpath* paths = &D3D->paths[call->pathOffset];
	int i, npaths = call->pathCount;

	// Draw shapes

	D3D->dx12->Use_PPSO(D3D->psoId + DX12_SUBSET_PPSO_DEF_DrawShapes_NoWrite_NoCull);
	
	// set bindpoint for solid loc
	D3Dnvg__setUniforms(D3D, call->uniformOffset, 0);

	ID3D12GraphicsCommandList* cl = D3D->dx12->FrameCL();

	cl->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	for (i = 0; i < npaths; i++)
	{
		unsigned int numIndices = ((paths[i].fillCount - 2) * 3);
		
		if (numIndices < D3D->VertexBuffer.MaxBufferEntries)		
			cl->DrawIndexedInstanced(numIndices, 1, 0, paths[i].fillOffset, 0);		
	}

	// Draw anti-aliased pixels		
	cl->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);	

	D3Dnvg__setUniforms(D3D, call->uniformOffset + D3D->fragSize, call->image);

	if (D3D->flags & NVG_ANTIALIAS)
	{
		D3D->dx12->Use_PPSO(D3D->psoId + DX12_SUBSET_PPSO_DEF_DrawAA_Blend_Cull);		
		// Draw fringes
		for (i = 0; i < npaths; i++)
		{
			cl->DrawInstanced(paths[i].strokeCount, 1, paths[i].strokeOffset, 0);			
		}
	}

	// Draw fill	
	D3D->dx12->Use_PPSO(D3D->psoId + DX12_SUBSET_PPSO_DEF_Fill_Blend_NoCull);

	cl->DrawInstanced(call->triangleCount, 1, call->triangleOffset, 0);

	D3D->dx12->Use_PPSO(D3D->psoId + DX12_SUBSET_PPSO_DEF_Def_Blend_Cull);
}

void D3Dnvg__convexFill(D3DNVGcontext* D3D, struct D3DNVGcall* call)
{
	struct D3DNVGpath* paths = &D3D->paths[call->pathOffset];
	int i, npaths = call->pathCount;

	D3Dnvg__setUniforms(D3D, call->uniformOffset, call->image);

	ID3D12GraphicsCommandList* cl = D3D->dx12->FrameCL();
	cl->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (i = 0; i < npaths; i++)
	{
		// Draws a fan using indices to fake it up, since there isn't a fan primitive in D3D11.
		if (paths[i].fillCount > 2)
		{
			unsigned int numIndices = ((paths[i].fillCount - 2) * 3);			
			if (numIndices < D3D->VertexBuffer.MaxBufferEntries)
			{
				cl->DrawIndexedInstanced(numIndices, 1, 0, paths[i].fillOffset, 0);				
			}
		}
	}

	cl->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	if (D3D->flags & NVG_ANTIALIAS)
	{
		// Draw fringes
		for (i = 0; i < npaths; i++)
		{
			cl->DrawInstanced(paths[i].strokeCount, 1, paths[i].strokeOffset, 0);			
		}
	}
}

void D3Dnvg__stroke(D3DNVGcontext* D3D, struct D3DNVGcall* call)
{
	struct D3DNVGpath* paths = &D3D->paths[call->pathOffset];
	int npaths = call->pathCount, i;

	ID3D12GraphicsCommandList* cl = D3D->dx12->FrameCL();
	cl->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	if (D3D->flags & NVG_STENCIL_STROKES)
	{
		// Fill the stroke base without overlap		
		D3D->dx12->Use_PPSO(D3D->psoId + DX12_SUBSET_PPSO_DEF_Def_Blend_Cull);

		D3Dnvg__setUniforms(D3D, call->uniformOffset + D3D->fragSize, call->image);
		for (i = 0; i < npaths; i++)
		{
			cl->DrawInstanced(paths[i].strokeCount, 1, paths[i].strokeOffset, 0);
		}

		// Draw anti-aliased pixels.
		D3Dnvg__setUniforms(D3D, call->uniformOffset, call->image);
		D3D->dx12->Use_PPSO(D3D->psoId + DX12_SUBSET_PPSO_DEF_DrawAA_Blend_Cull);

		for (i = 0; i < npaths; i++)
		{
			cl->DrawInstanced(paths[i].strokeCount, 1, paths[i].strokeOffset, 0);
		}

		// Clear stencil buffer.
		D3D->dx12->Use_PPSO(D3D->psoId + DX12_SUBSET_PPSO_DEF_Fill_Blend_Cull);
		for (i = 0; i < npaths; i++)
		{
			cl->DrawInstanced(paths[i].strokeCount, 1, paths[i].strokeOffset, 0);
		}

		D3D->dx12->Use_PPSO(D3D->psoId + DX12_SUBSET_PPSO_DEF_Def_Blend_Cull);
	}
	else
	{
		D3Dnvg__setUniforms(D3D, call->uniformOffset, call->image);
		// Draw Strokes
		for (i = 0; i < npaths; i++)
		{
			cl->DrawInstanced(paths[i].strokeCount, 1, paths[i].strokeOffset, 0);
		}
	}
}

void D3Dnvg__triangles(D3DNVGcontext* D3D, struct D3DNVGcall* call)
{
	ID3D12GraphicsCommandList* cl = D3D->dx12->FrameCL();	
	cl->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3Dnvg__setUniforms(D3D, call->uniformOffset, call->image);

	cl->DrawInstanced(call->triangleCount, 1, call->triangleOffset, 0);	
}

int D3Dnvg__maxVertCount(const struct NVGpath* paths, int npaths)
{
	int i, count = 0;
	for (i = 0; i < npaths; i++) {
		count += paths[i].nfill;
		count += paths[i].nstroke;
	}
	return count;
}