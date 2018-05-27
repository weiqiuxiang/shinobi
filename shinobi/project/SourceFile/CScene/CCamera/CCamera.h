#ifndef _CCAMERA_H_
#define _CCAMERA_H_

#include "main.h"

class CCamera
{
public:
	CCamera();
	~CCamera();

	//Init�֐�
	virtual void Init(const D3DXVECTOR3 &CameraPos, const D3DXVECTOR3 &Lookat, const D3DXVECTOR3 &Up);
	
	//�t�@���N�V�����֐�
	virtual void SetViewMtx(void);
	virtual void SetProjectionMtx(void);

	//�Z�b�^�[
	void SetCameraLinkLookat(const D3DXVECTOR3& CameraPos);
	void SetLookatLinkCamera(const D3DXVECTOR3& LookatPos);
	void SetCamera(const D3DXVECTOR3& CameraPos);
	void SetLookat(const D3DXVECTOR3& LookatPos);
	void SetCameraAndLookat(const D3DXVECTOR3& CameraPos, const D3DXVECTOR3& Lookat);
	void SetCameraUp(const D3DXVECTOR3 &Up) { m_PosLookatUp.up = Up; }

	//�Q�b�^�[
	D3DXVECTOR3 GetCameraPos(void) { return m_PosLookatUp.pos; }
	D3DXVECTOR3 GetLookat(void) { return m_PosLookatUp.lookat; }
	D3DXVECTOR3 GetCameraUp(void) { return m_PosLookatUp.up; }
	D3DXMATRIX GetViewMtx(void) { return m_ViewMtx; }
	D3DXMATRIX GetProjeMtx(void) { return m_ProjeMtx; }
	float GetCameraHoriAngle(void) { return m_fAngleHorizon; }
	float GetCameraVertiAngle(void) { return m_fAngleVertical; }
	float GetCameraDistance(void) { return m_fDistance; }
	D3DXVECTOR3 GetDirLookatToCamera(void);
	D3DXVECTOR3 GetDirCameraToLookat(void);
	float GetFar(void) { return m_far; }

	//�J��������֐�
	void Zoom(float fDistance/*����*/);
	void FpsRotation(float fHorizonAdd, float fVerticalAdd);
	void TpsRotation(float fHorizonAdd,float fVerticalAdd, float limitVertiAngle);
	void TpsRotationBasePercent(float Percent, float fHorizonAdd, float fVerticalAdd, float limitVertiAngle);
	void MoveX(float fDistance) { m_PosLookatUp.pos.x += fDistance; m_PosLookatUp.lookat.x += fDistance;}
	void MoveY(float fDistance) { m_PosLookatUp.pos.y += fDistance; m_PosLookatUp.lookat.y += fDistance; }
	void MoveZ(float fDistance) { m_PosLookatUp.pos.z += fDistance; m_PosLookatUp.lookat.z += fDistance; }
	void MoveXZ(const D3DXVECTOR2 &fDirection,float fDistance);
	void Move3D(const D3DXVECTOR3 &fDirection, float fDistance);

protected:
	CAMERA_PARA		m_PosLookatUp;		//�J�����̎��_�A�����_�A������̒l
	float			m_fAngleHorizon;	//�J��������̐��������p�x
	float			m_fAngleVertical;	//�J��������̐��������p�x
	float			m_fDistance;		//�J�����ƒ����_�̋���
	D3DXMATRIX		m_ViewMtx;			//�r���[�}�g���N�X
	D3DXMATRIX		m_ProjeMtx;			//�v���W�F�N�V�����}�g���N�X
	float			m_far;				//�t�@�[
};

#endif