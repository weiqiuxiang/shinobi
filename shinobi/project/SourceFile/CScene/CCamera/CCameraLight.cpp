#include "CCameraLight.h"
#include "CPolygonMath.h"
#include "CRenderer.h"

#define CAMERA_LOOKAT_DISTANCE_DEFAULT (30.0f)	//カメラと注視点のデフォルト距離

CCameraLight::CCameraLight()
{
	m_LightDir = D3DXVECTOR3(0,0,0);
}

CCameraLight::~CCameraLight()
{
	
}

//Init関数
void CCameraLight::Init(const D3DXVECTOR3 &LookatPos, const D3DXVECTOR3 &LightDir, const D3DXVECTOR3 &Up) 
{
	//初期化
	m_PosLookatUp.pos = LookatPos - LightDir * CAMERA_LOOKAT_DISTANCE_DEFAULT;
	m_PosLookatUp.lookat = LookatPos;
	m_PosLookatUp.up = Up;
	m_LightDir = LightDir;

	m_fDistance = CAMERA_LOOKAT_DISTANCE_DEFAULT;

	//初期角度算出
	D3DXVECTOR3 CameraAngle = CPolygonMath::CalcTwoPoint_HorizonAngleAndVerticleAngle(m_PosLookatUp.pos, m_PosLookatUp.lookat);
	m_fAngleHorizon = CameraAngle.x;
	m_fAngleVertical = CameraAngle.y;

	//マトリクス初期化
	D3DXMatrixIdentity(&m_ViewMtx);
	D3DXMatrixIdentity(&m_ProjeMtx);

	//プロジェクション行列と
	D3DXMatrixOrthoLH(&m_ProjeMtx, 5.0f, 5.0f, 1.0f, m_far);
	D3DXMatrixLookAtLH(&m_ViewMtx, &m_PosLookatUp.pos, &m_PosLookatUp.lookat, &m_PosLookatUp.up);
}

//ファンクション関数
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
