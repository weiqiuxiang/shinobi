//=============================================================================
//
// �T�E���h���� [sound.h]
// Author : AKIRA TANAKA
//
//=============================================================================
#ifndef _SOUND_H_
#define _SOUND_H_

#include "main.h"

//*****************************************************************************
// �T�E���h�t�@�C��
//*****************************************************************************
typedef enum
{
	//SE
	TITLE_SE_CURSOR = 0,
	TITLE_SE_DECISION,

	//BGM
	TITLE_BGM,            //TitleBgm

	SOUND_LABEL_MAX
} SOUND_LABEL;

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitSound(HWND hWnd);                                 //�T�E���h�̏���������
void UninitSound(void);                                       //�I�������i�f�o�C�X������j
HRESULT PlaySound(SOUND_LABEL label);                         //�w�胉�x���̃T�E���h���Đ�
HRESULT SetSoundVolume(SOUND_LABEL label, float fVolume);             //�{�����[���ݒ�
HRESULT GetSoundVolume(SOUND_LABEL label, float *pVolume);            //�{�����[���擾
void SetAllSoundVolum(float fVolume);
void StopSound(SOUND_LABEL label);                            //����̃T�E���h���~�߂�
void StopSound(void);                                         //�S���̃T�E���h���~�߂�

#endif
