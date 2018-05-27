#include <algorithm>
#include "CEnemyDogStateManager.h"
#include "CEnemyState.h"
#include "CEnemyDogAttackModeState.h"
#include "CEnemyDogBiteAttackState.h"
#include "CEnemyDogDeadState.h"
#include "CEnemyDogRorateToPlayerState.h"
#include "CEnemyDogJumpAttackState.h"
#include "CEnemyDogPatrolState.h"
#include "CEnemyDogRunState.h"
#include "CEnemyDogStunState.h"
#include "CEnemyDogTopState.h"
#include "CEnemyDogWaitState.h"
#include "CEnemyDogWalkState.h"
#include "CEnemyDogAttackRecoveryState.h"

CEnemyDogStateManager::CEnemyDogStateManager()
{
	for (int i = 0; i < ENEMY_DOG_STATE_MAX; i++) {
		m_StateArray[i] = nullptr;
	}
}

CEnemyDogStateManager::~CEnemyDogStateManager() 
{
	
}

//���ׂẴp�^�[���C���X�^���X��
bool CEnemyDogStateManager::InitAllInstance(CEnemy* pEnemy)
{
	//�k���`�F�b�N
	if (pEnemy == nullptr) {
		return false;
	}

	//�z��̃N���A
	int ArrayNum = m_StateArray.size();
	if(ArrayNum != 0){
		for (int i = 0; i < ENEMY_DOG_STATE_MAX; i++) {
			if (m_StateArray[i] != nullptr) {
				delete m_StateArray[i];
				m_StateArray[i] = nullptr;
			}
		}
	}

	CEnemyState* pEnemyState = nullptr;

	//top�X�e�[�g������
	pEnemyState = new CEnemyDogTopState(pEnemy,this);
	if (pEnemyState == nullptr) {
		return false;
	}
	m_StateArray[ENEMY_DOG_TOP_STATE] = pEnemyState;

	//attack�X�e�[�g������
	pEnemyState = new CEnemyDogAttackModeState(pEnemy, this);
	if (pEnemyState == nullptr) {
		return false;
	}
	m_StateArray[ENEMY_DOG_ATTACK_MODE_STATE] = pEnemyState;

	//biteAttack�X�e�[�g������
	pEnemyState = new CEnemyDogBiteAttackState(pEnemy, this);
	if (pEnemyState == nullptr) {
		return false;
	}
	m_StateArray[ENEMY_DOG_BITE_ATTACK_STATE] = pEnemyState;

	//dead�X�e�[�g������
	pEnemyState = new CEnemyDogDeadState(pEnemy, this);
	if (pEnemyState == nullptr) {
		return false;
	}
	m_StateArray[ENEMY_DOG_DEAD_STATE] = pEnemyState;

	//RorateToPlayer�X�e�[�g������
	pEnemyState = new CEnemyDogRorateToPlayerState(pEnemy, this);
	if (pEnemyState == nullptr) {
		return false;
	}
	m_StateArray[ENEMY_DOG_RORATE_TO_PLAYER_STATE] = pEnemyState;

	//JumpAttack�X�e�[�g������
	pEnemyState = new CEnemyDogJumpAttackState(pEnemy, this);
	if (pEnemyState == nullptr) {
		return false;
	}
	m_StateArray[ENEMY_DOG_JUMPATTACK_STATE] = pEnemyState;

	//AttackRecovery�X�e�[�g������
	pEnemyState = new CEnemyDogAttackRecoveryState(pEnemy, this);
	if (pEnemyState == nullptr) {
		return false;
	}
	m_StateArray[ENEMY_DOG_ATTACK_RECOVERY_STATE] = pEnemyState;
	
	//Patrol�X�e�[�g������
	pEnemyState = new CEnemyDogPatrolState(pEnemy, this);
	if (pEnemyState == nullptr) {
		return false;
	}
	m_StateArray[ENEMY_DOG_PATROL_STATE] = pEnemyState;

	//Run�X�e�[�g������
	pEnemyState = new CEnemyDogRunState(pEnemy, this);
	if (pEnemyState == nullptr) {
		return false;
	}
	m_StateArray[ENEMY_DOG_RUN_STATE] = pEnemyState;

	//Stun�X�e�[�g������
	pEnemyState = new CEnemyDogStunState(pEnemy, this);
	if (pEnemyState == nullptr) {
		return false;
	}
	m_StateArray[ENEMY_DOG_STUN_STATE] = pEnemyState;

	//Wait�X�e�[�g������
	pEnemyState = new CEnemyDogWaitState(pEnemy, this);
	if (pEnemyState == nullptr) {
		return false;
	}
	m_StateArray[ENEMY_DOG_WAIT_STATE] = pEnemyState;

	//Walk�X�e�[�g������
	pEnemyState = new CEnemyDogWalkState(pEnemy, this);
	if (pEnemyState == nullptr) {
		return false;
	}
	m_StateArray[ENEMY_DOG_WALK_STATE] = pEnemyState;

	//�S�p�^�[������������
	for (int i = 0; i < ENEMY_DOG_STATE_MAX; i++) {
		if (m_StateArray[i] != nullptr) {
			m_StateArray[i]->InitParameter(pEnemy,this);
		}
	}

	//�e�q�֌W�����т�
	LinkParentAndChild();

	return true;
}

