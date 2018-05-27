#include "CMeshField.h"
#include "TexManager.h"
#include "CRenderer.h"
#include "CPolygonMath.h"
#include "CShaderManager.h"
#include "CCameraManager.h"
#include "CLightManager.h"

CMeshField::CMeshField()
{
	m_pVtxBuffer = nullptr;
	m_pIdxBuffer = nullptr;
	m_MeshVertex = nullptr;
	m_MeshVertexLock = nullptr;
	m_MeshFace = nullptr;
	m_VertexHighRelative = nullptr;
	m_IndexData = { 0,0,0 };
	m_IndexSize = CINDEX_SIZE(D3DXVECTOR2(0.0f,0.0f), D3DXVECTOR2(0.0f,0.0f));
	m_TexID = -1;
	m_pVertexDeclaration = nullptr;
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
}

CMeshField::~CMeshField()
{
	if (m_pVtxBuffer != nullptr) { m_pVtxBuffer->Release(); m_pVtxBuffer = nullptr; }
	if (m_pIdxBuffer != nullptr) { m_pIdxBuffer->Release(); m_pIdxBuffer = nullptr; }
	if (m_MeshVertex != nullptr) { delete[] m_MeshVertex; m_MeshVertex = nullptr; }
	if (m_MeshVertexLock != nullptr) { delete[] m_MeshVertexLock; m_MeshVertexLock = nullptr; }
	if (m_MeshFace != nullptr) { delete[] m_MeshFace; m_MeshFace = nullptr; }
	if (m_VertexHighRelative != nullptr) { delete[] m_VertexHighRelative; m_VertexHighRelative = nullptr; }
}

HRESULT CMeshField::Init(const D3DXVECTOR3& Pos,const CINDEX_SIZE& IndexSize,const char *TexFilePass)
{
	//未解放防止
	if (m_pVtxBuffer != nullptr) { m_pVtxBuffer->Release(); m_pVtxBuffer = nullptr; }
	if (m_pIdxBuffer != nullptr) { m_pIdxBuffer->Release(); m_pIdxBuffer = nullptr; }

	//ポインタ初期化
	m_pVtxBuffer = nullptr;
	m_pIdxBuffer = nullptr;

	//パラメータ設定
	m_pos = Pos;
	m_rot = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	m_IndexSize = IndexSize;

	//値が1より小さいかどうかチェック
	m_IndexSize.m_PieceNum.x = max(1, m_IndexSize.m_PieceNum.x); 
	m_IndexSize.m_PieceNum.y = max(1, m_IndexSize.m_PieceNum.y);

	//テクスチャ読み込み
	m_TexID = -1;
	m_TexID = CTexManager::LoadTex(TexFilePass);       //テクスチャロード

	//頂点数とインデックス数と三角ポリゴンの計算
	m_IndexData.nVertexNum = ((int)m_IndexSize.m_PieceNum.x + 1) * ((int)m_IndexSize.m_PieceNum.y + 1);
	m_IndexData.nIndexNum = (2 + 2 * (int)m_IndexSize.m_PieceNum.x) * (int)m_IndexSize.m_PieceNum.y + ((int)m_IndexSize.m_PieceNum.y - 1) * 2;
	m_IndexData.nPolygonNum = m_IndexData.nIndexNum - 2;

	if (m_pVertexBuffer != nullptr) {
		delete[] m_pVertexBuffer;
	}
	m_pVertexBuffer = new VERTEX_MESHFIELD[m_IndexData.nVertexNum];

	if (m_pIndexBuffer == nullptr) {
		delete[] m_pIndexBuffer;
	}
	m_pIndexBuffer = new WORD[m_IndexData.nIndexNum];

	InitLockVertexBufAndIndexBuf();			//バッファ確保
	InitMeshVertexAndFace();				//頂点と面情報初期化
	HRESULT hr = CreateDeclaration();		//declaration初期化
	if (hr == E_FAIL) {
		return E_FAIL;
	}

	return S_OK;
}

