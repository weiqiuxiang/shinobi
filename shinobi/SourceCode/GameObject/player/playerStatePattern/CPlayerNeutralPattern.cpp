#include "CPlayer.h"
#include "CPlayerNeutralPattern.h"
#include "SceneTransition.h"
#include "CCameraManager.h"
#include "CPolygonMath.h"

//�}�N��
#define STICK_WALK_ZONE (0.5f)		//�X�e�B�b�N��������̃]�[��
#define STICK_RUN_ZONE (0.8f)		//�X�e�B�b�N���铮��̃]�[��
#define STAMINA_RECOVERY (5)		//�X�^�~�i�񕜒l
#define STAMINA_DOWN_WHEN_RUN (-5)			//�X�^�~�i�����l

#define WALK_POWER (0.02f)			//�ړ����̕�����
#define RUN_POWER (0.05f)			//�ړ����̑����
#define DASH_POWER (0.08f)			//�ړ����̃_�b�V���̗�
#define ROLLING_POWER (0.6f)		//���[�����O�̗�
#define ROLLING_POWERY (0.4f)		//���[�����O���̃W�����v��
#define STEP_POWER (0.6f)			//�X�e�b�v�̗�

#define RELEASE_BUTTON_TO_ROLLING_FRAME (10)	//���[�����O/�X�e�b�v�A�N�V�����̃{�^���������莞��(�����t���[�����͂��̃}�N����菬�����ꍇ�A�A�N�V��������)

CPlayerNeutralPattern::CPlayerNeutralPattern()
{
	m_StickValue = D3DXVECTOR2(0.0f, 0.0f);
	m_IsRun = false;
	m_MotionNumOld = CPlayer::MOTION_WSWORD_NEUTRAL;
}

CPlayerNeutralPattern::CPlayerNeutralPattern(CPlayer* pPlayer, CPlayerPatternManager* pManager)
{
	m_StickValue = D3DXVECTOR2(0.0f, 0.0f);
	m_IsRun = false;

	if (pPlayer != nullptr) {
		//����ɂ�郂�[�V�����ݒ�
		CPlayer::WEAPON_MODE WeaponMode = pPlayer->GetWeaponMode();
		switch (WeaponMode) {
		case CPlayer::WEAPON_WSWORD:
			pPlayer->SetMotionMode(CPlayer::MOTION_WSWORD_NEUTRAL);
			break;
		case CPlayer::WEAPON_CLAYMORE:
			pPlayer->SetMotionMode(CPlayer::MOTION_CLAYMORE_NEUTRAL);
			break;
		}

		m_MotionNumOld = pPlayer->GetMotionNow();
		pPlayer->SetStateNow(PLAYER_NEUTRAL_PATTERN);
	}

	m_PlayerPatternManager = pManager;
	m_PushRunButtonTime = 0;
}

bool CPlayerNeutralPattern::InitParameter(CPlayer* pPlayer , CPlayerPatternManager*pManager)
{
	m_StickValue = D3DXVECTOR2(0.0f, 0.0f);
	m_IsRun = false;

	if (pPlayer != nullptr) {
		//����ɂ��j���[�g�������[�V�����ݒ�
		CPlayer::WEAPON_MODE WeaponMode = pPlayer->GetWeaponMode();
		switch (WeaponMode) {
		case CPlayer::WEAPON_WSWORD:
			pPlayer->SetMotionMode(CPlayer::MOTION_WSWORD_NEUTRAL);
			break;
		case CPlayer::WEAPON_CLAYMORE:
			pPlayer->SetMotionMode(CPlayer::MOTION_CLAYMORE_NEUTRAL);
			break;
		}

		m_MotionNumOld = pPlayer->GetMotionNow();
		pPlayer->SetStateNow(PLAYER_NEUTRAL_PATTERN);
	}

	m_PlayerPatternManager = pManager;
	m_PushRunButtonTime = 0;

	return true;
}