CEnemyState* CEnemyDogStateManager::GetState(int PatternNum)
{
	if(PatternNum < 0 || PatternNum >= ENEMY_DOG_STATE_MAX){
		_RPT0(0,"[CEnemyDogStateManager.cpp][GetPattern]����PatternNum�̒l�͔͈͊O\n");
		return nullptr;
	}

	return m_StateArray[PatternNum];
}

void CEnemyDogStateManager::UninitAllInstance(void)
{
	int ArrayNum = m_StateArray.size();
	if (ArrayNum != 0) {
		for (int i = 0; i < ENEMY_DOG_STATE_MAX; i++) {
			if (m_StateArray[i] != nullptr) {
				delete m_StateArray[i];
				m_StateArray[i] = nullptr;
			}
		}
	}
}

void CEnemyDogStateManager::LinkParentAndChild(void)
{
	//�u����v�X�e�[�g�̐e�X�e�[�g�́uTOP�v�X�e�[�g
	if (m_StateArray[ENEMY_DOG_PATROL_STATE] != nullptr) {
		m_StateArray[ENEMY_DOG_PATROL_STATE]->SetParentState(m_StateArray[ENEMY_DOG_TOP_STATE]);
	}

	//�u�U�����[�h�v�X�e�[�g�̐e�X�e�[�g�́uTOP�v�X�e�[�g
	if (m_StateArray[ENEMY_DOG_ATTACK_MODE_STATE] != nullptr) {
		m_StateArray[ENEMY_DOG_ATTACK_MODE_STATE]->SetParentState(m_StateArray[ENEMY_DOG_TOP_STATE]);
	}

	//�u�X�^���v�X�e�[�g�̐e�X�e�[�g�́uTOP�v�X�e�[�g
	if (m_StateArray[ENEMY_DOG_STUN_STATE] != nullptr) {
		m_StateArray[ENEMY_DOG_STUN_STATE]->SetParentState(m_StateArray[ENEMY_DOG_TOP_STATE]);
	}

	//�u���S�v�X�e�[�g�̐e�X�e�[�g�́uTOP�v�X�e�[�g
	if (m_StateArray[ENEMY_DOG_DEAD_STATE] != nullptr) {
		m_StateArray[ENEMY_DOG_DEAD_STATE]->SetParentState(m_StateArray[ENEMY_DOG_TOP_STATE]);
	}

	//�u�҂v�X�e�[�g�̐e�X�e�[�g�́u����v�X�e�[�g
	if (m_StateArray[ENEMY_DOG_WAIT_STATE] != nullptr) {
		m_StateArray[ENEMY_DOG_WAIT_STATE]->SetParentState(m_StateArray[ENEMY_DOG_PATROL_STATE]);
	}

	//�u�ړ��v�X�e�[�g�̐e�X�e�[�g�́u����v�X�e�[�g
	if (m_StateArray[ENEMY_DOG_WALK_STATE] != nullptr) {
		m_StateArray[ENEMY_DOG_WALK_STATE]->SetParentState(m_StateArray[ENEMY_DOG_PATROL_STATE]);
	}

	//�u�v���[���[�Ɍ����v�X�e�[�g�̐e�X�e�[�g�́u�U�����[�h�v�X�e�[�g
	if (m_StateArray[ENEMY_DOG_RORATE_TO_PLAYER_STATE] != nullptr) {
		m_StateArray[ENEMY_DOG_RORATE_TO_PLAYER_STATE]->SetParentState(m_StateArray[ENEMY_DOG_ATTACK_MODE_STATE]);
	}

	//�u����v�X�e�[�g�̐e�X�e�[�g�́u�U�����[�h�v�X�e�[�g
	if (m_StateArray[ENEMY_DOG_RUN_STATE] != nullptr) {
		m_StateArray[ENEMY_DOG_RUN_STATE]->SetParentState(m_StateArray[ENEMY_DOG_ATTACK_MODE_STATE]);
	}

	//�u���ݍU���v�X�e�[�g�̐e�X�e�[�g�́u�U�����[�h�v�X�e�[�g
	if (m_StateArray[ENEMY_DOG_BITE_ATTACK_STATE] != nullptr) {
		m_StateArray[ENEMY_DOG_BITE_ATTACK_STATE]->SetParentState(m_StateArray[ENEMY_DOG_ATTACK_MODE_STATE]);
	}

	//�u�W�����v�U���v�X�e�[�g�̐e�X�e�[�g�́u�U�����[�h�v�X�e�[�g
	if (m_StateArray[ENEMY_DOG_JUMPATTACK_STATE] != nullptr) {
		m_StateArray[ENEMY_DOG_JUMPATTACK_STATE]->SetParentState(m_StateArray[ENEMY_DOG_ATTACK_MODE_STATE]);
	}

	//�u�U���d���v�X�e�[�g�̐e�X�e�[�g�́u�U�����[�h�v�X�e�[�g
	if (m_StateArray[ENEMY_DOG_ATTACK_RECOVERY_STATE] != nullptr) {
		m_StateArray[ENEMY_DOG_ATTACK_RECOVERY_STATE]->SetParentState(m_StateArray[ENEMY_DOG_ATTACK_MODE_STATE]);
	}
}