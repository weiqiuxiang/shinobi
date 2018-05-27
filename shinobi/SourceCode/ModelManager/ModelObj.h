#pragma once
#ifndef _MODELOBJ_H_
#define _MODELOBJ_H_

#include "main.h"

class CModelObj
{
public:
	CModelObj();
	~CModelObj();

	//Init,Uninit,Draw関数
	HRESULT Init(void);
	void Uninit(void);
	void Draw(void);
	void DrawMtrl(const D3DMATERIAL9& Mtrl);
	void DrawOnDepthMap(const D3DXMATRIX& mtxWorld, const D3DXMATRIX& mtxLightView, const D3DXMATRIX& mtxLightProje);
	void DrawUseDepthMap(const D3DXMATRIX& world, const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje, LPDIRECT3DTEXTURE9 DepthTexture);
	void DrawSpecular(const D3DXMATRIX& world);
	void DrawMotionBlur(const D3DXMATRIX& worldMtx, const D3DXMATRIX& worldMtxOld, LPDIRECT3DTEXTURE9 OriginalTexture);
	
	//セッター
	void SetModelPass(const char* filepass);		//モデルのファイルパスを設定
	void SetAlpha(float Alpha);

	//ゲッター
	const char *GetModelPass(void);					//モデルパスの取得
	LPD3DXMESH GetMesh(void) { return m_pMesh; }

	//他の関数
	void CreateModelObj(void);						//モデルオブジェの生成

private:
	LPD3DXMESH m_pMesh;								//メッシュインターフェースポインタ
	LPD3DXBUFFER m_pBuffMat;						//マテリアル情報バッファ(D3DX汎用バッファインターフェース)
	DWORD m_nNumMat;								//マテリアルの数
	TEX_ID *m_TexID;								//テクスチャ番号
	char m_pFilepass[256];							//モデルファイルパス
};

#endif