void CPlayerNeutralPattern::Update(CPlayer* pPlayer)
{
	//�k���`�F�b�N
	if (pPlayer == nullptr) {
		return;
	}

	//���͂��ꂽ�X�e�B�b�N�̒l�v�Z
	m_StickValue = D3DXVECTOR2(0.0f, 0.0f);
	bool IsInputFromStick = false;
	IsInputFromStick = CalcStickValueFromKeyboard();
	if (!IsInputFromStick) {
		IsInputFromStick = CalcStickValueFromController();
	}

	//�����Ă��邩
	m_IsRun = IsRun();

	//�̗͂̑���
	if (m_IsRun) {
		pPlayer->AddStamina(STAMINA_DOWN_WHEN_RUN);
	}
	else {
		pPlayer->AddStamina(STAMINA_RECOVERY);
	}

	//�ړ�
	float MovePower = CalcMovePower();
	if (pPlayer->GetLockOnFlag()) {
		MoveLockOn(pPlayer,MovePower);
	}
	else {
		MoveNoLockOn(pPlayer, MovePower);
	}

	//�ʂ̌������v�Z
	if (pPlayer->GetLockOnFlag()) {
		CalcFaceGoalDirLockOn(pPlayer);
	}
	else {
		CalcFaceGoalDirNoLockOn(pPlayer);
	}
	CalcFaceDir(pPlayer);

	//���[�V�����ݒ�
	ChangeMotion(pPlayer,MovePower);

	//�p�^�[���ύX
	if (ChangePattern(pPlayer) == true) {
		return;
	}

	//����{�^�����������ςȂ��̎��Ԃ�ݐ�
	PushRunButtonTime();
}

bool CPlayerNeutralPattern::CalcStickValueFromKeyboard(void)
{
	bool PressW = CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_W);
	bool PressA = CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_A);
	bool PressS = CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_S);
	bool PressD = CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_D);

	//�R���g���[���X�e�B�b�N�����̎Z�o
	if (PressW) m_StickValue = D3DXVECTOR2(0.0f, 1.0f);
	if (PressA) m_StickValue = D3DXVECTOR2(-1.0f, 0.0f);
	if (PressS) m_StickValue = D3DXVECTOR2(0.0f, -1.0f);
	if (PressD) m_StickValue = D3DXVECTOR2(1.0f, 0.0f);
	if (PressW && PressA) m_StickValue = D3DXVECTOR2(-0.7071f, 0.7071f);
	if (PressW && PressD) m_StickValue = D3DXVECTOR2(0.7071f, 0.7071f);
	if (PressS && PressA) m_StickValue = D3DXVECTOR2(-0.7071f, -0.7071f);
	if (PressS && PressD) m_StickValue = D3DXVECTOR2(0.7071f, -0.7071f);

	//�{�^�������ꂽ�t���O
	bool bBottonPush = false;
	if (PressW || PressA || PressS || PressD) bBottonPush = true;

	return bBottonPush;
}

bool CPlayerNeutralPattern::CalcStickValueFromController(void)
{
	D3DXVECTOR2 AnalogInputValue = CSceneTransition::GetXInput()->GetLeftThumbStickValueAnalog();
	m_StickValue = AnalogInputValue;

	bool bBottonPush = false;
	if (D3DXVec2LengthSq(&AnalogInputValue) > 0.0f) {
		bBottonPush = true;
	}

	return bBottonPush;
}

bool CPlayerNeutralPattern::IsRun(void)
{
	bool bKeyboardRun = CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_SPACE);
	bool bControllerRun = CSceneTransition::GetXInput()->GetHoldButton(XINPUT_GAMEPAD_A);
	if (bKeyboardRun || bControllerRun) {
		return true;
	}
	return false;
}

//�߂�l : �ړ���
float CPlayerNeutralPattern::CalcMovePower(void)
{
	float MovePower = 0;

	float PushStickValue = D3DXVec2Length(&m_StickValue);
	if (PushStickValue < STICK_WALK_ZONE) {
		MovePower = 0;
	}
	else if (PushStickValue < STICK_RUN_ZONE) {
		MovePower = WALK_POWER;
	}
	else {
		MovePower = RUN_POWER;
	}

	if (PushStickValue >= STICK_WALK_ZONE && m_IsRun == true) {
		MovePower = DASH_POWER;
	}

	return MovePower;
}

