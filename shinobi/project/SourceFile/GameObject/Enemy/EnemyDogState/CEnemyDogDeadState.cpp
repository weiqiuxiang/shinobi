#include "CEnemyDogDeadState.h"
#include "CEnemy.h"
#include "CEnemyDogStateManager.h"
#include "CEnemyControl.h"

CEnemyDogDeadState::CEnemyDogDeadState(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_ParentState = nullptr;
	m_ChildState = nullptr;

	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;

	//モーション設定
	if (pEnemy != nullptr) {
		pEnemy->SetMotionMode(CEnemyControl::MOTION_DEAD);
	}
}

CEnemyDogDeadState::~CEnemyDogDeadState() 
{
	
}

bool CEnemyDogDeadState::InitParameter(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;

	//モーション設定
	if (pEnemy != nullptr) {
		pEnemy->SetMotionMode(CEnemyControl::MOTION_DEAD);
	}

	return true;
}

int CEnemyDogDeadState::Update(CEnemy* pEnemy)
{
	//ヌルチェック
	if(pEnemy == nullptr){
		return ENEMY_NULL_STATE;
	}

	return ENEMY_NULL_STATE;
}

void CEnemyDogDeadState::Draw(void)
{
	
}