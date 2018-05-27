#include "CCharacterCamera.h"
#include "CCameraManager.h"
#include "CCamera.h"
#include "CPolygonMath.h"
#include "CCameraManager.h"

/*==============================================================================
	�}�N��
===============================================================================*/
#define CAMERA_RELATIVE_DIST_FROM_CHARA (10.0f)		//�J�����ƃL�����N�^�[�̑��΋���
#define LOOKAT_RELATIVE_DIST_FROM_CHARA (16.6f)		//�����_�ƃL�����N�^�[�̑��΋���

#define RELATIVE_DIST_Y (5.0f)			//�J�����ƒ����_�̃L�����N�^�[�̑���Y����

#define VERTICAL_ANGLE_START_INTERPO_XZDISTANCE (10.0f)	//�J���������p�x��ԊJ�nXZ���ʂ̋���
#define VERTICAL_ANGLE_START_INTERPO_YDISTANCE (-2.0f)	//�J���������p�x��ԊJ�nY����
#define DEFAULT_VERTICAL_ANGLE (D3DX_PI*0.15f)			//���b�N�I���̎��J�����̃f�t�H���g�����p�x
#define MAX_VERTICAL_ANGLE (D3DX_PI*0.35f)				//���b�N�I���̎��J�����̍ő吂���p�x

#define CAMERA_ROTATION_SPEED_HORIZON (0.09f)			//�J�����̐�����]���x
#define CAMERA_ROTATION_SPEED_VERTICAL (0.09f)			//�J�����̐�����]���x

//�J�����ƒ����_�̐��`��ԂɊ֘A����}�N��
#define CAMERA_LINER_INTERPOLATION_SPEED (0.16f)		//�J�����̐��`��Ԃ̑��x
#define LOOKAT_LINER_INTERPOLATION_SPEED (0.24f)		//�����_�̐��`��Ԃ̑��x

//�J�����ƒ����_�̉~�`�ړ���ԂɊ֘A����}�N��
#define CIRCLE_INTERPO_BASE_SPEED (0.4f * D3DX_PI)		//�J�����̉~��Ԃ̎��̊�{��]���x
#define RADIUS_INTERPO_BASE_SPEED (0.1f)				//���a�����̕�Ԋ�{���x
#define YPOS_INTERPO_BASE_SPEED (0.1f)				//Y�����̕�Ԋ�{���x

/*==============================================================================
	�R���X�g���N�^
===============================================================================*/
CCharacterCamera::CCharacterCamera()
{
	m_CameraGoal = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_LookatGoal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_AngleHori = 0;
	m_AngleVerti = 0;
}

CCharacterCamera::CCharacterCamera(const D3DXVECTOR3& CameraPos, const D3DXVECTOR3& LookatPos, float HorizonAngle, float VerticleAngle) 
{
	m_CameraGoal = CameraPos;
	m_LookatGoal = LookatPos;
	m_AngleHori = HorizonAngle;
	m_AngleVerti = VerticleAngle;
}

/*==============================================================================
	�L�����N�^�[�̈ʒu�ŃJ�����ƒ����_�̖ړI�ʒu���v�Z
===============================================================================*/
void CCharacterCamera::CalcCameraAndLookatPos(const D3DXVECTOR3& CharacterPos)
{
	//�L����->�J���������x�N�g���̌v�Z
	CAMERA_PARA CameraPara = CCameraManager::GetCameraPara(CCameraManager::CAMERA_PLAYER);
	D3DXVECTOR3 CameraPlayerVec = CharacterPos - CameraPara.pos;
	CameraPlayerVec.y = 0;
	D3DXVec3Normalize(&CameraPlayerVec, &CameraPlayerVec);

	//�J�����ƒ����_�̖ړI�ʒu�̌v�Z
	D3DXVECTOR3 Camera, Lookat;
	Camera = CharacterPos - CAMERA_RELATIVE_DIST_FROM_CHARA * cosf(m_AngleVerti) * CameraPlayerVec;
	Lookat = CharacterPos + LOOKAT_RELATIVE_DIST_FROM_CHARA * cosf(-m_AngleVerti) * CameraPlayerVec;
	Camera.y = CAMERA_RELATIVE_DIST_FROM_CHARA * sinf(m_AngleVerti) + CharacterPos.y + RELATIVE_DIST_Y;
	Lookat.y = LOOKAT_RELATIVE_DIST_FROM_CHARA * sinf(-m_AngleVerti) + CharacterPos.y + RELATIVE_DIST_Y;
	m_CameraGoal = Camera;					//�J�����S�[�����W�ݒ�
	m_LookatGoal = Lookat;					//�����_�S�[�����W�ݒ�

	//�p�x�̎Z�o
	D3DXVECTOR2 GoalAngle = CPolygonMath::CalcTwoPoint_HorizonAngleAndVerticleAngle(m_CameraGoal, m_LookatGoal);
	m_AngleHori = GoalAngle.x;
	m_AngleVerti = GoalAngle.y;
}

