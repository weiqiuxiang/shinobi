#pragma once
#ifndef CPATH_POINT_H_
#define CPATH_POINT_H_

#include "main.h"
#include "CPathBall.h"

class CPathPoint
{
public:
	CPathPoint();
	~CPathPoint();

	typedef enum 
	{
		DRAW_USE_MTRL_DEFALUT = 0,
		DRAW_USE_MTRL_SELECTED
	}DRAW_TYPE;

	void Init(const D3DXVECTOR3& pos, int StopFrame);
	void Update(void);
	void Draw(void);
	void Uninit(void);

	//セッター
	void SetPos(const D3DXVECTOR3 &pos) { m_pos = pos; }
	void SetStopFrame(int StopFrame) { m_StopFrame = max(0,StopFrame); }
	void SetDrawType(DRAW_TYPE type) { m_DrawType = type; }

	//ゲッター
	D3DXVECTOR3 GetPos(void) { return m_pos; }
	int GetStopFrame(void) {return m_StopFrame;}
	inline LPD3DXMESH GetMesh(void) {		//メッシュ取得
		if (m_CPathBall != nullptr) {
			return m_CPathBall->GetMesh();
		}
		return nullptr;
	}
	inline D3DXMATRIX GetWorldMtx(void) {
		if (m_CPathBall != nullptr) {
			return m_CPathBall->GetWorldMtx();
		}
		D3DXMATRIX Dummy;
		D3DXMatrixIdentity(&Dummy);
		return Dummy;
	}

	//他の関数
	void AddPos(const D3DXVECTOR3& addPos) { m_pos += addPos; }

private:
	CPathBall*		m_CPathBall;		//描画ユニット
	D3DXVECTOR3		m_pos;			//位置
	int				m_StopFrame;			//パスポイントに着くと動きはしないフレーム数(時間)
	DRAW_TYPE		m_DrawType;		//描画タイプ
};

#endif