#include "CEnemyDogWaitState.h"
#include "CEnemy.h"
#include "CEnemyStateManager.h"
#include "CEnemyDogStateManager.h"
#include "CEnemyControl.h"

CEnemyDogWaitState::CEnemyDogWaitState(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_ParentState = nullptr;
	m_ChildState = nullptr;

	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;
	m_WaitTime = 0;

	if (pEnemy != nullptr) {
		//待ちフレーム数取得
		CPathPoint*pPathPoint =  pEnemy->GetPathNumberNow();
		if (pPathPoint != nullptr) {
			m_WaitTime = pPathPoint->GetStopFrame();
		}

		//モーション設定
		pEnemy->SetMotionMode(CEnemyControl::MOTION_NEUTRAL);
	}
}

CEnemyDogWaitState::~CEnemyDogWaitState() 
{
	
}

bool CEnemyDogWaitState::InitParameter(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;
	m_WaitTime = 0;

	if (pEnemy != nullptr) {
		//待ちフレーム数取得
		CPathPoint*pPathPoint = pEnemy->GetPathNumberNow();
		if (pPathPoint != nullptr) {
			m_WaitTime = pPathPoint->GetStopFrame();
		}

		//モーション設定
		pEnemy->SetMotionMode(CEnemyControl::MOTION_NEUTRAL);
	}

	return true;
}

int CEnemyDogWaitState::Update(CEnemy* pEnemy)
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

void CEnemyDogWaitState::Draw(void)
{
	
}

bool CEnemyDogWaitState::ChangeState(CEnemy* pEnemy)
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
			m_ParentState->SetState(pEnemy,m_EnemyStateManager, m_EnemyStateManager->GetState(ENEMY_DOG_WALK_STATE));
			return true;
		}
	}

	return false;
}