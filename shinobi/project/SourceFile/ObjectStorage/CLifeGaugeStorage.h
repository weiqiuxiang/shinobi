#pragma once
#ifndef CLIFE_GAUGE_STORAGE_H
#define CLIFE_GAUGE_STORAGE_H

#include "main.h"
#include "CLifeGauge.h"

//�}�N��
#define CLIFE_GAUGE_NUM_MAX (100)		//���C�t�Q�[�W�̍ő吔

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
	//�����o�֐�
	static void ZSort(void);
	static void QuickSort(int nLeftID, int nRightID);

	//�����o�ϐ�
	static CLifeGauge *m_Obj[CLIFE_GAUGE_NUM_MAX];
	static Z_SORT m_ZSort[CLIFE_GAUGE_NUM_MAX];
	static int m_TotalObj;
};

#endif