#include "CPlayerAttack01Pattern.h"
#include "CPlayer.h"
#include "SceneTransition.h"
#include "CCameraManager.h"
#include "CPolygonMath.h"
#include "CPlayerPatternManager.h"
#include "common.h"

#define DEAD_ZONE (0.1f)

//�R���X�g���N�^
CPlayerAttack01Pattern::CPlayerAttack01Pattern(CPlayer* pPlayer, CPlayerPatternManager* pManager)
{
	//����ɂ�郂�[�V����������
	if (pPlayer != nullptr) {
		//����ɂ��j���[�g�������[�V�����ݒ�
		CPlayer::WEAPON_MODE WeaponMode = pPlayer->GetWeaponMode();
		switch (WeaponMode) {
		case CPlayer::WEAPON_WSWORD:
			pPlayer->SetMotionMode(CPlayer::MOTION_WSWORD_ATTACK_ONE);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_WSWORD_ATTACK_ONE);
			pPlayer->SetAttackType(WSWORD_ATTACK_01);
			break;
		case CPlayer::WEAPON_CLAYMORE:
			pPlayer->SetMotionMode(CPlayer::MOTION_CLAYMORE_ATTACK_ONE);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_CLAYMORE_ATTACK_ONE);
			pPlayer->SetAttackType(CLAYMORE_ATTACK_01);
			break;
		}
		
		m_MotionNumOld = pPlayer->GetMotionNow();
		pPlayer->SetStateNow(PLAYER_ATTACK01_PATTERN);
	}

	//����c���G�t�F�N�g������
	m_LeftWeaponATKEffect.Init(AFTERIMAGE_EFFECT_TEXPASS);
	m_RightWeaponATKEffect.Init(AFTERIMAGE_EFFECT_TEXPASS);
	m_LeftWeaponATKEffect.NoUseAllVertex();
	m_RightWeaponATKEffect.NoUseAllVertex();
	if (pPlayer != nullptr) {
		UpdateAttackEffectPosition(pPlayer);
	}

	ChangeDirWhenAttack(pPlayer);					//��������

	m_PlayerPatternManager = pManager;
}

//�f�X�g���N�^
CPlayerAttack01Pattern::~CPlayerAttack01Pattern()
{
	m_LeftWeaponATKEffect.Uninit();
	m_RightWeaponATKEffect.Uninit();
}

bool CPlayerAttack01Pattern::InitParameter(CPlayer* pPlayer , CPlayerPatternManager* pManager)
{
	//����ɂ�郂�[�V����������
	if (pPlayer != nullptr) {
		//����ɂ��j���[�g�������[�V�����ݒ�
		CPlayer::WEAPON_MODE WeaponMode = pPlayer->GetWeaponMode();
		switch (WeaponMode) {
		case CPlayer::WEAPON_WSWORD:
			pPlayer->SetMotionMode(CPlayer::MOTION_WSWORD_ATTACK_ONE);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_WSWORD_ATTACK_ONE);
			pPlayer->SetAttackType(WSWORD_ATTACK_01);
			break;
		case CPlayer::WEAPON_CLAYMORE:
			pPlayer->SetMotionMode(CPlayer::MOTION_CLAYMORE_ATTACK_ONE);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_CLAYMORE_ATTACK_ONE);
			pPlayer->SetAttackType(CLAYMORE_ATTACK_01);
			break;
		}

		m_MotionNumOld = pPlayer->GetMotionNow();
		pPlayer->SetStateNow(PLAYER_ATTACK01_PATTERN);
	}

	//����c���G�t�F�N�g������
	m_LeftWeaponATKEffect.Init(AFTERIMAGE_EFFECT_TEXPASS);
	m_RightWeaponATKEffect.Init(AFTERIMAGE_EFFECT_TEXPASS);
	m_LeftWeaponATKEffect.NoUseAllVertex();
	m_RightWeaponATKEffect.NoUseAllVertex();
	if (pPlayer != nullptr) {
		UpdateAttackEffectPosition(pPlayer);
	}

	ChangeDirWhenAttack(pPlayer);					//��������

	m_PlayerPatternManager = pManager;

	return true;
}

