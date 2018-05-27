#include "CEnemyDogAttackRecoveryState.h"
#include "CEnemy.h"
#include "CEnemyStateManager.h"
#include "CEnemyDogStateManager.h"
#include "CEnemyControl.h"

#define RECOVERY_TIME_BITE_ATTACK (20)
#define RECOVERY_TIME_JUMP_ATTACK (30)

CEnemyDogAttackRecoveryState::CEnemyDogAttackRecoveryState(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_ParentState = nullptr;
	m_ChildState = nullptr;

	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;
	m_WaitTime = 0;

	if (pEnemy != nullptr) {
		//待ちフレーム数取得
		ATTACK_TYPE_ENEMY AttackType = pEnemy->GetAttackType();

		//攻撃タイプ
		switch (AttackType) {
		case DOG_BITE_ATTACK:
			m_WaitTime = RECOVERY_TIME_BITE_ATTACK;
			break;
		case DOG_JUMP_ATTACK:
			m_WaitTime = RECOVERY_TIME_JUMP_ATTACK;
			break;
		}

		//モーション設定
		pEnemy->SetMotionMode(CEnemyControl::MOTION_NEUTRAL);
	}
}

CEnemyDogAttackRecoveryState::~CEnemyDogAttackRecoveryState() 
{
	
}

bool CEnemyDogAttackRecoveryState::InitParameter(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;
	m_WaitTime = 0;

	if (pEnemy != nullptr) {
		//待ちフレーム数取得
		ATTACK_TYPE_ENEMY AttackType = pEnemy->GetAttackType();

		//攻撃タイプ
		switch (AttackType) {
		case DOG_BITE_ATTACK:
			m_WaitTime = RECOVERY_TIME_BITE_ATTACK;
			break;
		case DOG_JUMP_ATTACK:
			m_WaitTime = RECOVERY_TIME_JUMP_ATTACK;
			break;
		}

		//モーション設定
		pEnemy->SetMotionMode(CEnemyControl::MOTION_NEUTRAL);
	}

	return true;
}

int CEnemyDogAttackRecoveryState::Update(CEnemy* pEnemy)
{
	//ヌルチェック
	if(pEnemy == nullptr){
		return ENEMY_NULL_STATE;
	}

	//待ち時間減少
	m_WaitTime--;

	//ステート変更
	if (ChangeState(pEnemy) == true) {
		return ENEMY_NULL_STATE;
	}

	return ENEMY_NULL_STATE;
}

void CEnemyDogAttackRecoveryState::Draw(void)
{
	
}

bool CEnemyDogAttackRecoveryState::ChangeState(CEnemy* pEnemy)
{
	//ヌルチェック
	if (pEnemy == nullptr) {
		return false;
	}
	if (m_EnemyStateManager == nullptr) {
		return false;
	}

	//歩く状態に
	if (m_WaitTime <= 0) {
		if (m_ParentState != nullptr) {
			m_ParentState->SetState(pEnemy,m_EnemyStateManager, m_EnemyStateManager->GetState(ENEMY_DOG_RORATE_TO_PLAYER_STATE));
			return true;
		}
	}

	return false;
}