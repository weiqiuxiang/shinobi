/*==============================================================================
	ファイル名 : CAfterimageEffect.cpp
===============================================================================*/
#include "CAfterimageEffect.h"
#include "CRenderer.h"
#include "TexManager.h"
#include "CCameraManager.h"

/*==============================================================================
	コンストラクタ
===============================================================================*/
CAfterimageEffect::CAfterimageEffect()
{
	m_pVtxBuffer = nullptr;
	m_pIdxBuffer = nullptr;
	m_pIdxBufferBackFace = nullptr;
	m_IndexData = { 0,0,0 };
	m_TexID = -1;

	for (int i = 0; i < AFTERIMAGE_EF_VERTEX_MAX; i++)
	{
		m_VertexPos[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_TextureUValue[i] = 0.0f;
		m_UseVertex[i] = false;
	}
}

/*==============================================================================
	デストラクタ
===============================================================================*/
CAfterimageEffect::~CAfterimageEffect()
{
	
}

/*==============================================================================
	初期化処理
	引数説明:
		Pos			:	マトリクス合成用の位置情報
		IndexSize	:	インデックス情報(横縦マス数と横縦間隔)
		TexFilePass	:	テクスチャのファイルパス
===============================================================================*/
HRESULT CAfterimageEffect::Init( const char *TexFilePass) 
{
	//マトリクス合成情報初期化
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	//テクスチャ読み込み
	m_TexID = CTexManager::LoadTex(TexFilePass);

	//インデックスデータ初期化
	m_IndexData.nVertexNum = AFTERIMAGE_EF_VERTEX_MAX;
	m_IndexData.nIndexNum = AFTERIMAGE_EF_VERTEX_MAX;
	m_IndexData.nPolygonNum = m_IndexData.nIndexNum - 2;

	//デバイス取得
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	//頂点バッファとインデックスバッファ確保
	if (m_pVtxBuffer == nullptr) {
		if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D)*m_IndexData.nVertexNum, D3DUSAGE_WRITEONLY, FVF_VERTEX3D, D3DPOOL_MANAGED, &m_pVtxBuffer, nullptr)))
		{
			MessageBox(nullptr, "メッシュフィールドの頂点バッファ確保が失敗しました", "エラー", MB_OK | MB_ICONHAND);
			return E_FAIL;
		}
	}
	if (m_pIdxBuffer == nullptr)				//表面
	{
		HRESULT hr;
		hr = pDevice->CreateIndexBuffer(
			sizeof(WORD)*m_IndexData.nIndexNum,
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&m_pIdxBuffer,
			nullptr);
		if (FAILED(hr))
		{
			MessageBox(nullptr, "メッシュフィールドのインデックスバッファ確保が失敗しました", "エラー", MB_OK | MB_ICONHAND);
			return E_FAIL;
		}
	}
	if (m_pIdxBufferBackFace == nullptr)		//裏面
	{
		HRESULT hr;
		hr = pDevice->CreateIndexBuffer(
			sizeof(WORD)*m_IndexData.nIndexNum,
			D3DUSAGE_WRITEONLY,
			D3DFMT_INDEX16,
			D3DPOOL_MANAGED,
			&m_pIdxBufferBackFace,
			nullptr);
		if (FAILED(hr))
		{
			MessageBox(nullptr, "メッシュフィールドのインデックスバッファ確保が失敗しました", "エラー", MB_OK | MB_ICONHAND);
			return E_FAIL;
		}
	}

	//頂点の座標とアルファ値初期化
	for (int i = 0; i < AFTERIMAGE_EF_VERTEX_MAX; i++) 
	{
		m_VertexPos[i] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_TextureUValue[i] = 0.0f;
		m_UseVertex[i] = false;
	}

	m_IndexData.nVertexNum = 0;			//使用してる頂点数を0に

	return S_OK;
}

/*==============================================================================
	終了処理
===============================================================================*/
void CAfterimageEffect::Uninit(void)
{
	if (m_pVtxBuffer != nullptr) 
	{
		m_pVtxBuffer->Release(); 
		m_pVtxBuffer = nullptr; 
	}
	if (m_pIdxBuffer != nullptr) 
	{ 
		m_pIdxBuffer->Release(); 
		m_pIdxBuffer = nullptr; 
	}
	if (m_pIdxBufferBackFace != nullptr)
	{
		m_pIdxBufferBackFace->Release();
		m_pIdxBufferBackFace = nullptr;
	}
}

