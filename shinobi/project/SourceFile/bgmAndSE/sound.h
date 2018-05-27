//=============================================================================
//
// サウンド処理 [sound.h]
// Author : AKIRA TANAKA
//
//=============================================================================
#ifndef _SOUND_H_
#define _SOUND_H_

#include "main.h"

//*****************************************************************************
// サウンドファイル
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
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitSound(HWND hWnd);                                 //サウンドの初期化処理
void UninitSound(void);                                       //終了処理（デバイスを解放）
HRESULT PlaySound(SOUND_LABEL label);                         //指定ラベルのサウンドを再生
HRESULT SetSoundVolume(SOUND_LABEL label, float fVolume);             //ボリューム設定
HRESULT GetSoundVolume(SOUND_LABEL label, float *pVolume);            //ボリューム取得
void SetAllSoundVolum(float fVolume);
void StopSound(SOUND_LABEL label);                            //特定のサウンドを止める
void StopSound(void);                                         //全部のサウンドを止める

#endif
