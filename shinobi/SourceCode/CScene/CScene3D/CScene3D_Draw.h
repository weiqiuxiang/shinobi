#pragma once
#ifndef CSCENE3D_DRAW_H_
#define CSCENE3D_DRAW_H_

#include "main.h"

class CScene3D_Draw
{
public:
	virtual void VertexBufferLock(const D3DXVECTOR3 &size, const RGBA& rgba, LPDIRECT3DVERTEXBUFFER9 pVtxBuffer) = 0;
	virtual void MtxSetting(const SRP& SclPosRot,D3DXMATRIX *pOutWorldMtx) = 0;
};

class C3DPolygon : public CScene3D_Draw
{
public:
	void VertexBufferLock(const D3DXVECTOR3 &size, const RGBA& rgba, LPDIRECT3DVERTEXBUFFER9 pVtxBuffer) override;
	void MtxSetting(const SRP& SclPosRot, D3DXMATRIX *pOutWorldMtx);
};

class CBillboard : public CScene3D_Draw
{
public:
	void VertexBufferLock(const D3DXVECTOR3 &size, const RGBA& rgba, LPDIRECT3DVERTEXBUFFER9 pVtxBuffer) override;
	void MtxSetting(const SRP& SclPosRot, D3DXMATRIX *pOutWorldMtx);
};

class CBillboardBottom : public CScene3D_Draw
{
	void VertexBufferLock(const D3DXVECTOR3 &size, const RGBA& rgba, LPDIRECT3DVERTEXBUFFER9 pVtxBuffer) override;
	void MtxSetting(const SRP& SclPosRot, D3DXMATRIX *pOutWorldMtx);
};

#endif