void CMeshField::InitMeshVertexAndFace(void)
{
	//nullptrチェック
	if (m_MeshVertexLock != nullptr) { delete[] m_MeshVertexLock; m_MeshVertexLock = nullptr; }
	if (m_MeshVertex != nullptr) { delete[] m_MeshVertex; m_MeshVertex = nullptr; }
	if (m_MeshFace != nullptr) { delete[] m_MeshFace; m_MeshFace = nullptr; }
	if( m_VertexHighRelative != nullptr) { delete[] m_VertexHighRelative; m_VertexHighRelative = nullptr; }
	m_MeshVertex = nullptr;
	m_MeshFace = nullptr;
	m_VertexHighRelative = nullptr;

	//領域確保
	m_MeshVertexLock = new MESH_VERTEX[m_IndexData.nVertexNum];
	m_MeshVertex = new MESH_VERTEX[m_IndexData.nVertexNum];
	m_VertexHighRelative = new double[m_IndexData.nVertexNum];
	int FaceNum = (int)(m_IndexSize.m_PieceNum.x * m_IndexSize.m_PieceNum.y * 2);
	m_MeshFace = new MESH_FACE[FaceNum];

	InitVertexAndFacePara();
}

void CMeshField::InitVertexAndFacePara(void)
{
	D3DXVECTOR3 TotalSize;
	TotalSize.x = m_IndexSize.m_PieceSize.x * m_IndexSize.m_PieceNum.x;
	TotalSize.z = m_IndexSize.m_PieceSize.y * m_IndexSize.m_PieceNum.y;

	D3DXVECTOR3 StartPos;
	StartPos.x = -TotalSize.x * 0.5f;
	StartPos.z = TotalSize.z * 0.5f;

	//頂点相対高さ情報登録
	for (int i = 0; i < (m_IndexSize.m_PieceNum.y + 1); i++)
	{
		for (int j = 0; j < (m_IndexSize.m_PieceNum.x + 1); j++)
		{
			int k = i * ((int)m_IndexSize.m_PieceNum.x + 1) + j;
			m_VertexHighRelative[0 + k] = 0.0f;
		}
	}

	//頂点情報登録
	for (int i = 0; i < (m_IndexSize.m_PieceNum.y + 1); i++)
	{
		for (int j = 0; j < (m_IndexSize.m_PieceNum.x + 1); j++)
		{
			int k = i * ((int)m_IndexSize.m_PieceNum.x + 1) + j;
			m_MeshVertex[0 + k].pos = D3DXVECTOR3(StartPos.x + j * m_IndexSize.m_PieceSize.x + m_pos.x, m_pos.y, StartPos.z - i * m_IndexSize.m_PieceSize.y + m_pos.z);
			m_MeshVertex[0 + k].Normalize = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		}
	}

	//ローカル描画頂点登録
	for (int i = 0; i < (m_IndexSize.m_PieceNum.y + 1); i++)
	{
		for (int j = 0; j < (m_IndexSize.m_PieceNum.x + 1); j++)
		{
			int k = i * ((int)m_IndexSize.m_PieceNum.x + 1) + j;
			m_MeshVertexLock[0 + k].pos = D3DXVECTOR3(StartPos.x + j * m_IndexSize.m_PieceSize.x, 0.0f, StartPos.z - i * m_IndexSize.m_PieceSize.y);
			m_MeshVertexLock[0 + k].Normalize = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		}
	}

	//面情報登録
	for (int i = 0; i < m_IndexSize.m_PieceNum.y; i++)
	{
		for (int j = 0; j < m_IndexSize.m_PieceNum.x; j++)
		{
			int k = i * ((int)m_IndexSize.m_PieceNum.x * 2) + j * 2;
			int VertexHorNum = (int)m_IndexSize.m_PieceNum.x + 1;    //横方向の頂点数
			int VertexK = i * ((int)m_IndexSize.m_PieceNum.x) + j + i;

			m_MeshFace[k].p0 = m_MeshVertex[VertexK + VertexHorNum].pos;
			m_MeshFace[k].p1 = m_MeshVertex[VertexK].pos;
			m_MeshFace[k].p2 = m_MeshVertex[VertexK + 1 + VertexHorNum].pos;
			m_MeshFace[k].Normalize = CPolygonMath::CalcTriNormalize(m_MeshFace[k]);  //法線計算

			m_MeshFace[k + 1].p0 = m_MeshVertex[VertexK + 1 + VertexHorNum].pos;
			m_MeshFace[k + 1].p1 = m_MeshVertex[VertexK].pos;
			m_MeshFace[k + 1].p2 = m_MeshVertex[VertexK + 1].pos;
			m_MeshFace[k + 1].Normalize = CPolygonMath::CalcTriNormalize(m_MeshFace[k + 1]);  //法線計算
		}
	}
}

