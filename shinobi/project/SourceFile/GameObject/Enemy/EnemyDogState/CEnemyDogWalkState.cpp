#include "CEnemyDogWalkState.h"
#include "CEnemy.h"
#include "CEnemyStateManager.h"
#include "CEnemyControl.h"
#include "CEnemyDogStateManager.h"

#define WALK_POWER (0.02f)									//������
#define DEAD_ZONE_DISTANCE_FROM_PATHPOINT (1.0f)			//����|�C���g�ɒ������苗��

CEnemyDogWalkState::CEnemyDogWalkState(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_ParentState = nullptr;
	m_ChildState = nullptr;

	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;

	m_IsGetGoal = false;

	//���[�V�����ݒ�
	if (pEnemy != nullptr) {
		pEnemy->SetMotionMode(CEnemyControl::MOTION_MOVE);
	}
}

CEnemyDogWalkState::~CEnemyDogWalkState() 
{
	
}

bool CEnemyDogWalkState::InitParameter(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;

	m_IsGetGoal = false;

	//���[�V�����ݒ�
	if (pEnemy != nullptr) {
		pEnemy->SetMotionMode(CEnemyControl::MOTION_MOVE);
	}

	return true;
}

int CEnemyDogWalkState::Update(CEnemy* pEnemy)
{
	//�k���`�F�b�N
	if(pEnemy == nullptr){
		return ENEMY_NULL_STATE;
	}

	//�ڕW�n�_���擾����
	if (m_IsGetGoal == false) {
		m_GoalPos = FindGoalPosition(pEnemy);
		m_IsGetGoal = true;
	}

	//�ڕW�n�_�Ɍ������āA����
	D3DXVECTOR3 Vec3AIToGoalXZ = m_GoalPos - pEnemy->GetPos();
	Vec3AIToGoalXZ.y = 0;					//Y�l���J�b�g

	bool IsGoal = D3DXVec3Length(&Vec3AIToGoalXZ) < DEAD_ZONE_DISTANCE_FROM_PATHPOINT;
	if (!IsGoal) {
		D3DXVec3Normalize(&Vec3AIToGoalXZ, &Vec3AIToGoalXZ);
		pEnemy->AddPower(Vec3AIToGoalXZ * WALK_POWER);
		pEnemy->SetGoalDir(Vec3AIToGoalXZ);
	}

	//��ԕύX
	if (ChangeState(pEnemy) == true) {
		return ENEMY_NULL_STATE;
	}

	return ENEMY_NULL_STATE;
}

void CEnemyDogWalkState::Draw(void)
{
	
}

//�ڕW�n�_��T��
D3DXVECTOR3 CEnemyDogWalkState::FindGoalPosition(CEnemy* pEnemy)
{
	//�k���`�F�b�N
	if (pEnemy == nullptr) {
		_RPT0(0,"[CEnemyDogWalkState.cpp][FindGoalPosition]����pEnemy�̒l�̓k��\n");
		return D3DXVECTOR3(0.0f,0.0f,0.0f);
	}

	//����|�C���g������Ȃ��ꍇ
	if (pEnemy->GetPathPointListSize() == 1) {
		auto it = pEnemy->GetPathPointListBegin();
		if ((*it) != nullptr) {
			return (*it)->GetPos();
		}
	}

	//���݂̏���|�C���g���擾
	if (pEnemy->GetFlag_IsPathPointIsAscendingOrder() == true) {
		//���̏���|�C���g�擾
		auto NextPoint = pEnemy->GetPathPointListNext(pEnemy->GetPathNumberNow(), 1);
		if (NextPoint == pEnemy->GetPathPointListEnd()) {								//�v�f����������
			NextPoint = pEnemy->GetPathPointListPrev(pEnemy->GetPathNumberNow(), 2);	//���X�g��End��1�O�̃C�e���[�^�[�Ɉڍs
			pEnemy->SetFlag_IsPathPointIsAscendingOrder(false);
		}

		//����|�C���g�Ȃǂ̃f�[�^��pEnemy�ɕۑ�
		pEnemy->SetPathNumberNow(*NextPoint);
		D3DXVECTOR3 GoalPosition = (*NextPoint)->GetPos();
		return GoalPosition;
	}
	else {
		auto itBegin = pEnemy->GetPathPointListBegin();
		auto NextPoint = itBegin;
		//���̃|�C���g�̓��X�g�̃X�^�[�g�|�C���g��������
		if (*itBegin == pEnemy->GetPathNumberNow()) {
			NextPoint = pEnemy->GetPathPointListNext(*itBegin, 1);
			pEnemy->SetFlag_IsPathPointIsAscendingOrder(true);
		}
		else {
			NextPoint = pEnemy->GetPathPointListPrev(pEnemy->GetPathNumberNow(), 1);
		}

		//����|�C���g�Ȃǂ̃f�[�^��pEnemy�ɕۑ�
		pEnemy->SetPathNumberNow(*NextPoint);
		D3DXVECTOR3 GoalPosition = (*NextPoint)->GetPos();
		return GoalPosition;
	}
}

bool CEnemyDogWalkState::ChangeState(CEnemy* pEnemy)
{
	//�k���`�F�b�N
	if (pEnemy == nullptr) {
		return false;
	}
	if (m_EnemyStateManager == nullptr) {
		return false;
	}

	//�S�[���ɂ�����
	D3DXVECTOR3 Vec3AIToGoalXZ = m_GoalPos - pEnemy->GetPos();
	Vec3AIToGoalXZ.y = 0;
	bool IsGoal = D3DXVec3Length(&Vec3AIToGoalXZ) < DEAD_ZONE_DISTANCE_FROM_PATHPOINT;
	if (IsGoal) {
		if (m_ParentState != nullptr) {
			m_ParentState->SetState(pEnemy, m_EnemyStateManager, m_EnemyStateManager->GetState(ENEMY_DOG_WAIT_STATE));
			return true;
		}
	}

	return false;
}