//ファイル名 : SceneManager.h
//説明 : シーンのオブジェクトの生成役
///////////////////////////////////////
#pragma once
#ifndef SCENEMANAGER_H_
#define SCENEMANAGER_H_

#include "CScene.h"
#include "CLight.h"
#include "main.h"

//マクロ
#define CSCENE_OBJ_MAX (100)

class CSceneManager
{
public:
	CSceneManager();
	~CSceneManager();

	virtual bool Init(HWND hWnd) = 0;
	virtual void Uninit(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;
};

#endif