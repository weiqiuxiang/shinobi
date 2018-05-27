#include "CPlayerPattern.h"
#include "CPlayer.h"
#include "CCameraManager.h"
#include "CPolygonMath.h"
#include "SceneTransition.h"

void CPlayerPattern::PlayerGoalDir(CPlayer* pPlayer)
{
	if (pPlayer == nullptr) {
		return;
	}

	//敵をロックオンしていない状態
	if (pPlayer->GetLockOnFlag() == false)
	{
		D3DXVECTOR2 AnalogInputValue = CSceneTransition::GetXInput()->GetLeftThumbStickValueAnalog();
		if (D3DXVec2Length(&AnalogInputValue) < 0.2f) {
			return;
		}

		CAMERA_PARA CameraPara = CCameraManager::GetCameraPara(CCameraManager::CAMERA_PLAYER);		//カメラの水平方向の向きを取得

		//カメラの向いてるXZ平面の方向ベクトルを算出
		D3DXVECTOR3 CameraLookatVec = CameraPara.lookat - CameraPara.pos;
		CameraLookatVec.y = 0;

		D3DXVec3Normalize(&CameraLookatVec, &CameraLookatVec);

		//プレーヤーの向き算出
		float AngleDir = CPolygonMath::Vec2AngleAndAngleDir(D3DXVECTOR2(0.0f, 1.0f), AnalogInputValue);		//コントローラの成す角度を算出
		D3DXMATRIX MtxRot;
		D3DXMatrixRotationY(&MtxRot, -AngleDir);
		D3DXVECTOR3 PlayerDir;
		D3DXVec3TransformCoord(&PlayerDir, &CameraLookatVec, &MtxRot);
		D3DXVec3Normalize(&PlayerDir, &PlayerDir);
		pPlayer->SetGoalDir(PlayerDir);
	}

	else
	{
		D3DXVECTOR3 PlayerPos = pPlayer->GetPos();
		D3DXVECTOR3 EnemyPos = pPlayer->GetLockOnPosition();
		D3DXVECTOR3 PlayerEnemyVec = EnemyPos - PlayerPos;
		D3DXVec3Normalize(&PlayerEnemyVec, &PlayerEnemyVec);
		pPlayer->SetGoalDir(PlayerEnemyVec);
	}
}

void CPlayerPattern::PlayerFaceDir(CPlayer* pPlayer)
{
	if (pPlayer == nullptr) {
		return;
	}

	//プレーヤー向きベクトルでプレーヤーの姿勢を計算
	D3DXMATRIX PoseMtx;
	D3DXVECTOR3 X, Y, Z;
	Y = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 PlayerDir = pPlayer->GetFaceDir();
	if (D3DXVECTOR3(0.0f, 0.0f, 0.0f) == PlayerDir) return;
	D3DXVec3Normalize(&Z, &PlayerDir);
	D3DXVec3Cross(&X, &Y, &Z);
	D3DXVec3Normalize(&X, &X);

	PoseMtx._11 = X.x; PoseMtx._12 = X.y; PoseMtx._13 = X.z; PoseMtx._14 = 0.0f;
	PoseMtx._21 = Y.x; PoseMtx._22 = Y.y; PoseMtx._23 = Y.z; PoseMtx._24 = 0.0f;
	PoseMtx._31 = Z.x; PoseMtx._32 = Z.y; PoseMtx._33 = Z.z; PoseMtx._34 = 0.0f;
	PoseMtx._41 = 0.0f; PoseMtx._42 = 0.0f; PoseMtx._43 = 0.0f; PoseMtx._44 = 1.0f;

	pPlayer->SetPoseMtx(PoseMtx);
}