#include "CLightManager.h"
#include <crtdbg.h>

CLight *CLightManager::m_CLight[LIGHT_MAX] = {nullptr};

void CLightManager::InitAll(void)
{
	for (int i = 0; i < LIGHT_MAX; i++)
	{
		if(m_CLight[i] != nullptr)
		{
			delete m_CLight[i];
			m_CLight[i] = nullptr;
		}
		m_CLight[i] = nullptr;
	}
}

void CLightManager::UninitAll(void)
{
	for (int i = 0; i < LIGHT_MAX; i++)
	{
		if (m_CLight[i] != nullptr)
		{
			delete m_CLight[i];
			m_CLight[i] = nullptr;
		}
	}
}

void CLightManager::CreateLight(const D3DLIGHT9& Light)
{
	for(int i = 0;i < LIGHT_MAX;i++)
	{
		if(m_CLight[i] == nullptr)
		{
			m_CLight[i] = new CLight;
			m_CLight[i]->Init(Light);
			break;
		}
	}
}

void CLightManager::UseLight(int LightNumber)
{
	bool bNumberOver = (LightNumber < 0) || (LightNumber >= LIGHT_MAX);

	if (bNumberOver)
	{
		_RPT0(0, "[CLightManager.cpp][UseLight]使用するライト番号は存在しない\n");
		return;
	}

	if (m_CLight[LightNumber] == nullptr)
	{
		_RPT0(0, "[CLightManager.cpp][UseLight]使用するライト番号はまだcreateしていない\n");
		return;
	}

	m_CLight[LightNumber]->UseLight();
}

D3DXVECTOR3 CLightManager::GetLightDirection(int LightNumber)
{
	bool bNumberOver = (LightNumber < 0) || (LightNumber >= LIGHT_MAX);

	if (bNumberOver)
	{
		_RPT0(0, "[CLightManager.cpp][UseLight]使用するライト番号は存在しない\n");
		return D3DXVECTOR3(0.0f,0.0f,0.0f);
	}

	if (m_CLight[LightNumber] == nullptr)
	{
		_RPT0(0, "[CLightManager.cpp][UseLight]使用するライト番号はまだcreateしていない\n");
		return D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}

	return m_CLight[LightNumber]->GetLightDiretion();
}

void CLightManager::SetLightPara(int LightNumber, const D3DLIGHT9& Light)
{
	bool bNumberOver = (LightNumber < 0) || (LightNumber >= LIGHT_MAX);
	if(bNumberOver)
	{
		return;
	}

	if (m_CLight[LightNumber] == nullptr) return;
	m_CLight[LightNumber]->SetLightAllPara(Light);
}