#include "CPathBall.h"
#include "CRenderer.h"

CPathBall::CPathBall()
{
	m_pMesh = nullptr;
	D3DXMatrixIdentity(&m_WorldMtx);
}

CPathBall::~CPathBall()
{
	if (nullptr != m_pMesh)
	{
		m_pMesh->Release();
		m_pMesh = nullptr;
	}
}

HRESULT CPathBall::Init(float Radius)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	if (nullptr != m_pMesh)               //nullptrチェック
	{
		m_pMesh->Release();
		m_pMesh = nullptr;
	}

	if (FAILED(D3DXCreateSphere(
		pDevice,        // 描画先のデバイス
		1.0f,           // 半径。0.0fでも実はOK。マイナスはNG
		16,             // スライスの数。増やすときれいな球になるはず
		8,              // スタック数。増やすときれいな球になるはず
		&m_pMesh,         // 出力するメッシュ
		nullptr            // あまり使わないらしいんでnullptrにしといた
	)))
	{
		MessageBox(nullptr, "SphereのCreateが失敗", "エラー", MB_OK | MB_ICONHAND);
		return E_FAIL;
	}

	m_Radius = max(0.01f, Radius);
	m_scl = D3DXVECTOR3(m_Radius, m_Radius, m_Radius);

	//マテリアルの初期化(灰色)
	m_Mtrl.Diffuse = { 0.6f,0.6f,0.6f,0.8f };
	m_Mtrl.Ambient = { 0.2f,0.2f,0.2f,0.4f };
	m_Mtrl.Specular = { 0.0f,0.0f,0.0f,0.0f };
	m_Mtrl.Emissive = { 0.6f,0.6f,0.6f,0.8f };
	m_Mtrl.Power = 6.0f;

	m_MtrlSelectedByEditor.Diffuse = { 0.0f,0.6f,0.0f,0.8f };
	m_MtrlSelectedByEditor.Ambient = { 0.2f,0.2f,0.2f,0.4f };
	m_MtrlSelectedByEditor.Specular = { 0.0f,0.0f,0.0f,0.0f };
	m_MtrlSelectedByEditor.Emissive = { 0.0f,0.6f,0.0f,0.8f };
	m_MtrlSelectedByEditor.Power = 6.0f;

	return S_OK;
}

void CPathBall::MtxWorldSetting(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();             //デバイス取得
	D3DXMATRIX mtxScale/*拡大行列*/, mtxTran/*移動行列*/, mtxResult/*結果行列*/;

	//ワールド行列の設定
	D3DXMatrixScaling(&mtxScale, m_scl.x, m_scl.y, m_scl.z);
	D3DXMatrixTranslation(&mtxTran,m_pos.x,m_pos.y+m_Radius,m_pos.z);
	mtxResult = mtxScale*mtxTran;								//結果の変換行列の計算
	D3DXMatrixIdentity(&m_WorldMtx);							//ワールド行列を単位行列で初期化
	D3DXMatrixMultiply(&m_WorldMtx, &m_WorldMtx, &mtxResult);	//回転行列とワールド行列の掛け算
	pDevice->SetTransform(D3DTS_WORLD, &m_WorldMtx);			//ワールド変換行列
}

void CPathBall::Update(void)
{
	
}

void CPathBall::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	D3DMATERIAL9 matDef;
	pDevice->GetMaterial(&matDef);
	MtxWorldSetting();										//ワールド行列設定
	pDevice->SetMaterial(&m_Mtrl);
	m_pMesh->DrawSubset(0);
	pDevice->SetMaterial(&matDef);							//マテリアルを戻す
}

//エディター用描画
void CPathBall::DrawSelectedByEditor(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	D3DMATERIAL9 matDef;
	pDevice->GetMaterial(&matDef);
	MtxWorldSetting();										//ワールド行列設定
	pDevice->SetMaterial(&m_MtrlSelectedByEditor);
	m_pMesh->DrawSubset(0);
	pDevice->SetMaterial(&matDef);							//マテリアルを戻す
}

void CPathBall::Uninit(void)
{
	if (nullptr != m_pMesh)
	{
		m_pMesh->Release();
		m_pMesh = nullptr;
	}
}