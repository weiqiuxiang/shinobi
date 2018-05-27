#pragma once
#ifndef _CSKYDOME_H_
#define _CSKYDOME_H_

#include "main.h"
#include "CMeshField.h"

class CSkyDome
{
public:
	CSkyDome();
	~CSkyDome();

	//Init,Uninit,Update,Draw関数
	HRESULT Init(float fRadius, int nHor_PieceNum,int m_nVer_PieceNum, const char *TexFilePass);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	//セッター
	void SetStartEndAngleVertical(float fStart_Angel, float fEnd_Angel) {
		m_fStart_Angel_Vertical = fStart_Angel;
		m_fEnd_Angel_Vertical = fEnd_Angel;
	}
	void SetRot(const D3DXVECTOR3& rot) { m_rot = rot; }
private:
	//メンバ関数
	void LockSkyDomeVertexBuf(void);
	void LockSkyDomeIndexBuf(void);
	HRESULT InitLockVertexBufAndIndexBuf(void);

	//メンバ変数
	D3DXVECTOR3 m_pos;
	D3DXVECTOR3 m_rot;
	D3DXVECTOR3 m_scl;
	int	m_TexID;
	float m_fRadius;							//スカイドームの半径
	int   m_nHor_PieceNum;						//水平方向の分割数
	int   m_nVer_PieceNum;						//垂直方向の分割数
	float m_fStart_Angel_Vertical;				//垂直方向開始角度
	float m_fEnd_Angel_Vertical;				//垂直方向終了角度
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;		//頂点バッファ
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuffer;		//インデックスバッファ
	INDEX_DATA m_IndexData;
};

#endif