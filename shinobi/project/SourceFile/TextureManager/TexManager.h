#pragma once
#ifndef _TEXMANAGER_H_
#define _TEXMANAGER_H_

#include "main.h"
#include "Tex.h"

class CTexManager
{
public:
	//静的メンバ関数
	static HRESULT InitAll(void);
	static void UninitAll(void);

	static TEX_ID LoadTex(const char *FilePass);			//テクスチャをロードしIDを返す
	static LPDIRECT3DTEXTURE9 UseTex(TEX_ID IDNum);			//テクスチャをセットする
	static const char* GetTexturePass(TEX_ID IDNum);

private:
	//静的メンバ関数
	static HRESULT CreateTexObj(TEX_ID IDNum, const char *FilePass);           //テクスチャスロットにテクスチャを入れる
	
	//静的メンバ変数
	static TEX *m_TexTureSlot[TEXSLOT_NUM];    //テクスチャスロット
};

#endif