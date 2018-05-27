#include "CPlayerControl.h"
#include "SceneTransition.h"
#include "CCameraManager.h"
#include "CMeshFieldManager.h"
#include "CPolygonMath.h"
#include "CVectorMath.h"
#include "CDebug.h"
#include "CLinkPlayerToUI.h"
#include "CGame.h"
#include "XInputControl.h"
#include "CLockOnEnemy.h"
#include "CCharacterCamera.h"

#define STAMINA_DOWN_SPEED (5)					//�v���[���[�̃X�^�~�i���Ց��x
#define HOLD_BUTTON_TO_RUN_FRAME (10)			//���郂�[�V�����ɐ؂�ւ�����,�{�^������������K�v�t���[����

//�ړ��n�}�N��
#define WALK_POWER (0.02f)			//������
#define RUN_POWER (0.05f)			//�����
#define DASH_POWER (0.08f)			//�_�b�V���̗�
#define STEP_POWER (0.6f)			//�X�e�b�v�̗�
#define ROLLING_POWER (0.6f)		//���[�����O�̗�
#define ROLLING_POWERY (0.4f)		//���[�����O���̃W�����v��

#define STICK_WALK_ZONE (0.5f)
#define STICK_RUN_ZONE (0.8f)

#define DEFAULT_COFFI (0.2)					//�f�t�H���g�n�ʖ��C�W��
#define SKY_COFFI (0.02)					//�󒆖��C�W��

//������
void CPlayerControl::Init(void)
{
	m_Enemy = nullptr;
	m_Player = nullptr;
	m_MotionMode = CPlayer::MOTION_WSWORD_NEUTRAL;
	m_MotionModeOld = CPlayer::MOTION_WSWORD_NEUTRAL;
	m_bMotionChange = false;
	m_nMotionTime = 0;
	m_ControlerStickDir = D3DXVECTOR2(0.0f, 0.0f);
	m_ControlerRightStickDirOld = D3DXVECTOR2(0.0f, 0.0f);

	//�J�����I�u�W�F�N�g������
	CAMERA_PARA CameraPara = CCameraManager::GetCameraPara(CCameraManager::CAMERA_PLAYER);
	m_PlayerCamera = new CCharacterCamera(
		CameraPara.pos, CameraPara.lookat,
		CCameraManager::GetHoriAngle(CCameraManager::CAMERA_PLAYER),
		CCameraManager::GetVertiAngle(CCameraManager::CAMERA_PLAYER));
	
	m_MovePower = WALK_POWER;											//�ړ��p���[
	m_ActionMoveDir = D3DXVECTOR3(0.0f, 0.0f,0.0f);
	m_KeyFrameBefore = 0;
}

//�v���[���[���R���g���[������
void CPlayerControl::Update(void)
{
	m_MotionModeOld = m_MotionMode;
	if (nullptr != m_Enemy) {
		if (m_Enemy->GetDeadFlag()) {
			m_Enemy = nullptr;
		}

		if (m_Player != nullptr) {
			m_Player->SetLockOnFlag(true);
			m_Player->SetLockOnPosition(GetLockOnEnemyPos());
		}
	}
	else {
		m_Player->SetLockOnFlag(false);
	}

	PlayerLockOnEnemy();		//�G�Ƀ��b�N�I��
	LockOnTargetChange();		//���b�N�I�����Ă���W�I�`�F���W
	PlayerHitAllMesh();			//�v���[���[�ƃ��b�V���̓��蔻��
	PlayerFaceDir();			//�v���[���[�̎p���v�Z
	PlayerCameraControl();		//�v���[���[�̃J�����Ǐ]

	//�O���֐�
	CLinkPlayerToUI::SetPlayerLifeNow(m_Player->GetPlayerLife());
	CLinkPlayerToUI::SetPlayerStaminaNow(m_Player->GetPlayerStamina());
	m_ControlerRightStickDirOld = CSceneTransition::GetXInput()->GetRightThumbStickValueAnalog(); //�E�X�e�B�b�N�X�e�[�^�X�擾
	
}

void CPlayerControl::SceneTraDecision(CGame::SCENE_TRA* pTraFlag)
{
	if (nullptr == m_Player) return;
	if (m_Player->GetPlayerDeadFlag() == true) *pTraFlag = CGame::TRA_TO_RESULT;
}

