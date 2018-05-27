#include "CEnemyDogRunState.h"
#include "CEnemy.h"
#include "CEnemyDogStateManager.h"
#include "CEnemyControl.h"
#include "CPlayer.h"

#define RUN_POWER (0.10f)							//�����
#define CAN_JUMP_ATTACK_DISTANCE (20.0f)			//�U���\����

CEnemyDogRunState::CEnemyDogRunState(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_ParentState = nullptr;
	m_ChildState = nullptr;

	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;
	m_IsCanDoJumpAttack = true;

	float DistFromPlayer = CalcDistFromPlayer(pEnemy);

	if (pEnemy != nullptr) {
		if (DistFromPlayer < CAN_JUMP_ATTACK_DISTANCE) {
			pEnemy->SetMotionMode(CEnemyControl::MOTION_RUN);
		}
		else {
			m_IsCanDoJumpAttack = false;
		}
	}
}

CEnemyDogRunState::~CEnemyDogRunState() 
{
	
}

bool CEnemyDogRunState::InitParameter(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;
	m_IsCanDoJumpAttack = true;

	float DistFromPlayer = CalcDistFromPlayer(pEnemy);

	if (pEnemy != nullptr) {
		if (DistFromPlayer < CAN_JUMP_ATTACK_DISTANCE) {
			pEnemy->SetMotionMode(CEnemyControl::MOTION_RUN);
		}
		else {
			m_IsCanDoJumpAttack = false;		//��苗������Ȃ��ƁA�W�����v�U���͕s��
		}
	}

	return true;
}

int CEnemyDogRunState::Update(CEnemy* pEnemy)
{
	//�k���`�F�b�N
	if(pEnemy == nullptr){
		return ENEMY_NULL_STATE;
	}

	//����-->�v���[���[�̕����x�N�g���̌v�Z
	D3DXVECTOR3 DirectionToPlayer = CalcDirectionToPlayer(pEnemy);

	//�v���[���[�Ɍ����đ���
	pEnemy->AddPower(DirectionToPlayer * RUN_POWER);

	//�X�e�[�g�ύX
	if (ChangeState(pEnemy) == true) {
		return ENEMY_NULL_STATE;
	}

	return ENEMY_NULL_STATE;
}

void CEnemyDogRunState::Draw(void)
{
	
}

D3DXVECTOR3 CEnemyDogRunState::CalcDirectionToPlayer(CEnemy* pEnemy)
{
	//�k���`�F�b�N
	if (pEnemy == nullptr) {
		_RPT0(0,"[CEnemyDogRunState.cpp][CalcEnemyFaceToPlayerDir]����pEnemy�̓k��\n");
		return D3DXVECTOR3(0,0,0);
	}
	CPlayer* pPlayer = pEnemy->GetPlayer();
	if (pPlayer == nullptr) {
		_RPT0(0, "[CEnemyDogRunState.cpp][CalcEnemyFaceToPlayerDir]�ϐ�pPlayer�̓k��\n");
		return D3DXVECTOR3(0, 0, 0);
	}

	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();            //�v���[���[�ʒu�擾
	D3DXVECTOR3 Enemy = pEnemy->GetPos();
	D3DXVECTOR3 MoveDirGoal = PlayerPos - Enemy;
	MoveDirGoal.y = 0;
	D3DXVec3Normalize(&MoveDirGoal, &MoveDirGoal);
	return MoveDirGoal;
}

float CEnemyDogRunState::CalcDistFromPlayer(CEnemy* pEnemy)
{
	//�k���`�F�b�N
	if (pEnemy == nullptr) {
		_RPT0(0, "[CEnemyDogRunState.cpp][CalcDistFromPlayer]����pEnemy�̓k��\n");
		return -1;
	}
	CPlayer* pPlayer = pEnemy->GetPlayer();
	if (pPlayer == nullptr) {
		_RPT0(0, "[CEnemyDogRunState.cpp][CalcDistFromPlayer]�ϐ�pPlayer�̓k��\n");
		return -1;
	}

	D3DXVECTOR3 AIPos = pEnemy->GetPos();
	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();

	return D3DXVec3Length(&(AIPos - PlayerPos));
}

bool CEnemyDogRunState::ChangeState(CEnemy* pEnemy)
{
	//�k���`�F�b�N
	if (pEnemy == nullptr) {
		return false;
	}
	if (m_EnemyStateManager == nullptr) {
		return false;
	}

	//�W�����v�U���\�Ȃ̂��𔻒�
	if (m_IsCanDoJumpAttack == false) {
		if (m_ParentState != nullptr) {
			m_ParentState->SetState(pEnemy, m_EnemyStateManager, m_EnemyStateManager->GetState(ENEMY_DOG_BITE_ATTACK_STATE));
			return true;
		}
	}

	else {
		//�v���[���[�܂ł̋����̌v�Z
		float DistFromPlayer = CalcDistFromPlayer(pEnemy);

		//�W�����v�U���\�̋������Ȃ�
		if (DistFromPlayer <= CAN_JUMP_ATTACK_DISTANCE) {
			int RandomNum = rand() % 100 + 1;

			//25%�̊m���ŃW�����v�U���A75%�̊m���ő����Đڋ߂��A���ݍU��������
			if (RandomNum <= 25) {
				if (m_ParentState != nullptr) {
					m_ParentState->SetState(pEnemy, m_EnemyStateManager, m_EnemyStateManager->GetState(ENEMY_DOG_JUMPATTACK_STATE));
					return true;
				}
			}
			else {
				if (m_ParentState != nullptr) {
					m_ParentState->SetState(pEnemy, m_EnemyStateManager, m_EnemyStateManager->GetState(ENEMY_DOG_BITE_ATTACK_STATE));
					return true;
				}
			}
		}
	}

	return false;
}