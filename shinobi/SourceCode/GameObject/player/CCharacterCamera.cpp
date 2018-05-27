#include "CCharacterCamera.h"
#include "CCameraManager.h"
#include "CCamera.h"
#include "CPolygonMath.h"
#include "CCameraManager.h"

/*==============================================================================
	マクロ
===============================================================================*/
#define CAMERA_RELATIVE_DIST_FROM_CHARA (10.0f)		//カメラとキャラクターの相対距離
#define LOOKAT_RELATIVE_DIST_FROM_CHARA (16.6f)		//注視点とキャラクターの相対距離

#define RELATIVE_DIST_Y (5.0f)			//カメラと注視点のキャラクターの相対Y距離

#define VERTICAL_ANGLE_START_INTERPO_XZDISTANCE (10.0f)	//カメラ垂直角度補間開始XZ平面の距離
#define VERTICAL_ANGLE_START_INTERPO_YDISTANCE (-2.0f)	//カメラ垂直角度補間開始Y距離
#define DEFAULT_VERTICAL_ANGLE (D3DX_PI*0.15f)			//ロックオンの時カメラのデフォルト垂直角度
#define MAX_VERTICAL_ANGLE (D3DX_PI*0.35f)				//ロックオンの時カメラの最大垂直角度

#define CAMERA_ROTATION_SPEED_HORIZON (0.09f)			//カメラの水平回転速度
#define CAMERA_ROTATION_SPEED_VERTICAL (0.09f)			//カメラの垂直回転速度

//カメラと注視点の線形補間に関連するマクロ
#define CAMERA_LINER_INTERPOLATION_SPEED (0.16f)		//カメラの線形補間の速度
#define LOOKAT_LINER_INTERPOLATION_SPEED (0.24f)		//注視点の線形補間の速度

//カメラと注視点の円形移動補間に関連するマクロ
#define CIRCLE_INTERPO_BASE_SPEED (0.4f * D3DX_PI)		//カメラの円補間の時の基本回転速度
#define RADIUS_INTERPO_BASE_SPEED (0.1f)				//半径方向の補間基本速度
#define YPOS_INTERPO_BASE_SPEED (0.1f)				//Y方向の補間基本速度

/*==============================================================================
	コンストラクタ
===============================================================================*/
CCharacterCamera::CCharacterCamera()
{
	m_CameraGoal = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_LookatGoal = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_AngleHori = 0;
	m_AngleVerti = 0;
}

CCharacterCamera::CCharacterCamera(const D3DXVECTOR3& CameraPos, const D3DXVECTOR3& LookatPos, float HorizonAngle, float VerticleAngle) 
{
	m_CameraGoal = CameraPos;
	m_LookatGoal = LookatPos;
	m_AngleHori = HorizonAngle;
	m_AngleVerti = VerticleAngle;
}

/*==============================================================================
	キャラクターの位置でカメラと注視点の目的位置を計算
===============================================================================*/
void CCharacterCamera::CalcCameraAndLookatPos(const D3DXVECTOR3& CharacterPos)
{
	//キャラ->カメラ方向ベクトルの計算
	CAMERA_PARA CameraPara = CCameraManager::GetCameraPara(CCameraManager::CAMERA_PLAYER);
	D3DXVECTOR3 CameraPlayerVec = CharacterPos - CameraPara.pos;
	CameraPlayerVec.y = 0;
	D3DXVec3Normalize(&CameraPlayerVec, &CameraPlayerVec);

	//カメラと注視点の目的位置の計算
	D3DXVECTOR3 Camera, Lookat;
	Camera = CharacterPos - CAMERA_RELATIVE_DIST_FROM_CHARA * cosf(m_AngleVerti) * CameraPlayerVec;
	Lookat = CharacterPos + LOOKAT_RELATIVE_DIST_FROM_CHARA * cosf(-m_AngleVerti) * CameraPlayerVec;
	Camera.y = CAMERA_RELATIVE_DIST_FROM_CHARA * sinf(m_AngleVerti) + CharacterPos.y + RELATIVE_DIST_Y;
	Lookat.y = LOOKAT_RELATIVE_DIST_FROM_CHARA * sinf(-m_AngleVerti) + CharacterPos.y + RELATIVE_DIST_Y;
	m_CameraGoal = Camera;					//カメラゴール座標設定
	m_LookatGoal = Lookat;					//注視点ゴール座標設定

	//角度の算出
	D3DXVECTOR2 GoalAngle = CPolygonMath::CalcTwoPoint_HorizonAngleAndVerticleAngle(m_CameraGoal, m_LookatGoal);
	m_AngleHori = GoalAngle.x;
	m_AngleVerti = GoalAngle.y;
}