/*==============================================================================
	�L�����N�^�[�ƃ��b�N�I���ڕW�̈ʒu�ŃJ�����ƒ����_�̖ړI�ʒu���v�Z
		CharacterPos : �L�����N�^�[�ʒu
		LockOnPos : ���b�N�I���ڕW�̈ʒu
===============================================================================*/
void CCharacterCamera::CalcCameraAndLookatPosLockOn(const D3DXVECTOR3& CharacterPos,const D3DXVECTOR3& LockOnPos)
{
	//�L�����N�^�[�����b�N�I���ڕW�Ɍ����x�N�g�����v�Z
	D3DXVECTOR3 CharaLockonVec = LockOnPos - CharacterPos;
	D3DXVec3Normalize(&CharaLockonVec, &CharaLockonVec);

	//�L�����N�^�[�ƃ��b�N�I���ڕW�̋����͈��l��菬�����Ȏ��A�J�����ƒ����_�̐����p�x���ς��
	float fVertiAngle = 0;
	float DistanceY = CharacterPos.y - LockOnPos.y;

	if (DistanceY > VERTICAL_ANGLE_START_INTERPO_YDISTANCE) {
		float Distance = D3DXVec3Length(&(CharacterPos - LockOnPos));
		if (Distance < VERTICAL_ANGLE_START_INTERPO_XZDISTANCE) {
			float t = Distance / VERTICAL_ANGLE_START_INTERPO_XZDISTANCE;
			fVertiAngle = DEFAULT_VERTICAL_ANGLE + (MAX_VERTICAL_ANGLE - DEFAULT_VERTICAL_ANGLE) * (1 - t);		//���`���
		}
		else { 
			fVertiAngle = DEFAULT_VERTICAL_ANGLE; 
		}
	}
	else {
		float t = VERTICAL_ANGLE_START_INTERPO_YDISTANCE / DistanceY;
		fVertiAngle = DEFAULT_VERTICAL_ANGLE + (DEFAULT_VERTICAL_ANGLE - MAX_VERTICAL_ANGLE) * (1 - t);
	}

	//�J�����̎��_�ƒ����_�̖ڕW�ʒu�̌v�Z
	D3DXVECTOR3 Camera = CharacterPos - CharaLockonVec * cosf(fVertiAngle) * CAMERA_RELATIVE_DIST_FROM_CHARA;
	D3DXVECTOR3 Lookat = CharacterPos + CharaLockonVec * cosf(-fVertiAngle) * LOOKAT_RELATIVE_DIST_FROM_CHARA;
	Camera.y = CAMERA_RELATIVE_DIST_FROM_CHARA * sinf(fVertiAngle) + CharacterPos.y + RELATIVE_DIST_Y;
	Lookat.y = LOOKAT_RELATIVE_DIST_FROM_CHARA * sinf(-fVertiAngle) + CharacterPos.y + RELATIVE_DIST_Y;
	m_CameraGoal = Camera;					//�J�����S�[�����W�ݒ�
	m_LookatGoal = Lookat;					//�����_�S�[�����W�ݒ�
}

