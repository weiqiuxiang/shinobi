#include "CEnemyDogBiteAttackState.h"
#include "CEnemy.h"
#include "CEnemyDogStateManager.h"
#include "CEnemyControl.h"
#include "common.h"

#define RUN_POWER (0.10f)							//走る力
#define CAN_BITE_ATTACK_DISTANCE (10.0f)			//攻撃可能距離
#define ATTACK_MOVE_POWER (1.0f)					//攻撃移動力
#define ATTACK_RECOVERY_TIME (20)					//硬直時間

CEnemyDogBiteAttackState::CEnemyDogBiteAttackState(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_ParentState = nullptr;
	m_ChildState = nullptr;

	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;
	m_BiteAttackMotionTime = 0;
	m_AttackKeyFrameOld = 0;
	m_AttackRecovery = ATTACK_RECOVERY_TIME;

	//モーション設定
	if (pEnemy != nullptr) {
		float DistFromPlayer = CalcDistFromPlayer(pEnemy);

		if (DistFromPlayer < CAN_BITE_ATTACK_DISTANCE) {
			pEnemy->SetMotionMode(CEnemyControl::MOTION_ATTACK);
			m_BiteAttackMotionTime = pEnemy->GetMotionTotalFrame(CEnemyControl::MOTION_ATTACK);
		}

		else {
			int MotionNow = pEnemy->GetMotionNow();
			if (MotionNow != CEnemyControl::MOTION_RUN) {
				pEnemy->SetMotionMode(CEnemyControl::MOTION_RUN);
			}
		}

		pEnemy->SetAttackType(DOG_BITE_ATTACK);
	}
}

CEnemyDogBiteAttackState::~CEnemyDogBiteAttackState() 
{
	
}

bool CEnemyDogBiteAttackState::InitParameter(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;
	m_BiteAttackMotionTime = 0;
	m_AttackKeyFrameOld = 0;
	m_AttackRecovery = ATTACK_RECOVERY_TIME;

	//モーション設定
	if (pEnemy != nullptr) {
		float DistFromPlayer = CalcDistFromPlayer(pEnemy);

		if (DistFromPlayer < CAN_BITE_ATTACK_DISTANCE) {
			pEnemy->SetMotionMode(CEnemyControl::MOTION_ATTACK);
			m_BiteAttackMotionTime = pEnemy->GetMotionTotalFrame(CEnemyControl::MOTION_ATTACK);
		}

		else {
			int MotionNow = pEnemy->GetMotionNow();
			if (MotionNow != CEnemyControl::MOTION_RUN) {
				pEnemy->SetMotionMode(CEnemyControl::MOTION_RUN);
			}
		}

		pEnemy->SetAttackType(DOG_BITE_ATTACK);
	}

	return true;
}

int CEnemyDogBiteAttackState::Update(CEnemy* pEnemy)
{
	//ヌルチェック
	if(pEnemy == nullptr){
		return ENEMY_NULL_STATE;
	}
	if (m_EnemyStateManager == nullptr) {
		return ENEMY_NULL_STATE;
	}

	//走るモーション時
	if (pEnemy->GetMotionNow() == CEnemyControl::MOTION_RUN) {
		D3DXVECTOR3 DirectionToPlayer = CalcDirectionToPlayer(pEnemy);		//自分-->プレーヤーの方向ベクトルの計算
		pEnemy->AddPower(DirectionToPlayer * RUN_POWER);					//プレーヤーに向けて走る
		pEnemy->SetGoalDir(DirectionToPlayer);

		//攻撃に切り替える
		float DistFromPlayer = CalcDistFromPlayer(pEnemy);
		if (DistFromPlayer < CAN_BITE_ATTACK_DISTANCE) {
			pEnemy->SetMotionMode(CEnemyControl::MOTION_ATTACK);
			m_BiteAttackMotionTime = pEnemy->GetMotionTotalFrame(CEnemyControl::MOTION_ATTACK);
		}
	}

	//攻撃モーション時
	if (pEnemy->GetMotionNow() == CEnemyControl::MOTION_ATTACK) {
		//モーション時間減少
		m_BiteAttackMotionTime--;

		//攻撃モーション中の詳細
		AttackActionDetail(pEnemy);	

		//状態遷移
		if (m_BiteAttackMotionTime <= 0) {
			if (m_ParentState != nullptr) {
				pEnemy->DisableHitCircle();
				pEnemy->SetMotionMode(CEnemyControl::MOTION_NEUTRAL);
				return ENEMY_NULL_STATE;
			}
		}
	}

	//硬直モーション時
	if (pEnemy->GetMotionNow() == CEnemyControl::MOTION_NEUTRAL) {
		//硬直時間減少
		m_AttackRecovery--;

		if (m_AttackRecovery <= 0) {
			m_ParentState->SetState(pEnemy, m_EnemyStateManager, m_EnemyStateManager->GetState(ENEMY_DOG_ATTACK_RECOVERY_STATE));
			return ENEMY_NULL_STATE;
		}
	}

	return ENEMY_NULL_STATE;
}