//
void CPlayerNeutralPattern::PushRunButtonTime(void)
{
	bool IsPushKeyboardRunButton = CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_SPACE);
	bool IsPushControllerRunButton = CSceneTransition::GetXInput()->GetHoldButton(XINPUT_GAMEPAD_A);
	if (IsPushKeyboardRunButton || IsPushControllerRunButton) {
		m_PushRunButtonTime++;
		m_PushRunButtonTime = min(10000, m_PushRunButtonTime);	//�I�[�o�[�t���[�h�~
	}
	else {
		m_PushRunButtonTime = 0;
	}
}

void CPlayerNeutralPattern::MoveNoLockOn(CPlayer* pPlayer, float MovePower)
{
	//�k���`�F�b�N
	if (pPlayer == nullptr) {
		return;
	}

	//�J�����̐��������̌������擾
	CAMERA_PARA CameraPara = CCameraManager::GetCameraPara(CCameraManager::CAMERA_PLAYER);

	//�J�����̌����Ă�XZ���ʂ̕����x�N�g�����Z�o
	D3DXVECTOR3 CameraLookatVec = CameraPara.lookat - CameraPara.pos;
	CameraLookatVec.y = 0;
	D3DXVec3Normalize(&CameraLookatVec, &CameraLookatVec);

	float StickXPercent = 0.0f;
	float StickYPercent = 0.0f;
	if (D3DXVec2Length(&m_StickValue) != 0.0f) {
		StickXPercent = m_StickValue.x / D3DXVec2Length(&m_StickValue);
		StickYPercent = m_StickValue.y / D3DXVec2Length(&m_StickValue);
	}

	//�v���[���[�ɗ^�����O�͌v�Z(�X�e�B�b�N�c�|��)
	pPlayer->AddPower(CameraLookatVec * MovePower * StickYPercent);

	//�v���[���[�ɗ^�����O�͌v�Z(�X�e�B�b�N���|��)
	D3DXVECTOR3 PlayerSpeed = pPlayer->GetSpeed();							//�v���[���[���x�擾
	D3DXVECTOR3 CameraPlayerVec = pPlayer->GetPos() - CameraPara.pos;
	CameraPlayerVec.y = 0;													//Y�����J�b�g
	float SpeedValue = D3DXVec3Length(&PlayerSpeed);
	if (0 == SpeedValue) SpeedValue = 0.1f;				//��O���� : ���x��0�̎�
	float Distance = D3DXVec3Length(&CameraPlayerVec);
	float TopAngle = atan2f(SpeedValue * 0.5f, Distance) * 2;
	D3DXMATRIX MtxRot;
	D3DXMatrixRotationY(&MtxRot, TopAngle);
	D3DXVECTOR3 CameraPlayerNextVec;					//����XZ���ʂ�EnemyPlayer�x�N�g��
	D3DXVec3TransformCoord(&CameraPlayerNextVec, &CameraPlayerVec, &MtxRot);
	D3DXVECTOR3 NextPlayerPos = CameraPlayerNextVec + CameraPara.pos;
	D3DXVECTOR3 MoveDir = NextPlayerPos - pPlayer->GetPos();
	MoveDir.y = 0;									//Y�������J�b�g
													//��O���� : �����x�N�g���̒l��0�̎�
	if (D3DXVECTOR3(0.0f, 0.0f, 0.0f) == MoveDir) {
		D3DXVec3Cross(&MoveDir, &D3DXVECTOR3(0.0f, 1.0f, 1.0f), &CameraPlayerVec);
	}
	D3DXVec3Normalize(&MoveDir, &MoveDir);					//���K��
	pPlayer->AddPower(MoveDir * MovePower * StickXPercent);  //�O�͂�^����
}