/*==============================================================================
	キャラクターとロックオン目標の位置でカメラと注視点の目的位置を計算
		CharacterPos : キャラクター位置
		LockOnPos : ロックオン目標の位置
===============================================================================*/
void CCharacterCamera::CalcCameraAndLookatPosLockOn(const D3DXVECTOR3& CharacterPos,const D3DXVECTOR3& LockOnPos)
{
	//キャラクターがロックオン目標に向くベクトルを計算
	D3DXVECTOR3 CharaLockonVec = LockOnPos - CharacterPos;
	D3DXVec3Normalize(&CharaLockonVec, &CharaLockonVec);

	//キャラクターとロックオン目標の距離は一定値より小さいな時、カメラと注視点の垂直角度が変わる
	float fVertiAngle = 0;
	float DistanceY = CharacterPos.y - LockOnPos.y;

	if (DistanceY > VERTICAL_ANGLE_START_INTERPO_YDISTANCE) {
		float Distance = D3DXVec3Length(&(CharacterPos - LockOnPos));
		if (Distance < VERTICAL_ANGLE_START_INTERPO_XZDISTANCE) {
			float t = Distance / VERTICAL_ANGLE_START_INTERPO_XZDISTANCE;
			fVertiAngle = DEFAULT_VERTICAL_ANGLE + (MAX_VERTICAL_ANGLE - DEFAULT_VERTICAL_ANGLE) * (1 - t);		//線形補間
		}
		else { 
			fVertiAngle = DEFAULT_VERTICAL_ANGLE; 
		}
	}
	else {
		float t = VERTICAL_ANGLE_START_INTERPO_YDISTANCE / DistanceY;
		fVertiAngle = DEFAULT_VERTICAL_ANGLE + (DEFAULT_VERTICAL_ANGLE - MAX_VERTICAL_ANGLE) * (1 - t);
	}

	//カメラの視点と注視点の目標位置の計算
	D3DXVECTOR3 Camera = CharacterPos - CharaLockonVec * cosf(fVertiAngle) * CAMERA_RELATIVE_DIST_FROM_CHARA;
	D3DXVECTOR3 Lookat = CharacterPos + CharaLockonVec * cosf(-fVertiAngle) * LOOKAT_RELATIVE_DIST_FROM_CHARA;
	Camera.y = CAMERA_RELATIVE_DIST_FROM_CHARA * sinf(fVertiAngle) + CharacterPos.y + RELATIVE_DIST_Y;
	Lookat.y = LOOKAT_RELATIVE_DIST_FROM_CHARA * sinf(-fVertiAngle) + CharacterPos.y + RELATIVE_DIST_Y;
	m_CameraGoal = Camera;					//カメラゴール座標設定
	m_LookatGoal = Lookat;					//注視点ゴール座標設定
}

/*==============================================================================
スティックの入力でキャラクターを軸に視点と注視点の回転
	CharacterPos : キャラクター位置
===============================================================================*/
void CCharacterCamera::TpsCameraRotateStick(const D3DXVECTOR2& InputValue)
{
	//スティックの入力でカメラを回転
	float Horizon = InputValue.x * -CAMERA_ROTATION_SPEED_HORIZON;
	float Vertical = InputValue.y * -CAMERA_ROTATION_SPEED_VERTICAL;

	float Percent = CAMERA_RELATIVE_DIST_FROM_CHARA / (CAMERA_RELATIVE_DIST_FROM_CHARA + LOOKAT_RELATIVE_DIST_FROM_CHARA);
	CCameraManager::TpsRotationBasePercent(Percent, Horizon, Vertical, D3DX_PI*0.3);
	CameraTpsRotation(Percent, Horizon, Vertical);
}