void CEnemyDogBiteAttackState::Draw(void)
{
	
}

D3DXVECTOR3 CEnemyDogBiteAttackState::CalcDirectionToPlayer(CEnemy* pEnemy)
{
	//ヌルチェック
	if (pEnemy == nullptr) {
		_RPT0(0, "[CEnemyDogBiteAttackState.cpp][CalcDirectionToPlayer]引数pEnemyはヌル\n");
		return D3DXVECTOR3(0, 0, 0);
	}
	CPlayer* pPlayer = pEnemy->GetPlayer();
	if (pPlayer == nullptr) {
		_RPT0(0, "[CEnemyDogBiteAttackState.cpp][CalcDirectionToPlayer]変数pPlayerはヌル\n");
		return D3DXVECTOR3(0, 0, 0);
	}

	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();            //プレーヤー位置取得
	D3DXVECTOR3 Enemy = pEnemy->GetPos();
	D3DXVECTOR3 MoveDirGoal = PlayerPos - Enemy;
	MoveDirGoal.y = 0;
	D3DXVec3Normalize(&MoveDirGoal, &MoveDirGoal);
	return MoveDirGoal;
}

float CEnemyDogBiteAttackState::CalcDistFromPlayer(CEnemy* pEnemy)
{
	//ヌルチェック
	if (pEnemy == nullptr) {
		_RPT0(0, "[CEnemyDogRunState.cpp][CalcDistFromPlayer]引数pEnemyはヌル\n");
		return -1;
	}
	CPlayer* pPlayer = pEnemy->GetPlayer();
	if (pPlayer == nullptr) {
		_RPT0(0, "[CEnemyDogRunState.cpp][CalcDistFromPlayer]変数pPlayerはヌル\n");
		return -1;
	}

	D3DXVECTOR3 AIPos = pEnemy->GetPos();
	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();

	return D3DXVec3Length(&(AIPos - PlayerPos));
}

void CEnemyDogBiteAttackState::AttackActionDetail(CEnemy* pEnemy)
{
	//ヌルチェック
	if (pEnemy == nullptr) {
		return;
	}

	int AIKeyFrameNow = pEnemy->GetKeyFrameNow();				//今の進行中のキーフレーム取得
	bool bDiffKeyFrame = m_AttackKeyFrameOld != AIKeyFrameNow;	//前のキーフレームと異なるか

	//フレーム変動の時の処理
	if (bDiffKeyFrame) {
		D3DXVECTOR3 MovePowerDir = CalcDirectionToPlayer(pEnemy);
		pEnemy->SetGoalDir(MovePowerDir);						//目標向きを設定
		m_AttackKeyFrameOld = AIKeyFrameNow;					//進行中のキーフレーム登録

		if (AIKeyFrameNow == 1) {
			pEnemy->AddPower(MovePowerDir * ATTACK_MOVE_POWER);	//移動力力設定
		}
	}

	//当り判定球のタイプ変更
	bool HitColliEnable = pEnemy->GetKeyFrameNow() == 2 || pEnemy->GetKeyFrameNow() == 3;
	if (HitColliEnable) pEnemy->EnableHitCircle();									//攻撃判定球を有効に
	else pEnemy->DisableHitCircle();												//攻撃判定球を無効に
}