#include "CCamera.h"
#include "CRenderer.h"
#include "CPolygonMath.h"

#define CAMERA_LOOKAT_DISTANCE_MIN (1.0f)		//�J�����ƒ����_�̍ŒZ����
#define CAMERA_LOOKAT_DISTANCE_MAX (1000.0f)	//�J�����ƒ����_�̍ő勗��

CCamera::CCamera()
{
	m_PosLookatUp.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_PosLookatUp.lookat = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_PosLookatUp.up = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fDistance = 0.0f;
	m_fAngleHorizon = 0.0f;
	m_fAngleVertical = 0.0f;
	m_far = 500.0f;
}

CCamera::~CCamera()
{
	
}

void CCamera::Init(const D3DXVECTOR3 &CameraPos, const D3DXVECTOR3 &Lookat, const D3DXVECTOR3 &Up)
{
	//������
	m_PosLookatUp.pos = CameraPos;
	m_PosLookatUp.lookat = Lookat;
	m_PosLookatUp.up = Up;

	D3DXVECTOR3 DistanceVec = m_PosLookatUp.lookat - m_PosLookatUp.pos;
	float fDistance = D3DXVec3Length(&DistanceVec);
	bool fDistanceOverRange = fDistance < CAMERA_LOOKAT_DISTANCE_MIN || fDistance > CAMERA_LOOKAT_DISTANCE_MAX;

	//�J�����ƒ����_�̋������͈͊O���ǂ���
	if(fDistanceOverRange)
	{
		D3DXVec3Normalize (&DistanceVec, &DistanceVec);
		m_PosLookatUp.lookat = m_PosLookatUp.pos + CAMERA_LOOKAT_DISTANCE_MIN * DistanceVec;
		m_fDistance = CAMERA_LOOKAT_DISTANCE_MIN;
	}
	else m_fDistance = fDistance;

	//�����p�x�Z�o
	D3DXVECTOR3 CameraAngle = CPolygonMath::CalcTwoPoint_HorizonAngleAndVerticleAngle(m_PosLookatUp.pos, m_PosLookatUp.lookat);
	m_fAngleHorizon = CameraAngle.x;
	m_fAngleVertical = CameraAngle.y;

	//�t�@�[���`
	m_far = 500.0f;

	//�}�g���N�X������
	D3DXMatrixIdentity(&m_ViewMtx);
	D3DXMatrixIdentity(&m_ProjeMtx);
	D3DXMatrixLookAtLH(&m_ViewMtx, &m_PosLookatUp.pos, &m_PosLookatUp.lookat, &m_PosLookatUp.up); //�r���[�}�g���N�X����
	D3DXMatrixPerspectiveFovLH(&m_ProjeMtx, D3DX_PI / 3.0f , (float)SCREEN_WIDTH / SCREEN_HEIGHT, 1.0f, m_far);
}

void CCamera::SetViewMtx(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();
	D3DXMatrixLookAtLH(&m_ViewMtx, &m_PosLookatUp.pos, &m_PosLookatUp.lookat, &m_PosLookatUp.up); //�r���[�}�g���N�X����
	pDevice->SetTransform(D3DTS_VIEW, &m_ViewMtx);      //�r���[�ϊ��s��
}

void CCamera::SetProjectionMtx(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();
	D3DXMatrixPerspectiveFovLH(&m_ProjeMtx, D3DX_PI / 3.0f, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 1.0f, m_far);
	pDevice->SetTransform(D3DTS_PROJECTION, &m_ProjeMtx);
}

void CCamera::SetCameraLinkLookat(const D3DXVECTOR3& CameraPos)
{
	D3DXVECTOR3 CameraVec3 = m_PosLookatUp.lookat - m_PosLookatUp.pos;
	m_PosLookatUp.pos = CameraPos;
	m_PosLookatUp.lookat = CameraVec3 + m_PosLookatUp.pos;
}

void CCamera::SetLookatLinkCamera(const D3DXVECTOR3& LookatPos)
{
	D3DXVECTOR3 CameraVec3 = m_PosLookatUp.lookat - m_PosLookatUp.pos;
	m_PosLookatUp.lookat = LookatPos;
	m_PosLookatUp.pos = m_PosLookatUp.lookat - CameraVec3;
}

