#include "CEnemyControl.h"
#include "SceneTransition.h"
#include "CCameraManager.h"
#include "CMeshFieldManager.h"
#include "CPolygonMath.h"
#include "CEnemy.h"
#include "CPathPoint.h"
#include "CAIDogAction.h"

CEnemyControl::CEnemyControl() 
{
}

CEnemyControl::~CEnemyControl() 
{

}

//初期化
void CEnemyControl::Init(void)
{
	m_AI = nullptr;
	m_Player = nullptr;
}

//プレーヤーをコントロールする
void CEnemyControl::EnemyControl(void)
{
	EnemyFaceDir();							//向きベクトルによる姿勢調整
	EnemyHitAllMesh();						//メッシュの当り判定
}


void CEnemyControl::SetEnemy(CEnemy *Enemy)
{
	if (nullptr == Enemy)
	{
		MessageBox(nullptr, "CEnemyControlに渡したプレーヤーポインタはnullptr!", "ポインタの値はnullptr", MB_OK | MB_ICONHAND);
	}
	m_AI = Enemy;
}

void CEnemyControl::SetPlayer(CPlayer *Player)
{
	m_Player = Player;
}

void CEnemyControl::EnemyFaceDir(void)           //AIの向きからポーズを算出
{
	if (nullptr == m_AI) return;

	//ポーズを決める
	D3DXMATRIX PoseMtx;
	D3DXVECTOR3 X, Y, Z;
	Y = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	D3DXVECTOR3 EnemyDir = m_AI->GetFaceDir();             //向き方向取得

	//向き方向によるポーズ計算
	if (D3DXVECTOR3(0.0f, 0.0f, 0.0f) == EnemyDir) return;
	D3DXVec3Normalize(&Z, &EnemyDir);
	D3DXVec3Cross(&X, &Y, &Z);
	D3DXVec3Normalize(&X, &X);

	PoseMtx._11 = X.x; PoseMtx._12 = X.y; PoseMtx._13 = X.z; PoseMtx._14 = 0.0f;
	PoseMtx._21 = Y.x; PoseMtx._22 = Y.y; PoseMtx._23 = Y.z; PoseMtx._24 = 0.0f;
	PoseMtx._31 = Z.x; PoseMtx._32 = Z.y; PoseMtx._33 = Z.z; PoseMtx._34 = 0.0f;
	PoseMtx._41 = 0.0f; PoseMtx._42 = 0.0f; PoseMtx._43 = 0.0f; PoseMtx._44 = 1.0f;

	m_AI->SetPoseMtx(PoseMtx);
}

void CEnemyControl::EnemyHitAllMesh(void)       //仮です
{
	if (nullptr == m_AI) {
		return;
	}

	D3DXVECTOR3 EnemyPos = m_AI->GetPos();     //プレーヤーの位置取得
	CMeshField *pMesh = nullptr;

	for (int MeshNum = 0; MeshNum < MESH_MAX; MeshNum++)
	{
		pMesh = CMeshFieldManager::GetMesh(MeshNum);
		if (nullptr == pMesh) continue;

		const MESH_FACE *pFace = pMesh->GetMeshFace();       //面データ取得
		D3DXVECTOR2 PieceNum = pMesh->GetPieceNum();         //マス数取得

															 //面とプレーヤーの当り判定
		for (int i = 0; i < PieceNum.y; i++)
		{
			for (int j = 0; j < PieceNum.x * 2; j++)
			{
				int k = i * ((int)PieceNum.x * 2) + j;
				if (nullptr == &pFace[k]) continue;

				//プレーヤーは平面の範囲内にあるかどうか
				bool InMesh = CPolygonMath::PointInXZMeshFace(EnemyPos, pFace[k]);
				if (InMesh == false) continue;

				//平面内いる->プレーヤーのXZ座標を渡し,平面のY座標を求める
				float fElevation = CPolygonMath::PointElevationInFace(EnemyPos, pFace[k]);

				//平面のY座標をプレーヤーのY座標に代入
				if (m_AI->GetPos().y < fElevation) {
					m_AI->SetPosY(fElevation);
					m_AI->SetSpeedY(0);
				}
				/*
				//姿勢を求め
				D3DXMATRIX PoseMtx;
				D3DXVECTOR3 EnemyDir = m_AI->GetEnemyMoveDir();
				if (D3DXVECTOR3(0.0f, 0.0f, 0.0f) == EnemyDir) continue;
				CPolygonMath::CalcPoseMtx(&PoseMtx, EnemyDir, pFace[k].Normalize);
				m_AI->SetPoseMtx(PoseMtx);
				*/
			}
		}
	}
}