void CMeshField::Uninit(void)
{
	if (m_pVtxBuffer != nullptr) { m_pVtxBuffer->Release(); m_pVtxBuffer = nullptr; }
	if (m_pIdxBuffer != nullptr) { m_pIdxBuffer->Release(); m_pIdxBuffer = nullptr; }
	if (m_MeshVertex != nullptr) { delete[] m_MeshVertex; m_MeshVertex = nullptr; }
	if (m_MeshVertexLock != nullptr) { delete[] m_MeshVertexLock; m_MeshVertexLock = nullptr; }
	if (m_MeshFace != nullptr) { delete[] m_MeshFace; m_MeshFace = nullptr; }
	if (m_VertexHighRelative != nullptr) { delete[] m_VertexHighRelative; m_VertexHighRelative = nullptr; }

	if (m_pVertexBuffer != nullptr) {
		delete[] m_pVertexBuffer;
		m_pVertexBuffer = nullptr;
	}
	if (m_pIndexBuffer != nullptr) {
		delete[] m_pIndexBuffer;
		m_pIndexBuffer = nullptr;
	}
}

void CMeshField::Update(void)
{
	LockVertexBuf();				//頂点バッファロック
	LockIndexBuf();					//インデックスロック
}

void CMeshField::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	//頂点declaration設置
	pDevice->SetVertexDeclaration(m_pVertexDeclaration);

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

	//頂点シェーダーにデータを送る
	D3DXMATRIX mtxWVP = CalcWorldMtx() * CCameraManager::GetActiveCameraViewMtx() * CCameraManager::GetActiveCameraProjeMtx();
	HRESULT hr = pVSCT->SetMatrix(pDevice, "mtxWVP", &mtxWVP);

	//ピクセルシェーダーにデータを送る
	D3DXVECTOR3 EyeL = CCameraManager::GetActiveCameraPos();
	D3DXVec3TransformCoord(&EyeL, &EyeL, &CalcWorldInverseMtx());
	hr = pPSCT->SetFloatArray(pDevice, "posEyeL", EyeL, 3);

	D3DXVECTOR3 lightDirW = CLightManager::GetLightDirection(0);
	D3DXVECTOR3 lightDirL;
	D3DXVec3TransformNormal(&lightDirL, &lightDirW, &CalcWorldInverseMtx());
	D3DXVec3Normalize(&lightDirL, &lightDirL);
	hr = pPSCT->SetFloatArray(pDevice, "lightDirL", lightDirL, 3);
	hr = pPSCT->SetFloat(pDevice, "specularPower", 50);

	//サンプラーを送る
	UINT TextureSamplerIndex = pPSCT->GetSamplerIndex("Sampler0");
	pDevice->SetSamplerState(TextureSamplerIndex, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	pDevice->SetSamplerState(TextureSamplerIndex, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	hr = pDevice->SetTexture(TextureSamplerIndex, CTexManager::UseTex(m_TexID));

	//描画
	D3DXMATERIAL Mat;
	Mat.MatD3D.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	Mat.MatD3D.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
	Mat.MatD3D.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);

	pVSCT->SetFloatArray(pDevice, "DiffuseColor", (FLOAT*)&Mat.MatD3D.Diffuse, 4);
	pVSCT->SetFloatArray(pDevice, "AmbientColor", (FLOAT*)&Mat.MatD3D.Ambient, 4);
	pVSCT->SetFloatArray(pDevice, "EmissiveColor", (FLOAT*)&Mat.MatD3D.Emissive, 4);
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_MESHFIELD));
	pDevice->SetIndices(m_pIdxBuffer);
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, m_IndexData.nVertexNum, 0, m_IndexData.nPolygonNum);

	//シェーダーをヌルに戻す
	pDevice->SetVertexShader(nullptr);
	pDevice->SetPixelShader(nullptr);
}

