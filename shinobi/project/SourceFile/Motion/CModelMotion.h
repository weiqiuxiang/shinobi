#pragma once
#ifndef CMODEL_MOTION_H_
#define CMODEL_MOTION_H_

#include "main.h"
#include "CMotion.h"
#include "CPart.h"
#include <stdio.h>

#define MOTION_MAX (40)

class CModelMotion
{
public:
	CModelMotion();
	~CModelMotion();

	//Init,Uninit,Update,Draw関数
	void LoadMotionDataToInit(const char* MotionPass);        //モーションデータを読み込んで初期化する
	void Update(void);
	void Draw(void);
	void DrawAllOnDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje);
	void DrawUseDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje, LPDIRECT3DTEXTURE9 DepthTexture);
	void DrawSpecular(void);
	void DrawMotionBlur(LPDIRECT3DTEXTURE9 OriginalTexture);
	void Uninit(void);

	//ゲッター
	D3DXMATRIX GetPartMtx (int PartID) {
		if (-1 == PartID) return m_WorldMtx;
		return m_Part[PartID].m_MtxNow; 
	}
	LPD3DXMESH GetMesh(int PartID) {
		return m_Part[PartID].GetMesh();
	}
	int GetTotalFrame(int MotionID) { return m_MotionTotalFrame[MotionID]; }				//モーションの総フレーム数取得
	int GetFrameOfKeyFrame(int MotionID, int KeyFrameID) { return m_Motion[MotionID].m_KeyFrame[KeyFrameID].m_TotalFrame; }
	int GetTotalKeyFrame(int MotionID) { return m_Motion[MotionID].GetTotalKeyFrame(); }	//このモーションのキーフレーム総数を取得
	int GetKeyFrameNow(void){ return m_KeyFrameNow; }
	int GetMotionNow(void) { return m_MotionNow; }
	D3DXVECTOR3 GetPartPosInWorld(int PartID) { return D3DXVECTOR3(m_Part[PartID].m_MtxNow._41, m_Part[PartID].m_MtxNow._42, m_Part[PartID].m_MtxNow._43); }
	void GetHerminateDir(D3DXVECTOR2* pOutStartDir, D3DXVECTOR2 *pOutEndDir) {
		*pOutStartDir = m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_HermitianStartDir;
		*pOutEndDir = m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_HermitianEndDir;
	}

	//セッター
	void SetPosition(const D3DXVECTOR3& pos) { m_pos = pos; }
	void SetWorldRotMtx(const D3DXMATRIX& worldMtx) { m_WorldRotMtx = worldMtx; }
	void SetMotionNow(int MotionNumber);
	void SetOtherParentMtx(const D3DXMATRIX& worldMtx) {
		m_OtherParentMtx = worldMtx;
	}
	void SetDrawType(CModel::DRAW_TYPE type);
	void SetMaterial(const D3DMATERIAL9& Mtrl);

	//他の関数
	void AddMtxWorld() {}
	void ResetOtherParentMtx(void) { D3DXMatrixIdentity(&m_OtherParentMtx); }
private:
	//メンバ関数
	void LoadPartData(FILE *fp);
	void LoadMotionData(FILE *fp);
	void CalcPartWorldMtx(void);			//モデル全体のワールドマトリクス計算
	void CalcWorldMtx(void);				//ワールドマトリクスを計算
	void CalcPartWorldMtxRecursive(CPart*);	//モデル全体のワールドマトリクス再帰
	void KeyFrameChange(void);				//キーフレーム変わる
	int CalcNextKeyFrame(void);				//次のキーフレーム
	void LinkParentAndChildren(void);		//親と子供の関係結びつき

	//メンバ変数
	CPart m_Part[MAX_PART];
	CMotion m_Motion[MOTION_MAX];
	int   m_MotionTotalFrame[MOTION_MAX];		//モーションごとの総フレーム数
	int   m_PartNum;					//パーツ数
	int   m_TotalMotion;				//モーション数
	int   m_MotionOld;					//古いモーション
	D3DXVECTOR3 m_pos;					//ワールド位置(外部設定)
	D3DXMATRIX m_WorldMtx;				//ワールドのマトリクス
	D3DXMATRIX m_WorldRotMtx;			//現在ワールド回転マトリクス(外部設定)
	D3DXMATRIX m_OtherParentMtx;		//別のオブジェクトは親としての場合,その親のマトリクス

	int   m_MotionNow;					//今実行中のモーション番号
	int   m_KeyFrameNow;				//実行中のキーフレーム番号
	int   m_FrameNow;					//実行中のフレーム番号
};

#endif