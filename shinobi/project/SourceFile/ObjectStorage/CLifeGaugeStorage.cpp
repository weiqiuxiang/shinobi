#include "CLifeGaugeStorage.h"
#include "CCameraManager.h"
#include "CEnemyStorage.h"
#include <crtdbg.h>

//マクロ
#define DOG_LIFE_GAUGE_POSY_FROM_BODY (3.0f)		//ライフゲージの位置は、タイプは犬の場合、ボディの位置から離れる距離

//static変数
CLifeGauge *CLifeGaugeStorage::m_Obj[CLIFE_GAUGE_NUM_MAX] = {};
CLifeGaugeStorage::Z_SORT CLifeGaugeStorage::m_ZSort[CLIFE_GAUGE_NUM_MAX] = {};             //Zソードの順番格納行列
int CLifeGaugeStorage::m_TotalObj = 0;

#define ID_IS_nullptr (-1)

void CLifeGaugeStorage::InitAll(void)
{
	for (int ID = 0; ID < CLIFE_GAUGE_NUM_MAX; ID++)
	{
		if (m_Obj[ID] != nullptr)
		{
			m_Obj[ID]->Uninit();
			delete m_Obj[ID];
		}
		m_Obj[ID] = nullptr;
	}

	for (int ID = 0; ID < CLIFE_GAUGE_NUM_MAX; ID++)
	{
		m_ZSort[ID].ID = ID_IS_nullptr;
		m_ZSort[ID].ZValue = 0.0f;
	}
}

void CLifeGaugeStorage::UninitAll(void)
{
	for (int ID = 0; ID < CLIFE_GAUGE_NUM_MAX; ID++)
	{
		if (m_Obj[ID] != nullptr)
		{
			m_Obj[ID]->Uninit();
			delete m_Obj[ID];
			m_Obj[ID] = nullptr;
		}
	}
}

void CLifeGaugeStorage::UpdateAll(void)
{
	//ライフゲージの更新
	int Count = 0;
	auto it = CEnemyStorage::GetEnemiesListBegin();
	auto itEnd = CEnemyStorage::GetEnemiesListEnd();
	for (; it != itEnd; ++it) {
		if (*it != nullptr) {
			D3DXVECTOR3 BodyPos = (*it)->GetBodyPos();
			CEnemy::ENEMY_TYPE EnemyType = (*it)->GetEnemyType();
			switch (EnemyType) {
			case CEnemy::TYPE_DOG:
				BodyPos.y = BodyPos.y + DOG_LIFE_GAUGE_POSY_FROM_BODY;
				break;
			}
			CLifeGaugeStorage::SetWorldPos(Count, BodyPos);
			CLifeGaugeStorage::GetEnemyLife(Count, (float)(*it)->GetLife());
		}
		Count++;
	}

	for (int ID = 0; ID < CLIFE_GAUGE_NUM_MAX; ID++)
	{
		if (m_Obj[ID] != nullptr) m_Obj[ID]->Update();
	}
}

void CLifeGaugeStorage::DrawAll(void)
{
	ZSort();

	for (int ID = 0; ID < m_TotalObj; ID++)
	{
		int k = m_TotalObj - 1 - ID;
		if (m_Obj[m_ZSort[k].ID] != nullptr)
		{
			m_Obj[m_ZSort[k].ID]->Draw();
		}
	}
}

CLifeGauge* CLifeGaugeStorage::CreateObject(const D3DXVECTOR3& pos, const D3DXVECTOR3& size, int TotalLife)
{
	for (int ID = 0; ID < CLIFE_GAUGE_NUM_MAX; ID++)
	{
		if (m_Obj[ID] == nullptr)
		{
			m_Obj[ID] = new CLifeGauge();
			m_Obj[ID]->Init(pos, size, (float)TotalLife);
			return m_Obj[ID];
		}
	}
	return nullptr;
}

void CLifeGaugeStorage::Delete(CLifeGauge * pScene3D)
{
	for (int ID = 0; ID < CLIFE_GAUGE_NUM_MAX; ID++)
	{
		if (m_Obj[ID] != nullptr)
		{
			if (m_Obj[ID] != pScene3D) continue;
			m_Obj[ID]->Uninit();
			m_Obj[ID] = nullptr;
		}
	}
}

