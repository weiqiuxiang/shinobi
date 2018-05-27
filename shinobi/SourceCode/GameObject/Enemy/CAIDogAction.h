#pragma once
#ifndef CAI_DOG_ACTION_H_
#define CAI_DOG_ACTION_H_

//インクルードヘッダ
#include "CAIActionInterface.h"
#include "CPlayer.h"
#include "CEnemy.h"

class CAIDogAction : public CAIActionInterface
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

	CAIDogAction();
	~CAIDogAction();

	virtual void AIModeAction(void);	//実質上のUpdate関数

	//セッター
	virtual void SetPlayer(CPlayer *pPlayer) { m_pPlayer = pPlayer; }
	virtual void SetAI(CEnemy *pAI) {m_pAI = pAI; }

	//ゲッター
	virtual CPlayer* GetPlayer(void) { return m_pPlayer; }
	virtual CEnemy* GetAI(void) { return m_pAI; }

private:
	//メンバ関数
	void IfScenePlayerSetting(void);	//状態遷移前の処理
	D3DXVECTOR3 CalcEnemyFaceToPlayerDir(void);//プレーヤーの方向に向く(戻り値はプレーヤーの方向)
	void DistToDecide(float dist);

	//アクション変更の時の設定関数
	void ChangeActionToAttackSet(void);
	void ChangeActionToJumpAttackSet(void);
	void ChangeActionToStun(const D3DXVECTOR3& GoalFaceDir/*向き*/,int StunTime/*硬直時間*/, float MovePower/*移動パワー*/);

	//戦闘系関数
	void AttackAction(void);
	void JumpAttackAction(void);
	void RunToAttack(void);

	//AIモードに関係変数
	CPlayer*		m_pPlayer;
	CEnemy*			m_pAI;

	//巡回関係変数
	bool			m_PathDir;					//巡回ポイントを回る順番(falseは正順,trueは逆順)
	CPathPoint*		m_pPathPoint;				//CPathPointのポインター
};

#endif