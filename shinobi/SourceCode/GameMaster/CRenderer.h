#pragma once
#ifndef _CRENDERER_H_
#define _CRENDERER_H_

#include "main.h"
#include "CGame.h"

class CRenderer
{
public:
	static HRESULT Init(HWND hWnd, BOOL bWindow);
	static void Uninit(void);
	static void DrawClear(void);
	static void DrawBegin(void);
	static void DrawEnd(void);
	static void DrawPresent(void);
	static void DrawGameSceneOnTexture(LPDIRECT3DSURFACE9 pSurface, CGame* Scene);		//テクスチャに描画内容を書き込む関数

	static LPDIRECT3DDEVICE9 GetDevice(void) { return m_pD3DDevice; }
	static D3DPRESENT_PARAMETERS GetDpp(void) { return m_d3dpp; }
	static LPDIRECT3DSURFACE9 GetBackBufferSurface(void) { return m_BackBufferSurface; }
	static LPDIRECT3DSURFACE9 GetBackDepthSurface(void) { return m_BackDepthSurface; }

private:
	static LPDIRECT3D9 m_pD3D;
	static LPDIRECT3DDEVICE9 m_pD3DDevice;
	static D3DPRESENT_PARAMETERS m_d3dpp;
	static LPDIRECT3DSURFACE9 m_BackBufferSurface;
	static LPDIRECT3DSURFACE9 m_BackDepthSurface;		//Zバッファ
};

#endif