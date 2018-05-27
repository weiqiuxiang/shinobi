#include "ModelObj.h"
#include "TexManager.h"
#include "CRenderer.h"
#include "CShaderManager.h"
#include "CLightManager.h"
#include "CCameraManager.h"

CModelObj::CModelObj() :
	m_pMesh(nullptr),
	m_pBuffMat(nullptr),
	m_nNumMat(0),
	m_TexID(nullptr)
{
	memset(&m_pFilepass[0], '\0', sizeof(&m_pFilepass[0]));
}

CModelObj::~CModelObj()
{
	
}

HRESULT CModelObj::Init(void)
{
	//初期化
	if(m_pMesh != nullptr){m_pMesh->Release(); m_pMesh = nullptr;}
	if (m_pBuffMat != nullptr){m_pMesh->Release(); m_pBuffMat = nullptr;}
	if (m_TexID != nullptr){delete[] m_TexID; m_TexID = nullptr;}

	m_pMesh = nullptr;
	m_pBuffMat = nullptr;
	m_TexID = nullptr;
	m_nNumMat = 0;
	memset(&m_pFilepass[0], '\0', sizeof(&m_pFilepass[0]));

	return S_OK;
}

void CModelObj::Uninit(void)
{
	if (m_pMesh != nullptr) { 
		m_pMesh->Release(); 
		m_pMesh = nullptr;
	}
	if (m_pBuffMat != nullptr) { 
		m_pBuffMat->Release(); 
		m_pBuffMat = nullptr; 
	}
	if (m_TexID != nullptr) { 
		delete[] m_TexID; 
		m_TexID = nullptr; 
	}
}

void CModelObj::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();
	D3DXMATERIAL *pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();
	D3DMATERIAL9 matDef;

	pDevice->GetMaterial(&matDef);

	//描画
	for (int nCnt = 0; nCnt < (int)m_nNumMat; nCnt++)
	{
		pDevice->SetMaterial(&pMat[nCnt].MatD3D);
		pDevice->SetTexture(0, CTexManager::UseTex(m_TexID[nCnt]));
		m_pMesh->DrawSubset(nCnt);
	}

	pDevice->SetMaterial(&matDef);   //マテリアルを戻す
}

void CModelObj::DrawMtrl(const D3DMATERIAL9& Mtrl)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	D3DMATERIAL9 matDef;
	pDevice->GetMaterial(&matDef);

	for (int nCnt = 0; nCnt < (int)m_nNumMat; nCnt++)
	{
		pDevice->SetMaterial(&Mtrl);
		pDevice->SetTexture(0, CTexManager::UseTex(m_TexID[nCnt]));
		m_pMesh->DrawSubset(nCnt);
	}

	pDevice->SetMaterial(&matDef);   //マテリアルを戻す
}

void CModelObj::DrawOnDepthMap(const D3DXMATRIX& mtxWorld,const D3DXMATRIX& mtxLightView, const D3DXMATRIX& mtxLightProje)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	//シェーダーマネージャーのポインタを取得し、ヌルチェック
	CShaderManager* pShaderManager = CShaderManager::GetInstance();
	if (pShaderManager == nullptr) {
		return;
	}

	//ヴァーテックスシェーダーとピクセルシェーダーを取得
	LPDIRECT3DVERTEXSHADER9 pVS = pShaderManager->GetVertexShaderPointer(DEPTH_TEXTURE_VS);
	LPD3DXCONSTANTTABLE pVSCT = pShaderManager->GetVertexShaderCTPointer(DEPTH_TEXTURE_VS);
	LPDIRECT3DPIXELSHADER9 pPS = pShaderManager->GetPixelShaderPointer(DEPTH_TEXTURE_PS);

	//シェーダーをセット
	pDevice->SetVertexShader(pVS);
	pDevice->SetPixelShader(pPS);

	//シェーダーにデータを送る
	D3DXMATRIX mtxLightWVP = mtxWorld * mtxLightView * mtxLightProje;
	HRESULT hr = E_FAIL;
	hr= pVSCT->SetMatrix(pDevice, "mtxLightWVP", &mtxLightWVP);

	//描画
	for (int nCnt = 0; nCnt < (int)m_nNumMat; nCnt++)
	{
		m_pMesh->DrawSubset(nCnt);
	}

	//シェーダーをヌルに戻す
	pDevice->SetVertexShader(nullptr);
	pDevice->SetPixelShader(nullptr);
}

