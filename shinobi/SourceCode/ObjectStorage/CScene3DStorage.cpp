#include "CScene3DStorage.h"
#include "CCameraManager.h"

CScene3D *CScene3DStorage::m_Obj[CSCENE3D_OBJ_MAX] = {};
CScene3DStorage::Z_SORT CScene3DStorage::m_ZSort[CSCENE3D_OBJ_MAX] = {};             //Zソードの順番格納行列
int CScene3DStorage::m_TotalObj = 0;

#define ID_IS_nullptr (-1)

void CScene3DStorage::InitAll(void)
{
	for (int ID = 0; ID < CSCENE3D_OBJ_MAX; ID++)
	{
		if (m_Obj[ID] != nullptr)
		{
			m_Obj[ID]->Uninit();
			delete m_Obj[ID];
		}
		m_Obj[ID] = nullptr;
	}

	for (int ID = 0; ID < CSCENE3D_OBJ_MAX; ID++)
	{
		m_ZSort[ID].ID = ID_IS_nullptr;
		m_ZSort[ID].ZValue = 0.0f;
	}
}

void CScene3DStorage::UninitAll(void)
{
	for (int ID = 0; ID < CSCENE3D_OBJ_MAX; ID++)
	{
		if (m_Obj[ID] != nullptr)
		{
			m_Obj[ID]->Uninit();
			delete m_Obj[ID];
			m_Obj[ID] = nullptr;
		}
	}
}

void CScene3DStorage::UpdateAll(void)
{

	for (int ID = 0; ID < CSCENE3D_OBJ_MAX; ID++)
	{
		if (m_Obj[ID] != nullptr) m_Obj[ID]->Update();
	}
}

void CScene3DStorage::DrawAll(void)
{
	ZSort();             //オブジェクト全体Zソートする

	for (int ID = 0; ID < m_TotalObj; ID++)
	{
		int k = m_TotalObj - 1 - ID;
		if (m_Obj[m_ZSort[k].ID] != nullptr)
		{
			m_Obj[m_ZSort[k].ID]->Draw();
		}
	}
}

CScene3D* CScene3DStorage::CreateObject(const SRP &worldMtx, const D3DXVECTOR3 &pSize, const CScene3D::CSCENE3D_TYPE CScene3DType, const char* Texfilepass)
{
	for (int ID = 0; ID < CSCENE3D_OBJ_MAX; ID++)
	{
		if (m_Obj[ID] == nullptr)
		{
			m_Obj[ID] = new CScene3D();
			m_Obj[ID]->Init(worldMtx, pSize, Texfilepass);
			m_Obj[ID]->SetDrawType(CScene3DType);
			return m_Obj[ID];
		}
	}
	return nullptr;
}

void CScene3DStorage::Delete(CScene3D * pScene3D)
{
	for (int ID = 0; ID < CSCENE3D_OBJ_MAX; ID++)
	{
		if (m_Obj[ID] != nullptr)
		{
			if (m_Obj[ID] != pScene3D) continue;
			m_Obj[ID]->Uninit();
			m_Obj[ID] = nullptr;
		}
	}
}

CScene3D* CScene3DStorage::GetObj(int ID)
{
	bool OverRange = (ID < 0) || (ID > CSCENE3D_OBJ_MAX);
	if (OverRange) {
		MessageBox(nullptr, "CScene3D OBJ IDが範囲外,CScene3D OBJ取得不能", "エラー", MB_OK | MB_ICONHAND);
		return nullptr;
	}
	return m_Obj[ID];
}
void CScene3DStorage::ZSort(void)
{
	D3DXMATRIX ViewMtx = CCameraManager::GetActiveCameraViewMtx();
	D3DXMATRIX ProjeMtx = CCameraManager::GetActiveCameraProjeMtx();
	D3DXMATRIX ViewProje = ViewMtx* ProjeMtx;                       //ビュー射影行列を求め
	m_TotalObj = 0;                                                   //オブジェクト総数

	//配列格納庫初期化
	for (int ID = 0; ID < CSCENE3D_OBJ_MAX; ID++)
	{
		m_ZSort[ID].ID = ID_IS_nullptr;
		m_ZSort[ID].ZValue = 0.0f;
	}

	//格納開始
	for (int ID = 0; ID < CSCENE3D_OBJ_MAX; ID++)
	{
		if (nullptr != m_Obj[ID])                    //nullptrチェック
		{
			D3DXVECTOR3 POS = D3DXVECTOR3(0.0f,0.0f,0.0f);            //ローカル座標
			const D3DXMATRIX* pWorldMtx = m_Obj[ID]->GetWorldMtx();   //ワールド行列取得
			D3DXMATRIX WorldViewProje = (*pWorldMtx) * ViewProje;     //ワールドビュー射影行列を求める
			D3DXVec3TransformCoord(&POS,&POS, &WorldViewProje);       //変換後の頂点を求め

			//Zバッファの値とIDを代入
			for (int SortNum = 0; SortNum < CSCENE3D_OBJ_MAX; SortNum++)
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

void CScene3DStorage::QuickSort(int nLeftID, int nRightID)
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

	int aQuickSortIDL[CSCENE3D_OBJ_MAX];                 //クイックソートID左配列
	int aQuickSortIDR[CSCENE3D_OBJ_MAX];                 //クイックソートID右配列
	float aDistanceL[CSCENE3D_OBJ_MAX];                    //パーティクルからカメラまでの距離左配列
	float aDistanceR[CSCENE3D_OBJ_MAX];                    //パーティクルからカメラまでの距離右配列

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