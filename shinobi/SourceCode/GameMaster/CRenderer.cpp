#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "CRenderer.h"
#include "imgui_impl_dx9.h"

LPDIRECT3D9 CRenderer::m_pD3D = nullptr;
LPDIRECT3DDEVICE9 CRenderer::m_pD3DDevice = nullptr;
D3DPRESENT_PARAMETERS CRenderer::m_d3dpp;
LPDIRECT3DSURFACE9 CRenderer::m_BackBufferSurface = nullptr;
LPDIRECT3DSURFACE9 CRenderer::m_BackDepthSurface = nullptr;

HRESULT CRenderer::Init(HWND hWnd, BOOL bWindow)
{
	D3DDISPLAYMODE d3ddm;

	// Direct3Dオブジェクトの作成
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_pD3D == nullptr)
	{
		return E_FAIL;
	}

	// 現在のディスプレイモードを取得
	if (FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	// デバイスのプレゼンテーションパラメータの設定
	ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));					// ワークをゼロクリア
	m_d3dpp.BackBufferCount = 1;							// バックバッファの数
	m_d3dpp.BackBufferWidth = SCREEN_WIDTH;					// ゲーム画面サイズ(幅)
	m_d3dpp.BackBufferHeight = SCREEN_HEIGHT;				// ゲーム画面サイズ(高さ)
	m_d3dpp.BackBufferFormat = d3ddm.Format;				// カラーモードの指定
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;				// 映像信号に同期してフリップする
	m_d3dpp.EnableAutoDepthStencil = TRUE;					// デプスバッファ（Ｚバッファ）とステンシルバッファを作成
	m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;			// デプスバッファ(Zバッファ)として24bitを使う、ステンシルバッファとして8bitを使う
	m_d3dpp.Windowed = bWindow;								// ウィンドウモード
	m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;		// リフレッシュレート
	m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;	// インターバル

	// デバイスの生成
	// ディスプレイアダプタを表すためのデバイスを作成
	// 描画と頂点処理をハードウェアで行なう
	if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&m_d3dpp, &m_pD3DDevice)))
	{
		// 上記の設定が失敗したら
		// 描画をハードウェアで行い、頂点処理はCPUで行なう
		if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&m_d3dpp, &m_pD3DDevice)))
		{
			// 上記の設定が失敗したら
			// 描画と頂点処理をCPUで行なう
			if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF, hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&m_d3dpp, &m_pD3DDevice)))
			{
				// 生成失敗
				return E_FAIL;
			}
		}
	}

	// レンダーステートの設定
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);			//テクスチャの解像度より大きいなポリゴンに貼り付ける時のフィルタ設定
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);		//テクスチャの解像度より小さいなポリゴンに貼り付ける時のフィルタ設定
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);			//ミップマップの時のフィルタ設定
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 8);						//異方向フィルタリングのサンプリング回数
	
	//バックバッファのサーフェースポインタ取得
	m_pD3DDevice->GetRenderTarget(0, &m_BackBufferSurface);
	m_pD3DDevice->GetDepthStencilSurface(&m_BackDepthSurface);

	//imgui初期化
	ImGui_ImplDX9_Init(hWnd, m_pD3DDevice);
	
	return S_OK;
}

//解放関数
void CRenderer::Uninit(void)
{
	// デバイスの破棄
	if (m_pD3DDevice != nullptr)
	{
		m_pD3DDevice->Release();
		m_pD3DDevice = nullptr;
	}

	// Direct3Dオブジェクトの破棄
	if (m_pD3D != nullptr)
	{
		m_pD3D->Release();
		m_pD3D = nullptr;
	}

	//バックバッファポインタを解放
	if (m_BackBufferSurface != nullptr)
	{
		m_BackBufferSurface->Release();
		m_BackBufferSurface = nullptr;
	}
	if (m_BackDepthSurface != nullptr)
	{
		m_BackDepthSurface->Release();
		m_BackDepthSurface = nullptr;
	}

	ImGui_ImplDX9_Shutdown();
}

void CRenderer::DrawClear(void) 
{
	// バックバッファ＆Ｚバッファのクリア
	m_pD3DDevice->Clear(0, nullptr, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL), 
		D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);
}

void CRenderer::DrawBegin(void)
{
	// Direct3Dによる描画の開始
	if (!SUCCEEDED(m_pD3DDevice->BeginScene())) {
		return;
	}
}
void CRenderer::DrawEnd(void)
{
	// Direct3Dによる描画の終了
	m_pD3DDevice->EndScene();
}
void CRenderer::DrawPresent(void)
{
	// バックバッファとフロントバッファの入れ替え
	m_pD3DDevice->Present(nullptr, nullptr, nullptr, nullptr);
}

//ゲーム画面をテクスチャに書き込む関数
//引数説明
//pSurface : サーフェースポインタ
//void(*p)(void) : 描画関数のポインタ
void CRenderer::DrawGameSceneOnTexture(LPDIRECT3DSURFACE9 pSurface, CGame* Scene)
{
	if(Scene == nullptr){
		return;
	}
	m_pD3DDevice->SetRenderTarget(0, pSurface); //描画平面をテクスチャ平面に設定
	DrawClear();
	DrawBegin();
	Scene->DrawInRenderTarget();				//描画結果をテクスチャに書きこむ
	DrawEnd();
	m_pD3DDevice->SetRenderTarget(0, m_BackBufferSurface); //描画平面をバックバッファに戻す

}