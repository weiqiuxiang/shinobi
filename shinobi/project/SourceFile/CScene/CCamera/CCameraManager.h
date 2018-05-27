#pragma once
#ifndef _CCAMERA_MANAGER_H_
#define _CCAMERA_MANAGER_H_

#include "main.h"
#include "CCamera.h"

class CCameraManager
{
public:
	typedef enum
	{
		CAMERA_PLAYER = 0,
		CAMERA_EDIT01,
		CAMERA_EDIT02,
		CAMERA_LIGHT,
		CAMERA_MAX
	}CAMERA_NAME;

	//InitAll関数
	static void InitAll(void);

	//ファンクション関数
	static void ChangeActiveCamera(CCameraManager::CAMERA_NAME CameraID);

	//セッター
	static void SetCameraPara(CAMERA_NAME CameraName, const CAMERA_PARA &CameraPara);
	static void SetCameraLinkLookat(CAMERA_NAME CameraName,const D3DXVECTOR3& CameraPos) {
		if (CameraNameOverRangeCheck(CameraName) == true) return;
		m_CameraObj[CameraName].SetCameraLinkLookat(CameraPos);
	}
	static void SetLookatLinkCamera(CAMERA_NAME CameraName,const D3DXVECTOR3& LookatPos) {
		if (CameraNameOverRangeCheck(CameraName) == true) return;
		m_CameraObj[CameraName].SetLookatLinkCamera(LookatPos);
	}
	static void SetCamera(CAMERA_NAME CameraName, const D3DXVECTOR3& CameraPos){
		if (CameraNameOverRangeCheck(CameraName) == true) return;
		m_CameraObj[CameraName].SetCamera(CameraPos);
	}
	static void SetLookat(CAMERA_NAME CameraName, const D3DXVECTOR3& LookatPos) {
		if (CameraNameOverRangeCheck(CameraName) == true) return;
		m_CameraObj[CameraName].SetLookat(LookatPos);
	}
	static void SetCameraAndLookat(CAMERA_NAME CameraName, const D3DXVECTOR3& CameraPos,const D3DXVECTOR3& LookatPos){
		if (CameraNameOverRangeCheck(CameraName) == true) return;
		m_CameraObj[CameraName].SetCameraAndLookat(CameraPos,LookatPos);
	}

	//ゲッター
	static CAMERA_PARA GetCameraPara(CAMERA_NAME CameraName);
	static D3DXMATRIX GetActiveCameraViewMtx(void) { return m_CameraObj[m_ActiveID].GetViewMtx(); }
	static D3DXMATRIX GetActiveCameraProjeMtx(void) { return m_CameraObj[m_ActiveID].GetProjeMtx(); }
	static float GetHoriAngle(CAMERA_NAME CameraName) {
		if (CameraNameOverRangeCheck(CameraName) == true) return -1;
		return m_CameraObj[CameraName].GetCameraHoriAngle(); 
	}
	static float GetVertiAngle(CAMERA_NAME CameraName) {
		if (CameraNameOverRangeCheck(CameraName) == true) return -1;
		return m_CameraObj[CameraName].GetCameraVertiAngle();
	}
	static float GetDistance(CAMERA_NAME CameraName) { 
		if (CameraNameOverRangeCheck(CameraName) == true) return -1;
		return m_CameraObj[CameraName].GetCameraDistance(); 
	}
	static D3DXVECTOR3 GetDirLookatToCamera(CAMERA_NAME CameraName) {
		if (CameraNameOverRangeCheck(CameraName) == true) return D3DXVECTOR3(0.0f,0.0f,0.0f);
		return m_CameraObj[CameraName].GetDirLookatToCamera();
	}
	static D3DXVECTOR3 GetDirCameraToLookat(CAMERA_NAME CameraName) {
		if (CameraNameOverRangeCheck(CameraName) == true) return D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		return m_CameraObj[CameraName].GetDirCameraToLookat();
	}
	static CAMERA_NAME GetActiveCameraName(void) { return m_ActiveID; }
	static D3DXVECTOR3 GetActiveCameraPos(void) { return m_CameraObj[m_ActiveID].GetCameraPos(); }
	static D3DXMATRIX GetActiveCameraMtxVPVP(void);
	
	//使用中のカメラの操作関数
	static void ActiveCamera_ViewProMtxSet(void);
	static void Zoom(float fDistance/*距離*/) { m_CameraObj[m_ActiveID].Zoom(fDistance); }
	static void FpsRotation(float fHorizonAdd/*水平旋回増量*/, float fVerticalAdd/*垂直旋回増量*/) { m_CameraObj[m_ActiveID].FpsRotation(fHorizonAdd, fVerticalAdd); }
	static void TpsRotation(float fHorizonAdd/*水平旋回増量*/, float fVerticalAdd/*垂直旋回増量*/, float limitVertiAngle/*垂直制限角度*/) { m_CameraObj[m_ActiveID].TpsRotation(fHorizonAdd, fVerticalAdd, limitVertiAngle); }
	static void TpsRotationBasePercent(float Percent,float fHorizonAdd/*水平旋回増量*/, float fVerticalAdd/*垂直旋回増量*/, float limitVertiAngle/*垂直制限角度*/) { m_CameraObj[m_ActiveID].TpsRotationBasePercent(Percent,fHorizonAdd, fVerticalAdd, limitVertiAngle); }
	static void MoveX(float fDistance/*移動距離*/) { m_CameraObj[m_ActiveID].MoveX(fDistance); }
	static void MoveY(float fDistance/*移動距離*/) { m_CameraObj[m_ActiveID].MoveY(fDistance); }
	static void MoveZ(float fDistance/*移動距離*/) { m_CameraObj[m_ActiveID].MoveZ(fDistance); }
	static void MoveXZ(const D3DXVECTOR2 &fDirection/*移動方向*/, float fDistance/*移動距離*/) { m_CameraObj[m_ActiveID].MoveXZ(fDirection, fDistance); }
	static void Move3D(const D3DXVECTOR3 &fDirection/*移動方向*/, float fDistance/*移動距離*/) { m_CameraObj[m_ActiveID].Move3D(fDirection, fDistance); }

	//他の関数
	static bool CameraNameOverRangeCheck(CAMERA_NAME CameraName);

private:
	static CAMERA_NAME m_ActiveID;
	static CCamera m_CameraObj[CAMERA_MAX];
};

#endif