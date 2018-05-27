//説明 : デプスシャドウを実現するためのライト方向のカメラ
#ifndef CCAMERA_ORTHO_H
#define CCAMERA_ORTHO_H

#include "CCamera.h"

class CCameraLight : public CCamera
{
public:
	CCameraLight();
	~CCameraLight();

	//Init関数
	virtual void Init(const D3DXVECTOR3 &LookatPos, const D3DXVECTOR3 &LightDir, const D3DXVECTOR3 &Up);

	//ファンクション関数
	virtual void SetViewMtx(void);
	virtual void SetProjectionMtx(void);
	void SetCameraPosAndLightDir(const D3DXVECTOR3 &CameraPos, const D3DXVECTOR3 &LightDir, float Distance);
	void SetCameraLookAt(const D3DXVECTOR3 &CameraPos, const D3DXVECTOR3 &LookatPos);
	void SetFar(float Far) { m_far = Far; }
private:
	D3DXVECTOR3 m_LightDir;
};

#endif