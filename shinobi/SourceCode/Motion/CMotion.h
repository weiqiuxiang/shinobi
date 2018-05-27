#pragma once
#ifndef CMOTION_H_
#define CMOTION_H_

#include "main.h"

#define MAX_PART (20)
#define MAX_KEY_FRAME (20)
#define MOTION_NAME_MAX (64)

class CKeyFrame
{
public:
	CKeyFrame();
	~CKeyFrame();

	//メンバ変数
	SRP			m_SRPData[MAX_PART];	//ワールドマトリクスポインタ(パーツ数と同じ)
	int			m_TotalFrame;			//キーフレームのフレーム数
	D3DXVECTOR2	m_HermitianStartDir;	//エルミート曲線始点接線ベクトル
	D3DXVECTOR2	m_HermitianEndDir;		//エルミート曲線終点接線ベクトル
};

//説明:
//class CMotionはモーションのキーフレーム数を保存し,キーフレームごとのデータを読み込む
class CMotion
{
public:
	CMotion();
	~CMotion();

	//セッター

	//ゲッター
	int GetTotalKeyFrame(void) { return (int)m_TotalKeyFrame; }			//総キーフレーム数をゲット
	CKeyFrame* GetKeyFrameData(int KeyFrameNum) { return &m_KeyFrame[KeyFrameNum]; }
	//他の関数
	
	//メンバ変数
	bool			m_bLoop;					//ループフラグ
	KEY_FRAME_NUM	m_TotalKeyFrame;			//キーフレーム数
	CKeyFrame		m_KeyFrame[MAX_KEY_FRAME];	//キーフレームのポインタ
	char			m_Name[MOTION_NAME_MAX];	//モーションの名前
};

#endif