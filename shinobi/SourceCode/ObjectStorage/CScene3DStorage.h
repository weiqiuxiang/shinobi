#pragma once
#ifndef CSCENE3D_STORAGE_H
#define CSCENE3D_STORAGE_H

#include "main.h"
#include "CScene3D.h"

//�}�N��
#define CSCENE3D_OBJ_MAX (100)    //1���C���[�̍ő�I�u�W�F�N�g��

class CScene3DStorage
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
	static CScene3D* CreateObject(const SRP &worldMtx, const D3DXVECTOR3 &pSize, const CScene3D::CSCENE3D_TYPE CScene3DType, const char* Texfilepass);
	static void Delete(CScene3D * pScene3D);
	static CScene3D* GetObj(int ID);
private:
	//�����o�֐�
	static void ZSort(void);
	static void QuickSort(int nLeftID, int nRightID);

	//�����o�ϐ�
	static CScene3D *m_Obj[CSCENE3D_OBJ_MAX];
	static Z_SORT m_ZSort[CSCENE3D_OBJ_MAX];
	static int m_TotalObj;
};

#endif