void CMeshField::DrawOnDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	//頂点declaration設置
	pDevice->SetVertexDeclaration(m_pVertexDeclaration);

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
	D3DXMATRIX mtxLightWVP = CalcWorldMtx() * LightView * LightProje;
	HRESULT hr = E_FAIL;
	hr = pVSCT->SetMatrix(pDevice, "mtxLightWVP", &mtxLightWVP);

	//描画
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_MESHFIELD));
	pDevice->SetIndices(m_pIdxBuffer);
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, m_IndexData.nVertexNum, 0, m_IndexData.nPolygonNum);

	//シェーダーをヌルに戻す
	pDevice->SetVertexShader(nullptr);
	pDevice->SetPixelShader(nullptr);
}

void CMeshField::DrawUseDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje, LPDIRECT3DTEXTURE9 DepthTexture)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	//頂点declaration設置
	pDevice->SetVertexDeclaration(m_pVertexDeclaration);

	//シェーダーマネージャーのポインタを取得し、ヌルチェック
	CShaderManager* pShaderManager = CShaderManager::GetInstance();
	if (pShaderManager == nullptr) {
		return;
	}

	//ヴァーテックスシェーダーとピクセルシェーダーを取得
	LPDIRECT3DVERTEXSHADER9 pVS = pShaderManager->GetVertexShaderPointer(DIFFUSE_DEPTH_SHADOW_VS);
	LPD3DXCONSTANTTABLE pVSCT = pShaderManager->GetVertexShaderCTPointer(DIFFUSE_DEPTH_SHADOW_VS);
	LPDIRECT3DPIXELSHADER9 pPS = pShaderManager->GetPixelShaderPointer(DIFFUSE_DEPTH_SHADOW_PS);
	LPD3DXCONSTANTTABLE pPSCT = pShaderManager->GetPixelShaderCTPointer(DIFFUSE_DEPTH_SHADOW_PS);

	//シェーダーをセット
	pDevice->SetVertexShader(pVS);
	pDevice->SetPixelShader(pPS);

	//頂点シェーダーにデータを送る
	D3DXMATRIX mtxLightWVP = CalcWorldMtx() * LightView * LightProje;
	HRESULT hr = pVSCT->SetMatrix(pDevice, "mtxLightWVP", &mtxLightWVP);

	D3DXMATRIX mtxWVP = CalcWorldMtx() * CCameraManager::GetActiveCameraViewMtx() * CCameraManager::GetActiveCameraProjeMtx();
	hr = pVSCT->SetMatrix(pDevice, "mtxWVP", &mtxWVP);

	//ピクセルシェーダーにデータを送る
	D3DXVECTOR3 lightDirW = CLightManager::GetLightDirection(0);
	D3DXVECTOR3 lightDirL;
	D3DXVec3TransformNormal(&lightDirL, &lightDirW, &CalcWorldInverseMtx());
	D3DXVec3Normalize(&lightDirL, &lightDirL);
	hr = pPSCT->SetFloatArray(pDevice, "lightDirL", lightDirL, 3);

	//サンプラーを送る
	UINT DepthMapSamplerIndex = pPSCT->GetSamplerIndex("DepthMapSampler");
	pDevice->SetSamplerState(DepthMapSamplerIndex, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
	pDevice->SetSamplerState(DepthMapSamplerIndex, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
	pDevice->SetSamplerState(DepthMapSamplerIndex, D3DSAMP_BORDERCOLOR, 0xffffffff);
	hr = pDevice->SetTexture(DepthMapSamplerIndex, DepthTexture);

	UINT TextureSamplerIndex = pPSCT->GetSamplerIndex("Sampler0");
	pDevice->SetSamplerState(TextureSamplerIndex, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	pDevice->SetSamplerState(TextureSamplerIndex, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	hr = pDevice->SetTexture(TextureSamplerIndex, CTexManager::UseTex(m_TexID));

	//描画
	D3DXMATERIAL Mat;
	Mat.MatD3D.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	Mat.MatD3D.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);
	Mat.MatD3D.Emissive = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);

	pVSCT->SetFloatArray(pDevice, "DiffuseColor", (FLOAT*)&Mat.MatD3D.Diffuse, 4);
	pVSCT->SetFloatArray(pDevice, "AmbientColor", (FLOAT*)&Mat.MatD3D.Ambient, 4);
	pVSCT->SetFloatArray(pDevice, "EmissiveColor", (FLOAT*)&Mat.MatD3D.Emissive, 4);
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_MESHFIELD));
	pDevice->SetIndices(m_pIdxBuffer);
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, m_IndexData.nVertexNum, 0, m_IndexData.nPolygonNum);

	//シェーダーをヌルに戻す
	pDevice->SetVertexShader(nullptr);
	pDevice->SetPixelShader(nullptr);
}

