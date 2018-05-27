//���� : �f�v�X�V���h�E���������邽�߂̃��C�g�����̃J����
#ifndef CCAMERA_ORTHO_H
#define CCAMERA_ORTHO_H

#include "CCamera.h"

class CCameraLight : public CCamera
{
public:
	CCameraLight();
	~CCameraLight();

	//Init�֐�
	virtual void Init(const D3DXVECTOR3 &LookatPos, const D3DXVECTOR3 &LightDir, const D3DXVECTOR3 &Up);

	//�t�@���N�V�����֐�
	virtual void SetViewMtx(void);
	virtual void SetProjectionMtx(void);
	void SetCameraPosAndLightDir(const D3DXVECTOR3 &CameraPos, const D3DXVECTOR3 &LightDir, float Distance);
	void SetCameraLookAt(const D3DXVECTOR3 &CameraPos, const D3DXVECTOR3 &LookatPos);
	void SetFar(float Far) { m_far = Far; }
private:
	D3DXVECTOR3 m_LightDir;
};

#endif