CLifeGauge* CLifeGaugeStorage::GetObj(int ID)
{
	bool OverRange = (ID < 0) || (ID > CLIFE_GAUGE_NUM_MAX);
	if (OverRange) {
		_RPT0(0,"[CLifeGaugeStorage.cpp][GetObj]引数IDが範囲外");
		return nullptr;
	}
	return m_Obj[ID];
}

void CLifeGaugeStorage::SetWorldPos(int ID,const D3DXVECTOR3& pos)
{
	bool OverRange = (ID < 0) || (ID > CLIFE_GAUGE_NUM_MAX);
	if (OverRange) {
		_RPT0(0, "[CLifeGaugeStorage.cpp][SetWorldPos]引数IDが範囲外");
		return;
	}

	m_Obj[ID]->SetWorldPos(pos);
}

void CLifeGaugeStorage::GetEnemyLife(int ID, float Life)
{
	bool OverRange = (ID < 0) || (ID > CLIFE_GAUGE_NUM_MAX);
	if (OverRange) {
		_RPT0(0, "[CLifeGaugeStorage.cpp][AddDamageLife]引数IDが範囲外");
		return;
	}

	m_Obj[ID]->GetEnemyLife(Life);
}

void CLifeGaugeStorage::ZSort(void)
{
	D3DXMATRIX ViewMtx = CCameraManager::GetActiveCameraViewMtx();
	D3DXMATRIX ProjeMtx = CCameraManager::GetActiveCameraProjeMtx();
	D3DXMATRIX ViewProje = ViewMtx* ProjeMtx;                       //ビュー射影行列を求め
	m_TotalObj = 0;                                                   //オブジェクト総数

	//配列格納庫初期化
	for (int ID = 0; ID < CLIFE_GAUGE_NUM_MAX; ID++)
	{
		m_ZSort[ID].ID = ID_IS_nullptr;
		m_ZSort[ID].ZValue = 0.0f;
	}

	//格納開始
	for (int ID = 0; ID < CLIFE_GAUGE_NUM_MAX; ID++)
	{
		if (nullptr != m_Obj[ID])                    //nullptrチェック
		{
			D3DXVECTOR3 POS = D3DXVECTOR3(0.0f,0.0f,0.0f);				//ローカル座標
			D3DXMATRIX WorldMtx = m_Obj[ID]->GetWorldMtx();				//ワールド行列取得
			D3DXMATRIX WorldViewProje = (WorldMtx) * ViewProje;			//ワールドビュー射影行列を求める
			D3DXVec3TransformCoord(&POS,&POS, &WorldViewProje);			//変換後の頂点を求め

			//Zバッファの値とIDを代入
			for (int SortNum = 0; SortNum < CLIFE_GAUGE_NUM_MAX; SortNum++)
			{
				if (ID_IS_nullptr != m_ZSort[SortNum].ID) continue;
				m_ZSort[SortNum].ID = ID;
				m_ZSort[SortNum].ZValue = POS.z;
				m_TotalObj++;
				break;
			}
		}
	}

	//ソート開始
	QuickSort(0, m_TotalObj -1);
}

