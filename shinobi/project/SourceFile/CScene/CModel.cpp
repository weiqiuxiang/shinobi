#include "CModel.h"
#include "ModelManager.h"
#include "TexManager.h"
#include "CRenderer.h"

const D3DVERTEXELEMENT9 g_ConstElements[] = {
	{ 0,0,D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,0 },
	D3DDECL_END()
};

CModel::CModel()
{
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	D3DXMatrixIdentity(&m_PoseMtx);				//姿勢マトリクス初期化
	D3DXMatrixIdentity(&m_Result);				//結果マトリクス初期化
	m_ModelID = -1;
	m_DrawType = DRAW_USE_MTRL_DEFALUT;			//描画方式指定
	m_IsHitMesh = false;
	m_IsAlwaysCalcAABB = false;
	m_AABBMinXMaxX = D3DXVECTOR2(0.0f, 0.0f);
	m_AABBMinYMaxY = D3DXVECTOR2(0.0f, 0.0f);
	m_AABBMinZMaxZ = D3DXVECTOR2(0.0f, 0.0f);
	m_UseID = -1;
}

CModel::~CModel()
{
	
}

HRESULT CModel::Init(const SRP &PosRotScl, const char* filePass)
{
	m_ModelID = CModelManager::LoadModel(filePass); //モデルデータの読み込み
	m_UseID = -1;

	//データ初期化
	m_pos = PosRotScl.pos;
	m_rot = PosRotScl.rot;
	m_scl = PosRotScl.scl;

	D3DXMatrixIdentity(&m_PoseMtx);

	m_DirVecXZ = D3DXVECTOR3(0.0f, 0.0f, 1.0f);

	m_IsHitMesh = false;

	//AABB部分の初期化
	m_IsAlwaysCalcAABB = false;
	m_AABBMinXMaxX = D3DXVECTOR2(0.0f, 0.0f);
	m_AABBMinYMaxY = D3DXVECTOR2(0.0f, 0.0f);
	m_AABBMinZMaxZ = D3DXVECTOR2(0.0f, 0.0f);
	CalcAABB();			//AABBの計算を行う

	float X = fabsf(m_AABBMinXMaxX.x - m_AABBMinXMaxX.y);
	float Y = fabsf(m_AABBMinYMaxY.x - m_AABBMinYMaxY.y);
	float Z = fabsf(m_AABBMinZMaxZ.x - m_AABBMinZMaxZ.y);
	m_Cube.Init(X,Y,Z);

	return S_OK;
}

void CModel::Update(void)
{
	//回転角度でXZ平面の方向ベクトルを計算する
	D3DXVECTOR3 DirXZ = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	D3DXVECTOR3 RotY = D3DXVECTOR3(0.0f, m_rot.y,0.0f);
	D3DXMATRIX RotMtx;
	D3DXMatrixRotationYawPitchRoll(&RotMtx, RotY.y, RotY.x, RotY.z);
	D3DXVec3TransformCoord(&m_DirVecXZ,&DirXZ,&RotMtx);

	if (m_IsAlwaysCalcAABB) {
		//CalcAABB();
	}

	m_Cube.SetPos(m_pos);
}

void CModel::Draw(void)
{
	if (m_DrawType == DRAW_USE_MTRL_DEFALUT) {
		DrawDefault();
	}
	else if (m_DrawType == DRAW_USE_MTRL_CUSTOMIZE) {
		DrawMtrl();
	}
}

void CModel::DrawAABBTest(void)
{
	m_Cube.Draw();
}

void CModel::DrawOnDepthMap(const D3DXMATRIX& world, const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje)
{
	CModelManager::DrawModelOnDepthMap(m_ModelID, world, LightView, LightProje);
}

void CModel::DrawUseDepthMap(const D3DXMATRIX& world, const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje, LPDIRECT3DTEXTURE9 DepthTexture)
{
	CModelManager::DrawUseDepthMap(m_ModelID, world, LightView, LightProje, DepthTexture);
}

void CModel::DrawSpecular(const D3DXMATRIX& world)
{
	CModelManager::DrawSpecular(m_ModelID, world);
}

void CModel::DrawMotionBlur(const D3DXMATRIX& worldMtx, const D3DXMATRIX& worldMtxOld, LPDIRECT3DTEXTURE9 OriginalTexture)
{
	CModelManager::DrawMotionBlur(m_ModelID, worldMtx, worldMtxOld, OriginalTexture);
}