void CPlayerNeutralPattern::MoveLockOn(CPlayer* pPlayer, float MovePower)
{
	//�k���`�F�b�N
	if (pPlayer == nullptr) {
		return;
	}

	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();
	D3DXVECTOR3 EnemyPos = pPlayer->GetLockOnPosition();
	D3DXVECTOR3 EnemyPlayerVec = PlayerPos - EnemyPos;
	D3DXVECTOR3 EnemyPlayerXZVec = D3DXVECTOR3(EnemyPlayerVec.x, 0.0f, EnemyPlayerVec.z);
	D3DXVECTOR3 EnemyPlayerXZVecNor;
	D3DXVec3Normalize(&EnemyPlayerXZVecNor, &EnemyPlayerXZVec);				//XZ���ʂ̕����x�N�g���Z�o

	//�v���[���[�ɗ^�����O�͌v�Z(�X�e�B�b�N�c�|��)
	float StickXPercent = 0.0f;
	float StickYPercent = 0.0f;
	if (D3DXVec2Length(&m_StickValue) != 0.0f) {
		StickXPercent = m_StickValue.x / D3DXVec2Length(&m_StickValue);
		StickYPercent = m_StickValue.y / D3DXVec2Length(&m_StickValue);
	}

	pPlayer->AddPower(StickYPercent * MovePower * (-EnemyPlayerXZVecNor));	//�X�e�B�b�N��Y�����̊O��

	//�v���[���[�ɗ^�����O�͌v�Z(�X�e�B�b�N���|��)
	D3DXVECTOR3 PlayerSpeed = pPlayer->GetSpeed();
	float SpeedValue = D3DXVec3Length(&PlayerSpeed);
	if (0 == SpeedValue) SpeedValue = 0.1f;
	float Distance = D3DXVec3Length(&EnemyPlayerXZVec);
	float TopAngle = atan2f(SpeedValue * 0.5f, Distance) * 2;
	D3DXMATRIX MtxRot;
	D3DXMatrixRotationY(&MtxRot, TopAngle);
	D3DXVECTOR3 EnemyPlayerNextVec;					//����XZ���ʂ�EnemyPlayer�x�N�g��
	D3DXVec3TransformCoord(&EnemyPlayerNextVec, &EnemyPlayerVec, &MtxRot);
	D3DXVECTOR3 NextPlayerPos = EnemyPlayerNextVec + EnemyPos;
	D3DXVECTOR3 MoveDir = NextPlayerPos - PlayerPos;
	MoveDir.y = 0;									//Y�������J�b�g

	//��O���� : �����x�N�g���̒l��0�̎�
	if (D3DXVECTOR3(0.0f, 0.0f, 0.0f) == MoveDir) {
		D3DXVec3Cross(&MoveDir, &D3DXVECTOR3(0.0f, 1.0f, 1.0f), &EnemyPlayerNextVec);
	}
	D3DXVec3Normalize(&MoveDir, &MoveDir);

	pPlayer->AddPower(MoveDir * MovePower * -StickXPercent);					//�O�͂�^����
}

void CPlayerNeutralPattern::CalcFaceGoalDirNoLockOn(CPlayer* pPlayer)
{
	//�k���`�F�b�N
	if (pPlayer == nullptr) {
		return;
	}

	if (D3DXVec2Length(&m_StickValue) < 0.2f) {
		return;
	}

	CAMERA_PARA CameraPara = CCameraManager::GetCameraPara(CCameraManager::CAMERA_PLAYER);       //�J�����̐��������̌������擾

	//�J�����̌����Ă�XZ���ʂ̕����x�N�g�����Z�o
	D3DXVECTOR3 CameraLookatVec = CameraPara.lookat - CameraPara.pos;
	CameraLookatVec.y = 0;

	D3DXVec3Normalize(&CameraLookatVec, &CameraLookatVec);

	//�v���[���[�̌����Z�o
	float AngleDir = CPolygonMath::Vec2AngleAndAngleDir(D3DXVECTOR2(0.0f, 1.0f), m_StickValue);		//�R���g���[���̐����p�x���Z�o
	D3DXMATRIX MtxRot;
	D3DXMatrixRotationY(&MtxRot, -AngleDir);
	D3DXVECTOR3 PlayerDir;
	D3DXVec3TransformCoord(&PlayerDir, &CameraLookatVec, &MtxRot);
	D3DXVec3Normalize(&PlayerDir, &PlayerDir);
	pPlayer->SetGoalDir(PlayerDir);
}