void CLifeGaugeStorage::QuickSort(int nLeftID, int nRightID)
{
	//---------------------------
	//変数宣言
	//---------------------------
	int nCnt = 0;           //カウント
	int nMiddleID = (nLeftID + nRightID) / 2;
	int nLeftNum_BE = nMiddleID - nLeftID;           //分割前の中間値の左側の配列メンバ数
	int nRightNum_BE = nRightID - nMiddleID;         //分割前の中間値の右側の配列メンバ数
	int nLeftNum_AF = 0; //分割後の中間値の左側の配列メンバ数
	int nRightNum_AF = 0; //分割後の中間値の右側の配列メンバ数

	int aQuickSortIDL[CLIFE_GAUGE_NUM_MAX];                 //クイックソートID左配列
	int aQuickSortIDR[CLIFE_GAUGE_NUM_MAX];                 //クイックソートID右配列
	float aDistanceL[CLIFE_GAUGE_NUM_MAX];                    //パーティクルからカメラまでの距離左配列
	float aDistanceR[CLIFE_GAUGE_NUM_MAX];                    //パーティクルからカメラまでの距離右配列

	//---------------------------
	//左と右分割
	//---------------------------
	for (int i = 1; i <= nLeftNum_BE; i++)
	{
		//---------------------------
		//中間値前のメンバ値チェック
		//---------------------------
		//左配列に入れる
		if (m_ZSort[nMiddleID - i].ZValue < m_ZSort[nMiddleID].ZValue)    //距離比較
		{
			aDistanceL[nLeftNum_AF] = (float)m_ZSort[nMiddleID - i].ZValue;          //距離配列に入れる
			aQuickSortIDL[nLeftNum_AF] = m_ZSort[nMiddleID - i].ID;    //ID配列に入れる
			nLeftNum_AF++;                                         //メンバ数カウントアップ
		}
		//右配列に入れる
		else
		{
			aDistanceR[nRightNum_AF] = (float)m_ZSort[nMiddleID - i].ZValue;  //配列に入れる
			aQuickSortIDR[nRightNum_AF] = m_ZSort[nMiddleID - i].ID;    //ID配列に入れる
			nRightNum_AF++;                                         //メンバ数カウントアップ
		}

		//---------------------------
		//中間値後ろのメンバ値チェック
		//---------------------------
		//左配列に入れる
		if (m_ZSort[nMiddleID + i].ZValue < m_ZSort[nMiddleID].ZValue)
		{
			aDistanceL[nLeftNum_AF] = (float)m_ZSort[nMiddleID + i].ZValue;          //距離配列に入れる
			aQuickSortIDL[nLeftNum_AF] = m_ZSort[nMiddleID + i].ID;    //ID配列に入れる
			nLeftNum_AF++;                                         //メンバ数カウントアップ
		}
		//右配列に入れる
		else
		{
			aDistanceR[nRightNum_AF] = (float)m_ZSort[nMiddleID + i].ZValue;  //配列に入れる
			aQuickSortIDR[nRightNum_AF] = m_ZSort[nMiddleID + i].ID;    //ID配列に入れる
			nRightNum_AF++;                                         //メンバ数カウントアップ
		}
	}

	//総数が偶数の場合
	if (nRightNum_BE > nLeftNum_BE)
	{
		//左配列に入れる
		if (m_ZSort[nRightID].ZValue < m_ZSort[nMiddleID].ZValue)
		{
			aDistanceL[nLeftNum_AF] = (float)m_ZSort[nRightID].ZValue;          //距離配列に入れる
			aQuickSortIDL[nLeftNum_AF] = m_ZSort[nRightID].ID;    //ID配列に入れる
			nLeftNum_AF++;                                         //メンバ数カウントアップ
		}
		//右配列に入れる
		else
		{
			aDistanceR[nRightNum_AF] = (float)m_ZSort[nRightID].ZValue;  //配列に入れる
			aQuickSortIDR[nRightNum_AF] = m_ZSort[nRightID].ID;    //ID配列に入れる
			nRightNum_AF++;                                         //メンバ数カウントアップ
		}
	}

	//----------------------------
	//分割の後の配列を配列に反映する
	//----------------------------
	nCnt = nLeftID;              //カウント初期化
	float MIDistance = (float)m_ZSort[nMiddleID].ZValue;      //中間値保存
	int MIID = m_ZSort[nMiddleID].ID;      //中間値保存

											   //左部分
	for (int j = 0; j < nLeftNum_AF; j++)
	{
		m_ZSort[nCnt].ZValue = aDistanceL[j];        //距離代入
		m_ZSort[nCnt].ID = aQuickSortIDL[j];     //ID代入
		nCnt++;            //カウントアップ
	}

	//中間値
	m_ZSort[nCnt].ZValue = MIDistance;        //距離代入
	m_ZSort[nCnt].ID = MIID;  //ID代入
	nCnt++;       //カウントアップ

	//右部分
	for (int j = 0; j < nRightNum_AF; j++)
	{
		m_ZSort[nCnt].ZValue = aDistanceR[j];        //距離代入
		m_ZSort[nCnt].ID = aQuickSortIDR[j];         //ID代入
		nCnt++;            //カウントアップ
	}

	//------------------
	//再帰部分
	//------------------
	//左部分
	if (nLeftNum_AF > 1)
	{
		QuickSort(nLeftID, nLeftID + nLeftNum_AF - 1);
	}

	//右部分
	if (nRightNum_AF > 1)
	{
		QuickSort(nRightID - nRightNum_AF + 1, nRightID);
	}
}