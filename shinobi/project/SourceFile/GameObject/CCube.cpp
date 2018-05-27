#include "CCube.h"
#include "CRenderer.h"

CCube::CCube()
{
	m_pMesh = nullptr;
	m_X = 0;
	m_Y = 0;
	m_Z = 0;
	D3DXMatrixIdentity(&m_WorldMtx);
}

CCube::~CCube()
{
	if (nullptr != m_pMesh)
	{
		m_pMesh->Release();
		m_pMesh = nullptr;
	}
}

HRESULT CCube::Init(float X, float Y, float Z)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	if (nullptr != m_pMesh)               //nullptrチェック
	{
		m_pMesh->Release();
		m_pMesh = nullptr;
	}

	if (FAILED(D3DXCreateBox(pDevice,X,Y,Z,&m_pMesh,nullptr)))
	{
		MessageBox(nullptr, "CubeのCreateが失敗", "エラー", MB_OK | MB_ICONHAND);
		return E_FAIL;
	}

	m_X = X;
	m_Y = Y;
	m_Z = Z;

	m_scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	//マテリアルの初期化(灰色)
	m_Mtrl.Diffuse = { 0.0f,0.0f,0.8f,0.5f };
	m_Mtrl.Ambient = { 0.2f,0.2f,0.2f,0.4f };
	m_Mtrl.Specular = { 0.0f,0.0f,0.0f,0.0f };
	m_Mtrl.Emissive = { 0.0f,0.0f,0.8f,0.5f };
	m_Mtrl.Power = 6.0f;

	m_MtrlSelectedByEditor.Diffuse = { 0.0f,0.0f,0.0f,0.0f };
	m_MtrlSelectedByEditor.Ambient = { 0.2f,0.2f,0.2f,0.4f };
	m_MtrlSelectedByEditor.Specular = { 0.0f,0.0f,0.0f,0.0f };
	m_MtrlSelectedByEditor.Emissive = { 0.0f,0.6f,0.0f,0.8f };
	m_MtrlSelectedByEditor.Power = 6.0f;

	return S_OK;
}

void CCube::MtxWorldSetting(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();             //デバイス取得
	D3DXMATRIX mtxScale/*拡大行列*/, mtxTran/*移動行列*/, mtxResult/*結果行列*/;

	//ワールド行列の設定
	D3DXMatrixScaling(&mtxScale, m_scl.x, m_scl.y, m_scl.z);
	D3DXMatrixTranslation(&mtxTran,m_pos.x,m_pos.y+ m_Y * 0.5f,m_pos.z);
	mtxResult = mtxScale*mtxTran;								//結果の変換行列の計算
	D3DXMatrixIdentity(&m_WorldMtx);							//ワールド行列を単位行列で初期化
	D3DXMatrixMultiply(&m_WorldMtx, &m_WorldMtx, &mtxResult);	//回転行列とワールド行列の掛け算
	pDevice->SetTransform(D3DTS_WORLD, &m_WorldMtx);			//ワールド変換行列
}

void CCube::Update(void)
{
	
}

void CCube::Draw(void)
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
void CCube::DrawSelectedByEditor(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	D3DMATERIAL9 matDef;
	pDevice->GetMaterial(&matDef);
	MtxWorldSetting();										//ワールド行列設定
	pDevice->SetMaterial(&m_MtrlSelectedByEditor);
	m_pMesh->DrawSubset(0);
	pDevice->SetMaterial(&matDef);							//マテリアルを戻す
}

void CCube::Uninit(void)
{
	if (nullptr != m_pMesh)
	{
		m_pMesh->Release();
		m_pMesh = nullptr;
	}
}