HRESULT CMeshField::InitLockVertexBufAndIndexBuf(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	//頂点バッファの生成
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_MESHFIELD) * m_IndexData.nVertexNum, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &m_pVtxBuffer, nullptr)))
	{
		MessageBox(nullptr, "メッシュフィールドの頂点バッファ確保が失敗しました", "エラー", MB_OK | MB_ICONHAND);
		return E_FAIL;
	}

	//インデックスバッファの生成
	HRESULT hr;
	hr = pDevice->CreateIndexBuffer(
		sizeof(WORD)*m_IndexData.nIndexNum,	//バッファ量(2バイト)
		D3DUSAGE_WRITEONLY,					//使用方法フラグ
		D3DFMT_INDEX16,						//インデックスフォーマット(重複しない頂点数は65536個は限界)
		D3DPOOL_MANAGED,					//メモリ管理方法
		&m_pIdxBuffer,						//インデックスバッファインターフェースへのポインタのアドレス
		nullptr);
	//失敗チェック
	if (FAILED(hr))
	{
		MessageBox(nullptr, "メッシュフィールドのインデックスバッファ確保が失敗しました", "エラー", MB_OK | MB_ICONHAND);
		return E_FAIL;
	}

	return S_OK;
}

//頂点ロック
void CMeshField::LockVertexBuf(void)
{
	VERTEX_MESHFIELD *pVtx = nullptr;

	//頂点ロック
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);
	memcpy(pVtx, m_pVertexBuffer, sizeof(m_pVertexBuffer));
	for (int i = 0; i < (m_IndexSize.m_PieceNum.y + 1); i++)
	{
		for (int j = 0; j < (m_IndexSize.m_PieceNum.x + 1); j++)
		{
			int k = i * ((int)m_IndexSize.m_PieceNum.x + 1) + j;
			pVtx[0 + k].pos = m_MeshVertexLock[0 + k].pos;
			pVtx[0 + k].normal = m_MeshVertexLock[0 + k].Normalize;
			pVtx[0 + k].tex = D3DXVECTOR2(j*1.0f, i*1.0f);
		}
	}

	m_pVtxBuffer->Unlock();
}

