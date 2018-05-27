#pragma once
#ifndef _CLIGHT_MANAGER_H_
#define _CLIGHT_MANAGER_H_

#include "main.h"
#include "CLight.h"

#define LIGHT_MAX (20)

class CLightManager
{
public:
	static void InitAll(void);
	static void UninitAll(void);
	static void UseLight(int LightNumber);
	static D3DXVECTOR3 GetLightDirection(int LightNumber);
	static void CreateLight(const D3DLIGHT9& Light);
	static void SetLightPara(int LightNumber, const D3DLIGHT9& Light);
private:
	static CLight *m_CLight[LIGHT_MAX];
};

#endif