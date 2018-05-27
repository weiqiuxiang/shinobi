#include "CPlayerStepPattern.h"
#include "CPlayer.h"
#include "SceneTransition.h"

//�}�N��
#define DEFAULT_COFFI (0.2f)					//�f�t�H���g�n�ʖ��C�W��
#define SKY_COFFI (0.02f)					//�󒆖��C�W��

CPlayerStepPattern::CPlayerStepPattern(CPlayer* pPlayer, CPlayerPatternManager* pManager)
{
	m_nMotionTime = 0;
	MotionSet(pPlayer);

	m_PlayerPatternManager = pManager;
}

bool CPlayerStepPattern::InitParameter(CPlayer* pPlayer, CPlayerPatternManager* pManager)
{
	m_nMotionTime = 0;
	MotionSet(pPlayer);

	m_PlayerPatternManager = pManager;

	return true;
}

void CPlayerStepPattern::Update(CPlayer* pPlayer)
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

void CPlayerStepPattern::MotionSet(CPlayer* pPlayer)
{
	//�k���`�F�b�N
	if (pPlayer == nullptr) {
		return;
	}

	pPlayer->SetStateNow(PLAYER_STEP_PATTERN);

	//�X�e�b�v�^�C�v�����߂�
	typedef enum {
		STEP_FORWORD = 0,
		STEP_BACK,
		STEP_LEFT,
		STEP_RIGHT
	}STEP_MOTION_TYPE;
	STEP_MOTION_TYPE StepType = STEP_FORWORD;

	D3DXVECTOR2 AnalogInputValue = CSceneTransition::GetXInput()->GetLeftThumbStickValueAnalog();

	float StickValueX = fabs(AnalogInputValue.x);
	float StickValueY = fabs(AnalogInputValue.y);
	if (StickValueX > StickValueY) {
		if (AnalogInputValue.x < 0) {
			StepType = STEP_LEFT;
		}
		else {
			StepType = STEP_RIGHT;
		}
	}
	else {
		if (AnalogInputValue.y > 0) {
			StepType = STEP_FORWORD;
		}
		else {
			StepType = STEP_BACK;
		}
	}
	if (D3DXVec2Length(&AnalogInputValue) < 0.1f) {
		StepType = STEP_BACK;
	}

	//�X�e�b�v���[�V�����ύX
	switch (pPlayer->GetWeaponMode())
	{
	case CPlayer::WEAPON_WSWORD:
		switch (StepType) {
		case STEP_FORWORD:
			pPlayer->SetMotionMode(CPlayer::MOTION_WSWORD_STEP_FORWARD);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_WSWORD_STEP_FORWARD);
			break;
		case STEP_BACK:
			pPlayer->SetMotionMode(CPlayer::MOTION_WSWORD_STEP_BACK);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_WSWORD_STEP_BACK);
			break;
		case STEP_LEFT:
			pPlayer->SetMotionMode(CPlayer::MOTION_WSWORD_STEP_LEFT);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_WSWORD_STEP_LEFT);
			break;
		case STEP_RIGHT:
			pPlayer->SetMotionMode(CPlayer::MOTION_WSWORD_STEP_RIGHT);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_WSWORD_STEP_RIGHT);
			break;
		}
		break;
	case CPlayer::WEAPON_CLAYMORE:
		switch (StepType) {
		case STEP_FORWORD:
			pPlayer->SetMotionMode(CPlayer::MOTION_CLAYMORE_STEP_FORWARD);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_CLAYMORE_STEP_FORWARD);
			break;
		case STEP_BACK:
			pPlayer->SetMotionMode(CPlayer::MOTION_CLAYMORE_STEP_BACK);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_CLAYMORE_STEP_BACK);
			break;
		case STEP_LEFT:
			pPlayer->SetMotionMode(CPlayer::MOTION_CLAYMORE_STEP_LEFT);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_CLAYMORE_STEP_LEFT);
			break;
		case STEP_RIGHT:
			pPlayer->SetMotionMode(CPlayer::MOTION_CLAYMORE_STEP_RIGHT);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_CLAYMORE_STEP_RIGHT);
			break;
		}
		break;
	}
}

bool CPlayerStepPattern::ChangePattern(CPlayer* pPlayer)
{
	if(pPlayer == nullptr)
	{
		return false;
	}
	if (m_PlayerPatternManager == nullptr) {
		return false;
	}

	m_nMotionTime--;				//���[�V�������Ԍ���

	//�j���[�g�����p�^�[���ɑJ��
	if (m_nMotionTime < 0) {
		CPlayerPattern* pPlayerPattern = m_PlayerPatternManager->GetPattern(PLAYER_NEUTRAL_PATTERN);
		if (pPlayerPattern != nullptr) {
			pPlayerPattern->InitParameter(pPlayer, m_PlayerPatternManager);
		}
		pPlayer->SetState(pPlayerPattern);

		return true;
	}

	int KeyFrameNow = pPlayer->GetKeyFrameNow();		//���ݎ��s���̃L�[�t���[���擾

	//�U���󂯂�����X�^���p�^�[���ɓ���
	if (pPlayer->GetIsBeAttackedFlag() == true && KeyFrameNow >= 3 && pPlayer->GetNoDamageTime() <= 0) {
		CPlayerPattern* pPlayerPattern = m_PlayerPatternManager->GetPattern(PLAYER_STUN_PATTERN);
		if (pPlayerPattern != nullptr) {
			pPlayerPattern->InitParameter(pPlayer, m_PlayerPatternManager);
		}
		pPlayer->DisableWeaponHitCircle(true, true);
		pPlayer->SetState(pPlayerPattern);
	}

	//���C�W���ω�
	int CoffiChangeKeyFrame = pPlayer->GetMotionTotalKeyFrame(pPlayer->GetMotionNow()) - 4;
	if (KeyFrameNow <= CoffiChangeKeyFrame) pPlayer->SetCoffi(SKY_COFFI);					//���C�W���ύX�̃L�[�t���[����菬����
	else {
		PlayerGoalDir(pPlayer);						//�v���[���[�̃S�[�������ݒ�
		PlayerFaceDir(pPlayer);						//�v���[���[�̌����ݒ�
		pPlayer->SetCoffi(DEFAULT_COFFI);
	}

	return false;
}