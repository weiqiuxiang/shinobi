#include "CPlayerAttack04Pattern.h"
#include "CPlayer.h"
#include "SceneTransition.h"
#include "CCameraManager.h"
#include "CPolygonMath.h"
#include "common.h"

#define DEAD_ZONE (0.1f)

//�R���X�g���N�^
CPlayerAttack04Pattern::CPlayerAttack04Pattern(CPlayer* pPlayer, CPlayerPatternManager* pManager)
{
	//����ɂ�郂�[�V����������
	if (pPlayer != nullptr) {
		//����ɂ��j���[�g�������[�V�����ݒ�
		CPlayer::WEAPON_MODE WeaponMode = pPlayer->GetWeaponMode();
		switch (WeaponMode) {
		case CPlayer::WEAPON_WSWORD:
			pPlayer->SetMotionMode(CPlayer::MOTION_WSWORD_ATTACK_FOUR);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_WSWORD_ATTACK_FOUR);
			pPlayer->SetAttackType(WSWORD_ATTACK_04);
			break;
		}

		m_MotionNumOld = pPlayer->GetMotionNow();
		pPlayer->SetStateNow(PLAYER_ATTACK04_PATTERN);
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
CPlayerAttack04Pattern::~CPlayerAttack04Pattern()
{
	m_LeftWeaponATKEffect.Uninit();
	m_RightWeaponATKEffect.Uninit();
}

bool CPlayerAttack04Pattern::InitParameter(CPlayer* pPlayer , CPlayerPatternManager* pManager)
{
	//����ɂ�郂�[�V����������
	if (pPlayer != nullptr) {
		//����ɂ��j���[�g�������[�V�����ݒ�
		CPlayer::WEAPON_MODE WeaponMode = pPlayer->GetWeaponMode();
		switch (WeaponMode) {
		case CPlayer::WEAPON_WSWORD:
			pPlayer->SetMotionMode(CPlayer::MOTION_WSWORD_ATTACK_FOUR);
			m_nMotionTime = pPlayer->GetMotionTotalFrame((int)CPlayer::MOTION_WSWORD_ATTACK_FOUR);
			pPlayer->SetAttackType(WSWORD_ATTACK_04);
			break;
		}

		m_MotionNumOld = pPlayer->GetMotionNow();
		pPlayer->SetStateNow(PLAYER_ATTACK04_PATTERN);
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

void CPlayerAttack04Pattern::Update(CPlayer* pPlayer)
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
	if (ChangePattern(pPlayer) == true) {
		return;
	}
}

//�`�揈��
void CPlayerAttack04Pattern::Draw(void)
{
	m_LeftWeaponATKEffect.Draw();
	m_RightWeaponATKEffect.Draw();
}

void CPlayerAttack04Pattern::UpdateAttackEffectPosition(CPlayer* pPlayer)
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

void CPlayerAttack04Pattern::UpdateWSwordAttackEffect(CPlayer* pPlayer)
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

void CPlayerAttack04Pattern::UpdateClaymoreAttackEffect(CPlayer* pPlayer)
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
void CPlayerAttack04Pattern::StopWeaponEffect(void)
{
	m_LeftWeaponATKEffect.NoUseAllVertex();
	m_RightWeaponATKEffect.NoUseAllVertex();
}

//���킲�Ƃ̍U�����[�V�����̋�̓���
void CPlayerAttack04Pattern::AttackActionDetail(CPlayer* pPlayer)
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
			pPlayer->AddPower(PlayerFaceDir * 0.16f * 0.1f);
		}
		if (KeyFrameNow == 1) {
			pPlayer->AddPower(PlayerFaceDir * 0.16f * 0.4f);
		}
		if (KeyFrameNow == 2) {
			pPlayer->AddPower(PlayerFaceDir * 0.16f * 0.1f);
		}

		//���蔻��~�̗L�������ݒ蕔��
		HitColliEnable = KeyFrameNow != 0 && KeyFrameNow != 1;
		if (HitColliEnable) {
			pPlayer->EnableWeaponHitCircle(true, true);
		}
		else {
			pPlayer->DisableWeaponHitCircle(true, true);
		}

		//�U���͐ݒ�
		pPlayer->SetAttackPower(30);
		break;
	}
}

//�p�^�[���̕ύX
bool CPlayerAttack04Pattern::ChangePattern(CPlayer* pPlayer)
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

	m_nMotionTime--;			//���[�V�����p�����Ԍ���

	//�j���[�g�����p�^�[���ɖ߂�
	if (m_nMotionTime < 0) {
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

	return false;
}

void CPlayerAttack04Pattern::ChangeDirWhenAttack(CPlayer* pPlayer)
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

D3DXVECTOR2 CPlayerAttack04Pattern::KeyboardValue(void)
{
	BOOL PressW = CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_W);
	BOOL PressA = CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_A);
	BOOL PressS = CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_S);
	BOOL PressD = CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_D);

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