/*==============================================================================
キーボードの入力でキャラクターを軸に視点と注視点の回転
		CharacterPos : キャラクター位置
		HorizonDir : 水平の入力方向(false : 左 , true : 右)
		VerticalDir : 垂直の入力方向(false : 上 , true : 下)
===============================================================================*/
void CCharacterCamera::TpsCameraRotateKeyboard( float HorizonValue, float VerticalValue)
{
	//回転角度
	float Horizon = 0;
	float Vertical = 0;
	
	//キーボードの入力でカメラを回転
	Vertical = CAMERA_ROTATION_SPEED_VERTICAL * VerticalValue;
	Horizon =  CAMERA_ROTATION_SPEED_HORIZON * HorizonValue;

	float Percent = CAMERA_RELATIVE_DIST_FROM_CHARA / (CAMERA_RELATIVE_DIST_FROM_CHARA + LOOKAT_RELATIVE_DIST_FROM_CHARA);
	CCameraManager::TpsRotationBasePercent(Percent, Horizon, Vertical, D3DX_PI*0.3);
	CameraTpsRotation(Percent, Horizon, Vertical);
}

/*==============================================================================
	現在カメラの位置が目標位置に向けて線形補間する
		pCameraPos : 動かすカメラの位置
		pLookatPos : 動かす注視点の位置
===============================================================================*/
void CCharacterCamera::CameraAndLookatToGoalLinearInterpolation(D3DXVECTOR3 *pCameraPos, D3DXVECTOR3 *pLookatPos)
{
	//ヌルチェック
	if (pCameraPos == nullptr || pLookatPos == nullptr) {
		return;
	}

	//カメラの現位置->目標位置ベクトルを求め
	D3DXVECTOR3 CameraGoalVec = m_CameraGoal - *pCameraPos;
	float CameraGoalDistance = D3DXVec3Length(&CameraGoalVec);
	D3DXVec3Normalize(&CameraGoalVec, &CameraGoalVec);

	//注視点の現位置->目標位置ベクトルを求め
	D3DXVECTOR3 LookatGoalVec = m_LookatGoal - *pLookatPos;
	float LookatGoalDistance = D3DXVec3Length(&LookatGoalVec);
	D3DXVec3Normalize(&LookatGoalVec, &LookatGoalVec);

	//線形補間 : pCameraPosとpLookatPosをゴール時点を向かせて動かす
	if (CameraGoalDistance < 0.1f) {
		*pCameraPos = m_CameraGoal;
	}
	else {
		*pCameraPos += CameraGoalDistance * CAMERA_LINER_INTERPOLATION_SPEED * CameraGoalVec;
	}
	if (LookatGoalDistance < 0.2f) {
		*pLookatPos = m_LookatGoal;
	}
	else {
		*pLookatPos += LookatGoalDistance * LOOKAT_LINER_INTERPOLATION_SPEED * LookatGoalVec;
	}
}

