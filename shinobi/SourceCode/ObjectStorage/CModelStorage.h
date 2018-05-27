#pragma once
#ifndef MODEL_STORAGE_H
#define MODEL_STORAGE_H

#include "main.h"
#include "CModel.h"

//マクロ
#define MODEL_OBJ_MAX (200)    //1レイヤーの最大オブジェクト数

class CModelStorage
{
public:
	static void InitAll(void);
	static void UninitAll(void);
	static void UpdateAll(void);
	static void DrawAll(void);
	static void DrawAllAABBTest(void);
	static void DrawAllOnDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje);
	static void DrawUseDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje, LPDIRECT3DTEXTURE9 DepthTexture);
	static void DrawSpecular(void);
	static void Draw_ID(int ID);
	static void DrawUseMtrl_ID(int ID, const D3DMATERIAL9& Mtrl);

	static CModel* CreateObject(const SRP& SclRotPos, const char* ModelFilepass);
	static void Delete(int ID);
	static CModel* GetObj(int ID);

	static void AddPos(int ID,const D3DXVECTOR3& pos);
private:
	static CModel*		m_Obj[MODEL_OBJ_MAX];
};

#endif