#include "CEnemyDogTopState.h"
#include "CEnemy.h"
#include "CEnemyDogStateManager.h"

CEnemyDogTopState::CEnemyDogTopState(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_ParentState = nullptr;
	m_ChildState = nullptr;

	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;

	//子とものデフォルトステート設定
	if (pEnemyManager != nullptr) {
		SetState(pEnemy, pEnemyManager, pEnemyManager->GetState(ENEMY_DOG_PATROL_STATE));
	}
}

CEnemyDogTopState::~CEnemyDogTopState() 
{
	
}

bool CEnemyDogTopState::InitParameter(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;

	//子とものデフォルトステート設定
	if (pEnemyManager != nullptr) {
		SetState(pEnemy, pEnemyManager, pEnemyManager->GetState(ENEMY_DOG_PATROL_STATE));
	}

	return true;
}

int CEnemyDogTopState::Update(CEnemy* pEnemy)
{
	//ヌルチェック
	if(pEnemy == nullptr){
		return ENEMY_NULL_STATE;
	}

	//子供ステートの更新
	if (m_ChildState != nullptr) {
		m_TransitionStateNum = m_ChildState->Update(pEnemy);
	}

	return ENEMY_NULL_STATE;
}

void CEnemyDogTopState::Draw(void)
{
	
}