void CPlayerAttack01Pattern::Update(CPlayer* pPlayer)
{
	//�k���`�F�b�N
	if (pPlayer == nullptr) {
		return;
	}

	//�U���O�ՃG�t�F�N�g�ʒu�X�V
	UpdateAttackEffectPosition(pPlayer);

	//����G�t�F�N�g�̍X�V
	m_LeftWeaponATKEffect.Update();
	m_RightWeaponATKEffect.Update();

	//���킲�Ƃ̍U�����[�V�����̋�̓���
	AttackActionDetail(pPlayer);

	//�p�^�[���̕ύX
	if(ChangePattern(pPlayer) == true){
		return;
	}
}

//�`�揈��
void CPlayerAttack01Pattern::Draw(void)
{
	m_LeftWeaponATKEffect.Draw();
	m_RightWeaponATKEffect.Draw();
}

void CPlayerAttack01Pattern::UpdateAttackEffectPosition(CPlayer* pPlayer)
{
	//�k���`�F�b�N
	if (pPlayer == nullptr) {
		return;
	}

	//����ɂ��G�t�F�N�g
	CPlayer::WEAPON_MODE WeaponMode = pPlayer->GetWeaponMode();
	switch (WeaponMode) {
	case CPlayer::WEAPON_WSWORD:
		UpdateWSwordAttackEffect(pPlayer);
		break;
	case CPlayer::WEAPON_CLAYMORE:
		UpdateClaymoreAttackEffect(pPlayer);
		break;
	}
}

void CPlayerAttack01Pattern::UpdateWSwordAttackEffect(CPlayer* pPlayer)
{
	//�k���`�F�b�N
	if (pPlayer == nullptr) {
		return;
	}

	/***************���蕐��**************/
	//�K�v�ϐ�
	D3DXMATRIX LeftWeaponMtx;
	D3DXMatrixIdentity(&LeftWeaponMtx);
	D3DXMATRIX OffsetMtx, ResultMtx;

	//�G�t�F�N�g��ʒu�v�Z
	pPlayer->GetWeaponLeftPartMtx(&LeftWeaponMtx, 1);
	D3DXMatrixTranslation(&OffsetMtx, 0.0f, 2.0f, 0.0f);	//�I�t�Z�b�g�v�Z
	ResultMtx = OffsetMtx * LeftWeaponMtx;				//���U���g�}�g���N�X�v�Z
	D3DXVECTOR3 LeftTopPos = D3DXVECTOR3(ResultMtx._41, ResultMtx._42, ResultMtx._43);

	//�G�t�F�N�g���ʒu�v�Z
	pPlayer->GetWeaponLeftPartMtx(&LeftWeaponMtx, 0);
	D3DXMatrixTranslation(&OffsetMtx, 0.0f, 0.0f, 0.0f);	//�I�t�Z�b�g�v�Z
	ResultMtx = OffsetMtx * LeftWeaponMtx;				//���U���g�}�g���N�X�v�Z
	D3DXVECTOR3 LeftBottomPos = D3DXVECTOR3(ResultMtx._41, ResultMtx._42, ResultMtx._43);

	//�O�ՃG�t�F�N�g�̒��_�ݒ�
	if (m_LeftWeaponATKEffect.UseEffectVertex(LeftTopPos, LeftBottomPos) == false)
	{
		m_LeftWeaponATKEffect.MoveEffectPosition(LeftTopPos, LeftBottomPos);
	}

	/***************�E�蕐��**************/
	D3DXMATRIX RightWeaponMtx;
	D3DXMatrixIdentity(&RightWeaponMtx);

	//�G�t�F�N�g��ʒu�v�Z
	pPlayer->GetWeaponRightPartMtx(&RightWeaponMtx, 1);
	D3DXMatrixTranslation(&OffsetMtx, 0.0f, 2.0f, 0.0f);	//�I�t�Z�b�g�v�Z
	ResultMtx = OffsetMtx * RightWeaponMtx;				//���U���g�}�g���N�X�v�Z
	D3DXVECTOR3 RightTopPos = D3DXVECTOR3(ResultMtx._41, ResultMtx._42, ResultMtx._43);
	//�G�t�F�N�g���ʒu�v�Z
	pPlayer->GetWeaponRightPartMtx(&RightWeaponMtx, 0);
	D3DXMatrixTranslation(&OffsetMtx, 0.0f, 0.0f, 0.0f);	//�I�t�Z�b�g�v�Z
	ResultMtx = OffsetMtx * RightWeaponMtx;				//���U���g�}�g���N�X�v�Z
	D3DXVECTOR3 RightBottomPos = D3DXVECTOR3(ResultMtx._41, ResultMtx._42, ResultMtx._43);
	//�O�ՃG�t�F�N�g�̒��_�ݒ�
	if (m_RightWeaponATKEffect.UseEffectVertex(RightTopPos, RightBottomPos) == false)
	{
		m_RightWeaponATKEffect.MoveEffectPosition(RightTopPos, RightBottomPos);
	}
}

