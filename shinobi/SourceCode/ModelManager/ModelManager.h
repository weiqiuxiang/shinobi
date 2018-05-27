#pragma once
#ifndef _MODELMANAGER_H_
#define _MODELMANAGER_H_

#include "main.h"
#include "ModelObj.h"

#define MODELSLOT_NUM (60)

class CModelManager
{
public:
	//静的メンバ関数
	static HRESULT InitAll(void);
	static void UninitAll(void);

	static MODEL_ID LoadModel(const char *FilePass);						//モデルをロードしIDを返す
	static void DrawModel(MODEL_ID IDNum);									//モデルを描画する
	static void DrawModelUseMtrl(MODEL_ID IDNum, const D3DMATERIAL9& Mtrl);	//モデルにマテリアル設定して描画
	static void DrawModelOnDepthMap(MODEL_ID IDNum,const D3DXMATRIX& world, const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje);
	static void DrawUseDepthMap(MODEL_ID IDNum, const D3DXMATRIX& world, const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje,LPDIRECT3DTEXTURE9 DepthTexture);
	static void DrawSpecular(MODEL_ID IDNum, const D3DXMATRIX& world);
	static void DrawMotionBlur(MODEL_ID IDNum, const D3DXMATRIX& worldMtx, const D3DXMATRIX& worldMtxOld, LPDIRECT3DTEXTURE9 OriginalTexture);
	static const char* GetModelPass (MODEL_ID IDNum);						//モデルパス取得
	static LPD3DXMESH GetMesh (MODEL_ID IDNum);								//モデルのメッシュポインタ取得
	static void SetAlpha(MODEL_ID IDNum, float Alpha);

private:
	//静的メンバ関数
	static HRESULT CreateModelObj(MODEL_ID IDNum, const char *FilePass);           //モデルスロットにモデルを入れる

	//静的メンバ変数
	static CModelObj *m_ModelSlot[MODELSLOT_NUM];    //モデルスロット
};

#endif