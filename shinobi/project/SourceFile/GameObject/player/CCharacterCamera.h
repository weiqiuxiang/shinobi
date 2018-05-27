/*==============================================================================
	�w�b�_����:
		�L�����N�^�[�ɂ������Ă���J�����̓���
===============================================================================*/
#ifndef CCHARACTER_CAMERA_H_
#define CCHARACTER_CAMERA_H_

#include "main.h"

//�N���X�錾
class CCharacterCamera
{
public:
	CCharacterCamera(const D3DXVECTOR3& CameraPos , const D3DXVECTOR3& LookatPos ,float HorizonAngle , float VerticleAngle);
	~CCharacterCamera() {}

	void CalcCameraAndLookatPos(const D3DXVECTOR3& CharacterPos);		//�L�����N�^�[�̈ʒu�ŃJ�����ƒ����_�̖ړI�ʒu���v�Z
	void CalcCameraAndLookatPosLockOn(const D3DXVECTOR3& CharacterPos,const D3DXVECTOR3& LockOnPos);	//�L�����N�^�[�ƃ��b�N�I���ڕW�̈ʒu�ŃJ�����ƒ����_�̖ړI�ʒu���v�Z
	void TpsCameraRotateStick( const D3DXVECTOR2& InputValue);
	void TpsCameraRotateKeyboard(float HorizonValue, float VerticalValue);
	void CameraAndLookatToGoalLinearInterpolation(D3DXVECTOR3 *pCameraPos, D3DXVECTOR3 *pLookatPos);	//�J�����ƒ����_���S�[���Ɍ������Ĉړ��A�ړ����x�̓S�[���̋����Ɗ֌W
	void CameraAndLookatToGoalCircleInterpolation(D3DXVECTOR3 *pCameraPos, D3DXVECTOR3 *pLookatPos);	//LockOnPos����]���Ƃ��A���݂̃J�����ƒ����_���S�[���Ɍ������ĉ�]����
private:
	CCharacterCamera();
	void CameraTpsRotation(float Percent, float HorizonAngle, float VerticalAngle);		//�J������TPS��]
private:
	D3DXVECTOR3			m_CameraGoal;		//�J�����̈ʒu���W
	D3DXVECTOR3			m_LookatGoal;		//�����_�̈ʒu���W
	float				m_AngleHori;		//�����p�x
	float				m_AngleVerti;		//�����p�x
};

#endif