void CPlayerAttack01Pattern::UpdateClaymoreAttackEffect(CPlayer* pPlayer)
{
	//�k���`�F�b�N
	if (pPlayer == nullptr) {
		return;
	}

	//�K�v�ϐ�
	D3DXMATRIX LeftWeaponMtx;
	D3DXMatrixIdentity(&LeftWeaponMtx);
	D3DXMATRIX OffsetMtx, ResultMtx;

	//�G�t�F�N�g��ʒu�v�Z
	pPlayer->GetWeaponLeftPartMtx(&LeftWeaponMtx, 1);
	D3DXMatrixTranslation(&OffsetMtx, 0.0f, 2.0f, 0.0f);	//�I�t�Z�b�g�v�Z
	ResultMtx = OffsetMtx * LeftWeaponMtx;				//���U���g�}�g���N�X�v�Z
	D3DXVECTOR3 LeftTopPos = D3DXVECTOR3(ResultMtx._41, ResultMtx._42, ResultMtx._43);
	//�G�t�F�N�g���ʒu�v�Z
	pPlayer->GetWeaponLeftPartMtx(&LeftWeaponMtx, 0);
	D3DXMatrixTranslation(&OffsetMtx, 0.0f, 0.0f, 0.0f);	//�I�t�Z�b�g�v�Z
	ResultMtx = OffsetMtx * LeftWeaponMtx;				//���U���g�}�g���N�X�v�Z
	D3DXVECTOR3 LeftBottomPos = D3DXVECTOR3(ResultMtx._41, ResultMtx._42, ResultMtx._43);
	//�O�ՃG�t�F�N�g�̒��_�ݒ�
	if (m_LeftWeaponATKEffect.UseEffectVertex(LeftTopPos, LeftBottomPos) == false)
	{
		m_LeftWeaponATKEffect.MoveEffectPosition(LeftTopPos, LeftBottomPos);
	}
}

//����̃G�t�F�N�g��~
void CPlayerAttack01Pattern::StopWeaponEffect(void)
{
	m_LeftWeaponATKEffect.NoUseAllVertex();
	m_RightWeaponATKEffect.NoUseAllVertex();
}

