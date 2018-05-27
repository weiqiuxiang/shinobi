#pragma once
#ifndef _CLIGHT_H_
#define _CLIGHT_H_

#include "main.h"

class CLight
{
public:
	CLight();
	~CLight();

	void Init(const D3DLIGHT9& Light);
	void UseLight(void);

	//セッター
	void SetDiffuse(const D3DXCOLOR &DiffuseColor);
	void SetAmbient(const D3DXCOLOR &AmbientColor);
	void SetLightAllPara(const D3DLIGHT9& Light) { m_Light = Light; }

	//ゲッター
	D3DXVECTOR3 GetLightDiretion(void);

private:
	D3DLIGHT9 m_Light;
};

#endif