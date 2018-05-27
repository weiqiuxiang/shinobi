#pragma once
#ifndef _MAIN_H_
#define _MAIN_H_

#pragma warning( disable: 4996 )
#pragma comment (lib,"d3d9.lib")
#pragma comment (lib,"d3dx9.lib")
#pragma comment (lib,"dxguid.lib")
#pragma comment (lib,"winmm.lib")
#pragma comment (lib,"dinput8.lib")
#pragma comment (lib,"Xinput.lib")

//-----------------------------------------------------------------------------------------
//ヘッダファイル
//-----------------------------------------------------------------------------------------
#include <d3dx9.h>
#include <dinput.h>
#include <XAudio2.h>
#include <XInput.h>
#include "structHeader.h"
#include "typedefHead01.h"
#include "TexPassHead.h"
#include "ModelPassHeader.h"
#include "TextFilePass.h"
#include "SoundPassHeader.h"
#include "namespaceHeader.h"

//-----------------------------------------------------------------------------------------
//マクロ
//-----------------------------------------------------------------------------------------
#define FVF_VERTEX2D (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)                     //2D頂点フォーマット
#define FVF_VERTEX3D (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1|D3DFVF_NORMAL)          //3D頂点フォーマット
#define TOKEN " \n\t"

#define SCREEN_WIDTH (1280)                         //ウィンドウの幅
#define SCREEN_HEIGHT (720)                         //ウィンドウの高さ

const D3DXMATRIX ScreenViewPortMtx = D3DXMATRIX(
	SCREEN_WIDTH * 0.5, 0, 0, 0,
	0, -SCREEN_HEIGHT*0.5, 0, 0,
	0, 0, 1, 0,
	SCREEN_WIDTH * 0.5, SCREEN_HEIGHT*0.5, 0, 1
);

#endif