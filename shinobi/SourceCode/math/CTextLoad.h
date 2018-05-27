#pragma once
#ifndef _CTEXT_LOAD_H_
#define _CTEXT_LOAD_H_

#include "main.h"
#include <stdio.h>

//テキストを読み込み関数
class CTextLoad
{
public:
	static int GetStrToken(FILE* fp, const char *pToken, char *pBuf);                      //指定されたトークンが見つかるまでループし,見つかるとデータの格納を停止し,格納データの文字数を返却
	static bool CTextLoad::FindFont(FILE *fp, const char* pToken, const char*pStr);     //指定された文字列が見つかるまでループし,見つかるとtrueを返す
};

#endif