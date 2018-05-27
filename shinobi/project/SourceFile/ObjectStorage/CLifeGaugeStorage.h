#pragma once
#ifndef CLIFE_GAUGE_STORAGE_H
#define CLIFE_GAUGE_STORAGE_H

#include "main.h"
#include "CLifeGauge.h"

//マクロ
#define CLIFE_GAUGE_NUM_MAX (100)		//ライフゲージの最大数

class CLifeGaugeStorage
{
public:
	typedef struct
	{
		int ID;
		double ZValue;
	}Z_SORT;

	static void InitAll(void);
	static void UninitAll(void);
	static void UpdateAll(void);
	static void DrawAll(void);
	static CLifeGauge* CreateObject(const D3DXVECTOR3& pos, const D3DXVECTOR3& size , int TotalLife);
	static void Delete(CLifeGauge * pScene3D);
	static CLifeGauge* GetObj(int ID);
	static void SetWorldPos(int ID,const D3DXVECTOR3& pos);
	static void GetEnemyLife(int ID, float DamageLife);
private:
	//メンバ関数
	static void ZSort(void);
	static void QuickSort(int nLeftID, int nRightID);

	//メンバ変数
	static CLifeGauge *m_Obj[CLIFE_GAUGE_NUM_MAX];
	static Z_SORT m_ZSort[CLIFE_GAUGE_NUM_MAX];
	static int m_TotalObj;
};

#endif