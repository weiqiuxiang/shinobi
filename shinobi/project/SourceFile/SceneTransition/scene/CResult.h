#pragma once
#ifndef _CRESULT_H_
#define _CRESULT_H_

#include "SceneManager.h"

class CResult : public CSceneManager
{
public:
	bool Init(HWND hWnd);
	void Uninit(void);
	void Update(void);
	void Draw(void);
};


#endif