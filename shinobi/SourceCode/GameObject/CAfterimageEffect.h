/*==============================================================================
	ヘッダ説明:
		残像エフェクトのヘッダ
===============================================================================*/
#ifndef C_AFTERIMAGE_EFFECT_H_
#define C_AFTERIMAGE_EFFECT_H_

/*==============================================================================
	インクルードヘッダ
===============================================================================*/
#include "main.h"
#include "CScene.h"
#include "CINDEX_SIZE.h"

/*==============================================================================
	マクロ定義
===============================================================================*/
#define AFTERIMAGE_EF_VERTEX_MAX (10)			//エフェクトの頂点最大数

/*==============================================================================
	クラス定義
===============================================================================*/
class CAfterimageEffect : public CScene
{
public:
	CAfterimageEffect();
	~CAfterimageEffect();

	HRESULT Init(const char *TexFilePass);
	void Update(void);
	void Uninit(void);
	void Draw(void);
	
	void MoveEffectPosition(const D3DXVECTOR3& TopPos,const D3DXVECTOR3& BottomPos);	//エフェクト位置の移動
	bool UseEffectVertex(const D3DXVECTOR3& TopPos, const D3DXVECTOR3& BottomPos);		//エフェクト頂点の使用
	void NoUseAllVertex(void);			//全ての頂点を未使用状態にする
private:
	void LockVertexBuf(void);				//頂点バッファロック
	void LockIndexBuf(void);				//インデックスバッファロック
	void MtxSetting(const SRP& SclPosRot);		//ワールドマトリクスセット
private:
	D3DXVECTOR3 m_VertexPos[AFTERIMAGE_EF_VERTEX_MAX];		//頂点座標
	float		m_TextureUValue[AFTERIMAGE_EF_VERTEX_MAX];	//頂点のテクスチャU値
	bool		m_UseVertex[AFTERIMAGE_EF_VERTEX_MAX];		//頂点の使用

	D3DXVECTOR3		m_rot;					//マトリクス合成用回転データ
	D3DXVECTOR3		m_scl;					//拡大縮小
	INDEX_DATA		m_IndexData;			//インデックスメッシュのデータ(頂点数,インデックス数,ポリゴン数)
	TEX_ID			m_TexID;				//使用テクスチャ番号

	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;			//頂点バッファ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuffer;			//インデックスバッファ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBufferBackFace;	//エフェクトの裏面インデックスバッファ
};

#endif