/*==============================================================================
	�I������
===============================================================================*/
void CPlayerControl::Uninit(void)
{
	if (m_PlayerCamera != nullptr) {
		delete m_PlayerCamera;
		m_PlayerCamera = nullptr;
	}
}

/*==============================================================================
	���b�N�I�����Ă���G�̍��W
===============================================================================*/
D3DXVECTOR3 CPlayerControl::GetLockOnEnemyPos(void)
{
	if (nullptr != m_Enemy) {
		return m_Enemy->GetBodyPos();
	}
	return D3DXVECTOR3(0.0f,0.0f,0.0f);
}

void CPlayerControl::SetPlayer(CPlayer *Player)
{
	if(nullptr == Player)
	{
		MessageBox(nullptr, "CPlayerControl�ɓn�����v���[���[�|�C���^��nullptr!", "�|�C���^�̒l��nullptr", MB_OK | MB_ICONHAND);
	}
	m_Player = Player;
}

/*
//�v���[���[�̌����̖ڕW�����߂�
void CPlayerControl::PlayerGoalDir(void)
{
	if (nullptr == m_Player) {
		return;
	}

	//�G�����b�N�I�����Ă��Ȃ����
	if (false == m_bLockEnemy)
	{
		if (D3DXVec2Length(&m_ControlerStickDir) < 0.2f) return;

		CAMERA_PARA CameraPara = CCameraManager::GetCameraPara(CCameraManager::CAMERA_PLAYER);		//�J�����̐��������̌������擾

		//�J�����̌����Ă�XZ���ʂ̕����x�N�g�����Z�o
		D3DXVECTOR3 CameraLookatVec = CameraPara.lookat - CameraPara.pos;
		CameraLookatVec.y = 0;
			
		D3DXVec3Normalize(&CameraLookatVec, &CameraLookatVec);

		//�v���[���[�̌����Z�o
		float AngleDir = CPolygonMath::Vec2AngleAndAngleDir(D3DXVECTOR2(0.0f, 1.0f), m_ControlerStickDir);		//�R���g���[���̐����p�x���Z�o
		D3DXMATRIX MtxRot;
		D3DXMatrixRotationY(&MtxRot, -AngleDir);
		D3DXVECTOR3 PlayerDir;
		D3DXVec3TransformCoord(&PlayerDir, &CameraLookatVec, &MtxRot);
		D3DXVec3Normalize(&PlayerDir, &PlayerDir);
		m_Player->SetGoalDir(PlayerDir);
	}

	else
	{
		if (nullptr == m_Enemy) return;
		D3DXVECTOR3 PlayerPos = m_Player->GetPos();
		D3DXVECTOR3 EnemyPos = m_Enemy->GetPos();
		D3DXVECTOR3 PlayerEnemyVec = EnemyPos - PlayerPos;
		D3DXVec3Normalize(&PlayerEnemyVec, &PlayerEnemyVec);
		m_Player->SetGoalDir(PlayerEnemyVec);
	}
}
*/

//�v���[���[�̖ʂ̌����̌v�Z
void CPlayerControl::PlayerFaceDir(void)
{
	if (nullptr == m_Player) {
		return;
	}

	//�v���[���[�����x�N�g���Ńv���[���[�̎p�����v�Z
	D3DXMATRIX PoseMtx;
	D3DXVECTOR3 X,Y,Z;
	Y = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 PlayerDir = m_Player->GetFaceDir();
	if (D3DXVECTOR3(0.0f, 0.0f, 0.0f) == PlayerDir) return;
	D3DXVec3Normalize(&Z, &PlayerDir);
	D3DXVec3Cross(&X, &Y,&Z);
	D3DXVec3Normalize(&X, &X);
	
	PoseMtx._11 = X.x; PoseMtx._12 = X.y; PoseMtx._13 = X.z; PoseMtx._14 = 0.0f;
	PoseMtx._21 = Y.x; PoseMtx._22 = Y.y; PoseMtx._23 = Y.z; PoseMtx._24 = 0.0f;
	PoseMtx._31 = Z.x; PoseMtx._32 = Z.y; PoseMtx._33 = Z.z; PoseMtx._34 = 0.0f;
	PoseMtx._41 = 0.0f; PoseMtx._42 = 0.0f; PoseMtx._43 = 0.0f; PoseMtx._44 = 1.0f;

	m_Player->SetPoseMtx(PoseMtx);
}

