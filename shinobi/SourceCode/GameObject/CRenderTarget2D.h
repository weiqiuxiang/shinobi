#ifndef CRENDER_TARGET2D_H_
#define CRENDER_TARGET2D_H_

#include "main.h"

class CRenderTarget2D
{
public:
	CRenderTarget2D();
	~CRenderTarget2D();

	virtual HRESULT Init(const D3DXVECTOR3 &pSize, D3DFORMAT format);
	void DrawTest(const D3DXVECTOR2& pos, const D3DXVECTOR2& size);
	void Uninit(void);

	//ÉQÉbÉ^Å[
	LPDIRECT3DTEXTURE9 GetTexturePointer(void) {
		return m_Texture;
	}
	LPDIRECT3DSURFACE9 GetSurfacePointer(void) {
		return m_Surface;
	}
	LPDIRECT3DSURFACE9 GetDepthSurfacePointer(void) {
		return m_Depth;
	}

private:
	LPDIRECT3DTEXTURE9		m_Texture;
	LPDIRECT3DSURFACE9		m_Surface;
	LPDIRECT3DSURFACE9		m_Depth;
};

#endif