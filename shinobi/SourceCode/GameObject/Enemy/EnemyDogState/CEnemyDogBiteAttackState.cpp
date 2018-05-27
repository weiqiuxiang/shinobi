#include "CEnemyDogBiteAttackState.h"
#include "CEnemy.h"
#include "CEnemyDogStateManager.h"
#include "CEnemyControl.h"
#include "common.h"

#define RUN_POWER (0.10f)							//�����
#define CAN_BITE_ATTACK_DISTANCE (10.0f)			//�U���\����
#define ATTACK_MOVE_POWER (1.0f)					//�U���ړ���
#define ATTACK_RECOVERY_TIME (20)					//�d������

CEnemyDogBiteAttackState::CEnemyDogBiteAttackState(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_ParentState = nullptr;
	m_ChildState = nullptr;

	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;
	m_BiteAttackMotionTime = 0;
	m_AttackKeyFrameOld = 0;
	m_AttackRecovery = ATTACK_RECOVERY_TIME;

	//���[�V�����ݒ�
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

	//���[�V�����ݒ�
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
	//�k���`�F�b�N
	if(pEnemy == nullptr){
		return ENEMY_NULL_STATE;
	}
	if (m_EnemyStateManager == nullptr) {
		return ENEMY_NULL_STATE;
	}

	//���郂�[�V������
	if (pEnemy->GetMotionNow() == CEnemyControl::MOTION_RUN) {
		D3DXVECTOR3 DirectionToPlayer = CalcDirectionToPlayer(pEnemy);		//����-->�v���[���[�̕����x�N�g���̌v�Z
		pEnemy->AddPower(DirectionToPlayer * RUN_POWER);					//�v���[���[�Ɍ����đ���
		pEnemy->SetGoalDir(DirectionToPlayer);

		//�U���ɐ؂�ւ���
		float DistFromPlayer = CalcDistFromPlayer(pEnemy);
		if (DistFromPlayer < CAN_BITE_ATTACK_DISTANCE) {
			pEnemy->SetMotionMode(CEnemyControl::MOTION_ATTACK);
			m_BiteAttackMotionTime = pEnemy->GetMotionTotalFrame(CEnemyControl::MOTION_ATTACK);
		}
	}

	//�U�����[�V������
	if (pEnemy->GetMotionNow() == CEnemyControl::MOTION_ATTACK) {
		//���[�V�������Ԍ���
		m_BiteAttackMotionTime--;

		//�U�����[�V�������̏ڍ�
		AttackActionDetail(pEnemy);	

		//��ԑJ��
		if (m_BiteAttackMotionTime <= 0) {
			if (m_ParentState != nullptr) {
				pEnemy->DisableHitCircle();
				pEnemy->SetMotionMode(CEnemyControl::MOTION_NEUTRAL);
				return ENEMY_NULL_STATE;
			}
		}
	}

	//�d�����[�V������
	if (pEnemy->GetMotionNow() == CEnemyControl::MOTION_NEUTRAL) {
		//�d�����Ԍ���
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
	//�k���`�F�b�N
	if (pEnemy == nullptr) {
		_RPT0(0, "[CEnemyDogBiteAttackState.cpp][CalcDirectionToPlayer]����pEnemy�̓k��\n");
		return D3DXVECTOR3(0, 0, 0);
	}
	CPlayer* pPlayer = pEnemy->GetPlayer();
	if (pPlayer == nullptr) {
		_RPT0(0, "[CEnemyDogBiteAttackState.cpp][CalcDirectionToPlayer]�ϐ�pPlayer�̓k��\n");
		return D3DXVECTOR3(0, 0, 0);
	}

	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();            //�v���[���[�ʒu�擾
	D3DXVECTOR3 Enemy = pEnemy->GetPos();
	D3DXVECTOR3 MoveDirGoal = PlayerPos - Enemy;
	MoveDirGoal.y = 0;
	D3DXVec3Normalize(&MoveDirGoal, &MoveDirGoal);
	return MoveDirGoal;
}

float CEnemyDogBiteAttackState::CalcDistFromPlayer(CEnemy* pEnemy)
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

void CEnemyDogBiteAttackState::AttackActionDetail(CEnemy* pEnemy)
{
	//�k���`�F�b�N
	if (pEnemy == nullptr) {
		return;
	}

	int AIKeyFrameNow = pEnemy->GetKeyFrameNow();				//���̐i�s���̃L�[�t���[���擾
	bool bDiffKeyFrame = m_AttackKeyFrameOld != AIKeyFrameNow;	//�O�̃L�[�t���[���ƈقȂ邩

	//�t���[���ϓ��̎��̏���
	if (bDiffKeyFrame) {
		D3DXVECTOR3 MovePowerDir = CalcDirectionToPlayer(pEnemy);
		pEnemy->SetGoalDir(MovePowerDir);						//�ڕW������ݒ�
		m_AttackKeyFrameOld = AIKeyFrameNow;					//�i�s���̃L�[�t���[���o�^

		if (AIKeyFrameNow == 1) {
			pEnemy->AddPower(MovePowerDir * ATTACK_MOVE_POWER);	//�ړ��͗͐ݒ�
		}
	}

	//���蔻�苅�̃^�C�v�ύX
	bool HitColliEnable = pEnemy->GetKeyFrameNow() == 2 || pEnemy->GetKeyFrameNow() == 3;
	if (HitColliEnable) pEnemy->EnableHitCircle();									//�U�����苅��L����
	else pEnemy->DisableHitCircle();												//�U�����苅�𖳌���
}