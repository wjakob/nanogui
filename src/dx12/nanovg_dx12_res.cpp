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

struct NVGvertex* D3Dnvg_beginVertexBuffer(D3DNVGcontext* D3D, unsigned int maxCount, unsigned int* baseOffset)
{	
	if (maxCount >= D3D->VertexBuffer.MaxBufferEntries)
	{
		D3Dnvg__checkError(E_FAIL, "Vertex buffer too small!");
		return NULL;
	}

	if ((D3D->VertexBuffer.CurrentBufferEntry + maxCount) >= D3D->VertexBuffer.MaxBufferEntries)
	{
		*baseOffset = 0;
		D3D->VertexBuffer.CurrentBufferEntry = maxCount;		
	}
	else
	{
		*baseOffset = D3D->VertexBuffer.CurrentBufferEntry;
		D3D->VertexBuffer.CurrentBufferEntry = *baseOffset + maxCount;		
	}
	return ((struct NVGvertex*)D3D->VertexBuffer.pBuffer->MapWrite(*baseOffset* sizeof(NVGvertex), maxCount* sizeof(NVGvertex)));
}

void D3Dnvg_endVertexBuffer(struct D3DNVGcontext* D3D)
{
	
}

void D3Dnvg__copyVerts(struct NVGvertex* pDest, const struct NVGvertex* pSource, unsigned int num)
{
	unsigned int i;

	memcpy(pDest, pSource, sizeof(NVGvertex)*num);

	/*for (i = 0; i < num; i++)
	{
		pDest[i].x = pSource[i].x;
		pDest[i].y = pSource[i].y;
		pDest[i].u = pSource[i].u;
		pDest[i].v = pSource[i].v;
	}*/
}

void D3Dnvg__vset(struct NVGvertex* vtx, float x, float y, float u, float v)
{
	vtx->x = x;
	vtx->y = y;
	vtx->u = u;
	vtx->v = v;
}

void D3Dnvg_buildFanIndices(D3DNVGcontext* D3D)
{
	UINT32 index0 = 0;
	UINT32 index1 = 1;
	UINT32 index2 = 2;
	UINT32 current = 0;
	UINT32* pIndices = (UINT32*)D3D->pFanIndexBuffer->MapWrite(0, sizeof(UINT32)* D3D->VertexBuffer.MaxBufferEntries);
	
	while (current < (D3D->VertexBuffer.MaxBufferEntries - 3))
	{
		pIndices[current++] = index0;
		pIndices[current++] = index1++;
		pIndices[current++] = index2++;
	}	
}

void D3Dnvg_setBuffers(struct D3DNVGcontext* D3D, unsigned int dynamicOffset)
{
	D3D->pFanIndexBuffer->SetIB();
	D3D->VertexBuffer.pBuffer->SetVB(0, dynamicOffset * sizeof(struct NVGvertex), sizeof(struct NVGvertex));
	//TODO: IA topo
}

unsigned int D3Dnvg_updateVertexBuffer(D3DNVGBuffer* buffer, const struct NVGvertex* verts, unsigned int nverts)
{	
	unsigned int retEntry;

	if (nverts > buffer->MaxBufferEntries)
	{
		D3Dnvg__checkError(E_FAIL, "Vertex buffer too small!");
		return 0;
	}

	if ((buffer->CurrentBufferEntry + nverts) >= buffer->MaxBufferEntries)
	{
		buffer->CurrentBufferEntry = 0;		
	}
	
	D3Dnvg__copyVerts(
		(NVGvertex*)buffer->pBuffer->MapWrite(buffer->CurrentBufferEntry * sizeof(NVGvertex), nverts * sizeof(NVGvertex)),
		(const struct NVGvertex*)verts, 
		nverts
	);
	
	retEntry = buffer->CurrentBufferEntry;
	buffer->CurrentBufferEntry += nverts;
	return retEntry;
}

void D3Dnvg__setUniforms(struct D3DNVGcontext* D3D, int uniformOffset, int image)
{
	struct D3DNVGfragUniforms* frag = nvg__fragUniformPtr(D3D, uniformOffset);
	
	// Pixel shader constants	
	memcpy(D3D->pPSConstants->MapWrite(0, sizeof(struct D3DNVGfragUniforms)), frag, sizeof(struct D3DNVGfragUniforms));
		
	if (image != 0)
	{
		//megai2: this is not optimal
		struct D3DNVGtexture* tex = D3Dnvg__findTexture(D3D, image);
		if (tex != NULL)
		{
			tex->tex->UseAsSRVatRSIG(DX12_SUBSET_RSIG_IDX_DEFAULT_TEXBIND);
		}
		else
		{
			// D3D_API_3(D3D->pDeviceContext, PSSetShaderResources,0, 1, NULL);
		}
	}
	else
	{
		// D3D_API_3(D3D->pDeviceContext, PSSetShaderResources,0, 1, NULL);
	}

}

int D3Dnvg__deleteTexture(D3DNVGcontext* D3D, int id)
{
	int i;
	for (i = 0; i < D3D->ntextures; i++) {
		if (D3D->textures[i].id == id) {
			if (D3D->textures[i].tex != 0 && (D3D->textures[i].flags & NVG_IMAGE_NODELETE) == 0)
			{
				D3D->textures[i].tex->ModRef(-1);				
			}
			memset(&D3D->textures[i], 0, sizeof(D3D->textures[i]));
			return 1;
		}
	}
	return 0;
}