//���킲�Ƃ̍U�����[�V�����̋�̓���
void CPlayerAttack01Pattern::AttackActionDetail(CPlayer* pPlayer)
{
	//�k���`�F�b�N
	if (pPlayer == nullptr) {
		return;
	}

	D3DXVECTOR3 PlayerFaceDir = pPlayer->GetFaceDir();		//���̃v���[���[�����擾
	int KeyFrameNow = pPlayer->GetKeyFrameNow();			//���̃L�[�t���[���擾
	bool HitColliEnable = false;							//�U�����蔻��~�̎g�p��

	switch (pPlayer->GetWeaponMode())
	{
	case CPlayer::WEAPON_WSWORD:
		//�L�[�t���[���ɂ��ړ�����
		if (KeyFrameNow == 0) {
			pPlayer->AddPower(PlayerFaceDir * 0.16f * 0.2f);
		}
		if (KeyFrameNow == 1) {
			pPlayer->AddPower(PlayerFaceDir * 0.16f * 0.2f);
		}
		if (KeyFrameNow == 2) {
			pPlayer->AddPower(PlayerFaceDir * 0.16f * 0.4f);
		}

		//���蔻��~�̗L�������ݒ蕔��
		HitColliEnable = KeyFrameNow != 0;
		if (HitColliEnable) {
			pPlayer->EnableWeaponHitCircle(false, true);
		}
		else {
			pPlayer->DisableWeaponHitCircle(true, true);
		}

		//�U���͐ݒ�
		pPlayer->SetAttackPower(40);
		break;

	case CPlayer::WEAPON_CLAYMORE:
		//�L�[�t���[���ɂ��ړ�����
		if (KeyFrameNow == 1) {
			pPlayer->AddPower(PlayerFaceDir * 0.16f * -0.1f);
		}
		if (KeyFrameNow == 3) {
			pPlayer->AddPower(PlayerFaceDir * 0.16f * 0.2f);
		}
		if (KeyFrameNow == 4) {
			pPlayer->AddPower(PlayerFaceDir * 0.16f * 0.4f);
		}

		//���蔻��~�̗L�������ݒ蕔��
		HitColliEnable = KeyFrameNow != 0 && KeyFrameNow != 1 && KeyFrameNow != 2;
		if (HitColliEnable) {
			pPlayer->EnableWeaponHitCircle(true, true);
		}
		else {
			pPlayer->DisableWeaponHitCircle(true, true);
		}

		//�U���͐ݒ�
		pPlayer->SetAttackPower(70);
		break;
	}
}

//�p�^�[���̕ύX
bool CPlayerAttack01Pattern::ChangePattern(CPlayer* pPlayer)
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

	m_nMotionTime--;			//���[�V�����p�����Ԍ���

	//�j���[�g�����p�^�[���ɖ߂�
	if(m_nMotionTime < 0){
		m_LeftWeaponATKEffect.NoUseAllVertex();
		m_RightWeaponATKEffect.NoUseAllVertex();

		CPlayerPattern* pPlayerPattern = m_PlayerPatternManager->GetPattern(PLAYER_NEUTRAL_PATTERN);
		if (pPlayerPattern != nullptr) {
			pPlayerPattern->InitParameter(pPlayer, m_PlayerPatternManager);
		}
		pPlayer->DisableWeaponHitCircle(true, true);
		pPlayer->SetState(pPlayerPattern);
		return true;
	}

	int MotionNow = pPlayer->GetMotionNow();								//�����s���̃��[�V�����擾
	int MotionTotalKeyFrame = pPlayer->GetMotionTotalKeyFrame(MotionNow);		//�L�[�t���[�������擾
	int KeyFrameNow = pPlayer->GetKeyFrameNow();							//�����s���̃L�[�t���[��

	//�U���t���[���͍Ō�̃L�[�t���[��
	if ((MotionTotalKeyFrame - 1) == KeyFrameNow) {
		//�U���{�^�����̗͂L���m�F
		bool bKeyboard = CSceneTransition::GetInputKeyboard()->GetKeyTrigger(DIK_J);
		bool bController = CSceneTransition::GetXInput()->GetTriggerButton(XINPUT_GAMEPAD_RIGHT_SHOULDER);

		//���͂Ȃ��Ȃ珈���I��
		if (!bKeyboard && !bController) {
			return false;
		}

		//����ɂ��U���p�^�[���̕ύX
		CPlayerPattern* pPlayerPattern = m_PlayerPatternManager->GetPattern(PLAYER_ATTACK02_PATTERN);
		if (pPlayerPattern != nullptr) {
			pPlayerPattern->InitParameter(pPlayer, m_PlayerPatternManager);
		}
		pPlayer->DisableWeaponHitCircle(true, true);
		pPlayer->SetState(pPlayerPattern);
		return true;
	}

	return false;
}

