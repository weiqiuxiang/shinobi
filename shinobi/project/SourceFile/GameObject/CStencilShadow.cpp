#include "CStencilShadow.h"
#include "CRenderer.h"

CStencilShadow::CStencilShadow() 
{
	
}

CStencilShadow::~CStencilShadow() 
{
	
}

HRESULT CStencilShadow::Init(const SRP& SclRotPos, const char* ModelFilepass)
{
	m_Model.Init(SclRotPos, ModelFilepass);
	m_Model.SetHitMeshFlag(false);

	//黒幕テクスチャ初期化
	m_CScene2D.Init(
		D3DXVECTOR3(SCREEN_WIDTH*0.5f,SCREEN_HEIGHT*0.5,0),
		D3DXVECTOR3(SCREEN_WIDTH, SCREEN_HEIGHT, 0),
		"data/TEXTURE/StencilShadow.png"
	);

	m_CScene2D.SetLayerA(100);
	m_CScene2D.Update();

	return S_OK;
}

void CStencilShadow::Update(void)
{
	m_Model.Update();
	m_CScene2D.Update();
}

void CStencilShadow::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	//モデル表面描画
	pDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);		//ステンシルバッファ有効
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);		//Zバッファ無効
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0);
	pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_STENCILREF, 1);
	pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR);	//ステンシルバッファインクリメント(ステンシルバッファ値1設定し、描画)
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);			//表面だけ描画
	m_Model.SetMtxSetting();
	m_Model.Draw();

	//モデル裏面描画
	pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_DECR);	//ステンシルバッファデクリメント(ステンシルバッファ値-1設定し、描画)
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);			//裏面だけ描画
	m_Model.SetMtxSetting();
	m_Model.Draw();

	//全スクリーンを翳す黒幕描画
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);		//表面だけ描画モードに戻る
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xf);		//書き込む有効
	pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);	//ステンシル1の値の部分だけ描画
	m_CScene2D.Draw();

	//設定を戻す
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);		//Zバッファ有効
	pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);	//ステンシルバッファ無効

}

void CStencilShadow::Uninit(void)
{
	m_Model.Uninit();
	m_CScene2D.Uninit();
}