#include "CModelStorage.h"
#include "CCameraManager.h"

CModel *CModelStorage::m_Obj[MODEL_OBJ_MAX] = {};

void CModelStorage::InitAll(void)
{
	for (int ID = 0; ID < MODEL_OBJ_MAX; ID++)
	{
		if (m_Obj[ID] != nullptr)
		{
			m_Obj[ID]->Uninit();
			delete m_Obj[ID];
		}
		m_Obj[ID] = nullptr;
	}
}

void CModelStorage::UninitAll(void)
{
	for (int ID = 0; ID < MODEL_OBJ_MAX; ID++)
	{
		if (m_Obj[ID] != nullptr)
		{
			m_Obj[ID]->Uninit();
			delete m_Obj[ID];
			m_Obj[ID] = nullptr;
		}
	}
}

void CModelStorage::UpdateAll(void)
{
	for (int ID = 0; ID < MODEL_OBJ_MAX; ID++)
	{
		if (m_Obj[ID] != nullptr) {
			m_Obj[ID]->Update();
		}
	}
}

void CModelStorage::DrawAll(void)
{
	for (int ID = 0; ID < MODEL_OBJ_MAX; ID++)
	{
		if (m_Obj[ID] != nullptr)
		{
			m_Obj[ID]->SetMtxSetting();
			m_Obj[ID]->Draw();
		}
	}
}

void CModelStorage::DrawAllAABBTest(void)
{
	for (int ID = 0; ID < MODEL_OBJ_MAX; ID++)
	{
		if (m_Obj[ID] != nullptr)
		{
			m_Obj[ID]->DrawAABBTest();
		}
	}
}

void CModelStorage::DrawAllOnDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje)
{
	for (int ID = 0; ID < MODEL_OBJ_MAX; ID++)
	{
		if (m_Obj[ID] != nullptr)
		{
			m_Obj[ID]->DrawOnDepthMap(m_Obj[ID]->GetResultMtx(),LightView, LightProje);
		}
	}
}

void CModelStorage::DrawUseDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje, LPDIRECT3DTEXTURE9 DepthTexture)
{
	for (int ID = 0; ID < MODEL_OBJ_MAX; ID++)
	{
		if (m_Obj[ID] != nullptr)
		{
			m_Obj[ID]->DrawUseDepthMap(m_Obj[ID]->GetResultMtx(), LightView, LightProje, DepthTexture);
		}
	}
}

void CModelStorage::DrawSpecular(void)
{
	for (int ID = 0; ID < MODEL_OBJ_MAX; ID++)
	{
		if (m_Obj[ID] != nullptr)
		{
			m_Obj[ID]->DrawSpecular(m_Obj[ID]->GetResultMtx());
		}
	}
}

void CModelStorage::Draw_ID(int ID)
{
	bool OverRange = (ID < 0) || (ID > MODEL_OBJ_MAX);
	if (OverRange) {
		MessageBox(nullptr, "MODEL OBJ IDが範囲外,MODEL OBJ取得不能", "エラー", MB_OK | MB_ICONHAND);
		return;
	}
	
	if (m_Obj[ID] != nullptr)
	{
		m_Obj[ID]->SetMtxSetting();
		m_Obj[ID]->Draw();
	}
}

void CModelStorage::DrawUseMtrl_ID(int ID, const D3DMATERIAL9& Mtrl)
{
	bool OverRange = (ID < 0) || (ID > MODEL_OBJ_MAX);
	if (OverRange) {
		MessageBox(nullptr, "MODEL OBJ IDが範囲外,MODEL OBJ取得不能", "エラー", MB_OK | MB_ICONHAND);
		return;
	}

	if (m_Obj[ID] != nullptr)
	{
		m_Obj[ID]->SetMtxSetting();
		m_Obj[ID]->DrawUseMtrl(Mtrl);
	}
}

CModel* CModelStorage::CreateObject(const SRP& SclRotPos, const char* ModelFilepass)
{
	for (int ID = 0; ID < MODEL_OBJ_MAX; ID++)
	{
		if (m_Obj[ID] == nullptr)
		{
			m_Obj[ID] = new CModel();
			m_Obj[ID]->Init(SclRotPos, ModelFilepass);
			m_Obj[ID]->SetHitMeshFlag(true);
			m_Obj[ID]->SetUseID(ID);
			return m_Obj[ID];
		}
	}

	return nullptr;
}

void CModelStorage::Delete(int ID)
{
	if (ID < 0 || ID >= MODEL_OBJ_MAX) {
		return;
	}

	if (m_Obj[ID] != nullptr)
	{
		m_Obj[ID]->Uninit();
		m_Obj[ID] = nullptr;
	}
}

CModel* CModelStorage::GetObj(int ID)
{
	bool OverRange = (ID < 0) || (ID > MODEL_OBJ_MAX);
	if (OverRange) {
		MessageBox(nullptr,"MODEL OBJ IDが範囲外,MODEL OBJ取得不能","エラー",MB_OK | MB_ICONHAND);
		return nullptr;
	}
	return m_Obj[ID];
}

void CModelStorage::AddPos(int ID, const D3DXVECTOR3& pos)
{
	bool OverRange = (ID < 0) || (ID > MODEL_OBJ_MAX);
	if (OverRange) {
		MessageBox(nullptr, "MODEL OBJ IDが範囲外,MODEL OBJ取得不能", "エラー", MB_OK | MB_ICONHAND);
		return;
	}

	m_Obj[ID]->AddPosX(pos.x);
	m_Obj[ID]->AddPosZ(pos.z);
}