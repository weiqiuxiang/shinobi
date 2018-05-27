#include "CPlayerChangeWeaponShapePattern.h"
#include "CPlayer.h"

CPlayerChangeWeaponShapePattern::CPlayerChangeWeaponShapePattern(CPlayer* pPlayer, CPlayerPatternManager*pManager)
{
	if (pPlayer != nullptr) {
		//����ɂ��j���[�g�������[�V�����ݒ�
		CPlayer::WEAPON_MODE WeaponMode = pPlayer->GetWeaponMode();
		switch (WeaponMode) {
		case CPlayer::WEAPON_WSWORD:
			pPlayer->SetMotionMode(CPlayer::MOTION_CHANGE_WSWORD_TO_CLAYMORE);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_CHANGE_WSWORD_TO_CLAYMORE);
			break;
		case CPlayer::WEAPON_CLAYMORE:
			pPlayer->SetMotionMode(CPlayer::MOTION_CHANGE_CLAYMORE_TO_WSWORD);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_CHANGE_CLAYMORE_TO_WSWORD);
			break;
		}

		m_MotionNumOld = pPlayer->GetMotionNow();
		pPlayer->SetStateNow(PLAYER_CHANGE_WEAPON_SHAPE_PATTERN);
	}

	m_KeyFrameBefore = 0;

	m_PlayerPatternManager = pManager;
}

bool CPlayerChangeWeaponShapePattern::InitParameter(CPlayer* pPlayer, CPlayerPatternManager* pManager)
{
	if (pPlayer != nullptr) {
		//����ɂ��j���[�g�������[�V�����ݒ�
		CPlayer::WEAPON_MODE WeaponMode = pPlayer->GetWeaponMode();
		switch (WeaponMode) {
		case CPlayer::WEAPON_WSWORD:
			pPlayer->SetMotionMode(CPlayer::MOTION_CHANGE_WSWORD_TO_CLAYMORE);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_CHANGE_WSWORD_TO_CLAYMORE);
			break;
		case CPlayer::WEAPON_CLAYMORE:
			pPlayer->SetMotionMode(CPlayer::MOTION_CHANGE_CLAYMORE_TO_WSWORD);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_CHANGE_CLAYMORE_TO_WSWORD);
			break;
		}

		m_MotionNumOld = pPlayer->GetMotionNow();
		pPlayer->SetStateNow(PLAYER_CHANGE_WEAPON_SHAPE_PATTERN);
	}

	m_PlayerPatternManager = pManager;

	return true;
}

void CPlayerChangeWeaponShapePattern::Update(CPlayer* pPlayer) 
{
	//�k���`�F�b�N
	if (pPlayer == nullptr) {
		return;
	}

	//�p�^�[���ύX
	if (ChangePattern(pPlayer) == true) {
		return;
	}
}

bool CPlayerChangeWeaponShapePattern::ChangePattern(CPlayer* pPlayer)
{
	//�k���`�F�b�N
	if (pPlayer == nullptr) {
		return false;
	}

	if(m_PlayerPatternManager == nullptr){
		return false;
	}

	//�U���󂯂�����X�^���p�^�[���ɓ���
	if (pPlayer->GetIsBeAttackedFlag() == true && pPlayer->GetNoDamageTime() <= 0) {
		CPlayerPattern* pPlayerPattern = m_PlayerPatternManager->GetPattern(PLAYER_STUN_PATTERN);
		if (pPlayerPattern != nullptr) {
			pPlayerPattern->InitParameter(pPlayer, m_PlayerPatternManager);
		}
		pPlayer->DisableWeaponHitCircle(true, true);
		pPlayer->SetState(pPlayerPattern);
	}

	m_nMotionTime--;
	int MotionNow = pPlayer->GetWeaponLeftMotionNow();			//���ݎ��s���̃��[�V�����擾

	//�j���[�g�����p�^�[���ɖ߂�
	if (m_nMotionTime <= 0)
	{
		if (MotionNow == (int)CWeapon::MOTION_DEFAULT) {
			pPlayer->SetWeaponMode(CPlayer::WEAPON_WSWORD);
		}
		else {
			pPlayer->SetWeaponMode(CPlayer::WEAPON_CLAYMORE);
		}

		//�j���[�g�����p�^�[���ɖ߂�
		CPlayerPattern* pPlayerPattern = m_PlayerPatternManager->GetPattern(PLAYER_NEUTRAL_PATTERN);
		if (pPlayerPattern != nullptr) {
			pPlayerPattern->InitParameter(pPlayer, m_PlayerPatternManager);
		}
		pPlayer->SetState(pPlayerPattern);
		return true;
	}

	int KeyFrameNow = pPlayer->GetKeyFrameNow();			//���ݎ��s���̃L�[�t���[���擾
	bool bDiffKeyFrame = m_KeyFrameBefore != KeyFrameNow;	//�O�̃L�[�t���[���ƈقȂ邩

	if (bDiffKeyFrame) {
		m_KeyFrameBefore = KeyFrameNow;					//�i�s���̃L�[�t���[���o�^
		if (KeyFrameNow == 1) {							//�L�[�t���[����1�̎�����ό`�s��
			if (MotionNow == (int)CWeapon::MOTION_DEFAULT) {
				pPlayer->SetWeaponLeftPara(CWeapon::MOTION_CHANGE, D3DXVECTOR3(0.3f, 0.0f, 0.1f), 8);
				pPlayer->SetWeaponRightPara(CWeapon::MOTION_CHANGE, D3DXVECTOR3(0.4f, 0.0f, 0.1f), 8);
			}
			else {
				pPlayer->SetWeaponLeftPara(CWeapon::MOTION_DEFAULT, D3DXVECTOR3(-0.25f, 0.0f, 0.1f), 5);
				pPlayer->SetWeaponRightPara(CWeapon::MOTION_DEFAULT, D3DXVECTOR3(0.25f, 0.0f, 0.1f), 8);
			}
		}
	}

	return false;
}