//�v���[���[�ƒn�ʂ̓��蔻��
void CPlayerControl::PlayerHitAllMesh(void)
{
	if (nullptr == m_Player) return;

	D3DXVECTOR3 PlayerPos = m_Player->GetPos();
	CMeshField *pMesh = nullptr;

	//�v���[���[�ƑS�Ẵ��b�V���̓��蔻��
	for(int MeshNum = 0; MeshNum < MESH_MAX; MeshNum++)
	{
		pMesh = CMeshFieldManager::GetMesh(MeshNum);
		if (nullptr == pMesh) continue;

		const MESH_FACE *pFace = pMesh->GetMeshFace();			//�ʃf�[�^�擾
		D3DXVECTOR2 PieceNum = pMesh->GetPieceNum();			//�}�X���擾

		//���b�V���̑S�Ă̖ʂƃv���[���[�̓��蔻��
		for (int PieceY = 0; PieceY < PieceNum.y; PieceY++)
		{
			for (int PieceX = 0; PieceX < (int)PieceNum.x * 2; PieceX++)
			{
				//�ʔԍ��v�Z
				int k = PieceY * ((int)PieceNum.x * 2) + PieceX;
				if (nullptr == &pFace[k]) continue;

				//�v���[���[�͕��ʂ͈͓̔��ɂ��邩�ǂ���
				bool InMesh = CPolygonMath::PointInXZMeshFace(PlayerPos, pFace[k]);
				if (InMesh == false) {
					continue;
				}

				//�v���[���[��XZ���W��n��,���̈ʒu�̖ʂ�Y���W�����߂�
				float fElevation = CPolygonMath::PointElevationInFace(PlayerPos, pFace[k]);

				//�v���[���[��Y���W�͕W�����Ⴂ��
				if (m_Player->GetPos().y < fElevation) {
					m_Player->SetPosY(fElevation);
					m_Player->SetSpeedY(0);
				}
			}
		}
	}
}

void CPlayerControl::PlayerLockOnEnemy(void)           //�G�Ƀ��b�N�I��
{
	if (nullptr == m_Player) {
		return;
	}
	
	//�G�Ƀ��b�N�I�����Ă�����
	if (m_Enemy != nullptr) {
		//�v���[���[�ƓG�̋��������߂�
		D3DXVECTOR3 PlayerEnemyVec = m_Enemy->GetPos() - m_Player->GetPos();
		float Distance = D3DXVec3Length(&PlayerEnemyVec);

		//���b�N�I���{�^�������ꂽ�t���O
		bool IsPushLockOnButton = CSceneTransition::GetInputKeyboard()->GetKeyTrigger(DIK_RETURN) || 
			CSceneTransition::GetXInput()->GetTriggerButton(XINPUT_GAMEPAD_RIGHT_THUMB);

		//���b�N�I���̉�������
		if (Distance > LOCKON_DISTANCE || IsPushLockOnButton) {
			m_Player->SetLockOnFlag(false);
			m_Enemy = nullptr;			//Enemy�|�C���^��nullptr��
			return; 
		}
	}

	else {
		//���b�N�I���{�^�������ꂽ�t���O
		bool IsPushLockOnButton = CSceneTransition::GetInputKeyboard()->GetKeyTrigger(DIK_RETURN) ||
			CSceneTransition::GetXInput()->GetTriggerButton(XINPUT_GAMEPAD_RIGHT_THUMB);
		
		//���b�N�I���ł���G��T��
		if (IsPushLockOnButton) {
			CEnemy* pEnemy = CLockOnEnemy::LockOnEnemy(m_Player);

			//���b�N�I���G�͂��Ȃ�
			if (pEnemy == nullptr) {
				return;
			}
			else {
				m_Enemy = pEnemy;
				m_Player->SetLockOnFlag(true);
			}
		}
	}
}


