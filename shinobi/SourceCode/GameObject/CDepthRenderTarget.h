#ifndef CDEPTH_RENDER_TARGET_H_
#define CDEPTH_RENDER_TARGET_H_

#include <d3d9.h>
#include <d3dx9.h>
#include "common.h"

//êÊï˚êÈåæ
class CGame;
class CCameraLight;

class CDepthRenderTarget
{
public:
	~CDepthRenderTarget();

	bool Init(void);
	void DrawTest(const D3DXVECTOR2& pos , const D3DXVECTOR2& size);
	void Uninit(void);

	LPDIRECT3DTEXTURE9 GetTexturePointer(void) {
		return m_Texture;
	}
	LPDIRECT3DSURFACE9 GetSurfacePointer(void) {
		return m_Surface;
	}
	LPDIRECT3DSURFACE9 GetDepthSurfacePointer(void) {
		return m_Depth;
	}

	static CDepthRenderTarget* GetInstance(void);
	static void DeleteInstance(void);

private:
	CDepthRenderTarget();
	CDepthRenderTarget(const CDepthRenderTarget &ref) { ref; }
	CDepthRenderTarget& operator=(const CDepthRenderTarget& ref){ ref; }

	HRESULT CreateRenderTarget(void);

private:
	LPDIRECT3DTEXTURE9		m_Texture;
	LPDIRECT3DSURFACE9		m_Surface;
	LPDIRECT3DSURFACE9		m_Depth;
	static CDepthRenderTarget *m_pInstance;
};

#endif