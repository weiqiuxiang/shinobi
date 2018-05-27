#pragma once

#ifndef _TEXTURE_HEADER_H_
#define _TEXTURE_HEADER_H_

#include "main.h"

//�}�N��
#define TEXSLOT_NUM (30) //���[�\�̃e�N�X�`���̐�

class TEX
{
public:
	//�����o�֐�
	TEX();
	~TEX();

	//Init,Uninit�֐�
	HRESULT Init(void);
	void Uninit(void);
	//�t�@���N�V�����֐�
	void SetTexPass(const char* filepass);                   //�e�N�X�`���̃t�@�C���p�X��ݒ�
	const char *GetTexPass(void);                      //�e�N�X�`���p�X�̎擾
	const LPDIRECT3DTEXTURE9 GetTexturePoint(void);    //�e�N�X�`���|�C���^�擾
	void CreateTexObj(void);                           //�e�N�X�`���I�u�W�F�̐���

private:
	//�����o�ϐ�
	LPDIRECT3DTEXTURE9 m_pTexture;
	char m_pFilepass[256];
};

#endif