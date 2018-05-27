#include "CPlayerRollingPattern.h"
#include "CPlayer.h"

//�}�N��
#define DEFAULT_COFFI (0.2f)					//�f�t�H���g�n�ʖ��C�W��
#define SKY_COFFI (0.02f)					//�󒆖��C�W��

CPlayerRollingPattern::CPlayerRollingPattern(CPlayer* pPlayer, CPlayerPatternManager*pManager)
{
	//����ɂ�郂�[�V����������
	if (pPlayer != nullptr) {
		CPlayer::WEAPON_MODE WeaponMode = pPlayer->GetWeaponMode();
		switch (WeaponMode) {
		case CPlayer::WEAPON_WSWORD:
			pPlayer->SetMotionMode(CPlayer::MOTION_WSWORD_ROLLING);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_WSWORD_ROLLING);
			break;
		case CPlayer::WEAPON_CLAYMORE:
			pPlayer->SetMotionMode(CPlayer::MOTION_CLAYMORE_ROLLING);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_CLAYMORE_ROLLING);
			break;
		}

		m_MotionNumOld = pPlayer->GetMotionNow();
		pPlayer->SetStateNow(PLAYER_ROLLING_PATTERN);
	}

	m_PlayerPatternManager = pManager;
}

bool CPlayerRollingPattern::InitParameter(CPlayer* pPlayer, CPlayerPatternManager* pManager)
{
	//����ɂ�郂�[�V����������
	if (pPlayer != nullptr) {
		CPlayer::WEAPON_MODE WeaponMode = pPlayer->GetWeaponMode();
		switch (WeaponMode) {
		case CPlayer::WEAPON_WSWORD:
			pPlayer->SetMotionMode(CPlayer::MOTION_WSWORD_ROLLING);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_WSWORD_ROLLING);
			break;
		case CPlayer::WEAPON_CLAYMORE:
			pPlayer->SetMotionMode(CPlayer::MOTION_CLAYMORE_ROLLING);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_CLAYMORE_ROLLING);
			break;
		}

		m_MotionNumOld = pPlayer->GetMotionNow();
		pPlayer->SetStateNow(PLAYER_ROLLING_PATTERN);
	}

	m_PlayerPatternManager = pManager;

	return true;
}

void CPlayerRollingPattern::Update(CPlayer* pPlayer)
{
	//�k���`�F�b�N
	if (pPlayer == nullptr) {
		return;
	}

	if (ChangePattern(pPlayer) == true) {
		return;
	}
}

bool CPlayerRollingPattern::ChangePattern(CPlayer* pPlayer)
{
	//�k���`�F�b�N
	if (pPlayer == nullptr) {
		return false;
	}
	if (m_PlayerPatternManager == nullptr) {
		return false;
	}

	m_nMotionTime--;

	//�j���[�g�����p�^�[���ɑJ��
	if (m_nMotionTime < 0){
		CPlayerPattern* pPlayerPattern = m_PlayerPatternManager->GetPattern(PLAYER_NEUTRAL_PATTERN);
		if (pPlayerPattern != nullptr) {
			pPlayerPattern->InitParameter(pPlayer, m_PlayerPatternManager);
		}
		pPlayer->SetState(pPlayerPattern);

		return true;
	}

	int KeyFrameNow = pPlayer->GetKeyFrameNow();			//���ݎ��s���̃L�[�t���[���擾
	bool bDiffKeyFrame = m_KeyFrameBefore != KeyFrameNow;	//�O�̃L�[�t���[���ƈقȂ邩

	//�U���󂯂�����X�^���p�^�[���ɓ���
	if (pPlayer->GetIsBeAttackedFlag() == true && KeyFrameNow >= 6 && pPlayer->GetNoDamageTime() <= 0) {
		CPlayerPattern* pPlayerPattern = m_PlayerPatternManager->GetPattern(PLAYER_STUN_PATTERN);
		if (pPlayerPattern != nullptr) {
			pPlayerPattern->InitParameter(pPlayer, m_PlayerPatternManager);
		}
		pPlayer->DisableWeaponHitCircle(true, true);
		pPlayer->SetState(pPlayerPattern);
	}

	//�قȂ�Ȃ�
	if (bDiffKeyFrame) {
		m_KeyFrameBefore = KeyFrameNow;					//�i�s���̃L�[�t���[���o�^
		if (KeyFrameNow == 4) {
			pPlayer->AddPower(pPlayer->GetFaceDir()*0.1f);
		}
		else if (KeyFrameNow == 5) {
			pPlayer->AddPower(pPlayer->GetFaceDir()*0.1f);
		}
		else if (KeyFrameNow == 6) {
			pPlayer->AddPower(pPlayer->GetFaceDir()*0.1f);
		}
		else if (KeyFrameNow == 7) {
			pPlayer->AddPower(pPlayer->GetFaceDir()*0.1f);
		}
	}

	//���C�W���ω�
	if (KeyFrameNow < 4) {
		pPlayer->SetCoffi(SKY_COFFI);
	}
	else {
		pPlayer->SetCoffi(DEFAULT_COFFI);
	}

	return false;
}