/*==============================================================================
�X�e�B�b�N�̓��͂ŃL�����N�^�[�����Ɏ��_�ƒ����_�̉�]
	CharacterPos : �L�����N�^�[�ʒu
===============================================================================*/
void CCharacterCamera::TpsCameraRotateStick(const D3DXVECTOR2& InputValue)
{
	//�X�e�B�b�N�̓��͂ŃJ��������]
	float Horizon = InputValue.x * -CAMERA_ROTATION_SPEED_HORIZON;
	float Vertical = InputValue.y * -CAMERA_ROTATION_SPEED_VERTICAL;

	float Percent = CAMERA_RELATIVE_DIST_FROM_CHARA / (CAMERA_RELATIVE_DIST_FROM_CHARA + LOOKAT_RELATIVE_DIST_FROM_CHARA);
	CCameraManager::TpsRotationBasePercent(Percent, Horizon, Vertical, D3DX_PI*0.3);
	CameraTpsRotation(Percent, Horizon, Vertical);
}

/*==============================================================================
�L�[�{�[�h�̓��͂ŃL�����N�^�[�����Ɏ��_�ƒ����_�̉�]
		CharacterPos : �L�����N�^�[�ʒu
		HorizonDir : �����̓��͕���(false : �� , true : �E)
		VerticalDir : �����̓��͕���(false : �� , true : ��)
===============================================================================*/
void CCharacterCamera::TpsCameraRotateKeyboard( float HorizonValue, float VerticalValue)
{
	//��]�p�x
	float Horizon = 0;
	float Vertical = 0;
	
	//�L�[�{�[�h�̓��͂ŃJ��������]
	Vertical = CAMERA_ROTATION_SPEED_VERTICAL * VerticalValue;
	Horizon =  CAMERA_ROTATION_SPEED_HORIZON * HorizonValue;

	float Percent = CAMERA_RELATIVE_DIST_FROM_CHARA / (CAMERA_RELATIVE_DIST_FROM_CHARA + LOOKAT_RELATIVE_DIST_FROM_CHARA);
	CCameraManager::TpsRotationBasePercent(Percent, Horizon, Vertical, D3DX_PI*0.3);
	CameraTpsRotation(Percent, Horizon, Vertical);
}

/*==============================================================================
	���݃J�����̈ʒu���ڕW�ʒu�Ɍ����Đ��`��Ԃ���
		pCameraPos : �������J�����̈ʒu
		pLookatPos : �����������_�̈ʒu
===============================================================================*/
void CCharacterCamera::CameraAndLookatToGoalLinearInterpolation(D3DXVECTOR3 *pCameraPos, D3DXVECTOR3 *pLookatPos)
{
	//�k���`�F�b�N
	if (pCameraPos == nullptr || pLookatPos == nullptr) {
		return;
	}

	//�J�����̌��ʒu->�ڕW�ʒu�x�N�g��������
	D3DXVECTOR3 CameraGoalVec = m_CameraGoal - *pCameraPos;
	float CameraGoalDistance = D3DXVec3Length(&CameraGoalVec);
	D3DXVec3Normalize(&CameraGoalVec, &CameraGoalVec);

	//�����_�̌��ʒu->�ڕW�ʒu�x�N�g��������
	D3DXVECTOR3 LookatGoalVec = m_LookatGoal - *pLookatPos;
	float LookatGoalDistance = D3DXVec3Length(&LookatGoalVec);
	D3DXVec3Normalize(&LookatGoalVec, &LookatGoalVec);

	//���`��� : pCameraPos��pLookatPos���S�[�����_���������ē�����
	if (CameraGoalDistance < 0.1f) {
		*pCameraPos = m_CameraGoal;
	}
	else {
		*pCameraPos += CameraGoalDistance * CAMERA_LINER_INTERPOLATION_SPEED * CameraGoalVec;
	}
	if (LookatGoalDistance < 0.2f) {
		*pLookatPos = m_LookatGoal;
	}
	else {
		*pLookatPos += LookatGoalDistance * LOOKAT_LINER_INTERPOLATION_SPEED * LookatGoalVec;
	}
}

