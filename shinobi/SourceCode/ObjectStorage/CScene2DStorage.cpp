#include "CScene2DStorage.h"

CScene2D *CScene2DStorage::m_Obj[CSCENE2D_PRIORITY_MAX][CSCENE2D_OBJ_MAX] = {};

void CScene2DStorage::InitAll(void)
{
	for (int priority = 0; priority < CSCENE2D_PRIORITY_MAX; priority++)
	{
		for (int ID = 0; ID < CSCENE2D_OBJ_MAX; ID++)
		{
			if (m_Obj[priority][ID] != nullptr)
			{
				m_Obj[priority][ID]->Uninit();
				delete m_Obj[priority][ID];
			}
			m_Obj[priority][ID] = nullptr;
		}
	}
}

void CScene2DStorage::UninitAll(void)
{
	for (int priority = 0; priority < CSCENE2D_PRIORITY_MAX; priority++)
	{
		for (int ID = 0; ID < CSCENE2D_OBJ_MAX; ID++)
		{
			if (m_Obj[priority][ID] != nullptr)
			{
				m_Obj[priority][ID]->Uninit();
				delete m_Obj[priority][ID];
			}
			m_Obj[priority][ID] = nullptr;
		}
	}
}

void CScene2DStorage::UpdateAll(void)
{
	for (int priority = 0; priority < CSCENE2D_PRIORITY_MAX; priority++)
	{
		for (int ID = 0; ID < CSCENE2D_OBJ_MAX; ID++)
		{
			if (m_Obj[priority][ID] != nullptr) m_Obj[priority][ID]->Update();
		}
	}
}

void CScene2DStorage::DrawAll(void)
{
	for (int priority = 0; priority < CSCENE2D_PRIORITY_MAX; priority++)
	{
		for (int ID = 0; ID < CSCENE2D_OBJ_MAX; ID++)
		{
			if (m_Obj[priority][ID] != nullptr) m_Obj[priority][ID]->Draw();
		}
	}
}

CScene2D* CScene2DStorage::CreateObject(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &Size, const PRIORITY priority, const char* Texfilepass)
{
	for (int ID = 0; ID < CSCENE2D_OBJ_MAX; ID++)
	{
		if (m_Obj[priority][ID] == nullptr)
		{
			m_Obj[priority][ID] = new CScene2D();
			m_Obj[priority][ID]->Init(pos, Size, Texfilepass);
			m_Obj[priority][ID]->SetPriority(priority);
			return m_Obj[priority][ID];
		}
	}

	return nullptr;
}