void CModel::DrawDefault(void)
{
	CModelManager::DrawModel(m_ModelID);				//モデル描画
}

void CModel::DrawMtrl(void)
{
	CModelManager::DrawModelUseMtrl(m_ModelID, m_Mtrl);	//モデル描画
}

//マテリアルをを使用して描画する
void CModel::DrawUseMtrl(const D3DMATERIAL9& Mtrl)
{
	CModelManager::DrawModelUseMtrl(m_ModelID, Mtrl);	//モデル描画
}

void CModel::Uninit(void)
{

}

D3DXMATRIX CModel::GetResultMtx(void) const
{
	D3DXMATRIX mtxScale, mtxRot, mtxMove, mtxResult;

	//ワールド行列の設定
	D3DXMatrixScaling(&mtxScale, m_scl.x, m_scl.y, m_scl.z);
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixTranslation(&mtxMove, m_pos.x, m_pos.y, m_pos.z);
	mtxResult = mtxScale*mtxRot*mtxMove;

	return mtxResult;
}

D3DXMATRIX CModel::GetWorldPoseMtx(void) const
{
	D3DXMATRIX mtxScale, mtxRot, mtxMove, mtxResult;

	//ワールド行列の設定
	D3DXMatrixScaling(&mtxScale, m_scl.x, m_scl.y, m_scl.z);
	D3DXMatrixTranslation(&mtxMove, m_pos.x, m_pos.y, m_pos.z);
	mtxResult = mtxScale*m_PoseMtx*mtxMove;

	return mtxResult;
}

/*==============================================================================
	関数説明 : AABBの計算をする
===============================================================================*/
void CModel::CalcAABB(void)
{
	//AABB用メッシュを作る
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();
	LPD3DXMESH AABBMesh = nullptr;
	LPD3DXMESH pMesh = CModelManager::GetMesh(m_ModelID);
	if (pMesh != nullptr) {
		HRESULT hr = pMesh->CloneMesh(D3DXMESH_MANAGED, g_ConstElements, pDevice, &AABBMesh);
		if (FAILED(hr)) {
			MessageBox(nullptr, "メッシュのクローンは失敗した", "[CModel.cpp][Init]", MB_ICONHAND);
			return;
		}
	}

	//頂点構造体の情報取得
	const DWORD NumVertices = AABBMesh->GetNumVertices();	//頂点の数を取得
	DWORD FVF = AABBMesh->GetFVF();						//頂点フォーマットを取得
	const DWORD VertexSize = D3DXGetFVFVertexSize(FVF);		//頂点のサイズを取得

	//頂点情報取得
	BYTE* p_vertex = nullptr;
	AABBMesh->LockVertexBuffer(D3DLOCK_READONLY, (LPVOID*)&p_vertex);

	//一つ目の頂点座標
	D3DXVECTOR3 VertexPos = *(D3DXVECTOR3*)(p_vertex);

	//データリセット
	m_AABBMinXMaxX = D3DXVECTOR2(0.0f, 0.0f);
	m_AABBMinYMaxY = D3DXVECTOR2(0.0f, 0.0f);
	m_AABBMinZMaxZ = D3DXVECTOR2(0.0f, 0.0f);

	//1頂点目を代入
	if (VertexPos.x < 0) {
		m_AABBMinXMaxX.x = VertexPos.x;
	}
	else {
		m_AABBMinXMaxX.y = VertexPos.x;
	}
	if (VertexPos.y < 0) {
		m_AABBMinYMaxY.x = VertexPos.y;
	}
	else {
		m_AABBMinYMaxY.y = VertexPos.y;
	}
	if (VertexPos.z < 0) {
		m_AABBMinZMaxZ.x = VertexPos.z;
	}
	else {
		m_AABBMinZMaxZ.y = VertexPos.z;
	}

	//AABBの計算
	for (int VertexNum = 1; VertexNum < (int)NumVertices; VertexNum++) {
		//頂点位置取得
		VertexPos = *(D3DXVECTOR3*)(p_vertex + VertexNum * VertexSize);

		//X軸比較
		if (VertexPos.x < m_AABBMinXMaxX.x) {
			m_AABBMinXMaxX.x = VertexPos.x;
		}
		if (VertexPos.x > m_AABBMinXMaxX.y) {
			m_AABBMinXMaxX.y = VertexPos.x;
		}
		//Y軸比較
		if (VertexPos.y < m_AABBMinYMaxY.x) {
			m_AABBMinYMaxY.x = VertexPos.y;
		}
		if (VertexPos.y > m_AABBMinYMaxY.y) {
			m_AABBMinYMaxY.y = VertexPos.y;
		}
		//Z軸比較
		if (VertexPos.z < m_AABBMinZMaxZ.x) {
			m_AABBMinZMaxZ.x = VertexPos.z;
		}
		if (VertexPos.z > m_AABBMinZMaxZ.y) {
			m_AABBMinZMaxZ.y = VertexPos.z;
		}
	}

	if (AABBMesh != nullptr) {
		AABBMesh->Release();
		AABBMesh = nullptr;
	}
}