/*==============================================================================
	更新処理
===============================================================================*/
void CAfterimageEffect::Update(void)
{
	LockVertexBuf();		//頂点バッファロック
	LockIndexBuf();			//インデックスロック
}

/*==============================================================================
	描画処理
===============================================================================*/
void CAfterimageEffect::Draw(void)
{
	//頂点は4つ以下なら実行しない
	if (m_IndexData.nVertexNum < 4)
	{
		return;
	}

	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();
	SRP PosRotScl = { m_pos,m_rot,m_scl };

	//ワールドマトリクス設定
	MtxSetting(PosRotScl);

	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);			//ライトオフ

	//表面描画
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_3D));	//VRAMからGPUにデータをストリーム
	pDevice->SetIndices(m_pIdxBuffer);									//インデックスバッファのセット
	pDevice->SetFVF(FVF_VERTEX3D);										//描画フォーマットセット
	pDevice->SetTexture(0, CTexManager::UseTex(m_TexID));				//テクスチャの設定
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0,			//描画
		m_IndexData.nVertexNum, 0, m_IndexData.nPolygonNum);

	//裏面描画
	pDevice->SetStreamSource(0, m_pVtxBuffer, 0, sizeof(VERTEX_3D));	//VRAMからGPUにデータをストリーム
	pDevice->SetIndices(m_pIdxBufferBackFace);							//インデックスバッファのセット
	pDevice->SetFVF(FVF_VERTEX3D);										//描画フォーマットセット
	pDevice->SetTexture(0, CTexManager::UseTex(m_TexID));				//テクスチャの設定
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0,			//描画
		m_IndexData.nVertexNum, 0, m_IndexData.nPolygonNum);

	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);			//ライトオン
}

/*==============================================================================
	エフェクト位置の移動
		TopPos : エフェクト発生先上位置
		BottomPos : エフェクト発生先下位置
===============================================================================*/
void CAfterimageEffect::MoveEffectPosition(const D3DXVECTOR3& TopPos, const D3DXVECTOR3& BottomPos)
{
	int HoriNum = (int)(AFTERIMAGE_EF_VERTEX_MAX * 0.5f);	//横マス数

	//頂点座標の入れ替えを行う
	for (int i = 0; i < HoriNum - 1; i++)
	{
		int K = i * 2;
		m_VertexPos[K] = m_VertexPos[K + 2];
		m_VertexPos[K + 1] = m_VertexPos[(K + 1) + 2];
	}

	//最後の2頂点に高さのデータを代入
	m_VertexPos[AFTERIMAGE_EF_VERTEX_MAX - 2] = TopPos;
	m_VertexPos[AFTERIMAGE_EF_VERTEX_MAX - 1] = BottomPos;
}
/*==============================================================================
エフェクトの頂点の使用
引数説明:
	TopPos : エフェクト発生先上位置
	BottomPos : エフェクト発生先下位置
戻り値説明:
	false : 使用できる頂点存在しない
	true : 使用できる頂点存在する
===============================================================================*/
bool CAfterimageEffect::UseEffectVertex(const D3DXVECTOR3& TopPos, const D3DXVECTOR3& BottomPos)
{
	if (m_UseVertex[AFTERIMAGE_EF_VERTEX_MAX - 1] == true)
	{
		return false;
	}

	//使用していない頂点を探す
	int HoriNum = (int)(AFTERIMAGE_EF_VERTEX_MAX * 0.5f);	//横マス数
	int UseVertexNum = 0;									//使用された頂点数
	for (int i = 0; i < HoriNum; i++)
	{
		int K = i * 2;
		if (m_UseVertex[K] == false) 
		{
			m_UseVertex[K] = true;
			m_VertexPos[K] = TopPos;
			m_UseVertex[K+1] = true;
			m_VertexPos[K + 1] = BottomPos;
			UseVertexNum = K + 2;
			break;
		}
	}

	m_IndexData.nVertexNum = UseVertexNum;
	m_IndexData.nIndexNum = UseVertexNum;
	m_IndexData.nPolygonNum = m_IndexData.nIndexNum - 2;

	//アルファ値の計算(頂点4つ以上使用されるなら)
	if (m_IndexData.nVertexNum >= 4)
	{
		HoriNum = (int)(m_IndexData.nVertexNum * 0.5f);		//横マス数
		float AlphaChangeValue = 1.0f / (HoriNum - 1);	//アルファ変化値
		for (int i = 0; i < HoriNum; i++)
		{
			int K = i * 2;
			m_TextureUValue[K] = i * AlphaChangeValue;
			m_TextureUValue[K + 1] = i * AlphaChangeValue;
		}
	}

	return true;
}

