#pragma once
#ifndef _CTUTORIAL_H_
#define _CTUTORIAL_H_

#include "SceneManager.h"

class CTutorial : public CSceneManager
{
public:
	bool Init(HWND hWnd);
	void Uninit(void);
	void Update(void);
	void Draw(void);
};

#endif