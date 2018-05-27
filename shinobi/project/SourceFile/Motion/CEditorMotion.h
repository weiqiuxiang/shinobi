//説明 : モーションエディター用モーション
#pragma once
#ifndef CEDITOR_MOTION_H_
#define CEDITOR_MOTION_H_

#include "main.h"
#include "CMotion.h"
#include "CPart.h"
#include <stdio.h>
#include <string>

#define EDITOR_MOTION_MAX (40)

class CEditorMotion
{
public:
	CEditorMotion();
	~CEditorMotion();

	//Init,Uninit,Update,Draw関数
	void LoadMotionDataToInit(const char* MotionPass);		//モーションデータを読み込んで初期化する
	void UpdateOffset(void);
	void UpdateKeyFrame(void);
	void UpdateKeyFramePreview(float Percent);
	void UpdateMotion(void);
	void Draw(void);
	void Uninit(void);

	//ゲッター
	D3DXMATRIX GetPartMtx (int PartID) {
		if (-1 == PartID) return m_WorldMtx;
		return m_Part[PartID].m_MtxNow; 
	}
	int GetTotalFrame(int MotionID) { return m_MotionTotalFrame[MotionID]; }				//モーションの総フレーム数取得
	int GetFrameOfKeyFrame(int MotionID, int KeyFrameID) { return m_Motion[MotionID].m_KeyFrame[KeyFrameID].m_TotalFrame; }
	int GetTotalKeyFrame(int MotionID) { return m_Motion[MotionID].GetTotalKeyFrame(); }	//このモーションのキーフレーム総数を取得
	int GetKeyFrameNow(void){ return m_KeyFrameNow; }
	int GetMotionNow(void) { return m_MotionNow; }
	D3DXVECTOR3 GetPartPosInWorld(int PartID) { return D3DXVECTOR3(m_Part[PartID].m_MtxNow._41, m_Part[PartID].m_MtxNow._42, m_Part[PartID].m_MtxNow._43); }
	int GetPartTotalNum(void) { return m_PartNum; }
	CPart* GetPartPointer(int PartID) {
		if (nullptr == m_Part) return nullptr;
		return &m_Part[PartID];
	}
	int GetTotalMotionNum(void) { return m_TotalMotion; }
	CMotion* GetMotion(int MotionNum) { return &m_Motion[MotionNum]; }
	inline void GetKeyFrameHerminateDirNow(D3DXVECTOR2* pOutStartDir, D3DXVECTOR2 *pOutEndDir) {
		*pOutStartDir = m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_HermitianStartDir;
		*pOutEndDir = m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_HermitianEndDir;
	}
	inline float GetKeyFrameChangeSpeed(void) {	//キーフレームの変更速度
		return (1.0f / m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_TotalFrame);
	}
	const char* GetPartFilePass(int PartID) { return m_PartFilePass[PartID].c_str(); }

	//セッター
	void SetPosition(const D3DXVECTOR3& pos) { m_pos = pos; }
	void SetWorldRotMtx(const D3DXMATRIX& worldMtx) { m_WorldRotMtx = worldMtx; }
	void SetMotionNow(int MotionNumber);
	void SetKeyFrameNow(int KeyFrame);
	void SetFrameOfKeyFrame(int MotionID, int KeyFrameID,int Value) { m_Motion[MotionID].m_KeyFrame[KeyFrameID].m_TotalFrame = Value; }
	void SetOtherParentMtx(const D3DXMATRIX& worldMtx) {
		m_OtherParentMtx = worldMtx;
	}
	inline void SetKeyFrameHerminateDir(const D3DXVECTOR2& StartDir, const D3DXVECTOR2 &EndDir)
	{
		m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_HermitianStartDir = StartDir;
		m_Motion[m_MotionNow].m_KeyFrame[m_KeyFrameNow].m_HermitianEndDir = EndDir;
	}

	//他の関数
	void AddKeyFrame(void);			//現在のモーションにキーフレームを増やす
	void AddKeyFrame(int MotionNum);//指定するモーションにキーフレームを増やす
	void AddMotion(void);			//モーションを増やす(キーフレームのデフォルトは2つ)
	void InsertKeyFrame(void);		//キーフレームの挿入
	void InsertMotion(void);		//モーションの挿入
	void DeleteMotionNow(void);		//今のモーションを削除し、配列の後ろの部分を詰める
	void DeleteKeyFrameNow(void);	//今のモーションの今のキーフレームを削除し、配列の後ろの部分を詰める
	void ResetOtherParentMtx(void) { D3DXMatrixIdentity(&m_OtherParentMtx); }
private:
	//メンバ関数
	void LoadPartData(FILE *fp);
	void LoadMotionData(FILE *fp);
	void KeyFrameChange(void);				//キーフレーム変わる
	int CalcNextKeyFrame(void);				//次のキーフレーム
	void LinkParentAndChildren(void);		//親と子供の関係結びつき

	//表示系関数
	void ShowOffset(void);					//オフセットの再生
	void ShowKeyFrame(void);				//指定キーフレームの再生
	void ShowKeyFramePreview(float Percent);//指定キーフレームのプレビュー
	void ShowMotion(void);					//指定モーションの再生
	void CalcOffset(CPart*);				//全てパーツのオフセットの計算
	void CalcKeyFrame(CPart*);				//全てパーツのキーフレームの計算
	void CalcKeyFramePreview(CPart*,float Percent);
	void CalcPartWorldMtxRecursive(CPart*);	//モデル全体のワールドマトリクス再帰
private:
	CPart	m_Part[MAX_PART];
	std::string m_PartFilePass[MAX_PART];			//パーツのファイルパス
	CMotion	m_Motion[EDITOR_MOTION_MAX];
	int		m_MotionTotalFrame[EDITOR_MOTION_MAX];	//モーションごとの総フレーム数
	int		m_PartNum;					//パーツ数
	int		m_TotalMotion;				//モーション数
	int		m_MotionOld;				//古いモーション
	D3DXVECTOR3	m_pos;					//ワールド位置(外部設定)
	D3DXMATRIX	m_WorldMtx;				//ワールドのマトリクス
	D3DXMATRIX	m_WorldRotMtx;			//現在ワールド回転マトリクス(外部設定)
	D3DXMATRIX	m_OtherParentMtx;		//別のオブジェクトは親としての場合,その親のマトリクス

	int		m_MotionNow;				//今実行中のモーション番号
	int		m_KeyFrameNow;				//実行中のキーフレーム番号
	int		m_FrameNow;					//実行中のフレーム番号
};

#endif