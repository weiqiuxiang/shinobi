/*==============================================================================
	ヘッダ説明:
		キャラクターにくっついているカメラの動作
===============================================================================*/
#ifndef CCHARACTER_CAMERA_H_
#define CCHARACTER_CAMERA_H_

#include "main.h"

//クラス宣言
class CCharacterCamera
{
public:
	CCharacterCamera(const D3DXVECTOR3& CameraPos , const D3DXVECTOR3& LookatPos ,float HorizonAngle , float VerticleAngle);
	~CCharacterCamera() {}

	void CalcCameraAndLookatPos(const D3DXVECTOR3& CharacterPos);		//キャラクターの位置でカメラと注視点の目的位置を計算
	void CalcCameraAndLookatPosLockOn(const D3DXVECTOR3& CharacterPos,const D3DXVECTOR3& LockOnPos);	//キャラクターとロックオン目標の位置でカメラと注視点の目的位置を計算
	void TpsCameraRotateStick( const D3DXVECTOR2& InputValue);
	void TpsCameraRotateKeyboard(float HorizonValue, float VerticalValue);
	void CameraAndLookatToGoalLinearInterpolation(D3DXVECTOR3 *pCameraPos, D3DXVECTOR3 *pLookatPos);	//カメラと注視点をゴールに向かせて移動、移動速度はゴールの距離と関係
	void CameraAndLookatToGoalCircleInterpolation(D3DXVECTOR3 *pCameraPos, D3DXVECTOR3 *pLookatPos);	//LockOnPosを回転軸とし、現在のカメラと注視点をゴールに向かせて回転する
private:
	CCharacterCamera();
	void CameraTpsRotation(float Percent, float HorizonAngle, float VerticalAngle);		//カメラのTPS回転
private:
	D3DXVECTOR3			m_CameraGoal;		//カメラの位置座標
	D3DXVECTOR3			m_LookatGoal;		//注視点の位置座標
	float				m_AngleHori;		//水平角度
	float				m_AngleVerti;		//垂直角度
};

#endif