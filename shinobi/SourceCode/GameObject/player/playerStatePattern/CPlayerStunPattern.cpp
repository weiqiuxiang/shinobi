#include "CPlayerStunPattern.h"
#include "CPlayer.h"
#include "CPlayerPatternManager.h"
#include "CPlayerControl.h"
#include "common.h"

#define STUN_COFFI (0.1f)	//�X�^�����̒�R�W��

CPlayerStunPattern::CPlayerStunPattern(CPlayer* pPlayer, CPlayerPatternManager* pPlayerManager)
{
	m_PlayerPatternManager = pPlayerManager;
	m_StunMotionTime = 0;
	m_DecoveryTime = 0;

	//�X�^���p�����[�^�ݒ�
	StunSetting(pPlayer);

	//���[�V�����ݒ�
	if (pPlayer != nullptr) {
		pPlayer->DisableWeaponHitCircle(true,true);
		pPlayer->SetCoffi(STUN_COFFI);

		CPlayer::WEAPON_MODE WeaponMode = pPlayer->GetWeaponMode();
		switch (WeaponMode) {
		case CPlayer::WEAPON_WSWORD:
			pPlayer->SetMotionMode(CPlayer::MOTION_WSWORD_STUN);
			m_StunMotionTime = pPlayer->GetMotionTotalFrame(CPlayer::MOTION_WSWORD_STUN);
			break;

		case CPlayer::WEAPON_CLAYMORE:
			pPlayer->SetMotionMode(CPlayer::MOTION_CLAYMORE_STUN);
			m_StunMotionTime = pPlayer->GetMotionTotalFrame(CPlayer::MOTION_CLAYMORE_STUN);
			break;
		}

		pPlayer->SetStateNow(PLAYER_STUN_PATTERN);
	}
}

CPlayerStunPattern::~CPlayerStunPattern()
{
	
}

bool CPlayerStunPattern::InitParameter(CPlayer* pPlayer, CPlayerPatternManager* pPlayerManager)
{
	m_PlayerPatternManager = pPlayerManager;
	m_StunMotionTime = 0;
	m_DecoveryTime = 0;

	//�X�^���p�����[�^�ݒ�
	StunSetting(pPlayer);
	
	//���[�V�����ݒ�
	if (pPlayer != nullptr) {
		pPlayer->DisableWeaponHitCircle(true, true);
		pPlayer->SetCoffi(STUN_COFFI);

		CPlayer::WEAPON_MODE WeaponMode = pPlayer->GetWeaponMode();
		switch (WeaponMode) {
		case CPlayer::WEAPON_WSWORD:
			pPlayer->SetMotionMode(CPlayer::MOTION_WSWORD_STUN);
			m_StunMotionTime = pPlayer->GetMotionTotalFrame(CPlayer::MOTION_WSWORD_STUN);
			break;

		case CPlayer::WEAPON_CLAYMORE:
			pPlayer->SetMotionMode(CPlayer::MOTION_CLAYMORE_STUN);
			m_StunMotionTime = pPlayer->GetMotionTotalFrame(CPlayer::MOTION_CLAYMORE_STUN);
			break;
		}

		pPlayer->SetStateNow(PLAYER_STUN_PATTERN);
	}

	return true;
}

void CPlayerStunPattern::Update(CPlayer* pPlayer)
{
	//�k���`�F�b�N
	if(pPlayer == nullptr){
		return;
	}
	if (m_PlayerPatternManager == nullptr) {
		return;
	}
	
	//��O����
	if (pPlayer->GetDamageType() == ENEMY_ATTACK_TYPE_NONE) {
		CPlayerPattern *pPattern = m_PlayerPatternManager->GetPattern(PLAYER_NEUTRAL_PATTERN);
		if (pPattern != nullptr) {
			pPattern->InitParameter(pPlayer, m_PlayerPatternManager);
		}
		pPlayer->SetState(pPattern);
		return;
	}

	if (pPlayer->GetPlayerLife() <= 0) {
		pPlayer->SetIsBeAttackedFlag(false);
		CPlayerPattern *pPattern = m_PlayerPatternManager->GetPattern(PLAYER_DEAD_PATTERN);
		if (pPattern != nullptr) {
			pPattern->InitParameter(pPlayer, m_PlayerPatternManager);
		}
		pPlayer->SetState(pPattern);
		return;
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
		pPlayer->SetCoffi(0.2f);
		pPlayer->SetIsBeAttackedFlag(false);
		pPlayer->SetState(m_PlayerPatternManager->GetPattern(PLAYER_NEUTRAL_PATTERN));
		return;
	}

	if (pPlayer->GetIsBeAttackedFlag() == true) {
		if (pPlayer->GetNoDamageTime() <= 0) {
			pPlayer->SetIsBeAttackedFlag(false);
			pPlayer->SetNoDamageTime(3);
			CPlayerPattern *pPattern = m_PlayerPatternManager->GetPattern(PLAYER_STUN_PATTERN);
			if (pPattern != nullptr) {
				pPattern->InitParameter(pPlayer,m_PlayerPatternManager);
			}
			pPlayer->SetState(pPattern);
			return;
		}

		if (pPlayer->GetPlayerLife() <= 0) {
			pPlayer->SetIsBeAttackedFlag(false);
			CPlayerPattern *pPattern = m_PlayerPatternManager->GetPattern(PLAYER_DEAD_PATTERN);
			if (pPattern != nullptr) {
				pPattern->InitParameter(pPlayer, m_PlayerPatternManager);
			}
			pPlayer->SetState(pPattern);
			return;
		}
	}
}

void CPlayerStunPattern::Draw(void)
{
	
}

void CPlayerStunPattern::StunSetting(CPlayer* pPlayer)
{
	//�k���`�F�b�N
	if (pPlayer == nullptr) {
		return;
	}

	//�X�^�������擾
	D3DXVECTOR3 StunDir = pPlayer->GetStunDir();

	//�v���[���[�̍U���^�C�v�ɉ����ă_���[�W���󂯁A��苗���ɔ�΂����
	switch (pPlayer->GetDamageType())
	{
	case PLAYER_ATTACK_TYPE_NONE:
		break;
	case DOG_BITE_ATTACK:
		pPlayer->AddPower(0.4f * StunDir);		//�X�^���ړ��p���[�ݒ�
		pPlayer->SetNoDamageTime(60);			//���G���Ԑݒ�
		pPlayer->AddLife(-200);					//HP����
		m_DecoveryTime = 10;					//�X�^�����Ԑݒ�
		break;
	case DOG_JUMP_ATTACK:
		pPlayer->AddPower(0.6f * StunDir);		//�X�^���ړ��p���[�ݒ�
		pPlayer->SetNoDamageTime(120);			//���G���Ԑݒ�
		pPlayer->AddLife(-300);					//HP����
		m_DecoveryTime = 20;					//�X�^�����Ԑݒ�
		break;
	default:
		break;
	}
}