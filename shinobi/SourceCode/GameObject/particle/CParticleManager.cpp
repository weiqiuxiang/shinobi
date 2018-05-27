#include "CParticleManager.h"

int CParticleManager::m_nActiveNumber = 0;
CParticleMaker CParticleManager::m_ParticleMaker[PARTICLEMAKER_MAX] = {};
bool CParticleManager::m_bUse[PARTICLEMAKER_MAX] = { false };

void CParticleManager::InitAll(void)
{
	for(int i = 0; i < PARTICLEMAKER_MAX; i++)
	{
		m_ParticleMaker[i].Uninit();
		m_bUse[i] = false;
	}
	m_nActiveNumber = 0;
}

void CParticleManager::UninitAll(void)
{
	for (int i = 0; i < PARTICLEMAKER_MAX; i++)
	{
		m_ParticleMaker[i].Uninit();
	}
}

void CParticleManager::UpdateAll(void)
{
	for (int i = 0; i < PARTICLEMAKER_MAX; i++)
	{
		if (true == m_bUse[i])
		{
			m_ParticleMaker[i].Update();
		}
	}
}

void CParticleManager::DrawAll(void)
{
	for (int i = 0; i < PARTICLEMAKER_MAX; i++)
	{
		if (true == m_bUse[i])
		{
			m_ParticleMaker[i].Draw();
		}
	}
}

void CParticleManager::SetActiveNumber(int nNum)
{
	nNum = max(0, nNum);
	nNum = min(nNum, PARTICLEMAKER_MAX - 1);
	m_nActiveNumber = nNum;
}

void CParticleManager::SetUse(bool bUse)
{
	if (false == m_bUse[m_nActiveNumber])
	{
		m_bUse[m_nActiveNumber] = true;
	}
	else
	{
		m_ParticleMaker[m_nActiveNumber].Uninit();
		m_bUse[m_nActiveNumber] = false;
	}
}

void CParticleManager::SetMakerBase(const D3DXVECTOR3& pos, const D3DXVECTOR3& size, float PerFrameNum, int nLife, const char* BaseTexPass, const char* EffectTexPass)
{
	m_ParticleMaker[m_nActiveNumber].SetMakerBase(pos, size, PerFrameNum, nLife, BaseTexPass, EffectTexPass);
}

void CParticleManager::SetEnvironment(const D3DXVECTOR3& WindDirect, float WindPower, float fMass, float GravityAcc, float CoeffiResist)
{
	m_ParticleMaker[m_nActiveNumber].SetEnvironment(WindDirect, WindPower, fMass, GravityAcc, CoeffiResist);
}

void CParticleManager::SetShootPara(const D3DXVECTOR3& ShootDir, const D3DXVECTOR2& RangeAngle, float Speed)
{
	m_ParticleMaker[m_nActiveNumber].SetShootPara(ShootDir, RangeAngle, Speed);
	
}

void CParticleManager::SetParticleBaseRGBA(RGBA rgba)
{
	m_ParticleMaker[m_nActiveNumber].SetParticleBaseRGBA(rgba);
}

void CParticleManager::SetParticleEffectRGBA(RGBA rgba)
{
	m_ParticleMaker[m_nActiveNumber].SetParticleEffectRGBA(rgba);
}

void CParticleManager::SetBaseEffectRGBA(RGBA BaseRGBA, RGBA EffectRGBA)
{
	m_ParticleMaker[m_nActiveNumber].SetParticleBaseRGBA(BaseRGBA);
	m_ParticleMaker[m_nActiveNumber].SetParticleEffectRGBA(EffectRGBA);
}

void CParticleManager::SetParticleDrawTime(int DrawTimes_Base, int DrawTimes_Effect)
{
	m_ParticleMaker[m_nActiveNumber].SetParticleDrawTime(DrawTimes_Base, DrawTimes_Effect);
}