/*==============================================================================
現在カメラの位置が目標位置に向けて線形補間する
	pCameraPos : 動かすカメラの位置
	pLookatPos : 動かす注視点の位置
===============================================================================*/
void CCharacterCamera::CameraAndLookatToGoalCircleInterpolation(D3DXVECTOR3 *pCameraPos, D3DXVECTOR3 *pLookatPos)
{
	//ヌルチェック
	if (pCameraPos == nullptr || pLookatPos == nullptr) {
		return;
	}

	D3DXVECTOR2 NowRayVec = //現カメラから現注視点に向かうXZ平面の方向ベクトルを求める
		D3DXVECTOR2(pLookatPos->x, pLookatPos->z) - D3DXVECTOR2(pCameraPos->x, pCameraPos->z);
	D3DXVECTOR2 GoalRayVec = //目標カメラから目標注視点に向かうXZ平面の方向ベクトルを求め
		D3DXVECTOR2(m_LookatGoal.x, m_LookatGoal.z) - D3DXVECTOR2(m_CameraGoal.x, m_CameraGoal.z);
	D3DXVECTOR2 NowToGoalVec = //現カメラと目標カメラのベクトルを求める
		D3DXVECTOR2(m_CameraGoal.x, m_CameraGoal.z) - D3DXVECTOR2(pCameraPos->x, pCameraPos->z);

	//内分比を求める
	float Nowt = D3DXVec2CCW(&NowToGoalVec, &GoalRayVec) / D3DXVec2CCW(&NowRayVec, &GoalRayVec);
	float Goalt = D3DXVec2CCW(&NowToGoalVec, &NowRayVec) / D3DXVec2CCW(&NowRayVec, &GoalRayVec);

	//交差はないなら、線形補間に切り替え
	bool IsCross = 
		( (Nowt >= 0) && (Nowt <= 1) ) &&
		( (Goalt >= 0) && (Goalt <= 1) );

	if (IsCross == false) {
		CameraAndLookatToGoalLinearInterpolation(pCameraPos, pLookatPos);
		return;
	}

	//回転中心の位置を求める
	D3DXVECTOR2 RorateCenterXZ = D3DXVECTOR2(m_CameraGoal.x, m_CameraGoal.z) + Goalt * GoalRayVec;

	//ベクトルとベクトルのなす角を求める
	float Angle = D3DXVec2CCW(&NowRayVec, &GoalRayVec) / (D3DXVec2Length(&NowRayVec) * D3DXVec2Length(&GoalRayVec));
	Angle = asinf(Angle);

	//回転速度を求め
	float Rate = Angle / (D3DX_PI * 2);

	//角度は一定値以下の時、ゴールのXZ座標を現のXZ座標に代入
	if(fabs(Rate) > CIRCLE_INTERPO_BASE_SPEED * 0.1f){
		//角度の線形補間
		float RotSpeed = Rate * CIRCLE_INTERPO_BASE_SPEED;

		//回転行列を求める
		D3DXMATRIX RotMtxY;
		D3DXMatrixRotationY(&RotMtxY, RotSpeed);

		//回転後のカメラと注視点の座標を求める
		D3DXVECTOR2 CameraOriginPos = D3DXVECTOR2(pCameraPos->x, pCameraPos->z) - RorateCenterXZ;	//原点を回転中心にする時の位置
		D3DXVECTOR3 CameraRotPos;
		D3DXVec3TransformCoord(&CameraRotPos, &D3DXVECTOR3(CameraOriginPos.x, 0.0f, CameraOriginPos.y), &RotMtxY);
		CameraRotPos = CameraRotPos + D3DXVECTOR3(RorateCenterXZ.x, pCameraPos->y, RorateCenterXZ.y);

		D3DXVECTOR2 LookatOriginPos = D3DXVECTOR2(pLookatPos->x, pLookatPos->z) - RorateCenterXZ;	//原点を回転中心にする時の位置
		D3DXVECTOR3 LookatRotPos;
		D3DXVec3TransformCoord(&LookatRotPos, &D3DXVECTOR3(LookatOriginPos.x, 0.0f, LookatOriginPos.y), &RotMtxY);
		LookatRotPos = LookatRotPos + D3DXVECTOR3(RorateCenterXZ.x, pLookatPos->y, RorateCenterXZ.y);

		//結果を代入
		*pCameraPos = CameraRotPos;
		*pLookatPos = LookatRotPos;
	}

	//半径方向の線形補間
	float DistXZNowCameraToRorateCenter = D3DXVec2Length( &D3DXVECTOR2(pCameraPos->x - RorateCenterXZ.x, pCameraPos->z - RorateCenterXZ.y) );
	float DistXZGoalCameraToRorateCenter = D3DXVec2Length( &D3DXVECTOR2(m_CameraGoal.x - RorateCenterXZ.x, m_CameraGoal.z - RorateCenterXZ.y) );
	float RadiusDist = DistXZGoalCameraToRorateCenter - DistXZNowCameraToRorateCenter;		//半径方向の距離差
	if (fabs(RadiusDist) > RADIUS_INTERPO_BASE_SPEED){
		D3DXVECTOR3 RadiusVec = *pLookatPos - *pCameraPos;	//半径方向のベクトル
		RadiusVec.y = 0.0f;
		D3DXVec3Normalize(&RadiusVec, &RadiusVec);
		*pCameraPos = *pCameraPos + RadiusDist * RADIUS_INTERPO_BASE_SPEED * RadiusVec;
		*pLookatPos = *pLookatPos + RadiusDist * RADIUS_INTERPO_BASE_SPEED * RadiusVec;
	}

	//Y方向の線形補間
	float CameraYDist = m_CameraGoal.y - pCameraPos->y;
	if (fabs(CameraYDist) < YPOS_INTERPO_BASE_SPEED) {
		pCameraPos->y = m_CameraGoal.y;
	}
	else {
		pCameraPos->y += CameraYDist * YPOS_INTERPO_BASE_SPEED;
	}

	float LookatYDist = m_LookatGoal.y - pLookatPos->y;
	if (fabs(LookatYDist) < YPOS_INTERPO_BASE_SPEED) {
		pLookatPos->y = m_LookatGoal.y;
	}
	else {
		pLookatPos->y += LookatYDist * YPOS_INTERPO_BASE_SPEED;
	}
}

