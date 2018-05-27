#pragma once

#ifndef _TEXTURE_HEADER_H_
#define _TEXTURE_HEADER_H_

#include "main.h"

//マクロ
#define TEXSLOT_NUM (30) //収納可能のテクスチャの数

class TEX
{
public:
	//メンバ関数
	TEX();
	~TEX();

	//Init,Uninit関数
	HRESULT Init(void);
	void Uninit(void);
	//ファンクション関数
	void SetTexPass(const char* filepass);                   //テクスチャのファイルパスを設定
	const char *GetTexPass(void);                      //テクスチャパスの取得
	const LPDIRECT3DTEXTURE9 GetTexturePoint(void);    //テクスチャポインタ取得
	void CreateTexObj(void);                           //テクスチャオブジェの生成

private:
	//メンバ変数
	LPDIRECT3DTEXTURE9 m_pTexture;
	char m_pFilepass[256];
};

#endif