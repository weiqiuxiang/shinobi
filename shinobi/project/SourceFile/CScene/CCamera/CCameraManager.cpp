#include "CCameraManager.h"

CCameraManager::CAMERA_NAME CCameraManager::m_ActiveID = CCameraManager::CAMERA_PLAYER;
CCamera CCameraManager::m_CameraObj[CAMERA_MAX] = {};

void CCameraManager::InitAll(void)
{
	//すべて初期化
	m_ActiveID = CAMERA_PLAYER;

	for(int i = 0; i < CAMERA_MAX; i++)
	{
		m_CameraObj[i].Init(D3DXVECTOR3(0.0f, 0.0f, -5.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	}
}

CAMERA_PARA CCameraManager::GetCameraPara(CAMERA_NAME CameraName)
{
	if (CameraNameOverRangeCheck(CameraName) == true) CameraName = CAMERA_PLAYER;
	CAMERA_PARA CameraPara;
	CameraPara.pos = m_CameraObj[CameraName].GetCameraPos();
	CameraPara.lookat = m_CameraObj[CameraName].GetLookat();
	CameraPara.up = m_CameraObj[CameraName].GetCameraUp();
	return CameraPara;
}

void CCameraManager::ChangeActiveCamera(CCameraManager::CAMERA_NAME CameraName)
{
	if (CameraNameOverRangeCheck(CameraName) == true) return;
	m_ActiveID = CameraName;
}

void CCameraManager::SetCameraPara(CAMERA_NAME CameraName, const CAMERA_PARA &CameraPara)
{
	if (CameraNameOverRangeCheck(CameraName) == true) return;
	m_CameraObj[CameraName].Init(CameraPara.pos, CameraPara.lookat, CameraPara.up);
}

D3DXMATRIX CCameraManager::GetActiveCameraMtxVPVP(void)
{
	D3DXMATRIX MtxVPVP = CCameraManager::GetActiveCameraViewMtx() * CCameraManager::GetActiveCameraProjeMtx() *ScreenViewPortMtx;
	return MtxVPVP;
}

void CCameraManager::ActiveCamera_ViewProMtxSet(void)
{
	m_CameraObj[m_ActiveID].SetViewMtx();
	m_CameraObj[m_ActiveID].SetProjectionMtx();
}

bool CCameraManager::CameraNameOverRangeCheck(CAMERA_NAME CameraName)
{
	bool ID_OverRange = (CameraName < 0) || (CameraName >= CAMERA_MAX);
	if (ID_OverRange)
	{
		MessageBox(nullptr, "入力されたカメラIDが範囲外", "エラー", MB_OK | MB_ICONHAND);
		return true;
	}

	return false;
}
