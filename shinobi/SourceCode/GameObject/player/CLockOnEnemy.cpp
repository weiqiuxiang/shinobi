#include "CLockOnEnemy.h"
#include "CEnemyStorage.h"
#include "CCameraManager.h"
#include "CPolygonMath.h"
#include "CVectorMath.h"

#define LOCKON_ANGLE_HORI (D3DX_PI * 0.4)		//ロックオンできる角度(水平)
#define LOCKON_ANGLE_VERTI (D3DX_PI * 0.3)		//ロックオンできる角度(垂直)

//引数説明:
//プレーヤーポインター
//今ロックオンしている敵
//ロックオン変更の方向(左 : false,右 : true)
CEnemy* CLockOnEnemy::ChangeTarget(CPlayer* pPlayer, CEnemy *pEnemy, bool bDir)
{
	//早期リターン
	if (nullptr == pPlayer) {
		return nullptr;
	}
	if (nullptr == pEnemy) {
		return nullptr;
	}

	//変数宣言
	CEnemy* pEnemyReturn = nullptr;						//CEnemyポインタ
	float HoriAngleSabunMin = D3DX_PI*10;				//水平角度の差分の最小値

	//データ取得
	auto itEnemy = CEnemyStorage::GetEnemiesListBegin();	//リストのBeginイテレーター
	auto itEnd = CEnemyStorage::GetEnemiesListEnd();		//リストのEndイテレーター

	//角度計算
	CAMERA_PARA CameraPara = CCameraManager::GetCameraPara(CCameraManager::CAMERA_PLAYER);	//カメラデータ取得
	D3DXVECTOR2 AngleCL = CPolygonMath::CalcTwoPoint_HorizonAngleAndVerticleAngle(CameraPara.pos, CameraPara.lookat);	//カメラと注視点の水平、垂直角度計算

	for (; itEnemy != itEnd; ++itEnemy) {
		if ((*itEnemy) == pEnemy) continue;
		if ((*itEnemy)->GetDeadFlag()) continue;

		//プレーヤーと敵との距離計算
		D3DXVECTOR3 PlayerEnemyVec = (*itEnemy)->GetPos() - pPlayer->GetPos();
		float Distance = D3DXVec3Length(&PlayerEnemyVec);
		if (Distance > LOCKON_DISTANCE)  continue; 						//ロックオン距離に満足してないなら

		//角度差計算
		D3DXVECTOR2 AngleCE = CPolygonMath::CalcTwoPoint_HorizonAngleAndVerticleAngle(CameraPara.pos, (*itEnemy)->GetPos());	//カメラと敵の水平、垂直角度計算
		float HoriAngleSabun = fabs(AngleCL.x - AngleCE.x);					//水平角度差計算
		float VertiAngleSabun = fabs(AngleCL.y - AngleCE.y);				//垂直角度差計算
		bool bAngleCanLockon = (HoriAngleSabun < LOCKON_ANGLE_HORI) && (VertiAngleSabun < LOCKON_ANGLE_VERTI);					//水平と垂直角度が範囲内か
		if (!bAngleCanLockon) continue;

		//方向チェック
		bool bDirCheck = false;
		D3DXVECTOR3 Cross;
		D3DXVec3Cross(
			&Cross,
			&(D3DXVECTOR3(CameraPara.lookat.x - CameraPara.pos.x,0.0f, CameraPara.lookat.z - CameraPara.pos.z)),
			&(D3DXVECTOR3((*itEnemy)->GetPos().x - CameraPara.pos.x, 0.0f, (*itEnemy)->GetPos().z - CameraPara.pos.z))
			);
		if ((false == bDir) && (Cross.y <= 0.0f)) bDirCheck = true;		//敵が左にいる
		if ((true == bDir) && (Cross.y > 0.0f)) bDirCheck = true;		//敵が右にいる
		if (!bDirCheck) continue;

		//水平角度差は最小値か
		bool bIsHoriAngleSabunMin = HoriAngleSabun < HoriAngleSabunMin;															//水平角度差値の最小値か
		if (bIsHoriAngleSabunMin) {
			pEnemyReturn = (*itEnemy);				//ポインタ代入
			HoriAngleSabunMin = HoriAngleSabun;		//最小値上書き
		}
	}

	return pEnemyReturn;
}

CEnemy* CLockOnEnemy::LockOnEnemy(CPlayer* pPlayer)
{
	//早期リターン
	if (nullptr == pPlayer) return nullptr;

	//変数宣言
	CEnemy* pEnemy = nullptr;								//CEnemyポインタ
	float HoriAngleSabunMin = D3DX_PI * 10;				//水平角度の差分の最小値

	//データ取得
	auto itEnemy = CEnemyStorage::GetEnemiesListBegin();		//リストのBeginイテレーター
	auto itEnd = CEnemyStorage::GetEnemiesListEnd();	//リストのEndイテレーター

	//角度計算
	CAMERA_PARA CameraPara = CCameraManager::GetCameraPara(CCameraManager::CAMERA_PLAYER);	//カメラデータ取得
	D3DXVECTOR2 AngleCL = CPolygonMath::CalcTwoPoint_HorizonAngleAndVerticleAngle(CameraPara.pos, CameraPara.lookat);	//カメラと注視点の水平、垂直角度計算

	for (; itEnemy != itEnd; ++itEnemy) {
		if ((*itEnemy)->GetDeadFlag()) continue;

		//プレーヤーと敵との距離計算
		D3DXVECTOR3 PlayerEnemyVec = (*itEnemy)->GetPos() - pPlayer->GetPos();
		float Distance = D3DXVec3Length(&PlayerEnemyVec);
		if (Distance > LOCKON_DISTANCE)  continue; 						//ロックオン距離に満足してないなら

		//角度差計算
		D3DXVECTOR2 AngleCE = CPolygonMath::CalcTwoPoint_HorizonAngleAndVerticleAngle(CameraPara.pos, (*itEnemy)->GetPos());	//カメラと敵の水平、垂直角度計算
		float HoriAngleSabun = fabs(AngleCL.x - AngleCE.x);					//水平角度差計算
		float VertiAngleSabun = fabs(AngleCL.y - AngleCE.y);				//垂直角度差計算
		bool bAngleCanLockon = (HoriAngleSabun < LOCKON_ANGLE_HORI) && (VertiAngleSabun < LOCKON_ANGLE_VERTI);					//水平と垂直角度が範囲内か
		bool bIsHoriAngleSabunMin = HoriAngleSabun < HoriAngleSabunMin;															//水平角度差値の最小値か

		if (bAngleCanLockon && bIsHoriAngleSabunMin) {
			pEnemy = (*itEnemy);					//ポインタ代入
			HoriAngleSabunMin = HoriAngleSabun;		//最小値上書き
		}
	}

	return pEnemy;
}