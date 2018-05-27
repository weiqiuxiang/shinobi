#pragma once
#ifndef CSCENE2D_STORAGE_H
#define CSCENE2D_STORAGE_H

#include "main.h"
#include "CScene2D.h"

//�}�N��
#define CSCENE2D_PRIORITY_MAX (4)    //���C���[�ő吔
#define CSCENE2D_OBJ_MAX (100)    //1���C���[�̍ő�I�u�W�F�N�g��

class CScene2DStorage
{
public:
	static void InitAll(void);
	static void UninitAll(void);
	static void UpdateAll(void);
	static void DrawAll(void);
	static CScene2D* CreateObject(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &Size, const PRIORITY priority, const char* Texfilepass);
private:
	static CScene2D *m_Obj[CSCENE2D_PRIORITY_MAX][CSCENE2D_OBJ_MAX];
};

#endif