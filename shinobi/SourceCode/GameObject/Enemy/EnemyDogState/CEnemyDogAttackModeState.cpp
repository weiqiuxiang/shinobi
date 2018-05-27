#include "CEnemyDogAttackModeState.h"
#include "CEnemy.h"
#include "CEnemyDogStateManager.h"

CEnemyDogAttackModeState::CEnemyDogAttackModeState(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_ParentState = nullptr;
	m_ChildState = nullptr;

	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;
	
	//子とものデフォルトステート設定
	if (pEnemyManager != nullptr) {
		SetState(pEnemy, pEnemyManager, pEnemyManager->GetState(ENEMY_DOG_RORATE_TO_PLAYER_STATE));
	}
}

CEnemyDogAttackModeState::~CEnemyDogAttackModeState() 
{
	
}

bool CEnemyDogAttackModeState::InitParameter(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;

	//子とものデフォルトステート設定
	if (pEnemyManager != nullptr) {
		SetState(pEnemy, pEnemyManager, pEnemyManager->GetState(ENEMY_DOG_RORATE_TO_PLAYER_STATE));
	}

	return true;
}

int CEnemyDogAttackModeState::Update(CEnemy* pEnemy)
{
	//ヌルチェック
	if(pEnemy == nullptr){
		return ENEMY_NULL_STATE;
	}

	//子供のステート
	if (m_ChildState != nullptr) {
		m_TransitionStateNum = m_ChildState->Update(pEnemy);
	}

	//状態遷移
	if(ChangeState(pEnemy) == true) {
		return ENEMY_NULL_STATE;
	}

	return ENEMY_NULL_STATE;
}

void CEnemyDogAttackModeState::Draw(void)
{
	
}

//ステート変更
bool CEnemyDogAttackModeState::ChangeState(CEnemy* pEnemy)
{
	//ヌルチェック
	if (pEnemy == nullptr) {
		return false;
	}
	if (m_EnemyStateManager == nullptr) {
		return false;
	}

	//子供の帰り値でステート遷移
	if (m_TransitionStateNum != ENEMY_NULL_STATE) {
		if (m_ParentState != nullptr) {
			m_ParentState->SetState(pEnemy, m_EnemyStateManager, m_EnemyStateManager->GetState(m_TransitionStateNum));
			return true;
		}
	}

	//ダメージ受けるとスタン状態に遷移
	if (pEnemy->GetIsBeAttackedFlag() == true) {
		if (m_ParentState != nullptr) {
			pEnemy->SetIsBeAttackedFlag(false);
			pEnemy->SetNoDamageTime(3);
			pEnemy->DisableHitCircle();
			m_ParentState->SetState(pEnemy, m_EnemyStateManager, m_EnemyStateManager->GetState(ENEMY_DOG_STUN_STATE));
			return true;
		}
	}

	return false;
}