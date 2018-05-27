#pragma once
#ifndef _TEXMANAGER_H_
#define _TEXMANAGER_H_

#include "main.h"
#include "Tex.h"

class CTexManager
{
public:
	//�ÓI�����o�֐�
	static HRESULT InitAll(void);
	static void UninitAll(void);

	static TEX_ID LoadTex(const char *FilePass);			//�e�N�X�`�������[�h��ID��Ԃ�
	static LPDIRECT3DTEXTURE9 UseTex(TEX_ID IDNum);			//�e�N�X�`�����Z�b�g����
	static const char* GetTexturePass(TEX_ID IDNum);

private:
	//�ÓI�����o�֐�
	static HRESULT CreateTexObj(TEX_ID IDNum, const char *FilePass);           //�e�N�X�`���X���b�g�Ƀe�N�X�`��������
	
	//�ÓI�����o�ϐ�
	static TEX *m_TexTureSlot[TEXSLOT_NUM];    //�e�N�X�`���X���b�g
};

#endif