void CCamera::SetCameraAndLookat(const D3DXVECTOR3& CameraPos, const D3DXVECTOR3& Lookat)
{
	//�l���
	m_PosLookatUp.pos = CameraPos;
	m_PosLookatUp.lookat = Lookat;

	//�����v�Z
	D3DXVECTOR3 CameraVec3 = m_PosLookatUp.lookat - m_PosLookatUp.pos;
	m_fDistance = D3DXVec3Length(&CameraVec3);

	//�p�x�v�Z
	D3DXVECTOR3 CameraAngle = CPolygonMath::CalcTwoPoint_HorizonAngleAndVerticleAngle(m_PosLookatUp.pos, m_PosLookatUp.lookat);
	m_fAngleHorizon = CameraAngle.x;
	m_fAngleVertical = CameraAngle.y;
}
void CCamera::SetCamera(const D3DXVECTOR3& CameraPos)
{
	m_PosLookatUp.pos = CameraPos;
	D3DXVECTOR3 CameraVec3 = m_PosLookatUp.lookat - m_PosLookatUp.pos;
	m_fDistance = D3DXVec3Length(&CameraVec3);                               //�����v�Z

	//�p�x�v�Z
	D3DXVECTOR3 CameraAngle = CPolygonMath::CalcTwoPoint_HorizonAngleAndVerticleAngle(m_PosLookatUp.pos, m_PosLookatUp.lookat);
	m_fAngleHorizon = CameraAngle.x;
	m_fAngleVertical = CameraAngle.y;
}

void CCamera::SetLookat(const D3DXVECTOR3& LookatPos)
{
	m_PosLookatUp.lookat = LookatPos;
	D3DXVECTOR3 CameraVec3 = m_PosLookatUp.lookat - m_PosLookatUp.pos;
	m_fDistance = D3DXVec3Length(&CameraVec3);                               //�����v�Z

	//�p�x�v�Z
	D3DXVECTOR3 CameraAngle = CPolygonMath::CalcTwoPoint_HorizonAngleAndVerticleAngle(m_PosLookatUp.pos, m_PosLookatUp.lookat);
	m_fAngleHorizon = CameraAngle.x;
	m_fAngleVertical = CameraAngle.y;
}

void CCamera::Zoom(float fDistance)
{
	if (0 == fDistance) return;
	D3DXVECTOR3 DistanceVec = m_PosLookatUp.lookat - m_PosLookatUp.pos;   //�����x�N�g��
	float fDistanceNow = D3DXVec3Length(&DistanceVec);
	float fChangeDistance = fDistanceNow + fDistance;                 //�ω���̋���

	//��������
	if (fChangeDistance < CAMERA_LOOKAT_DISTANCE_MIN) fChangeDistance = CAMERA_LOOKAT_DISTANCE_MIN;
	if (fChangeDistance > CAMERA_LOOKAT_DISTANCE_MAX) fChangeDistance = CAMERA_LOOKAT_DISTANCE_MAX;

	D3DXVECTOR3 DirectionVec;                                         //�����x�N�g��
	D3DXVec3Normalize(&DirectionVec, &DistanceVec);

	m_PosLookatUp.pos = m_PosLookatUp.lookat - fChangeDistance * DirectionVec;
	m_fDistance = fChangeDistance;
}

void CCamera::FpsRotation(float fHorizonAdd, float fVerticalAdd)
{
	float OldAngel = m_fAngleVertical;

	m_fAngleHorizon += fHorizonAdd;
	m_fAngleVertical += fVerticalAdd;

	// -180�`180�x�Ɋۂ߂�
	if (m_fAngleHorizon > D3DX_PI) { m_fAngleHorizon -= 2 * D3DX_PI; }
	if (m_fAngleHorizon < -D3DX_PI) { m_fAngleHorizon += 2 * D3DX_PI; }

	//��������:���͈͂�-90�`90�x
	if (m_fAngleVertical > 0.5 * D3DX_PI) { m_fAngleVertical = OldAngel; }
	if (m_fAngleVertical < -0.5 * D3DX_PI) { m_fAngleVertical = OldAngel; }

	m_PosLookatUp.lookat.x = m_fDistance * cosf(m_fAngleHorizon) * cosf(m_fAngleVertical) + m_PosLookatUp.pos.x;
	m_PosLookatUp.lookat.z = m_fDistance * sinf(m_fAngleHorizon) * cosf(m_fAngleVertical) + m_PosLookatUp.pos.z;
	m_PosLookatUp.lookat.y = m_fDistance * sinf(m_fAngleVertical) + m_PosLookatUp.pos.y;
}

