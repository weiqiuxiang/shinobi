#include "CDepthRenderTarget.h"
#include "CRenderer.h"
#include "main.h"
#include "CShaderManager.h"
#include "CCameraLight.h"
#include <crtdbg.h>

CDepthRenderTarget* CDepthRenderTarget::m_pInstance = nullptr;

#define FVF_VERTEX_2D (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

CDepthRenderTarget::CDepthRenderTarget()
{
	m_Texture = nullptr;
	m_Surface = nullptr;
	m_Depth = nullptr;
}

CDepthRenderTarget::~CDepthRenderTarget()
{
	if (m_Texture != nullptr) {
		m_Texture->Release();
		m_Texture = nullptr;
	}
	if (m_Surface != nullptr) {
		m_Surface->Release();
		m_Surface = nullptr;
	}
	if (m_Depth != nullptr) {
		m_Depth->Release();
		m_Depth = nullptr;
	}
}

bool CDepthRenderTarget::Init(void)
{
	m_Texture = nullptr;
	m_Surface = nullptr;
	m_Depth = nullptr;

	if (CreateRenderTarget() == E_FAIL) {
		return false;
	}

	return true;
}

void CDepthRenderTarget::DrawTest(const D3DXVECTOR2& pos, const D3DXVECTOR2& size)
{
	// ポリゴンの描画処理
	VERTEX_2D vtx[] = {
		{ D3DXVECTOR4(pos.x, pos.y, 0.0f, 1.0f), 1.0f, D3DCOLOR_RGBA(255, 255, 255, 255), D3DXVECTOR2(0.0f, 0.0f) },
		{ D3DXVECTOR4(pos.x + size.x, pos.y, 0.0f, 1.0f),1.0f, D3DCOLOR_RGBA(255, 255, 255, 255), D3DXVECTOR2(1.0f, 0.0f) },
		{ D3DXVECTOR4(pos.x, pos.y + size.y, 0.0f, 1.0f), 1.0f,D3DCOLOR_RGBA(255, 255, 255, 255), D3DXVECTOR2(0.0f, 1.0f) },
		{ D3DXVECTOR4(pos.x + size.x, pos.y + size.y, 0.0f, 1.0f),1.0f, D3DCOLOR_RGBA(255, 255, 255, 255), D3DXVECTOR2(1.0f, 1.0f) },
	};

	// FVF（今から使用する頂点情報）の設定
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	LPDIRECT3DPIXELSHADER9 pPS = CShaderManager::GetInstance()->GetPixelShaderPointer(BASIC_PS);
	LPD3DXCONSTANTTABLE pPSCT = CShaderManager::GetInstance()->GetPixelShaderCTPointer(BASIC_PS);

	pDevice->SetPixelShader(pPS);

	pDevice->SetFVF(FVF_VERTEX_2D);

	//テクスチャセット
	UINT TextureSampler = pPSCT->GetSamplerIndex("Sampler0");
	pDevice->SetTexture(TextureSampler,m_Texture);
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &vtx[0], sizeof(VERTEX_2D));

	pDevice->SetPixelShader(nullptr);
}

void CDepthRenderTarget::Uninit(void)
{
	if (m_Texture != nullptr) {
		m_Texture->Release();
		m_Texture = nullptr;
	}
	if (m_Surface != nullptr) {
		m_Surface->Release();
		m_Surface = nullptr;
	}
	if (m_Depth != nullptr) {
		m_Depth->Release();
		m_Depth = nullptr;
	}
}

CDepthRenderTarget* CDepthRenderTarget::GetInstance(void)
{
	if (m_pInstance == nullptr) {
		m_pInstance = new CDepthRenderTarget();
	}
	return m_pInstance;
}

void CDepthRenderTarget::DeleteInstance(void)
{
	if (m_pInstance != nullptr) {
		m_pInstance->Uninit();
		delete m_pInstance;
		m_pInstance = nullptr;
	}
}

HRESULT CDepthRenderTarget::CreateRenderTarget(void)
{
	//空のテクスチャ生成
	HRESULT hr;
	hr = D3DXCreateTexture(CRenderer::GetDevice(), 2048, 2048,
		1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT,
		&m_Texture
	);

	if (hr == E_FAIL)
	{
		MessageBox(nullptr, "レンダーターゲットのテクスチャ生成失敗", "ERROR", MB_ICONHAND);
		return E_FAIL;
	}

	//テクスチャのサーフェースポインタ取得
	m_Texture->GetSurfaceLevel(0, &m_Surface);

	//深度バッファを作る
	hr = CRenderer::GetDevice()->CreateDepthStencilSurface(2048, 2048, D3DFMT_D24S8, D3DMULTISAMPLE_NONE,
		0, FALSE, &m_Depth, nullptr);
	
	if (hr == E_FAIL)
	{
		MessageBox(nullptr, "レンダーターゲットの深度バッファ生成失敗", "ERROR", MB_ICONHAND);
		return E_FAIL;
	}

	return S_OK;
}