void CMeshField::LockIndexBuf(void)
{
	WORD *pIdx = nullptr;

	m_pIdxBuffer->Lock(0, 0, (void**)&pIdx, 0);
	memcpy(pIdx, m_pIndexBuffer, sizeof(m_pIndexBuffer));
	//インデックスバッファ登録
	for (int Y = 0; Y < (int)m_IndexSize.m_PieceNum.y; Y++)
	{
		//普通部分
		for (int X = 0; X < (int)m_IndexSize.m_PieceNum.x + 1; X++)
		{
			int k = X * 2 + Y * ((int)m_IndexSize.m_PieceNum.x + 2) * 2;
			pIdx[0 + k] = (WORD)( (Y + 1)*((int)m_IndexSize.m_PieceNum.x + 1) + X );
			pIdx[1 + k] = pIdx[0 + k] - (WORD)((int)m_IndexSize.m_PieceNum.x + 1);
		}

		int q = (2 + 2 * (int)m_IndexSize.m_PieceNum.x)*(Y + 1) + ((Y + 1) - 1) * 2;

		//縮退部分
		if (q < m_IndexData.nIndexNum)
		{
			pIdx[q] = pIdx[q - 1];         //縮退1の頂点番号
			pIdx[q + 1] = pIdx[q] + (WORD)((int)m_IndexSize.m_PieceNum.x + 1) + 1;     //縮退2の頂点番号
		}
	}

	m_pIdxBuffer->Unlock();
}

HRESULT CMeshField::CreateDeclaration(void)
{
	D3DVERTEXELEMENT9 elements[] = {
		{ 0,0,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,0 },
		{ 0,12,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_NORMAL,0 },
		{ 0,24,D3DDECLTYPE_FLOAT2,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_TEXCOORD,0 },
		D3DDECL_END()
	};

	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	if (pDevice->CreateVertexDeclaration(elements, &m_pVertexDeclaration) == E_FAIL) {
		return E_FAIL;
	}

	return S_OK;
}

void CMeshField::MtxSetting(const SRP& SclPosRot)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();             //デバイス取得
	D3DXMATRIX mtxScale/*拡大行列*/, mtxRot/*回転行列*/, mtxMove/*移動行列*/, mtxResult/*結果行列*/, WorldMatrix;

	//ワールド行列の設定
	D3DXMatrixScaling(&mtxScale, SclPosRot.scl.x, SclPosRot.scl.y, SclPosRot.scl.z);
	D3DXMatrixRotationYawPitchRoll(&mtxRot, SclPosRot.rot.y/*y軸*/, SclPosRot.rot.x/*x軸*/, SclPosRot.rot.z/*z軸*/); //回転行列を設定
	D3DXMatrixTranslation(&mtxMove, SclPosRot.pos.x, SclPosRot.pos.y, SclPosRot.pos.z);
	mtxResult = mtxScale*mtxRot*mtxMove;                                           //結果の変換行列の計算
	D3DXMatrixIdentity(&WorldMatrix);                              //ワールド行列を単位行列で初期化
	D3DXMatrixMultiply(&WorldMatrix, &WorldMatrix, &mtxResult);    //回転行列とワールド行列の掛け算
	pDevice->SetTransform(D3DTS_WORLD, &WorldMatrix);              //ワールド変換行列
}

D3DXMATRIX CMeshField::CalcWorldMtx(void)
{
	D3DXMATRIX mtxScale, mtxRot, mtxMove, mtxResult;
	
	D3DXMatrixScaling(&mtxScale, m_scl.x, m_scl.y, m_scl.z);
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixTranslation(&mtxMove, m_pos.x, m_pos.y, m_pos.z);
	mtxResult = mtxScale*mtxRot*mtxMove;

	return mtxResult;
}

D3DXMATRIX CMeshField::CalcWorldInverseMtx(void)
{
	D3DXMATRIX mtxScale, mtxRot, mtxMove, mtxResult;

	D3DXMatrixScaling(&mtxScale, m_scl.x, m_scl.y, m_scl.z);
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixTranslation(&mtxMove, m_pos.x, m_pos.y, m_pos.z);
	mtxResult = mtxScale*mtxRot*mtxMove;
	D3DXMatrixInverse(&mtxResult,nullptr, &mtxResult);

	return mtxResult;
}