void CPlayerNeutralPattern::CalcFaceGoalDirLockOn(CPlayer* pPlayer)
{
	//�k���`�F�b�N
	if (pPlayer == nullptr) {
		return;
	}

	D3DXVECTOR3 PlayerPos = pPlayer->GetPos();
	D3DXVECTOR3 EnemyPos = pPlayer->GetLockOnPosition();
	D3DXVECTOR3 PlayerEnemyVec = EnemyPos - PlayerPos;
	D3DXVec3Normalize(&PlayerEnemyVec, &PlayerEnemyVec);
	pPlayer->SetGoalDir(PlayerEnemyVec);
}

void CPlayerNeutralPattern::CalcFaceDir(CPlayer* pPlayer)
{
	//�k���`�F�b�N
	if (pPlayer == nullptr) {
		return;
	}

	//�v���[���[�����x�N�g���Ńv���[���[�̎p�����v�Z
	D3DXMATRIX PoseMtx;
	D3DXVECTOR3 X, Y, Z;
	Y = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 PlayerDir = pPlayer->GetFaceDir();
	if (D3DXVECTOR3(0.0f, 0.0f, 0.0f) == PlayerDir) return;
	D3DXVec3Normalize(&Z, &PlayerDir);
	D3DXVec3Cross(&X, &Y, &Z);
	D3DXVec3Normalize(&X, &X);

	PoseMtx._11 = X.x; PoseMtx._12 = X.y; PoseMtx._13 = X.z; PoseMtx._14 = 0.0f;
	PoseMtx._21 = Y.x; PoseMtx._22 = Y.y; PoseMtx._23 = Y.z; PoseMtx._24 = 0.0f;
	PoseMtx._31 = Z.x; PoseMtx._32 = Z.y; PoseMtx._33 = Z.z; PoseMtx._34 = 0.0f;
	PoseMtx._41 = 0.0f; PoseMtx._42 = 0.0f; PoseMtx._43 = 0.0f; PoseMtx._44 = 1.0f;

	pPlayer->SetPoseMtx(PoseMtx);
}

//���[�V�����̕ω�
void CPlayerNeutralPattern::ChangeMotion(CPlayer* pPlayer, float MovePower)
{
	//�k���`�F�b�N
	if (pPlayer == nullptr) {
		return;
	}

	//���݂̃��[�V������ۑ�����
	m_MotionNumOld = pPlayer->GetMotionNow();

	//����󋵎擾
	CPlayer::WEAPON_MODE WeaponMode = pPlayer->GetWeaponMode();

	//�󋵂ɉ����ă��[�V�����ύX
	//0�̓j���[�g�����A1�͕����A2�͑���A3�̓_�b�V��
	int MoveModeNum = 0;
	if (MovePower == WALK_POWER) {
		MoveModeNum = 1;
	}
	if (MovePower == RUN_POWER) {
		MoveModeNum = 2;
	}
	if (MovePower == DASH_POWER) {
		MoveModeNum = 3;
	}

	int MotionNum = 0;
	switch(WeaponMode){
	case CPlayer::WEAPON_WSWORD:
		if (MoveModeNum == 0) {
			MotionNum = (int)CPlayer::MOTION_WSWORD_NEUTRAL;
		}
		else if (MoveModeNum == 1) {
			MotionNum = (int)CPlayer::MOTION_WSWORD_WALK;
		}
		else if (MoveModeNum == 2) {
			MotionNum = (int)CPlayer::MOTION_WSWORD_RUN;
		}
		else if (MoveModeNum == 3) {
			MotionNum = (int)CPlayer::MOTION_WSWORD_DASH;
		}
		break;
	case CPlayer::WEAPON_CLAYMORE:
		if (MoveModeNum == 0) {
			MotionNum = (int)CPlayer::MOTION_CLAYMORE_NEUTRAL;
		}
		else if (MoveModeNum == 1) {
			MotionNum = (int)CPlayer::MOTION_CLAYMORE_WALK;
		}
		else if (MoveModeNum == 2) {
			MotionNum = (int)CPlayer::MOTION_CLAYMORE_RUN;
		}
		else if (MoveModeNum == 3) {
			MotionNum = (int)CPlayer::MOTION_CLAYMORE_DASH;
		}
		break;
	}

	//�O�̃��[�V�����ƕω��͂����
	if (m_MotionNumOld != MotionNum) {
		pPlayer->SetMotionMode(MotionNum);
	}
}

