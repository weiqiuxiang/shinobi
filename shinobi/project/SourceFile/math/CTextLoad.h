#pragma once
#ifndef _CTEXT_LOAD_H_
#define _CTEXT_LOAD_H_

#include "main.h"
#include <stdio.h>

//�e�L�X�g��ǂݍ��݊֐�
class CTextLoad
{
public:
	static int GetStrToken(FILE* fp, const char *pToken, char *pBuf);                      //�w�肳�ꂽ�g�[�N����������܂Ń��[�v��,������ƃf�[�^�̊i�[���~��,�i�[�f�[�^�̕�������ԋp
	static bool CTextLoad::FindFont(FILE *fp, const char* pToken, const char*pStr);     //�w�肳�ꂽ�����񂪌�����܂Ń��[�v��,�������true��Ԃ�
};

#endif