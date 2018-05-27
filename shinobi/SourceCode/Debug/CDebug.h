#pragma once
#ifndef CDEBUG_H_
#define CDEBUG_H_

#include "main.h"

class CDebug
{
public : 
	static void Init(void);
	static void Uninit(void);
	static void ClearText(void);
	static void AddText(char *Text);
	static void Draw(void);
private:
	static char m_Text[1024];        //�f�o�b�N����
	static LPD3DXFONT  m_pFont;			// �t�H���g�ւ̃|�C���^
};

#endif