/*==============================================================================
���݃J�����̈ʒu���ڕW�ʒu�Ɍ����Đ��`��Ԃ���
	pCameraPos : �������J�����̈ʒu
	pLookatPos : �����������_�̈ʒu
===============================================================================*/
void CCharacterCamera::CameraAndLookatToGoalCircleInterpolation(D3DXVECTOR3 *pCameraPos, D3DXVECTOR3 *pLookatPos)
{
	//�k���`�F�b�N
	if (pCameraPos == nullptr || pLookatPos == nullptr) {
		return;
	}

	D3DXVECTOR2 NowRayVec = //���J�������猻�����_�Ɍ�����XZ���ʂ̕����x�N�g�������߂�
		D3DXVECTOR2(pLookatPos->x, pLookatPos->z) - D3DXVECTOR2(pCameraPos->x, pCameraPos->z);
	D3DXVECTOR2 GoalRayVec = //�ڕW�J��������ڕW�����_�Ɍ�����XZ���ʂ̕����x�N�g��������
		D3DXVECTOR2(m_LookatGoal.x, m_LookatGoal.z) - D3DXVECTOR2(m_CameraGoal.x, m_CameraGoal.z);
	D3DXVECTOR2 NowToGoalVec = //���J�����ƖڕW�J�����̃x�N�g�������߂�
		D3DXVECTOR2(m_CameraGoal.x, m_CameraGoal.z) - D3DXVECTOR2(pCameraPos->x, pCameraPos->z);

	//����������߂�
	float Nowt = D3DXVec2CCW(&NowToGoalVec, &GoalRayVec) / D3DXVec2CCW(&NowRayVec, &GoalRayVec);
	float Goalt = D3DXVec2CCW(&NowToGoalVec, &NowRayVec) / D3DXVec2CCW(&NowRayVec, &GoalRayVec);

	//�����͂Ȃ��Ȃ�A���`��Ԃɐ؂�ւ�
	bool IsCross = 
		( (Nowt >= 0) && (Nowt <= 1) ) &&
		( (Goalt >= 0) && (Goalt <= 1) );

	if (IsCross == false) {
		CameraAndLookatToGoalLinearInterpolation(pCameraPos, pLookatPos);
		return;
	}

	//��]���S�̈ʒu�����߂�
	D3DXVECTOR2 RorateCenterXZ = D3DXVECTOR2(m_CameraGoal.x, m_CameraGoal.z) + Goalt * GoalRayVec;

	//�x�N�g���ƃx�N�g���̂Ȃ��p�����߂�
	float Angle = D3DXVec2CCW(&NowRayVec, &GoalRayVec) / (D3DXVec2Length(&NowRayVec) * D3DXVec2Length(&GoalRayVec));
	Angle = asinf(Angle);

	//��]���x������
	float Rate = Angle / (D3DX_PI * 2);

	//�p�x�͈��l�ȉ��̎��A�S�[����XZ���W������XZ���W�ɑ��
	if(fabs(Rate) > CIRCLE_INTERPO_BASE_SPEED * 0.1f){
		//�p�x�̐��`���
		float RotSpeed = Rate * CIRCLE_INTERPO_BASE_SPEED;

		//��]�s������߂�
		D3DXMATRIX RotMtxY;
		D3DXMatrixRotationY(&RotMtxY, RotSpeed);

		//��]��̃J�����ƒ����_�̍��W�����߂�
		D3DXVECTOR2 CameraOriginPos = D3DXVECTOR2(pCameraPos->x, pCameraPos->z) - RorateCenterXZ;	//���_����]���S�ɂ��鎞�̈ʒu
		D3DXVECTOR3 CameraRotPos;
		D3DXVec3TransformCoord(&CameraRotPos, &D3DXVECTOR3(CameraOriginPos.x, 0.0f, CameraOriginPos.y), &RotMtxY);
		CameraRotPos = CameraRotPos + D3DXVECTOR3(RorateCenterXZ.x, pCameraPos->y, RorateCenterXZ.y);

		D3DXVECTOR2 LookatOriginPos = D3DXVECTOR2(pLookatPos->x, pLookatPos->z) - RorateCenterXZ;	//���_����]���S�ɂ��鎞�̈ʒu
		D3DXVECTOR3 LookatRotPos;
		D3DXVec3TransformCoord(&LookatRotPos, &D3DXVECTOR3(LookatOriginPos.x, 0.0f, LookatOriginPos.y), &RotMtxY);
		LookatRotPos = LookatRotPos + D3DXVECTOR3(RorateCenterXZ.x, pLookatPos->y, RorateCenterXZ.y);

		//���ʂ���
		*pCameraPos = CameraRotPos;
		*pLookatPos = LookatRotPos;
	}

	//���a�����̐��`���
	float DistXZNowCameraToRorateCenter = D3DXVec2Length( &D3DXVECTOR2(pCameraPos->x - RorateCenterXZ.x, pCameraPos->z - RorateCenterXZ.y) );
	float DistXZGoalCameraToRorateCenter = D3DXVec2Length( &D3DXVECTOR2(m_CameraGoal.x - RorateCenterXZ.x, m_CameraGoal.z - RorateCenterXZ.y) );
	float RadiusDist = DistXZGoalCameraToRorateCenter - DistXZNowCameraToRorateCenter;		//���a�����̋�����
	if (fabs(RadiusDist) > RADIUS_INTERPO_BASE_SPEED){
		D3DXVECTOR3 RadiusVec = *pLookatPos - *pCameraPos;	//���a�����̃x�N�g��
		RadiusVec.y = 0.0f;
		D3DXVec3Normalize(&RadiusVec, &RadiusVec);
		*pCameraPos = *pCameraPos + RadiusDist * RADIUS_INTERPO_BASE_SPEED * RadiusVec;
		*pLookatPos = *pLookatPos + RadiusDist * RADIUS_INTERPO_BASE_SPEED * RadiusVec;
	}

	//Y�����̐��`���
	float CameraYDist = m_CameraGoal.y - pCameraPos->y;
	if (fabs(CameraYDist) < YPOS_INTERPO_BASE_SPEED) {
		pCameraPos->y = m_CameraGoal.y;
	}
	else {
		pCameraPos->y += CameraYDist * YPOS_INTERPO_BASE_SPEED;
	}

	float LookatYDist = m_LookatGoal.y - pLookatPos->y;
	if (fabs(LookatYDist) < YPOS_INTERPO_BASE_SPEED) {
		pLookatPos->y = m_LookatGoal.y;
	}
	else {
		pLookatPos->y += LookatYDist * YPOS_INTERPO_BASE_SPEED;
	}
}