void CPlayerAttack01Pattern::ChangeDirWhenAttack(CPlayer* pPlayer)
{
	//�k���`�F�b�N
	if (pPlayer == nullptr) {
		return;
	}

	if (pPlayer->GetLockOnFlag() == false) {
		D3DXVECTOR2 AnalogInputValue = KeyboardValue();
		if (D3DXVec2Length(&AnalogInputValue) < DEAD_ZONE) {
			AnalogInputValue = CSceneTransition::GetXInput()->GetLeftThumbStickValueAnalog();
		}
		if (D3DXVec2Length(&AnalogInputValue) < DEAD_ZONE) {
			return;
		}

		CAMERA_PARA CameraPara = CCameraManager::GetCameraPara(CCameraManager::CAMERA_PLAYER);		//�J�����̐��������̌������擾

		//�J�����̌����Ă�XZ���ʂ̕����x�N�g�����Z�o
		D3DXVECTOR3 CameraLookatVec = CameraPara.lookat - CameraPara.pos;
		CameraLookatVec.y = 0;

		D3DXVec3Normalize(&CameraLookatVec, &CameraLookatVec);

		//�v���[���[�̌����Z�o
		float AngleDir = CPolygonMath::Vec2AngleAndAngleDir(D3DXVECTOR2(0.0f, 1.0f), AnalogInputValue);		//�R���g���[���̐����p�x���Z�o
		D3DXMATRIX MtxRot;
		D3DXMatrixRotationY(&MtxRot, -AngleDir);
		D3DXVECTOR3 PlayerDir;
		D3DXVec3TransformCoord(&PlayerDir, &CameraLookatVec, &MtxRot);
		D3DXVec3Normalize(&PlayerDir, &PlayerDir);
		pPlayer->SetGoalDir(PlayerDir);
		pPlayer->SetFaceDir(PlayerDir);
	}

	else {
		D3DXVECTOR3 PlayerPos = pPlayer->GetPos();
		D3DXVECTOR3 EnemyPos = pPlayer->GetLockOnPosition();
		D3DXVECTOR3 PlayerEnemyVec = EnemyPos - PlayerPos;
		D3DXVec3Normalize(&PlayerEnemyVec, &PlayerEnemyVec);
		pPlayer->SetGoalDir(PlayerEnemyVec);
		pPlayer->SetFaceDir(PlayerEnemyVec);
	}
}

D3DXVECTOR2 CPlayerAttack01Pattern::KeyboardValue(void)
{
	bool PressW = CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_W);
	bool PressA = CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_A);
	bool PressS = CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_S);
	bool PressD = CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_D);

	//�R���g���[���X�e�B�b�N�����̎Z�o
	D3DXVECTOR2 StickValue = D3DXVECTOR2(0.0f, 0.0f);
	if (PressW) StickValue = D3DXVECTOR2(0.0f, 1.0f);
	if (PressA) StickValue = D3DXVECTOR2(-1.0f, 0.0f);
	if (PressS) StickValue = D3DXVECTOR2(0.0f, -1.0f);
	if (PressD) StickValue = D3DXVECTOR2(1.0f, 0.0f);
	if (PressW && PressA) StickValue = D3DXVECTOR2(-0.7071f, 0.7071f);
	if (PressW && PressD) StickValue = D3DXVECTOR2(0.7071f, 0.7071f);
	if (PressS && PressA) StickValue = D3DXVECTOR2(-0.7071f, -0.7071f);
	if (PressS && PressD) StickValue = D3DXVECTOR2(0.7071f, -0.7071f);

	return StickValue;
}