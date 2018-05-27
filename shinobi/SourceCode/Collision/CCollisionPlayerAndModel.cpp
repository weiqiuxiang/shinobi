#include "CCollisionPlayerAndModel.h"
#include "CPlayer.h"
#include "CModelStorage.h"
#include "CPolygonMath.h"

#define PUSH_VALUE (0.01f)

//static変数
D3DXVECTOR3 CCollisionPlayerAndModel::m_PlayerBodyPosOld = D3DXVECTOR3(0,0,0);

//初期化処理
void CCollisionPlayerAndModel::Init(const D3DXVECTOR3& PlayerBodyPosOld)
{
	m_PlayerBodyPosOld = PlayerBodyPosOld;
}

void CCollisionPlayerAndModel::Update(CPlayer* pPlayer)
{
	//ヌルチェック
	if (pPlayer == nullptr) {
		_RPT0(0,"[CCollisionPlayerAndModel.cpp][Update]引数pPlayerの値はヌル\n");
		return;
	}

	//プレーヤー座標取得
	D3DXVECTOR3 PlayerBodyPos = pPlayer->GetBodyPos();

	//モデルのAABBとプレーヤーの当り判定
	for (int i = 0; i < MODEL_OBJ_MAX; i++) {
		//モデルポインタ取得
		CModel* pModel = CModelStorage::GetObj(i);
		if (pModel == nullptr) {
			continue;
		}

		//プレーヤー座標はモデルのAABBの範囲内かを計算
		D3DXVECTOR3 ModelPos = pModel->GetPosition();
		D3DXVECTOR2 XValue, YValue, ZValue;
		pModel->GetAABBValue(&XValue, &YValue, &ZValue);

		float XMinValue = XValue.x + ModelPos.x;
		float XMaxValue = XValue.y + ModelPos.x;
		float YMinValue = YValue.x + ModelPos.y;
		float YMaxValue = YValue.y + ModelPos.y;
		float ZMinValue = ZValue.x + ModelPos.z;
		float ZMaxValue = ZValue.y + ModelPos.z;

		bool IsPlayerInModelAABB_XDir = (PlayerBodyPos.x >= XMinValue) && (PlayerBodyPos.x <= XMaxValue);
		bool IsPlayerInModelAABB_YDir = (PlayerBodyPos.y >= YMinValue) && (PlayerBodyPos.y <= YMaxValue);
		bool IsPlayerInModelAABB_ZDir = (PlayerBodyPos.z >= ZMinValue) && (PlayerBodyPos.z <= ZMaxValue);
		bool IsPlayerInModel = IsPlayerInModelAABB_XDir && IsPlayerInModelAABB_YDir && IsPlayerInModelAABB_ZDir;

		//範囲内いないなら次のモデルに行く
		if (IsPlayerInModel == false) {
			continue;
		}

		//古い座標がモデル内にいるか
		bool IsPlayerOldInModelAABB_XDir = (m_PlayerBodyPosOld.x >= XMinValue) && (m_PlayerBodyPosOld.x <= XMaxValue);
		bool IsPlayerOldInModelAABB_YDir = (m_PlayerBodyPosOld.y >= YMinValue) && (m_PlayerBodyPosOld.y <= YMaxValue);
		bool IsPlayerOldInModelAABB_ZDir = (m_PlayerBodyPosOld.z >= ZMinValue) && (m_PlayerBodyPosOld.z <= ZMaxValue);
		bool IsPlayerOldInModel = IsPlayerOldInModelAABB_XDir && IsPlayerOldInModelAABB_YDir && IsPlayerOldInModelAABB_ZDir;

		//古い座標は範囲内
		if (IsPlayerOldInModel == true) {
			float DistanceFromXMin = fabs(PlayerBodyPos.x - XValue.x - ModelPos.x);
			float DistanceFromXMax = fabs(PlayerBodyPos.x - XValue.y - ModelPos.x);
			float DistanceFromZMin = fabs(PlayerBodyPos.z - ZValue.x - ModelPos.z);
			float DistanceFromZMax = fabs(PlayerBodyPos.z - ZValue.y - ModelPos.z);
			
			//以上の四つの値の最小値を取得
			float Dist[4] = { DistanceFromXMin ,DistanceFromXMax ,DistanceFromZMin ,DistanceFromZMax };
			int Number = MinDistance(Dist);

			//プレーヤーの座標をずらす
			switch (Number){
			case 0:
				pPlayer->SetPosX(XValue.x + ModelPos.x - PUSH_VALUE);
				break;
			case 1:
				pPlayer->SetPosX(XValue.y + ModelPos.x + PUSH_VALUE);
				break;
			case 2:
				pPlayer->SetPosZ(ZValue.x + ModelPos.z - PUSH_VALUE);
				break;
			case 3:
				pPlayer->SetPosZ(ZValue.y + ModelPos.z + PUSH_VALUE);
				break;
			}

			//ループから抜ける
			break;
		}

		else {
			bool IsHit = false;
			float Dummyt1, Dummyt2;
			D3DXVECTOR2 CrossPoint = D3DXVECTOR2(0, 0);		//交差点

			//XZ平面上のプレーヤー線分
			D3DXVECTOR2 PlayerXZVec = D3DXVECTOR2(PlayerBodyPos.x, PlayerBodyPos.z) - D3DXVECTOR2(m_PlayerBodyPosOld.x, m_PlayerBodyPosOld.z);
			Segment SegPlayer;
			SegPlayer.s = D3DXVECTOR2(m_PlayerBodyPosOld.x, m_PlayerBodyPosOld.z);
			SegPlayer.v = PlayerXZVec;

			//XZ平面上AABBの四つの点
			D3DXVECTOR2 PointXMinZMin = D3DXVECTOR2(XMinValue,ZMinValue);
			D3DXVECTOR2 PointXMaxZMin = D3DXVECTOR2(XMaxValue, ZMinValue);
			D3DXVECTOR2 PointXMaxZMax = D3DXVECTOR2(XMaxValue, ZMaxValue);
			D3DXVECTOR2 PointXMinZMax = D3DXVECTOR2(XMinValue, ZMaxValue);

			//XMinZMinからXMaxZMinまでの線分とプレーヤー線分の当り判定を求める
			Segment XMinZMinToXMaxZMin;
			XMinZMinToXMaxZMin.s = PointXMinZMin;
			XMinZMinToXMaxZMin.v = PointXMaxZMin - PointXMinZMin;

			//当り判定があったらプレーヤーの座標を調整
			IsHit = CPolygonMath::ColSegments(SegPlayer, XMinZMinToXMaxZMin, &Dummyt1, &Dummyt2, &CrossPoint);
			if (IsHit == true) {
				pPlayer->SetPosZ(ZMinValue - PUSH_VALUE);
				break;
			}

			//XMaxZMinからXMaxZMaxまでの線分とプレーヤー線分の当り判定を求める
			Segment XMaxZMinToXMaxZMax;
			XMaxZMinToXMaxZMax.s = PointXMaxZMin;
			XMaxZMinToXMaxZMax.v = PointXMaxZMax - PointXMaxZMin;

			//当り判定があったらプレーヤーの座標を調整
			IsHit = CPolygonMath::ColSegments(SegPlayer, XMaxZMinToXMaxZMax, &Dummyt1, &Dummyt2, &CrossPoint);
			if (IsHit == true) {
				pPlayer->SetPosX(XMaxValue + PUSH_VALUE);
				break;
			}

			//XMaxZMaxからXMinZMaxまでの線分とプレーヤー線分の当り判定を求める
			Segment XMaxZMaxToXMinZMax;
			XMaxZMaxToXMinZMax.s = PointXMaxZMax;
			XMaxZMaxToXMinZMax.v = PointXMinZMax - PointXMaxZMax;

			//当り判定があったらプレーヤーの座標を調整
			IsHit = CPolygonMath::ColSegments(SegPlayer, XMaxZMaxToXMinZMax, &Dummyt1, &Dummyt2, &CrossPoint);
			if (IsHit == true) {
				pPlayer->SetPosZ(ZMaxValue + PUSH_VALUE);
				break;
			}

			//XMinZMaxからXMinZMinまでの線分とプレーヤー線分の当り判定を求める
			Segment XMinZMaxToXMinZMin;
			XMinZMaxToXMinZMin.s = PointXMinZMax;
			XMinZMaxToXMinZMin.v = PointXMinZMin - PointXMinZMax;

			//当り判定があったらプレーヤーの座標を調整
			IsHit = CPolygonMath::ColSegments(SegPlayer, XMinZMaxToXMinZMin, &Dummyt1, &Dummyt2, &CrossPoint);
			if (IsHit == true) {
				pPlayer->SetPosX(XMinValue - PUSH_VALUE);
				break;
			}
		}	
	}

	//古い座標の上書き
	m_PlayerBodyPosOld = PlayerBodyPos;
}

//四つの距離の中の最小値を求める
//戻り値 : 最小値の要素番号
int CCollisionPlayerAndModel::MinDistance(float Dist[4])
{
	float Min = Dist[0];
	int Number = 0;
	for (int i = 0; i < 3; i++) {
		if (Min > Dist[i + 1]) {
			Min = Dist[i + 1];
			Number = i + 1;
		}
	}
	return Number;
}