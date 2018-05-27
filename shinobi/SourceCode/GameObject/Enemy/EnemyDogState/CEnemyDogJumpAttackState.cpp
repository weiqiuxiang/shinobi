#include "CEnemyDogJumpAttackState.h"
#include "CEnemy.h"
#include "CEnemyStateManager.h"
#include "CEnemyDogStateManager.h"
#include "CEnemyControl.h"
#include "common.h"

#define JUMP_POWER_XZ (1.2f)		//XZ���ʂ̃W�����v�p���[
#define JUMP_POWER_Y (0.8f)			//Y�����̃W�����v�p���[
#define ATTACK_RECOVERY_TIME (20)	//�d������

CEnemyDogJumpAttackState::CEnemyDogJumpAttackState(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_ParentState = nullptr;
	m_ChildState = nullptr;

	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;
	m_MotionTime = 0;
	m_KeyFrameOld = 0;
	m_AttackRecovery = ATTACK_RECOVERY_TIME;

	//���[�V�����ݒ�
	if (pEnemy != nullptr) {
		pEnemy->SetMotionMode(CEnemyControl::MOTION_JUMPATTACK);
		m_MotionTime = pEnemy->GetMotionTotalFrame(CEnemyControl::MOTION_JUMPATTACK);

		pEnemy->SetAttackType(DOG_JUMP_ATTACK);
	}
}

CEnemyDogJumpAttackState::~CEnemyDogJumpAttackState() 
{
	
}

bool CEnemyDogJumpAttackState::InitParameter(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;
	m_MotionTime = 0;
	m_KeyFrameOld = 0;
	m_AttackRecovery = ATTACK_RECOVERY_TIME;

	//���[�V�����ݒ�
	if (pEnemy != nullptr) {
		pEnemy->SetMotionMode(CEnemyControl::MOTION_JUMPATTACK);
		m_MotionTime = pEnemy->GetMotionTotalFrame(CEnemyControl::MOTION_JUMPATTACK);

		pEnemy->SetAttackType(DOG_JUMP_ATTACK);
	}

	return true;
}

int CEnemyDogJumpAttackState::Update(CEnemy* pEnemy)
{
	//�k���`�F�b�N
	if(pEnemy == nullptr){
		return ENEMY_NULL_STATE;
	}
	if (m_EnemyStateManager == nullptr) {
		return ENEMY_NULL_STATE;
	}
	
	//���[�V�������Ԍ���
	m_MotionTime--;

	//�W�����v�U�����̋�̓���
	AttackDetailAction(pEnemy);

	//��ԑJ��
	if (ChangeState(pEnemy) == true) {
		return ENEMY_NULL_STATE;
	}

	return ENEMY_NULL_STATE;
}

void CEnemyDogJumpAttackState::Draw(void)
{
	
}

void CEnemyDogJumpAttackState::AttackDetailAction(CEnemy* pEnemy)
{
	//�k���`�F�b�N
	if (pEnemy == nullptr) {
		return;
	}

	int AIKeyFrameNow = pEnemy->GetKeyFrameNow();				//���̐i�s���̃L�[�t���[���擾
	bool bDiffKeyFrame = m_KeyFrameOld != AIKeyFrameNow;		//�O�̃L�[�t���[���ƈقȂ邩

	//�L�[�t���[����2��菬�������A�v���[���[�ɕ��Ɍ���
	if (AIKeyFrameNow < 2) {
		pEnemy->SetGoalDir(CalcDirectionToPlayer(pEnemy));
	}

	//�L�[�t���[���ύX���̏���
	if (bDiffKeyFrame) {
		m_KeyFrameOld = AIKeyFrameNow;			//�O�̃L�[�t���[���ɍ��̃L�[�t���[������

		if (AIKeyFrameNow == 2) {
			pEnemy->SetCoffi(0.05f);
			pEnemy->AddPower(CalcDirectionToPlayer(pEnemy) * JUMP_POWER_XZ);
			pEnemy->AddPower(D3DXVECTOR3(0.0f, JUMP_POWER_Y, 0.0f));
		}
	}

	//�U������̗L��/�����̐؂�ւ�
	bool HitColliEnable = (AIKeyFrameNow != 0) && (AIKeyFrameNow != 1) && (AIKeyFrameNow != 5);
	if (HitColliEnable) {
		pEnemy->EnableHitCircle();		//�U�����苅��L����
	}
	else {
		pEnemy->DisableHitCircle();		//�U�����苅�𖳌���
	}
}

//����->�v���[���[�̕����x�N�g���̌v�Z
D3DXVECTOR3 CEnemyDogJumpAttackState::CalcDirectionToPlayer(CEnemy* pEnemy)
{
	//�k���`�F�b�N
	if (pEnemy == nullptr) {
		_RPT0(0, "[CEnemyDogJumpAttackState.cpp][CalcEnemyFaceToPlayerDir]����pEnemy�̓k��\n");
		return D3DXVECTOR3(0, 0, 0);
	}
	CPlayer* pPlayer = pEnemy->GetPlayer();
	if (pPlayer == nullptr) {
		_RPT0(0, "[CEnemyDogJumpAttackState.cpp][CalcEnemyFaceToPlayerDir]�ϐ�pPlayer�̓k��\n");
		return D3DXVECTOR3(0, 0, 0);
	}

	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();            //�v���[���[�ʒu�擾
	D3DXVECTOR3 Enemy = pEnemy->GetPos();
	D3DXVECTOR3 MoveDirGoal = PlayerPos - Enemy;
	MoveDirGoal.y = 0;
	D3DXVec3Normalize(&MoveDirGoal, &MoveDirGoal);
	return MoveDirGoal;
}

bool CEnemyDogJumpAttackState::ChangeState(CEnemy* pEnemy)
{
	//�k���`�F�b�N
	if (pEnemy == nullptr) {
		return false;
	}
	if (m_EnemyStateManager == nullptr) {
		return false;
	}

	//�u�v���[���[�Ɍ����v�X�e�[�g�ɑJ��
	if (pEnemy->GetMotionNow() == CEnemyControl::MOTION_JUMPATTACK) {
		if (m_MotionTime <= 0) {
			if (m_ParentState != nullptr) {
				pEnemy->SetCoffi(0.2f);
				pEnemy->DisableHitCircle();
				pEnemy->SetMotionMode(CEnemyControl::MOTION_NEUTRAL);
			}
			return false;
		}
	}
	
	//�j���[�g�������[�V�������d������
	if (pEnemy->GetMotionNow() == CEnemyControl::MOTION_NEUTRAL) {
		m_AttackRecovery--;

		if (m_AttackRecovery <= 0) {
			m_ParentState->SetState(pEnemy, m_EnemyStateManager, m_EnemyStateManager->GetState(ENEMY_DOG_ATTACK_RECOVERY_STATE));
			return true;
		}
	}

	return false;
}