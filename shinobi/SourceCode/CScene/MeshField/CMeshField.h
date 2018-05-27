#pragma once
#ifndef _CMESH_FILED_H_
#define _CMESH_FILED_H_

#include "main.h"
#include "CScene.h"
#include "CINDEX_SIZE.h"

typedef struct
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 tex;
}VERTEX_MESHFIELD;

class CMeshField : public CScene
{
public:
	CMeshField();
	~CMeshField();

	//Init,Uninit,Update,Draw関数
	HRESULT Init(const D3DXVECTOR3& SclRotPos,const CINDEX_SIZE& IndexSize,const char *TexFilePass);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void DrawOnDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje);
	void DrawUseDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje, LPDIRECT3DTEXTURE9 DepthTexture);

	//ゲッター
	D3DXVECTOR2 GetPieceNum(void) { return m_IndexSize.m_PieceNum; }		//メッシュの分割数取得
	D3DXVECTOR2 GetPieceSize(void) { return m_IndexSize.m_PieceSize; }
	TEX_ID GetTexID(void) { return m_TexID; }
	int GetVertexNum(void) { return m_IndexData.nVertexNum; }				//頂点数取得
	const MESH_FACE* GetMeshFace(void) { return m_MeshFace; }
	const MESH_VERTEX* GetMeshVertex(void) { return m_MeshVertex; }
	const double* GetHighRelative(void) { return m_VertexHighRelative; }

	//他の関数
	void LoadRelativeHigh(const double *MeshHigh);

private:
	//メンバ関数
	void LockVertexBuf(void);				//頂点バッファロック
	void LockIndexBuf(void);				//インデックスバッファロック
	HRESULT CreateDeclaration(void);			//Declarationを作る

	//メンバ変数
	CINDEX_SIZE		m_IndexSize;				//メッシュフィールドサイズパラメータ
	MESH_VERTEX*	m_MeshVertexLock;			//メッシュの頂点ロック情報
	MESH_VERTEX*	m_MeshVertex;				//メッシュの頂点情報
	MESH_FACE*		m_MeshFace;					//メッシュフェース(面情報)
	double*			m_VertexHighRelative;		//頂点の相対高さ

protected:
	//メンバ関数
	HRESULT InitLockVertexBufAndIndexBuf(void);	//テクスチャバッファとインデックスバッファの初期化
	void InitMeshVertexAndFace(void);			//頂点とポリゴン面の初期化
	void InitVertexAndFacePara(void);			//頂点とポリゴン面のデータの初期化
	void MtxSetting(const SRP& SclPosRot);		//ワールドマトリクスセット
	D3DXMATRIX CalcWorldMtx(void);				//ワールドマトリクスの計算
	D3DXMATRIX CalcWorldInverseMtx(void);		//ワールドマトリクスの逆行列の計算

	//メンバ変数
	D3DXVECTOR3 m_rot;							//マトリクス合成用回転データ
	D3DXVECTOR3 m_scl;							//拡大縮小
	INDEX_DATA m_IndexData;						//インデックスメッシュのデータ(頂点数,インデックス数,ポリゴン数)
	TEX_ID m_TexID;								//使用テクスチャ番号
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;		//頂点バッファ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuffer;		//インデックスバッファ
	LPDIRECT3DVERTEXDECLARATION9 m_pVertexDeclaration;	//declaration

	VERTEX_MESHFIELD	*m_pVertexBuffer = nullptr;
	WORD				*m_pIndexBuffer = nullptr;
};

#endif