void CModelObj::DrawUseDepthMap(const D3DXMATRIX& worldMtx, const D3DXMATRIX& LightViewMtx, const D3DXMATRIX& LightProjeMtx, LPDIRECT3DTEXTURE9 DepthTexture)
{
	//ヌルチェック
	if (DepthTexture == nullptr) {
		_RPT0(0, "[ModelObj.cpp][DrawUseDepthMap]引数DepthTextureはヌル");
		return;
	}

	//デバイス取得
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	//シェーダーマネージャーのポインタを取得し、ヌルチェック
	CShaderManager* pShaderManager = CShaderManager::GetInstance();
	if (pShaderManager == nullptr) {
		return;
	}

	//ヴァーテックスシェーダーとピクセルシェーダーを取得
	LPDIRECT3DVERTEXSHADER9 pVS = pShaderManager->GetVertexShaderPointer(SPECULAR_DEPTH_SHADOW_VS);
	LPD3DXCONSTANTTABLE pVSCT = pShaderManager->GetVertexShaderCTPointer(SPECULAR_DEPTH_SHADOW_VS);
	LPDIRECT3DPIXELSHADER9 pPS = pShaderManager->GetPixelShaderPointer(SPECULAR_DEPTH_SHADOW_PS);
	LPD3DXCONSTANTTABLE pPSCT = pShaderManager->GetPixelShaderCTPointer(SPECULAR_DEPTH_SHADOW_PS);

	//シェーダーをセット
	pDevice->SetVertexShader(pVS);
	pDevice->SetPixelShader(pPS);

	//ヴァーテックスシェーダーの定数テーブルにデータを送る
	D3DXMATRIX mtxWVP = worldMtx * CCameraManager::GetActiveCameraViewMtx() * CCameraManager::GetActiveCameraProjeMtx();
	HRESULT hr = pVSCT->SetMatrix(pDevice, "mtxWVP", &mtxWVP);
	D3DXMATRIX mtxLightWVP = worldMtx * LightViewMtx * LightProjeMtx;
	hr = pVSCT->SetMatrix(pDevice, "mtxLightWVP", &mtxLightWVP);

	//ピクセルシェーダーの定数テーブルにデータを送る
	D3DXMATRIX worldInverse;
	D3DXMatrixInverse(&worldInverse,nullptr,&worldMtx);
	D3DXVECTOR3 EyeL = CCameraManager::GetActiveCameraPos();
	D3DXVec3TransformCoord(&EyeL, &EyeL, &worldInverse);
	hr = pPSCT->SetFloatArray(pDevice, "posEyeL", EyeL, 3);

	D3DXVECTOR3 lightDirW = CLightManager::GetLightDirection(0);
	D3DXVECTOR3 lightDirL;
	D3DXVec3TransformNormal(&lightDirL, &lightDirW, &worldInverse);
	D3DXVec3Normalize(&lightDirL, &lightDirL);
	hr = pPSCT->SetFloatArray(pDevice, "lightDirL", lightDirL, 3);
	hr = pPSCT->SetFloat(pDevice, "specularPower", 10);

	//サンプラーを送る
	UINT DepthMapSamplerIndex = pPSCT->GetSamplerIndex("DepthMapSampler");
	pDevice->SetSamplerState(DepthMapSamplerIndex, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	pDevice->SetSamplerState(DepthMapSamplerIndex, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	hr = pDevice->SetTexture(DepthMapSamplerIndex, DepthTexture);

	//描画
	D3DXMATERIAL *pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();
	for (int nCnt = 0; nCnt < (int)m_nNumMat; nCnt++)
	{
		pVSCT->SetFloatArray(pDevice, "DiffuseColor", (FLOAT*)&pMat[nCnt].MatD3D.Diffuse, 4);
		pVSCT->SetFloatArray(pDevice, "AmbientColor", (FLOAT*)&pMat[nCnt].MatD3D.Ambient, 4);
		pVSCT->SetFloatArray(pDevice, "EmissiveColor", (FLOAT*)&pMat[nCnt].MatD3D.Emissive, 4);
		UINT SamplerIndex0 = pPSCT->GetSamplerIndex("Sampler0");
		pDevice->SetTexture(SamplerIndex0, CTexManager::UseTex(m_TexID[nCnt]) );
		m_pMesh->DrawSubset(nCnt);
	}

	//シェーダーをヌルに戻す
	pDevice->SetVertexShader(nullptr);
	pDevice->SetPixelShader(nullptr);
}

void CModelObj::DrawSpecular(const D3DXMATRIX& worldMtx)
{
	//デバイス取得
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	//シェーダーマネージャーのポインタを取得し、ヌルチェック
	CShaderManager* pShaderManager = CShaderManager::GetInstance();
	if (pShaderManager == nullptr) {
		return;
	}

	//ヴァーテックスシェーダーとピクセルシェーダーを取得
	LPDIRECT3DVERTEXSHADER9 pVS = pShaderManager->GetVertexShaderPointer(SPECULAR_VS);
	LPD3DXCONSTANTTABLE pVSCT = pShaderManager->GetVertexShaderCTPointer(SPECULAR_VS);
	LPDIRECT3DPIXELSHADER9 pPS = pShaderManager->GetPixelShaderPointer(SPECULAR_PS);
	LPD3DXCONSTANTTABLE pPSCT = pShaderManager->GetPixelShaderCTPointer(SPECULAR_PS);

	//シェーダーをセット
	pDevice->SetVertexShader(pVS);
	pDevice->SetPixelShader(pPS);

	//ヴァーテックスシェーダーの定数テーブルにデータを送る
	D3DXMATRIX mtxWVP = worldMtx * CCameraManager::GetActiveCameraViewMtx() * CCameraManager::GetActiveCameraProjeMtx();
	HRESULT hr = pVSCT->SetMatrix(pDevice, "mtxWVP", &mtxWVP);

	//ピクセルシェーダーの定数テーブルにデータを送る
	D3DXMATRIX worldInverse;
	D3DXMatrixInverse(&worldInverse, nullptr, &worldMtx);
	D3DXVECTOR3 EyeL = CCameraManager::GetActiveCameraPos();
	D3DXVec3TransformCoord(&EyeL, &EyeL, &worldInverse);
	hr = pPSCT->SetFloatArray(pDevice, "posEyeL", EyeL, 3);

	D3DXVECTOR3 lightDirW = CLightManager::GetLightDirection(0);
	D3DXVECTOR3 lightDirL;
	D3DXVec3TransformNormal(&lightDirL, &lightDirW, &worldInverse);
	D3DXVec3Normalize(&lightDirL, &lightDirL);
	hr = pPSCT->SetFloatArray(pDevice, "lightDirL", lightDirL, 3);
	hr = pPSCT->SetFloat(pDevice, "specularPower", 10);

	//描画
	D3DXMATERIAL *pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();
	for (int nCnt = 0; nCnt < (int)m_nNumMat; nCnt++)
	{
		pVSCT->SetFloatArray(pDevice, "DiffuseColor", (FLOAT*)&pMat[nCnt].MatD3D.Diffuse, 4);
		pVSCT->SetFloatArray(pDevice, "AmbientColor", (FLOAT*)&pMat[nCnt].MatD3D.Ambient, 4);
		pVSCT->SetFloatArray(pDevice, "EmissiveColor", (FLOAT*)&pMat[nCnt].MatD3D.Emissive, 4);
		UINT SamplerIndex0 = pPSCT->GetSamplerIndex("Sampler0");
		pDevice->SetTexture(SamplerIndex0, CTexManager::UseTex(m_TexID[nCnt]));
		m_pMesh->DrawSubset(nCnt);
	}

	//シェーダーをヌルに戻す
	pDevice->SetVertexShader(nullptr);
	pDevice->SetPixelShader(nullptr);
}

void CModelObj::DrawMotionBlur(const D3DXMATRIX& worldMtx, const D3DXMATRIX& worldMtxOld, LPDIRECT3DTEXTURE9 OriginalTexture)
{
	//デバイス取得
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	//シェーダーマネージャーのポインタを取得し、ヌルチェック
	CShaderManager* pShaderManager = CShaderManager::GetInstance();
	if (pShaderManager == nullptr) {
		return;
	}

	//ヴァーテックスシェーダーとピクセルシェーダーを取得
	LPDIRECT3DVERTEXSHADER9 pVS = pShaderManager->GetVertexShaderPointer(MOTION_BLUR_VS);
	LPD3DXCONSTANTTABLE pVSCT = pShaderManager->GetVertexShaderCTPointer(MOTION_BLUR_VS);
	LPDIRECT3DPIXELSHADER9 pPS = pShaderManager->GetPixelShaderPointer(MOTION_BLUR_PS);
	LPD3DXCONSTANTTABLE pPSCT = pShaderManager->GetPixelShaderCTPointer(MOTION_BLUR_PS);

	//シェーダーをセット
	pDevice->SetVertexShader(pVS);
	pDevice->SetPixelShader(pPS);

	//バーテックスシェーダーの定数テーブルにデータを送る
	D3DXMATRIX mtxVP = CCameraManager::GetActiveCameraViewMtx() * CCameraManager::GetActiveCameraProjeMtx();
	D3DXMATRIX mtxWVP = worldMtx * mtxVP;
	HRESULT hr = pVSCT->SetMatrix(pDevice, "mtxWVP", &mtxWVP);
	D3DXMATRIX mtxWVPOld = worldMtxOld * mtxVP;
	hr = pVSCT->SetMatrix(pDevice, "mtxWVPOld", &mtxWVPOld);

	D3DXMATRIX mtxWITVP;
	D3DXMatrixInverse(&mtxWITVP, nullptr, &worldMtx);
	D3DXMatrixTranspose(&mtxWITVP, &mtxWITVP);
	mtxWITVP = mtxWITVP * mtxVP;
	hr = pVSCT->SetMatrix(pDevice, "mtxWITVP", &mtxWITVP);

	//ピクセルシェーダーの定数テーブルにデータを送る
	UINT OriginalSamplerIndex = pPSCT->GetSamplerIndex("OriginalSampler");
	pDevice->SetSamplerState(OriginalSamplerIndex, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	pDevice->SetSamplerState(OriginalSamplerIndex, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	hr = pDevice->SetTexture(OriginalSamplerIndex, OriginalTexture);

	//モデル描画
	for (int nCnt = 0; nCnt < (int)m_nNumMat; nCnt++)
	{
		m_pMesh->DrawSubset(nCnt);
	}

	//シェーダーをヌルに戻す
	pDevice->SetVertexShader(nullptr);
	pDevice->SetPixelShader(nullptr);
}

//-----------------------
//ファンクション関数
//-----------------------
//モデルのファイルパスを設定
void CModelObj::SetModelPass(const char* filepass)
{
	strcpy(&m_pFilepass[0], filepass);     //ファイルパス記録
}

//α設定
void CModelObj::SetAlpha(float Alpha)
{
	Alpha = max(Alpha,0.0f);
	Alpha = min(1.0f, Alpha);

	D3DXMATERIAL *pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();
	for (int nCnt = 0; nCnt < (int)m_nNumMat; nCnt++) {
		pMat[nCnt].MatD3D.Diffuse.a = Alpha;
		pMat[nCnt].MatD3D.Ambient.a = Alpha;
		pMat[nCnt].MatD3D.Emissive.a = Alpha;
	}
}

//モデルパスの取得
const char *CModelObj::GetModelPass(void)
{
	return m_pFilepass;
}

//モデルオブジェの生成
void CModelObj::CreateModelObj(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	m_pMesh = nullptr;
	m_pBuffMat = nullptr;
	
	//モデルデータの読み込み
	if (FAILED(D3DXLoadMeshFromX(m_pFilepass, D3DXMESH_MANAGED/*指数メモリ*/, pDevice/*デバイス*/, nullptr/*隣接バッファ*/, &m_pBuffMat/*マテリアル情報を格納する*/, nullptr, &m_nNumMat/*マテリアル数*/, &m_pMesh/*メッシュ*/)))
	{
		MessageBox(nullptr, "モデルMODEL_NAMEの読み込みが失敗した", "エラー", MB_OK | MB_ICONHAND);
		return;
	}

	D3DXMATERIAL *pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();   //マテリアル情報のポインタとしてバッファのアドレスを取得

	//テクスチャ読み込む
	if (m_TexID != nullptr)
	{
		delete[] m_TexID;
		m_TexID = nullptr;
	}
	m_TexID = new TEX_ID[(TEX_ID)m_nNumMat];
	for (int nCnt = 0; nCnt < (int)m_nNumMat; nCnt++)
	{
		m_TexID[nCnt] = CTexManager::LoadTex(pMat[nCnt].pTextureFilename);
	}
}