/*==============================================================================
	カメラのTPS回転
===============================================================================*/
void CCharacterCamera::CameraTpsRotation(float Percent, float HorizonAngle, float VerticalAngle)
{
	//パーセンテージの範囲検証
	if (Percent < 0 || Percent > 1) {
		MessageBox(nullptr, "関数TpsRotationGoalの引数tの値は不適切,値の範囲は0～1", "WARNING", MB_OK | MB_ICONHAND);
		return;
	}

	float OldAngel = m_AngleVerti;

	m_AngleHori += HorizonAngle;
	m_AngleVerti += VerticalAngle;

	// -180～180度に丸める
	if (m_AngleHori > D3DX_PI) { m_AngleHori -= 2 * D3DX_PI; }
	if (m_AngleHori < -D3DX_PI) { m_AngleHori += 2 * D3DX_PI; }

	//垂直方向:可動範囲は-90～90度
	if (m_AngleVerti > 0.3 * D3DX_PI) { m_AngleVerti = OldAngel; }
	if (m_AngleVerti < -0.3 * D3DX_PI) { m_AngleVerti = OldAngel; }

	float Distance = CAMERA_RELATIVE_DIST_FROM_CHARA + LOOKAT_RELATIVE_DIST_FROM_CHARA;

	//回転中心を求める
	D3DXVECTOR3 CameraLookat = m_LookatGoal - m_CameraGoal;
	D3DXVECTOR3 RorateCenter = m_CameraGoal + CameraLookat * Percent;
	float DistaceCameraRotCenter = Distance * Percent;

	//カメラの座標を求め
	m_CameraGoal.x = DistaceCameraRotCenter * cosf(m_AngleHori) * cosf(m_AngleVerti) + RorateCenter.x;
	m_CameraGoal.z = DistaceCameraRotCenter * sinf(m_AngleHori) * cosf(m_AngleVerti) + RorateCenter.z;
	m_CameraGoal.y = DistaceCameraRotCenter * sinf(m_AngleVerti) + RorateCenter.y;

	//注視点の座標を求め
	D3DXVECTOR3 CameraRotCenterVec = RorateCenter - m_CameraGoal;
	D3DXVec3Normalize(&CameraRotCenterVec, &CameraRotCenterVec);
	m_LookatGoal = RorateCenter + CameraRotCenterVec* Distance * (1 - Percent);
}

