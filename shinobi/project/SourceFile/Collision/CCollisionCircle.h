#pragma once
#ifndef CCOLLISION_CIRCLE_H_
#define CCOLLISION_CIRCLE_H_

#include "main.h"

class CCollisionCircle
{
public:
	CCollisionCircle();
	~CCollisionCircle();

	typedef enum
	{
		TYPE_NONE,						//ダミー
		TYPE_BODY,						//物体に当たると進まないとか,TYPE_HITとの当り判定で攻撃が受けたかどうかを判定するタイプ
		TYPE_HIT_PREVIEW,				//当り判定円未使用状態
		TYPE_HIT,						//攻撃用のタイプ
		TYPE_HIT_ENABLE,				//当り判定計算が有効になる
	}TYPE;

	HRESULT Init(float Radius);
	HRESULT Init(float Radius , int ParentID, TYPE type);
	void Update(void);
	void Draw(void);
	void Uninit(void);

	//ゲッター
	D3DXVECTOR3 GetPos(void) { return m_pos; }
	D3DXMATRIX GetMtxResultOld(void){ return m_MtxResultOld; }
	D3DXMATRIX GetMtxResult(void) { return m_MtxResult; }
	float GetRadius(void) { return m_Radius; }
	int GetParentID(void) { return m_ParentID; }
	TYPE GetType(void) { return m_type; }
	D3DXVECTOR3 GetOffset(void) { return m_posOffset; }
	bool GetUse(void) { return m_bUse; }

	//セッター
	void SetType(TYPE type) { m_type = type; }                      //タイプセット(変更)
	void SetRadius(float Radius) { m_Radius = max(0.01f, Radius); }
	void SetOffset(const D3DXVECTOR3& Pos) { m_posOffset = Pos; }
	void SetUse(bool bUse) { m_bUse = bUse; }
	void SetMtxResult(const D3DXMATRIX& Mtx) { m_MtxResult = Mtx; }

	//他の関数
	static bool HitCheckUseNormal(CCollisionCircle* pA, CCollisionCircle* pB);
	static bool HitCheckUseSSV(CCollisionCircle* pA, CCollisionCircle* pB);
	static bool HitCheckUseSSV(CCollisionCircle* pA, CCollisionCircle* pB,float *t);

private:
	//メンバ関数
	void MtxWorldSetting(void);
	void InitMaterialBaseOnType(void);
	void SetMaterialBaseOnType(void);

	//メンバ変数
	D3DXVECTOR3 m_pos;					//位置
	D3DXMATRIX m_MtxResult;				//結果マトリクス
	D3DXMATRIX m_MtxResultOld;			//結果マトリクス
	D3DXVECTOR3 m_posOffset;			//オフセット
	D3DXVECTOR3 m_scl;					//スケール
	TYPE m_type;						//タイプ
	TYPE m_typeOld;						//古いタイプ
	int m_ParentID;						//親になるモデルのモーションのパーツのID
	float m_Radius;						//半径
	LPD3DXMESH m_pMesh;					//メッシュポインタ
	D3DMATERIAL9 m_Mtrl;				//マテリアル
	bool m_bUse;						//外部用Useフラグ
};

#endif