/*==============================================================================
	全ての頂点を未使用状態にする
===============================================================================*/
void CAfterimageEffect::NoUseAllVertex(void)
{
	//フラグリセット
	for (int i = 0; i < AFTERIMAGE_EF_VERTEX_MAX; i++)
	{
		m_UseVertex[i] = false;
	}

	m_IndexData.nVertexNum = 0;			//使用してる頂点数を0に
}

/*==============================================================================
	頂点バッファロック
===============================================================================*/
void CAfterimageEffect::LockVertexBuf(void)
{
	//頂点は4つ以下なら実行しない
	if (m_IndexData.nVertexNum < 4) 
	{
		return;
	}

	VERTEX_3D *pVtx = nullptr;

	//頂点ロック(法線と色情報含まれらない)
	m_pVtxBuffer->Lock(0, 0, (void**)&pVtx, 0);

	int HoriNum = (int)(m_IndexData.nVertexNum * 0.5f);	//水平方向マス数

	for (int X = 0; X < HoriNum; X++)
	{
		int K = X * 2;
		pVtx[K].pos = m_VertexPos[K];
		pVtx[K].tex = D3DXVECTOR2(m_TextureUValue[K], 0.0f);
		pVtx[K].color = D3DCOLOR_RGBA(255,255,255, 200);
		pVtx[K + 1].pos = m_VertexPos[K + 1];
		pVtx[K + 1].tex = D3DXVECTOR2(m_TextureUValue[K + 1], 1.0f);
		pVtx[K + 1].color = D3DCOLOR_RGBA(255, 255, 255, 200);
	}

	m_pVtxBuffer->Unlock();
}

/*==============================================================================
	インデックスバッファロック
===============================================================================*/
void CAfterimageEffect::LockIndexBuf(void)
{
	//頂点は4つ以下なら実行しない
	if (m_IndexData.nVertexNum < 4)
	{
		return;
	}

	WORD *pIdx = nullptr;

	//表面インデックスバッファ登録
	m_pIdxBuffer->Lock(0, 0, (void**)&pIdx, 0);

	for (int X = 0; X < m_IndexData.nVertexNum; X++)
	{
		pIdx[X] = (WORD)X;
	}

	m_pIdxBuffer->Unlock();

	//裏面インデックスバッファ登録
	m_pIdxBufferBackFace->Lock(0, 0, (void**)&pIdx, 0);

	int HoriNum = (int)(m_IndexData.nVertexNum * 0.5f);	//横マス数
	for (int X = 0; X < HoriNum; X++)
	{
		int K = X * 2;
		pIdx[K] = (WORD)( (m_IndexData.nVertexNum - 2) - K );
		pIdx[K + 1] = (WORD)( (m_IndexData.nVertexNum - 1) - K );

	}

	m_pIdxBufferBackFace->Unlock();
}

/*==============================================================================
	ワールドマトリクスのセット
===============================================================================*/
void CAfterimageEffect::MtxSetting(const SRP& SclPosRot)
{
	//TODO: エフェクトはビルボードの描画なので、ビルボールのようにしないと
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();             //デバイス取得
	D3DXMATRIX mtxScale, mtxRot, mtxMove, mtxResult, WorldMatrix;

	//拡大縮小、回転、移動などの行列の合成
	D3DXMatrixScaling(&mtxScale, SclPosRot.scl.x, SclPosRot.scl.y, SclPosRot.scl.z);
	D3DXMatrixRotationYawPitchRoll(&mtxRot, SclPosRot.rot.y, SclPosRot.rot.x, SclPosRot.rot.z);
	D3DXMatrixTranslation(&mtxMove, SclPosRot.pos.x, SclPosRot.pos.y, SclPosRot.pos.z);

	//行列の合成
	mtxResult = mtxScale*mtxRot*mtxMove;
	D3DXMatrixIdentity(&WorldMatrix);
	D3DXMatrixMultiply(&WorldMatrix, &WorldMatrix, &mtxResult);

	//ワールドに反映
	pDevice->SetTransform(D3DTS_WORLD, &WorldMatrix);
}