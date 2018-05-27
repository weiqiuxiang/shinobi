#include "CEnemyDogStunState.h"
#include "CEnemy.h"
#include "CEnemyDogStateManager.h"
#include "CEnemyControl.h"
#include "common.h"
#include "CEnemyControl.h"

#define STUN_COFFI (0.4f)	//�X�^�����̒�R�W��

CEnemyDogStunState::CEnemyDogStunState(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_ParentState = nullptr;
	m_ChildState = nullptr;

	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;
	m_StunMotionTime = 0;
	m_DecoveryTime = 0;

	//�X�^���p�����[�^�ݒ�
	StunSetting(pEnemy);

	//���[�V�����ݒ�
	if (pEnemy != nullptr) {
		pEnemy->DisableHitCircle();
		pEnemy->SetCoffi(STUN_COFFI);
		pEnemy->SetMotionMode(CEnemyControl::MOTION_STUN);
		m_StunMotionTime = pEnemy->GetMotionTotalFrame(CEnemyControl::MOTION_STUN);
	}
}

CEnemyDogStunState::~CEnemyDogStunState() 
{
	
}

bool CEnemyDogStunState::InitParameter(CEnemy* pEnemy, CEnemyStateManager* pEnemyManager)
{
	m_EnemyStateManager = pEnemyManager;
	m_TransitionStateNum = ENEMY_NULL_STATE;
	m_StunMotionTime = 0;
	m_DecoveryTime = 0;

	//�X�^���p�����[�^�ݒ�
	StunSetting(pEnemy);
	
	if (pEnemy != nullptr) {
		pEnemy->DisableHitCircle();
		pEnemy->SetCoffi(STUN_COFFI);
		pEnemy->SetMotionMode(CEnemyControl::MOTION_STUN);
		m_StunMotionTime = pEnemy->GetMotionTotalFrame(CEnemyControl::MOTION_STUN);
	}

	return true;
}

int CEnemyDogStunState::Update(CEnemy* pEnemy)
{
	//�k���`�F�b�N
	if(pEnemy == nullptr){
		return ENEMY_NULL_STATE;
	}
	if (m_EnemyStateManager == nullptr) {
		return ENEMY_NULL_STATE;
	}
	
	//��O����
	if (pEnemy->GetDamageType() == PLAYER_ATTACK_TYPE_NONE) {
		if (m_ParentState != nullptr) {
			m_ParentState->SetState(pEnemy, m_EnemyStateManager, m_EnemyStateManager->GetState(ENEMY_DOG_PATROL_STATE));
			return ENEMY_NULL_STATE;
		}
	}

	//���[�V�������Ԍ���
	if (m_StunMotionTime >= 0) {
		m_StunMotionTime--;
	}
	else {
		m_DecoveryTime--;
	}

	//��ԑJ��,life��0�������玀�S��ԂɑJ�ځAlife��0�ȏゾ������U����ԂɑJ��
	if (m_DecoveryTime < 0) {
		if (pEnemy->GetLife() <= 0) {
			if (m_ParentState != nullptr) {
				m_ParentState->SetState(pEnemy, m_EnemyStateManager, m_EnemyStateManager->GetState(ENEMY_DOG_DEAD_STATE));
				return ENEMY_NULL_STATE;
			}
		}
		else {
			if (m_ParentState != nullptr) {
				pEnemy->SetCoffi(0.2f);
				m_ParentState->SetState(pEnemy, m_EnemyStateManager, m_EnemyStateManager->GetState(ENEMY_DOG_ATTACK_MODE_STATE));
				return ENEMY_NULL_STATE;
			}
		}
	}

	if (pEnemy->GetIsBeAttackedFlag() == true) {
		if (pEnemy->GetNoDamageTime() <= 0) {
			if (m_ParentState != nullptr) {
				pEnemy->SetIsBeAttackedFlag(false);
				pEnemy->SetNoDamageTime(3);
				m_ParentState->SetState(pEnemy, m_EnemyStateManager, m_EnemyStateManager->GetState(ENEMY_DOG_STUN_STATE));
				return true;
			}
		}
	}

	return ENEMY_NULL_STATE;
}

void CEnemyDogStunState::Draw(void)
{
	
}

void CEnemyDogStunState::StunSetting(CEnemy* pEnemy)
{
	//�k���`�F�b�N
	if (pEnemy == nullptr) {
		return;
	}
	if (m_EnemyStateManager == nullptr) {
		return;
	}

	//�X�^�������擾
	D3DXVECTOR3 StunDir = pEnemy->GetStunDir();

	//�v���[���[�̍U���^�C�v�ɉ����ă_���[�W���󂯁A��苗���ɔ�΂����
	switch (pEnemy->GetDamageType()) 
	{
	case PLAYER_ATTACK_TYPE_NONE:
		break;
	case WSWORD_ATTACK_01:
		pEnemy->AddPower(0.8f * StunDir);		//�X�^���ړ��p���[�ݒ�
		pEnemy->SetNoDamageTime(3);				//���G���Ԑݒ�
		pEnemy->AddLife(-20);					//HP����
		m_DecoveryTime = 10;					//�X�^�����Ԑݒ�
		break;
	case WSWORD_ATTACK_02:
		pEnemy->AddPower(0.8f * StunDir);		//�X�^���ړ��p���[�ݒ�
		pEnemy->SetNoDamageTime(3);				//���G���Ԑݒ�
		pEnemy->AddLife(-20);					//HP����
		m_DecoveryTime = 10;					//�X�^�����Ԑݒ�
		break;
	case WSWORD_ATTACK_03:
		pEnemy->AddPower(0.8f * StunDir);		//�X�^���ړ��p���[�ݒ�
		pEnemy->SetNoDamageTime(3);				//���G���Ԑݒ�
		pEnemy->AddLife(-20);					//HP����
		m_DecoveryTime = 10;					//�X�^�����Ԑݒ�
		break;
	case WSWORD_ATTACK_04:
		pEnemy->AddPower(0.8f * StunDir);		//�X�^���ړ��p���[�ݒ�
		pEnemy->SetNoDamageTime(3);				//���G���Ԑݒ�
		pEnemy->AddLife(-20);					//HP����
		m_DecoveryTime = 10;					//�X�^�����Ԑݒ�
		break;
	case CLAYMORE_ATTACK_01:
		pEnemy->AddPower(2.5f * StunDir);		//�X�^���ړ��p���[�ݒ�
		pEnemy->SetNoDamageTime(6);				//���G���Ԑݒ�
		pEnemy->AddLife(-30);					//HP����
		m_DecoveryTime = 20;					//�X�^�����Ԑݒ�
		break;
	case CLAYMORE_ATTACK_02:
		pEnemy->AddPower(2.5f * StunDir);		//�X�^���ړ��p���[�ݒ�
		pEnemy->SetNoDamageTime(6);			//���G���Ԑݒ�
		pEnemy->AddLife(-40);					//HP����
		m_DecoveryTime = 20;					//�X�^�����Ԑݒ�
		break;
	case CLAYMORE_ATTACK_03:
		pEnemy->AddPower(2.5f * StunDir);		//�X�^���ړ��p���[�ݒ�
		pEnemy->SetNoDamageTime(6);			//���G���Ԑݒ�
		pEnemy->AddLife(-50);					//HP����
		m_DecoveryTime = 20;					//�X�^�����Ԑݒ�
		break;
	default:
		break;
	}
}