//�ʂ̃p�^�[���ɑJ�ڂ��锻��
bool CPlayerNeutralPattern::ChangePattern(CPlayer* pPlayer)
{
	//�k���`�F�b�N
	if (pPlayer == nullptr) {
		return false;
	}
	if (m_PlayerPatternManager == nullptr) {
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

	//�U���p�^�[���ɑJ��
	if (ChangePatternToAttack(pPlayer) == true) {
		return true;
	}

	//���[�����O / �X�e�b�v�p�^�[���ɑJ��
	if (pPlayer->GetLockOnFlag() == false) {
		if (ChangePatternToRolling(pPlayer) == true) {
			return true;
		}
	}
	else {
		if (ChangePatternToStep(pPlayer) == true) {
			return true;
		}
	}

	//����ό`�p�^�[���ɑJ��
	if (ChangePatternToChangeWeaponShape(pPlayer) == true) {
		return true;
	}

	return false;
}

//�U���p�^�[���ɕύX
bool CPlayerNeutralPattern::ChangePatternToAttack(CPlayer* pPlayer)
{
	//�k���`�F�b�N
	if (pPlayer == nullptr) {
		return false;
	}
	if (m_PlayerPatternManager == nullptr) {
		return false;
	}

	bool IsPushKeyboardAttack = CSceneTransition::GetInputKeyboard()->GetKeyTrigger(DIK_J);
	bool IsPushControllerAttack = CSceneTransition::GetXInput()->GetTriggerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER);
	if (IsPushKeyboardAttack || IsPushControllerAttack) {
		CPlayerPattern* pPlayerPattern = m_PlayerPatternManager->GetPattern(PLAYER_ATTACK01_PATTERN);
		if (pPlayerPattern != nullptr) {
			pPlayerPattern->InitParameter(pPlayer, m_PlayerPatternManager);
		}
		pPlayer->SetState(pPlayerPattern);
		return true;
	}

	return false;
}

//���[�����O�p�^�[���ɕύX
bool CPlayerNeutralPattern::ChangePatternToRolling(CPlayer* pPlayer)
{
	//�k���`�F�b�N
	if (pPlayer == nullptr) {
		return false;
	}
	if (m_PlayerPatternManager == nullptr) {
		return false;
	}

	if (m_PushRunButtonTime > RELEASE_BUTTON_TO_ROLLING_FRAME) {
		return false;
	}
	bool bReleaseLShift = CSceneTransition::GetInputKeyboard()->GetKeyRelease(DIK_SPACE);
	bool bReleaseAButton = CSceneTransition::GetXInput()->GetReleaseButton(XINPUT_GAMEPAD_A);
	if (!bReleaseAButton && !bReleaseLShift) {
		return false;
	}

	//�i�s��������
	D3DXVECTOR3 MoveDir;
	if (m_StickValue == D3DXVECTOR2(0.0f, 0.0f)) {		//�X�f�B�b�N�����Ă��Ȃ���
		MoveDir = D3DXVECTOR3(pPlayer->GetFaceDir().x, 0.0f, pPlayer->GetFaceDir().z);
		D3DXVec3Normalize(&MoveDir, &MoveDir);
	}
	else {
		D3DXVECTOR3 CameraLookatDir = CCameraManager::GetDirCameraToLookat(CCameraManager::CAMERA_PLAYER);
		float RotAngle = atan2f(m_StickValue.y, m_StickValue.x);
		MoveDir = D3DXVECTOR3(CameraLookatDir.x, 0.0f, CameraLookatDir.z);
		D3DXVec3Normalize(&MoveDir, &MoveDir);
		CPolygonMath::DirVec3QuaternionRot(&MoveDir, &MoveDir, D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DX_PI * 0.5f - RotAngle);
	}

	pPlayer->AddPower(MoveDir * ROLLING_POWER);								//���x�^����
	pPlayer->AddPower(D3DXVECTOR3(0.0f, ROLLING_POWERY, 0.0f));				//���[�����O���̃W�����v�͂�^��
	pPlayer->SetGoalDir(MoveDir);												//�v���[���[�����ݒ�
	
	//���[�����O�p�^�[���ɕύX
	CPlayerPattern* pPlayerPattern = m_PlayerPatternManager->GetPattern(PLAYER_ROLLING_PATTERN);
	if (pPlayerPattern != nullptr) {
		pPlayerPattern->InitParameter(pPlayer, m_PlayerPatternManager);
	}
	pPlayer->SetState(pPlayerPattern);

	return true;
}

