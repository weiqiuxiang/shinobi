#ifndef _CCAMERA_H_
#define _CCAMERA_H_

#include "main.h"

class CCamera
{
public:
	CCamera();
	~CCamera();

	//Init関数
	virtual void Init(const D3DXVECTOR3 &CameraPos, const D3DXVECTOR3 &Lookat, const D3DXVECTOR3 &Up);
	
	//ファンクション関数
	virtual void SetViewMtx(void);
	virtual void SetProjectionMtx(void);

	//セッター
	void SetCameraLinkLookat(const D3DXVECTOR3& CameraPos);
	void SetLookatLinkCamera(const D3DXVECTOR3& LookatPos);
	void SetCamera(const D3DXVECTOR3& CameraPos);
	void SetLookat(const D3DXVECTOR3& LookatPos);
	void SetCameraAndLookat(const D3DXVECTOR3& CameraPos, const D3DXVECTOR3& Lookat);
	void SetCameraUp(const D3DXVECTOR3 &Up) { m_PosLookatUp.up = Up; }

	//ゲッター
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

	//カメラ操作関数
	void Zoom(float fDistance/*距離*/);
	void FpsRotation(float fHorizonAdd, float fVerticalAdd);
	void TpsRotation(float fHorizonAdd,float fVerticalAdd, float limitVertiAngle);
	void TpsRotationBasePercent(float Percent, float fHorizonAdd, float fVerticalAdd, float limitVertiAngle);
	void MoveX(float fDistance) { m_PosLookatUp.pos.x += fDistance; m_PosLookatUp.lookat.x += fDistance;}
	void MoveY(float fDistance) { m_PosLookatUp.pos.y += fDistance; m_PosLookatUp.lookat.y += fDistance; }
	void MoveZ(float fDistance) { m_PosLookatUp.pos.z += fDistance; m_PosLookatUp.lookat.z += fDistance; }
	void MoveXZ(const D3DXVECTOR2 &fDirection,float fDistance);
	void Move3D(const D3DXVECTOR3 &fDirection, float fDistance);

protected:
	CAMERA_PARA		m_PosLookatUp;		//カメラの視点、注視点、上方向の値
	float			m_fAngleHorizon;	//カメラ旋回の水平方向角度
	float			m_fAngleVertical;	//カメラ旋回の垂直方向角度
	float			m_fDistance;		//カメラと注視点の距離
	D3DXMATRIX		m_ViewMtx;			//ビューマトリクス
	D3DXMATRIX		m_ProjeMtx;			//プロジェクションマトリクス
	float			m_far;				//ファー
};

#endif