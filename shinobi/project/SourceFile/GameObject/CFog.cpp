#include "CFog.h"
#include "CRenderer.h"
#include "SceneTransition.h"

float CFog::m_fFogDensity = 0.05f;

void CFog::FogSetting(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	// 範囲ベースのフォグを使用
	pDevice->SetRenderState(D3DRS_RANGEFOGENABLE, TRUE);

	float FogStart = 50.0f, FogEnd = 100.0f;
	pDevice->SetRenderState(D3DRS_FOGENABLE, TRUE); // フォグ有効
	pDevice->SetRenderState(D3DRS_FOGCOLOR, D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f)); // フォグ色
	pDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR); // バーテックスフォグ
	pDevice->SetRenderState(D3DRS_RANGEFOGENABLE, TRUE); // 範囲ベースのフォグ
	pDevice->SetRenderState(D3DRS_FOGSTART, *((DWORD*)(&FogStart))); // フォグ開始点
	pDevice->SetRenderState(D3DRS_FOGEND, *((DWORD*)(&FogEnd))); // フォグ終了点
}

void CFog::EnableFog(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();
	pDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_RANGEFOGENABLE, TRUE); // 範囲ベースのフォグ
}

void CFog::DisableFog(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();
	pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_RANGEFOGENABLE, FALSE); // 範囲ベースのフォグ
}