#pragma once
#ifndef CPARTICLE_MANAGER_H_
#define CPARTICLE_MANAGER_H_

#include "CParticleMaker.h"

#define PARTICLEMAKER_MAX (10)

class CParticleManager
{
public:
	static void InitAll(void);
	static void UninitAll(void);
	static void UpdateAll(void);
	static void DrawAll(void);

	static void SetActiveNumber(int nNum);
	static void SetUse(bool bUse);
	static void SetMakerBase(const D3DXVECTOR3& pos, const D3DXVECTOR3& size, float PerFrameNum, int nLife, const char* BaseTexPass, const char* EffectTexPass);
	static void SetEnvironment(const D3DXVECTOR3& WindDirect, float WindPower, float fMass, float GravityAcc, float CoeffiResist);
	static void SetShootPara(const D3DXVECTOR3& ShootDir, const D3DXVECTOR2& RangeAngle, float Speed);
	static void SetParticleBaseRGBA(RGBA rgba);
	static void SetParticleEffectRGBA(RGBA rgba);
	static void SetBaseEffectRGBA(RGBA BaseRGBA, RGBA EffectRGBA);
	static void SetParticleDrawTime(int DrawTimes_Base, int DrawTimes_Effect);
private:
	static int m_nActiveNumber;
	static CParticleMaker m_ParticleMaker[PARTICLEMAKER_MAX];
	static bool m_bUse[PARTICLEMAKER_MAX];
};

#endif