void CCamera::TpsRotation(float fHorizonAdd, float fVerticalAdd, float limitVertiAngle)
{
	float OldAngel = m_fAngleVertical;
	limitVertiAngle = (float)max(0, limitVertiAngle);
	limitVertiAngle = (float)min(D3DX_PI * 0.5f, limitVertiAngle);

	m_fAngleHorizon += fHorizonAdd;
	m_fAngleVertical += fVerticalAdd;

	// -180�`180�x�Ɋۂ߂�
	if (m_fAngleHorizon > D3DX_PI) { m_fAngleHorizon -= 2 * D3DX_PI; }
	if (m_fAngleHorizon < -D3DX_PI) { m_fAngleHorizon += 2 * D3DX_PI; }

	//��������:���͈͂�-90�`90�x
	if (m_fAngleVertical > limitVertiAngle) { m_fAngleVertical = OldAngel; }
	if (m_fAngleVertical < -limitVertiAngle) { m_fAngleVertical = OldAngel; }

	m_PosLookatUp.pos.x = m_fDistance * cosf(m_fAngleHorizon) * cosf(m_fAngleVertical) + m_PosLookatUp.lookat.x;
	m_PosLookatUp.pos.z = m_fDistance * sinf(m_fAngleHorizon) * cosf(m_fAngleVertical) + m_PosLookatUp.lookat.z;
	m_PosLookatUp.pos.y = m_fDistance * sinf(m_fAngleVertical) + m_PosLookatUp.lookat.y;
}

void CCamera::MoveXZ(const D3DXVECTOR2 &fDirection, float fDistance)
{
	D3DXVECTOR2 DirectionVec2;
	D3DXVec2Normalize(&DirectionVec2,&fDirection);

	m_PosLookatUp.pos.x += DirectionVec2.x * fDistance;
	m_PosLookatUp.pos.z += DirectionVec2.y * fDistance;
	m_PosLookatUp.lookat.x += DirectionVec2.x * fDistance;
	m_PosLookatUp.lookat.z += DirectionVec2.y * fDistance;
}

void CCamera::TpsRotationBasePercent(float Percent, float fHorizonAdd/*�������񑝗�*/, float fVerticalAdd/*�������񑝗�*/,float limitVertiAngle)
{
	if (Percent < 0 || Percent > 1) {
		MessageBox(nullptr, "�֐�TpsRotationBasePercent�̈���Percent�̒l�͕s�K��,�l�͈̔͂�0�`1", "WARNING", MB_OK | MB_ICONHAND);
		return;
	}
	limitVertiAngle = (float)max(0, limitVertiAngle);
	limitVertiAngle = (float)min(D3DX_PI * 0.5f, limitVertiAngle);

	float OldAngel = m_fAngleVertical;

	m_fAngleHorizon += fHorizonAdd;
	m_fAngleVertical += fVerticalAdd;

	// -180�`180�x�Ɋۂ߂�
	if (m_fAngleHorizon > D3DX_PI) { m_fAngleHorizon -= 2 * D3DX_PI; }
	if (m_fAngleHorizon < -D3DX_PI) { m_fAngleHorizon += 2 * D3DX_PI; }

	//��������:���͈͂�-90�`90�x
	if (m_fAngleVertical > limitVertiAngle) { m_fAngleVertical = OldAngel; }
	if (m_fAngleVertical < -limitVertiAngle) { m_fAngleVertical = OldAngel; }

	D3DXVECTOR3 CameraLookat = m_PosLookatUp.lookat - m_PosLookatUp.pos;
	D3DXVECTOR3 RorateCenter = m_PosLookatUp.pos + CameraLookat * Percent;
	float DistaceCameraRotCenter = m_fDistance * Percent;

	//�J�����̍��W������
	m_PosLookatUp.pos.x = DistaceCameraRotCenter * cosf(m_fAngleHorizon) * cosf(m_fAngleVertical) + RorateCenter.x;
	m_PosLookatUp.pos.z = DistaceCameraRotCenter * sinf(m_fAngleHorizon) * cosf(m_fAngleVertical) + RorateCenter.z;
	m_PosLookatUp.pos.y = DistaceCameraRotCenter * sinf(m_fAngleVertical) + RorateCenter.y;

	//�����_�̍��W������
	D3DXVECTOR3 CameraRotCenterVec = RorateCenter - m_PosLookatUp.pos;
	D3DXVec3Normalize(&CameraRotCenterVec, &CameraRotCenterVec);
	m_PosLookatUp.lookat = RorateCenter + CameraRotCenterVec* m_fDistance * (1 - Percent);
}

void CCamera::Move3D(const D3DXVECTOR3 &fDirection, float fDistance)
{
	D3DXVECTOR3 DirectionVec3;
	D3DXVec3Normalize(&DirectionVec3, &fDirection);
	
	m_PosLookatUp.pos += DirectionVec3 * fDistance;
	m_PosLookatUp.lookat += DirectionVec3 * fDistance;
}

D3DXVECTOR3 CCamera::GetDirLookatToCamera(void)
{
	D3DXVECTOR3 Dir = m_PosLookatUp.pos - m_PosLookatUp.lookat;
	D3DXVec3Normalize(&Dir, &Dir);
	return Dir;
}

D3DXVECTOR3 CCamera::GetDirCameraToLookat(void)
{
	D3DXVECTOR3 Dir = m_PosLookatUp.lookat - m_PosLookatUp.pos;
	D3DXVec3Normalize(&Dir, &Dir);
	return Dir;
}