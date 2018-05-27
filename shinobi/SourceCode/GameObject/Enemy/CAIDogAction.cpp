#include "CAIDogAction.h"

//マクロ
#define WALK_POWER (0.02f)							//歩く力
#define RUN_POWER (0.10f)							//走る力
#define JUMP_ATTACK_POWER (1.2f)					//ジャンプ攻撃の移動力
#define JUMP_POWER (0.8f)							//ジャンプ攻撃の跳躍力
#define ATTACK_MOVE_POWER1 (0.6f)					//攻撃移動力1
#define ATTACK_MOVE_POWER2 (1.0f)					//攻撃移動力2

#define EYE_HALF_HORIANGLE (D3DX_PI * 0.25)			//視覚の半分角度(水平)
#define EYE_HALF_VERTIANGLE (D3DX_PI * 0.25)		//視覚の半分角度(垂直)
#define EYE_DISTANCE (50.0f)						//視覚の距離
#define LISTEN_DISTANCE (20.0f)						//聴覚の距離
#define ATTACK_CAN_START_DISTANCE (10.0f)			//攻撃の距離
#define JUMPATTACK_CAN_START_DISTANCE (20.0f)		//飛び掛かる攻撃実行可能距離

#define DEAD_ZONE_DISTANCE_FROM_PATHPOINT (1.0f)	//巡回は開始/終了できる距離

CAIDogAction::CAIDogAction()
{
	m_pPlayer	= nullptr;
	m_pAI		= nullptr;

	//巡回関係の変数
	m_pPathPoint	= nullptr;
	m_PathDir		= false;
}

CAIDogAction::~CAIDogAction() 
{

}

void CAIDogAction::AIModeAction(void)
{
	if (nullptr == m_pAI) {
		return;
	}
	if (nullptr == m_pPlayer) {
		return;
	}

	switch (m_AIState) {		//AIの状態
	case STATE_PATROL:
		if (m_Action != ACTION_NORMAL) m_Action = ACTION_NORMAL;	//アクション設定
		AIPatrol();							//巡回
		AISencePlayer();					//プレーヤー感知
		IfScenePlayerSetting();				//もしプレーヤーを発見
		break;

	case STATE_THINK:
		switch (m_ThinkState) {
		case TS_RORATE_TO_PLAYER:	//プレーヤーの方に向く
		{
			m_Action = ACTION_NORMAL;
			m_MovePowerDir = CalcEnemyFaceToPlayerDir();	//プレーヤーの方に向く
			if (acos( D3DXVec3Dot(&m_MovePowerDir, &m_pAI->GetFaceDir()) ) < D3DX_PI*0.1) {
				m_ThinkState = TS_THINK_NO_MOVE;		//移動しない思考に遷移
			}
			break;
		}

		case TS_THINK_NO_MOVE:			//移動しない思考
		{
			m_nThinkTime--;
			if (m_nThinkTime > 0) break;
			DistToDecide(CalcDistPlayerAI());	//プレーヤーとの距離で動きを決定
			break;
		}

		case TS_THINK_MOVE:
			break;

		case TS_RUN_TO_ATTACK_PLAYER:
			RunToAttack();
			break;

		case TS_JUMP_ATTACK:
			if (m_nMotionTime > 0) break;
			m_ThinkState = TS_RORATE_TO_PLAYER;
			m_nThinkTime = 30;
			m_pAI->SetCoffi(0.2);
			m_pAI->DisableHitCircle();			//攻撃当り判定球を無効に
			break;
		}
		break;

	case STATE_DEAD:
		m_Action = ACTION_DEAD;
		break;
	}

	switch (m_Action) {		//AIのアクション(AIの状態の中に決める)
	case ACTION_NORMAL:		//立ち留まる
		SetMotion(MOTION_NEUTRAL);
		m_pAI->SetGoalDir(m_MovePowerDir);
		break;
	case ACTION_WALK:		//歩く
		SetMotion(MOTION_MOVE);
		m_pAI->SetGoalDir(m_MovePowerDir);				//向きを移動方向に設定
		m_pAI->AddPower(m_MovePowerDir*WALK_POWER);		//移動力を与える
		break;
	case ACTION_RUN:		//走る
		SetMotion(MOTION_RUN);
		m_pAI->SetGoalDir(m_MovePowerDir);				//向きを移動方向に設定
		m_pAI->AddPower(m_MovePowerDir*RUN_POWER);
		break;
	case ACTION_ATTACK:		//攻撃
		SetMotion(MOTION_ATTACK);
		AttackAction();										//攻撃の具体動作
		m_nMotionTime = max(--m_nMotionTime,0);				//モーション時間減少
		if (m_nMotionTime <= 0)m_pAI->DisableHitCircle();	//当り判定球有効に
		break;
	case ACTION_JUMPATTACK:	//飛び掛かる攻撃
		SetMotion(MOTION_JUMPATTACK);
		JumpAttackAction();
		m_nMotionTime = max(--m_nMotionTime, 0);			//モーション時間減少
		if(m_nMotionTime <= 0)m_pAI->DisableHitCircle();	//当り判定球有無効に
		break;
	case ACTION_STUN:		//怯み(スタン)
		SetMotion(MOTION_STUN);
		m_nStunTime = max(--m_nStunTime, 0);		//スタン時間減少
		break;
	case ACTION_DEAD:		//死亡
		SetMotion(MOTION_DEAD);
		break;
	}

	MotionChangeCheck();				//モーション変更が有効か(元のモーションと同じだったら無効)
}

//プレーヤーの方に向く
D3DXVECTOR3 CAIDogAction::CalcEnemyFaceToPlayerDir(void)
{
	D3DXVECTOR3 PlayerPos = m_pPlayer->GetPos();            //プレーヤー位置取得
	D3DXVECTOR3 Enemy = m_pAI->GetPos();
	D3DXVECTOR3 MoveDirGoal = PlayerPos - Enemy;
	MoveDirGoal.y = 0;
	D3DXVec3Normalize(&MoveDirGoal, &MoveDirGoal);
	return MoveDirGoal;
}