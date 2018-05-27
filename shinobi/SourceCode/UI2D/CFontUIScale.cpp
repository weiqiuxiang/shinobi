#include "CFontUIScale.h"
#include "SceneTransition.h"
#include "CRenderer.h"

#define SCROLL_SPEED (0.005f)

CFontUIScale::CFontUIScale()
{

}

CFontUIScale::~CFontUIScale()
{

}

//プレーヤーの初期化
HRESULT CFontUIScale::Init(const D3DXVECTOR3 &Pos, const D3DXVECTOR3 &pSize, const char *filePass)
{
	CScene2D::Init(Pos, pSize, filePass);

	//スケール関係
	m_OriginPos = Pos;
	m_OriginSize = pSize;
	m_Scale = D3DXVECTOR2(1.0f,1.0f);

	return S_OK;
}

//更新処理
void CFontUIScale::Update(void)
{
	D3DXVECTOR3 DistanceFromSP = m_OriginPos - m_ScalePoint;	//元原点とスケール中心の差分
	//スケールの後の差分サイズ
	DistanceFromSP.x = DistanceFromSP.x * m_Scale.x;
	DistanceFromSP.y = DistanceFromSP.y * m_Scale.y;
	m_pos = m_ScalePoint + DistanceFromSP;						//スケールCenter座標を求める
	//スケール後のサイズ
	m_size.x = m_OriginSize.x * m_Scale.x;
	m_size.y = m_OriginSize.y * m_Scale.y;

	CScene2D::VertexBufferLockUV();
}

//描画処理
void CFontUIScale::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();
	if (m_bAddBlending == true){
		//加算有効
		pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

		CScene2D::Draw();

		//加算無効
		pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	}

	else {
		CScene2D::Draw();
	}
}

//終了処理
void CFontUIScale::Uninit(void)
{
	CScene2D::Uninit();
}