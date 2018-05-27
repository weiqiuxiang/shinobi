#pragma once
#ifndef _CENEMY_ACTION_H_
#define _CENEMY_ACTION_H_

#include "CPlayer.h"
#include "CMeshField.h"
#include "CEnemy.h"
#include <list>

//前方宣言
class CEnemy;
class CPathPoint;

class CEnemyControl  //プレーヤーをコントロールするためのクラス
{
public:
	typedef enum
	{
		MOTION_NEUTRAL = 0,
		MOTION_MOVE,
		MOTION_RUN,
		MOTION_ATTACK,
		MOTION_JUMPATTACK,
		MOTION_STUN,
		MOTION_DEAD
	}MOTION_MODE;			//モーション

	CEnemyControl();
	~CEnemyControl();

	void Init(void);						//初期化
	void EnemyControl(void);				//プレーヤーをコントロールする(まとめクラス)

	//セッター
	void SetEnemy(CEnemy *Enemy);
	void SetPlayer(CPlayer *Player);

	//ゲッター
	CPlayer* GetPlayer(void) { return m_Player; }

private:
	void EnemyFaceDir(void);			//AIの向きからポーズを算出
	void EnemyHitAllMesh(void);			//AIとメッシュフィールドとの当り判定

	//メンバ変数
	CEnemy*				m_AI;
	CPlayer*			m_Player;
};
#endif