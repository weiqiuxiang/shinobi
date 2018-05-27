#include "CTextLoad.h"

int CTextLoad::GetStrToken(FILE* fp, const char *pToken, char *pBuf)
{
	int strCount = 0;

	char c;

	while ((c = (char)fgetc(fp)) != EOF)
	{
		//指定されたトークンが現れたか調べる
		for (unsigned i = 0; i < strlen(pToken); i++)
		{
			if (c == pToken[i])
			{
				pBuf[strCount] = 0;
				return strCount;
			}
		}
		pBuf[strCount++] = c;
	}

	pBuf[strCount] = 0;
	return -1;
}

bool CTextLoad::FindFont(FILE *fp, const char* pToken, const char*pStr)
{
	char buf[256];
	while (GetStrToken(fp, pToken, buf) != -1)            //トークン順番スペース
	{
		if (strcmp(pStr, buf) == 0)
		{
			return true;
		}
	}

	return false;
}