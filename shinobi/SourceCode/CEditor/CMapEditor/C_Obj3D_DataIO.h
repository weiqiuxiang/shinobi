#pragma once
#ifndef C_OBJ3D_SETTER_H_
#define C_OBJ3D_SETTER_H_

#include "main.h"
#include "CModel.h"
#include "CScene3D.h"
#include <stdio.h>

class C_Obj3D_DataIO
{
public:
	void ExportObj3DData(void);                                //+の書き出し
	void ImportObj3DData(const char* pObjFilePass);           //メッシュデータの読み込み

private:
	//読み込み関数b
	void ReadBillBoard(FILE *fp);
	void ReadModel(FILE *fp);
};

#endif