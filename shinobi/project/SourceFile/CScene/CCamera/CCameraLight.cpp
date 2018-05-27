#include "CCameraLight.h"
#include "CPolygonMath.h"
#include "CRenderer.h"

#define CAMERA_LOOKAT_DISTANCE_DEFAULT (30.0f)	//�J�����ƒ����_�̃f�t�H���g����

CCameraLight::CCameraLight()
{
	m_LightDir = D3DXVECTOR3(0,0,0);
}

CCameraLight::~CCameraLight()
{
	
}

//Init�֐�
void CCameraLight::Init(const D3DXVECTOR3 &LookatPos, const D3DXVECTOR3 &LightDir, const D3DXVECTOR3 &Up) 
{
	//������
	m_PosLookatUp.pos = LookatPos - LightDir * CAMERA_LOOKAT_DISTANCE_DEFAULT;
	m_PosLookatUp.lookat = LookatPos;
	m_PosLookatUp.up = Up;
	m_LightDir = LightDir;

	m_fDistance = CAMERA_LOOKAT_DISTANCE_DEFAULT;

	//�����p�x�Z�o
	D3DXVECTOR3 CameraAngle = CPolygonMath::CalcTwoPoint_HorizonAngleAndVerticleAngle(m_PosLookatUp.pos, m_PosLookatUp.lookat);
	m_fAngleHorizon = CameraAngle.x;
	m_fAngleVertical = CameraAngle.y;

	//�}�g���N�X������
	D3DXMatrixIdentity(&m_ViewMtx);
	D3DXMatrixIdentity(&m_ProjeMtx);

	//�v���W�F�N�V�����s���
	D3DXMatrixOrthoLH(&m_ProjeMtx, 5.0f, 5.0f, 1.0f, m_far);
	D3DXMatrixLookAtLH(&m_ViewMtx, &m_PosLookatUp.pos, &m_PosLookatUp.lookat, &m_PosLookatUp.up);
}

//�t�@���N�V�����֐�
void CCameraLight::SetViewMtx(void) 
{
	D3DXMatrixLookAtLH(&m_ViewMtx, &m_PosLookatUp.pos, &m_PosLookatUp.lookat, &m_PosLookatUp.up);
}

void CCameraLight::SetProjectionMtx(void) 
{
	D3DXMatrixOrthoLH(&m_ProjeMtx, 60.0f, 60.0f, 1.0f, m_far);
}

void CCameraLight::SetCameraPosAndLightDir(const D3DXVECTOR3 &CameraPos, const D3DXVECTOR3 &LightDir, float Distance)
{
	m_PosLookatUp.pos = CameraPos;
	m_LightDir = LightDir;
	m_PosLookatUp.lookat = m_PosLookatUp.pos + m_LightDir * Distance;
}

void CCameraLight::SetCameraLookAt(const D3DXVECTOR3 &CameraPos, const D3DXVECTOR3 &LookatPos)
{
	m_PosLookatUp.pos = CameraPos;
	m_PosLookatUp.lookat = LookatPos;
	D3DXVec3Normalize(&m_LightDir,&(LookatPos - CameraPos));
}