/*==============================================================================
	�J������TPS��]
===============================================================================*/
void CCharacterCamera::CameraTpsRotation(float Percent, float HorizonAngle, float VerticalAngle)
{
	//�p�[�Z���e�[�W�͈̔͌���
	if (Percent < 0 || Percent > 1) {
		MessageBox(nullptr, "�֐�TpsRotationGoal�̈���t�̒l�͕s�K��,�l�͈̔͂�0�`1", "WARNING", MB_OK | MB_ICONHAND);
		return;
	}

	float OldAngel = m_AngleVerti;

	m_AngleHori += HorizonAngle;
	m_AngleVerti += VerticalAngle;

	// -180�`180�x�Ɋۂ߂�
	if (m_AngleHori > D3DX_PI) { m_AngleHori -= 2 * D3DX_PI; }
	if (m_AngleHori < -D3DX_PI) { m_AngleHori += 2 * D3DX_PI; }

	//��������:���͈͂�-90�`90�x
	if (m_AngleVerti > 0.3 * D3DX_PI) { m_AngleVerti = OldAngel; }
	if (m_AngleVerti < -0.3 * D3DX_PI) { m_AngleVerti = OldAngel; }

	float Distance = CAMERA_RELATIVE_DIST_FROM_CHARA + LOOKAT_RELATIVE_DIST_FROM_CHARA;

	//��]���S�����߂�
	D3DXVECTOR3 CameraLookat = m_LookatGoal - m_CameraGoal;
	D3DXVECTOR3 RorateCenter = m_CameraGoal + CameraLookat * Percent;
	float DistaceCameraRotCenter = Distance * Percent;

	//�J�����̍��W������
	m_CameraGoal.x = DistaceCameraRotCenter * cosf(m_AngleHori) * cosf(m_AngleVerti) + RorateCenter.x;
	m_CameraGoal.z = DistaceCameraRotCenter * sinf(m_AngleHori) * cosf(m_AngleVerti) + RorateCenter.z;
	m_CameraGoal.y = DistaceCameraRotCenter * sinf(m_AngleVerti) + RorateCenter.y;

	//�����_�̍��W������
	D3DXVECTOR3 CameraRotCenterVec = RorateCenter - m_CameraGoal;
	D3DXVec3Normalize(&CameraRotCenterVec, &CameraRotCenterVec);
	m_LookatGoal = RorateCenter + CameraRotCenterVec* Distance * (1 - Percent);
}

