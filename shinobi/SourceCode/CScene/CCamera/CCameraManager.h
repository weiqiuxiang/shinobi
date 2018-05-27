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

	//InitAll�֐�
	static void InitAll(void);

	//�t�@���N�V�����֐�
	static void ChangeActiveCamera(CCameraManager::CAMERA_NAME CameraID);

	//�Z�b�^�[
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

	//�Q�b�^�[
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
	
	//�g�p���̃J�����̑���֐�
	static void ActiveCamera_ViewProMtxSet(void);
	static void Zoom(float fDistance/*����*/) { m_CameraObj[m_ActiveID].Zoom(fDistance); }
	static void FpsRotation(float fHorizonAdd/*�������񑝗�*/, float fVerticalAdd/*�������񑝗�*/) { m_CameraObj[m_ActiveID].FpsRotation(fHorizonAdd, fVerticalAdd); }
	static void TpsRotation(float fHorizonAdd/*�������񑝗�*/, float fVerticalAdd/*�������񑝗�*/, float limitVertiAngle/*���������p�x*/) { m_CameraObj[m_ActiveID].TpsRotation(fHorizonAdd, fVerticalAdd, limitVertiAngle); }
	static void TpsRotationBasePercent(float Percent,float fHorizonAdd/*�������񑝗�*/, float fVerticalAdd/*�������񑝗�*/, float limitVertiAngle/*���������p�x*/) { m_CameraObj[m_ActiveID].TpsRotationBasePercent(Percent,fHorizonAdd, fVerticalAdd, limitVertiAngle); }
	static void MoveX(float fDistance/*�ړ�����*/) { m_CameraObj[m_ActiveID].MoveX(fDistance); }
	static void MoveY(float fDistance/*�ړ�����*/) { m_CameraObj[m_ActiveID].MoveY(fDistance); }
	static void MoveZ(float fDistance/*�ړ�����*/) { m_CameraObj[m_ActiveID].MoveZ(fDistance); }
	static void MoveXZ(const D3DXVECTOR2 &fDirection/*�ړ�����*/, float fDistance/*�ړ�����*/) { m_CameraObj[m_ActiveID].MoveXZ(fDirection, fDistance); }
	static void Move3D(const D3DXVECTOR3 &fDirection/*�ړ�����*/, float fDistance/*�ړ�����*/) { m_CameraObj[m_ActiveID].Move3D(fDirection, fDistance); }

	//���̊֐�
	static bool CameraNameOverRangeCheck(CAMERA_NAME CameraName);

private:
	static CAMERA_NAME m_ActiveID;
	static CCamera m_CameraObj[CAMERA_MAX];
};

#endif