//�X�e�b�v�p�^�[���ɕύX
bool CPlayerNeutralPattern::ChangePatternToStep(CPlayer* pPlayer)
{
	//�k���`�F�b�N
	if (pPlayer == nullptr) {
		return false;
	}
	if (m_PlayerPatternManager == nullptr) {
		return false;
	}

	if (m_PushRunButtonTime > RELEASE_BUTTON_TO_ROLLING_FRAME) {
		return false;
	}
	bool bReleaseAButton = CSceneTransition::GetXInput()->GetReleaseButton(XINPUT_GAMEPAD_A);
	bool bReleaseLShift = CSceneTransition::GetInputKeyboard()->GetKeyRelease(DIK_SPACE);
	if (!bReleaseAButton && !bReleaseLShift) {
		return false;
	}

	//�i�s��������
	D3DXVECTOR3 MoveDir;
	if (m_StickValue == D3DXVECTOR2(0.0f, 0.0f)) {		//�X�f�B�b�N�����Ă��Ȃ���
		MoveDir = D3DXVECTOR3(pPlayer->GetFaceDir().x, 0.0f, pPlayer->GetFaceDir().z);
		D3DXVec3Normalize(&MoveDir, &MoveDir);
		MoveDir = -MoveDir;
	}
	else {
		D3DXVECTOR3 PlayerEnemyDir = pPlayer->GetLockOnPosition() - pPlayer->GetPos();
		D3DXVec3Normalize(&PlayerEnemyDir, &PlayerEnemyDir);
		float RotAngle = atan2f(m_StickValue.y, m_StickValue.x);
		MoveDir = D3DXVECTOR3(PlayerEnemyDir.x, 0.0f, PlayerEnemyDir.z);
		D3DXVec3Normalize(&MoveDir, &MoveDir);						//�P�ʉ�
		CPolygonMath::DirVec3QuaternionRot(&MoveDir, &MoveDir, D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DX_PI * 0.5f - RotAngle);
	}

	pPlayer->AddPower(MoveDir*STEP_POWER);

	//�X�e�b�v�p�^�[���ɕύX
	CPlayerPattern* pPlayerPattern = m_PlayerPatternManager->GetPattern(PLAYER_STEP_PATTERN);
	if (pPlayerPattern != nullptr) {
		pPlayerPattern->InitParameter(pPlayer, m_PlayerPatternManager);
	}
	pPlayer->SetState(pPlayerPattern);

	return true;
}

//����ό`�p�^�[���ɕύX
bool CPlayerNeutralPattern::ChangePatternToChangeWeaponShape(CPlayer* pPlayer)
{
	//�k���`�F�b�N
	if (pPlayer == nullptr) {
		return false;
	}
	if (m_PlayerPatternManager == nullptr) {
		return false;
	}

	//����ό`�p�^�[���ɑJ��
	bool IsPushController = CSceneTransition::GetXInput()->GetTriggerButton(XINPUT_GAMEPAD_LEFT_SHOULDER);
	bool IsPushKeyboard = CSceneTransition::GetInputKeyboard()->GetKeyTrigger(DIK_K);
	if (IsPushController || IsPushKeyboard) {
		CPlayerPattern* pPlayerPattern = m_PlayerPatternManager->GetPattern(PLAYER_CHANGE_WEAPON_SHAPE_PATTERN);
		if (pPlayerPattern != nullptr) {
			pPlayerPattern->InitParameter(pPlayer, m_PlayerPatternManager);
		}
		pPlayer->SetState(pPlayerPattern);

		return true;
	}

	return false;
}