//���b�N�I���̃^�[�Q�b�g�̕ύX
void CPlayerControl::LockOnTargetChange(void)
{
	//�k���`�F�b�N
	if (m_Player == nullptr) {
		return;
	}
	if (m_Player->GetLockOnFlag() == false) {
		return;
	}

	//�L�[�{�[�h�́��L�[�A���L�[�������ꂽ��
	bool bPushKeyLeft = CSceneTransition::GetInputKeyboard()->GetKeyTrigger(DIK_LEFT);
	bool bPushKeyRight = CSceneTransition::GetInputKeyboard()->GetKeyTrigger(DIK_RIGHT);
	bool bPushDirKey = bPushKeyLeft || bPushKeyRight;

	//�E�X�e�B�b�N�̂������E�ɉ�������
	bool bIsTiggerRightStick =
		(fabs(m_ControlerRightStickDirOld.x) < 0.5) && 
		(fabs(CSceneTransition::GetXInput()->GetRightThumbStickValueAnalog().x) > 0.5);

	//�L�[�{�[�h�������̓X�e�B�b�N�̑���͂Ȃ��ꍇ�A�������^�[������
	if (!bPushDirKey && !bIsTiggerRightStick) {
		return;
	}

	bool bDir = false;		//���͕���

	//�X�e�B�b�N�̓��͂͂Ȃ��ꍇ�A�L�[�{�[�h�̓��͂𔻒肷��
	if ( bIsTiggerRightStick == false) {
		if (bPushKeyLeft) {
			bDir = false;
		}
		if (bPushKeyRight) {
			bDir = true;
		}
	}
	else {
		if (CSceneTransition::GetXInput()->GetRightThumbStickValueAnalog().x < 0.0f) {
			bDir = false;
		}
		else {
			bDir = true;
		}
	}

	//���b�N�I���W�I�̕ύX
	CEnemy* pEnemy = CLockOnEnemy::ChangeTarget(m_Player, m_Enemy, bDir);
	if (pEnemy != nullptr) {
		m_Enemy = pEnemy;
	}
}

void CPlayerControl::RotateCamera(void)
{
	//�k���`�F�b�N
	if (m_Player == nullptr || m_PlayerCamera == nullptr) {
		return;
	}
	//�J�����`�F�b�N
	if (CCameraManager::GetActiveCameraName() != CCameraManager::CAMERA_PLAYER) {
		return;
	}

	//�R���g���[���̃J������]����
	D3DXVECTOR2 AnalogInputValue = CSceneTransition::GetXInput()->GetRightThumbStickValueAnalog();
	m_PlayerCamera->TpsCameraRotateStick(AnalogInputValue);

	//�L�[�{�[�h�̃J������]����
	float HorizonValue = 0;
	float VerticleValue = 0;
	if (CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_UP)) {
		VerticleValue = 1.0f;
	}
	if (CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_DOWN)) {
		VerticleValue = -1.0f;
	}
	if (CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_LEFT)) {
		HorizonValue = 1.0f;
	}
	if (CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_RIGHT)) {
		HorizonValue = -1.0f;
	}
	m_PlayerCamera->TpsCameraRotateKeyboard(HorizonValue, VerticleValue);
}

void CPlayerControl::PlayerCameraControl(void)
{
	if (m_Player == nullptr) {
		return;
	}

	//���b�N�I�����Ă��Ȃ���
	if (m_Player->GetLockOnFlag() == false) {
		if(m_PlayerCamera != nullptr && m_Player != nullptr){
			//�ڕW�ʒu�̌v�Z
			m_PlayerCamera->CalcCameraAndLookatPos(m_Player->GetPos());
			//��Ԃ���
			CAMERA_PARA CameraPara = CCameraManager::GetCameraPara(CCameraManager::CAMERA_PLAYER);	//�J�����f�[�^�擾
			m_PlayerCamera->CameraAndLookatToGoalLinearInterpolation(&CameraPara.pos, &CameraPara.lookat);
			//��Ԍ���W�����J�����ɑ��
			CCameraManager::SetCameraAndLookat(CCameraManager::CAMERA_PLAYER, CameraPara.pos, CameraPara.lookat);
		}

		//�J�����̉�]�R���g���[��
		RotateCamera();
	}
	else {
		if (m_PlayerCamera != nullptr && m_Player != nullptr && m_Enemy != nullptr) {
			//�ڕW�ʒu�̌v�Z
			m_PlayerCamera->CalcCameraAndLookatPosLockOn(m_Player->GetPos(), m_Enemy->GetPos() );
			//��Ԃ���
			CAMERA_PARA CameraPara = CCameraManager::GetCameraPara(CCameraManager::CAMERA_PLAYER);	//�J�����f�[�^�擾
			m_PlayerCamera->CameraAndLookatToGoalLinearInterpolation(&CameraPara.pos, &CameraPara.lookat);
			//��Ԍ���W�����J�����ɑ��
			CCameraManager::SetCameraAndLookat(CCameraManager::CAMERA_PLAYER, CameraPara.pos, CameraPara.lookat);
		}
	}
}