void CMeshField::LoadRelativeHigh(const double *MeshHigh)
{
	//頂点相対高さ情報登録
	for (int i = 0; i < (m_IndexSize.m_PieceNum.y + 1); i++)
	{
		for (int j = 0; j < (m_IndexSize.m_PieceNum.x + 1); j++)
		{
			int k = i * ((int)m_IndexSize.m_PieceNum.x + 1) + j;
			if (&MeshHigh[k] == nullptr) //nullptrチェック
			{
				MessageBox(nullptr, "ロードしたメッシュフィールド相対標高の配列がnullptrが存在します！", "WARNING", MB_OK | MB_ICONHAND);
				continue;
			}
			m_VertexHighRelative[k] = MeshHigh[k];
		}
	}

	//頂点のローカル高さ登録
	for (int i = 0; i < (m_IndexSize.m_PieceNum.y + 1); i++)
	{
		for (int j = 0; j < (m_IndexSize.m_PieceNum.x + 1); j++)
		{
			int k = i * ((int)m_IndexSize.m_PieceNum.x + 1) + j;
			m_MeshVertexLock[k].pos.y = (float)m_VertexHighRelative[k];
			m_MeshVertex[k].pos = m_MeshVertexLock[k].pos + m_pos;
		}
	}

	//面情報登録
	for (int i = 0; i < m_IndexSize.m_PieceNum.y; i++)
	{
		for (int j = 0; j < m_IndexSize.m_PieceNum.x; j++)
		{
			int k = i * ((int)m_IndexSize.m_PieceNum.x * 2) + j * 2;
			int VertexHorNum = (int)m_IndexSize.m_PieceNum.x + 1;    //横方向の頂点数
			int VertexK = i * ((int)m_IndexSize.m_PieceNum.x) + j + i;

			m_MeshFace[k].p0 = m_MeshVertex[VertexK + VertexHorNum].pos;
			m_MeshFace[k].p1 = m_MeshVertex[VertexK].pos;
			m_MeshFace[k].p2 = m_MeshVertex[VertexK + 1 + VertexHorNum].pos;
			m_MeshFace[k].Normalize = CPolygonMath::CalcTriNormalize(m_MeshFace[k]);  //法線計算

			m_MeshFace[k + 1].p0 = m_MeshVertex[VertexK + 1 + VertexHorNum].pos;
			m_MeshFace[k + 1].p1 = m_MeshVertex[VertexK].pos; 
			m_MeshFace[k + 1].p2 = m_MeshVertex[VertexK + 1].pos;
			m_MeshFace[k + 1].Normalize = CPolygonMath::CalcTriNormalize(m_MeshFace[k + 1]);  //法線計算
		}
	}

	//頂点の法線計算
	int Width = (int)m_IndexSize.m_PieceNum.x + 1;
	int Height = (int)m_IndexSize.m_PieceNum.y + 1;
	for (int i = 0; i < Height; i++)
	{
		for (int j = 0; j < Width; j++)
		{
			int k = i * ((int)m_IndexSize.m_PieceNum.x + 1) + j;
			if(i == 0) {}    //一番上の行
			
			else if (i == Height - 1) {}  //一番下の行
			
			else   //他の行
			{
				if (j == 0) {}                   //左端

				else if (j == Width - 1) {}           //右端

				else           //中間
				{
					int FaceNum = (i - 1) * (int)m_IndexSize.m_PieceNum.x * 2 + (j-1)*2;               //面の番号
					m_MeshVertex[k].Normalize                                           //法線計算
						= m_MeshFace[FaceNum].Normalize
						+ m_MeshFace[FaceNum + 1].Normalize
						+ m_MeshFace[FaceNum + 2].Normalize
						+ m_MeshFace[FaceNum + (int)m_IndexSize.m_PieceNum.x*2 + 1].Normalize
						+ m_MeshFace[FaceNum + (int)m_IndexSize.m_PieceNum.x*2 + 2].Normalize
						+ m_MeshFace[FaceNum + (int)m_IndexSize.m_PieceNum.x*2 + 3].Normalize;
					D3DXVec3Normalize(&m_MeshVertex[k].Normalize, &m_MeshVertex[k].Normalize);
				}
			}
		}
	}

	//データ頂点の法線を描画頂点に代入
	for(int i = 0; i < (int)m_IndexData.nVertexNum;i++)
	{
		m_MeshVertexLock[i].Normalize = m_MeshVertex[i].Normalize;
	}
}