/*==============================================================================
関数説明 : AABBの計算結果を取得
引数説明 :
OutMinXMaxX : AABBのx方向の値、OutMinXMaxX.xは最小値、OutMinXMaxX.yは最大値
OutMinYMaxY : AABBのy方向の値、OutMinYMaxY.xは最小値、OutMinYMaxY.yは最大値
OutMinZMaxZ : AABBのz方向の値、OutMinZMaxZ.xは最小値、OutMinZMaxZ.yは最大値
===============================================================================*/
void CModel::GetAABBValue(D3DXVECTOR2 *OutMinXMaxX, D3DXVECTOR2 *OutMinYMaxY, D3DXVECTOR2 *OutMinZMaxZ)
{
	*OutMinXMaxX = m_AABBMinXMaxX;
	*OutMinYMaxY = m_AABBMinYMaxY;
	*OutMinZMaxZ = m_AABBMinZMaxZ;
}

void CModel::SetMtxSetting(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();             //デバイス取得
	D3DXMATRIX mtxScale/*拡大行列*/, mtxRot/*回転行列*/, mtxMove/*移動行列*/, mtxResult/*結果行列*/;

	//ワールド行列の設定
	D3DXMatrixScaling(&mtxScale, m_scl.x, m_scl.y, m_scl.z);
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y/*y軸*/, m_rot.x/*x軸*/, m_rot.z/*z軸*/); //回転行列を設定
	D3DXMatrixTranslation(&mtxMove, m_pos.x, m_pos.y, m_pos.z);
	mtxResult = mtxScale*mtxRot*mtxMove;						//結果の変換行列の計算
	D3DXMatrixIdentity(&m_Result);								//ワールド行列を単位行列で初期化
	D3DXMatrixMultiply(&m_Result, &m_Result, &mtxResult);		//回転行列とワールド行列の掛け算
	pDevice->SetTransform(D3DTS_WORLD, &m_Result);				//ワールド変換行列
}

void CModel::SetMtxSettingPose(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();             //デバイス取得
	D3DXMATRIX mtxScale/*拡大行列*/, mtxMove/*移動行列*/, mtxResult/*結果行列*/;

	//ワールド行列の設定
	D3DXMatrixScaling(&mtxScale, m_scl.x, m_scl.y, m_scl.z);
	D3DXMatrixTranslation(&mtxMove, m_pos.x, m_pos.y, m_pos.z);
	mtxResult = mtxScale*m_PoseMtx*mtxMove;                     //結果の変換行列の計算
	D3DXMatrixIdentity(&m_Result);                    //ワールド行列を単位行列で初期化
	D3DXMatrixMultiply(&m_Result, &m_Result, &mtxResult);  //回転行列とワールド行列の掛け算
	pDevice->SetTransform(D3DTS_WORLD, &m_Result);              //ワールド変換行列
}

LPD3DXMESH CModel::GetMesh(void) const
{
	return CModelManager::GetMesh(m_ModelID);
}

void CModel::SetDrawType(DRAW_TYPE type)
{
	bool OveerRange = (int)type < 0 && (int)type >= DRAW_USE_MTRL_MAX;
	if (OveerRange) {
		MessageBox(nullptr,"関数「void CModel::SetDrawType(DRAW_TYPE type)」の引数の値指定が正しくない","エラー",MB_ICONHAND);
		return;
	}
	m_DrawType = type;
}

void CModel::SetAlpha(float Alpha)
{	
	CModelManager::SetAlpha(m_ModelID,Alpha);
}