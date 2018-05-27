#pragma once
#ifndef CSCENE2D_STORAGE_H
#define CSCENE2D_STORAGE_H

#include "main.h"
#include "CScene2D.h"

//マクロ
#define CSCENE2D_PRIORITY_MAX (4)    //レイヤー最大数
#define CSCENE2D_OBJ_MAX (100)    //1レイヤーの最大オブジェクト数

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