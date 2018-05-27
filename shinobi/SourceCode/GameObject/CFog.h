#pragma once
#ifndef CFOG_H_
#define CFOG_H_

#include "main.h"

class CFog
{
public:
	static void FogSetting(void);
	static void EnableFog(void);
	static void DisableFog(void);
private:
	static float m_fFogDensity;
};

#endif