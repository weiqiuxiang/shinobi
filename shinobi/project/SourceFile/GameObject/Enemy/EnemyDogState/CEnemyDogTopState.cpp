#include "CEnemyDogTopState.h"
#include "CEnemy.h"
#include "CEnemyDogStateManager.h"

CEnemyDogTopState::CEnemyDogTopState(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_ParentState = nullptr;
	m_ChildState = nullptr;

	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;

	//�q�Ƃ��̃f�t�H���g�X�e�[�g�ݒ�
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

	//�q�Ƃ��̃f�t�H���g�X�e�[�g�ݒ�
	if (pEnemyManager != nullptr) {
		SetState(pEnemy, pEnemyManager, pEnemyManager->GetState(ENEMY_DOG_PATROL_STATE));
	}

	return true;
}

int CEnemyDogTopState::Update(CEnemy* pEnemy)
{
	//�k���`�F�b�N
	if(pEnemy == nullptr){
		return ENEMY_NULL_STATE;
	}

	//�q���X�e�[�g�̍X�V
	if (m_ChildState != nullptr) {
		m_TransitionStateNum = m_ChildState->Update(pEnemy);
	}

	return ENEMY_NULL_STATE;
}

void CEnemyDogTopState::Draw(void)
{
	
}