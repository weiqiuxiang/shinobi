#include "ModelManager.h"
#include <crtdbg.h>

//グローバル
CModelObj *CModelManager::m_ModelSlot[MODELSLOT_NUM] = {};

//すべてのモデルスロット初期化
HRESULT CModelManager::InitAll(void)
{
	for (int nCnt = 0; nCnt < MODELSLOT_NUM; nCnt++)
	{
		if (m_ModelSlot[nCnt] != nullptr)
		{
			m_ModelSlot[nCnt]->Uninit();
			delete m_ModelSlot[nCnt];
			m_ModelSlot[nCnt] = nullptr;
		}

		m_ModelSlot[nCnt] = nullptr;
	}

	return S_OK;
}

//すべてのモデルスロット解放
void CModelManager::UninitAll(void)
{
	for (int nCnt = 0; nCnt < MODELSLOT_NUM; nCnt++)
	{
		if (m_ModelSlot[nCnt] != nullptr)
		{
			m_ModelSlot[nCnt]->Uninit();
			delete m_ModelSlot[nCnt];
			m_ModelSlot[nCnt] = nullptr;
		}
	}
}

//モデルをロードしIDを返す
MODEL_ID CModelManager::LoadModel(const char *FilePass)
{
	for (MODEL_ID IDNum = 0; IDNum < MODELSLOT_NUM; IDNum++)
	{
		if (m_ModelSlot[IDNum] != nullptr)//スロットすでに使用される場合
		{
			const char *SlotFilePass = m_ModelSlot[IDNum]->GetModelPass();   //ファイルパス取得
			int ValueStrcmp = strcmp(SlotFilePass, FilePass);
			if (ValueStrcmp == 0) return IDNum;
			else continue;
		}

		if (m_ModelSlot[IDNum] == nullptr)//スロット空があったらモデルを入れる
		{
			CreateModelObj(IDNum, FilePass);
			return IDNum;
		}
	}

	//例外処理
	MessageBox(nullptr, "使用できるモデルスロットがない", "エラー", MB_OK | MB_ICONHAND);
	return -1;
}

//モデル番号を渡し,モデルポインタを返す
void CModelManager::DrawModel(MODEL_ID IDNum)
{
	if (IDNum == -1) return;

	if (m_ModelSlot[IDNum] == nullptr)
	{
		MessageBox(nullptr, "モデル存在しない,描画できない", "エラー", MB_OK | MB_ICONHAND);
		return;
	}
	m_ModelSlot[IDNum]->Draw();
}

void CModelManager::DrawModelUseMtrl(MODEL_ID IDNum, const D3DMATERIAL9& Mtrl)
{
	if (IDNum == -1) return;

	if (m_ModelSlot[IDNum] == nullptr)
	{
		MessageBox(nullptr, "モデル存在しない,描画できない", "エラー", MB_OK | MB_ICONHAND);
		return;
	}
	m_ModelSlot[IDNum]->DrawMtrl(Mtrl);
}

void CModelManager::DrawModelOnDepthMap(MODEL_ID IDNum, const D3DXMATRIX& world, const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje)
{
	if (IDNum == -1) return;

	if (m_ModelSlot[IDNum] == nullptr)
	{
		_RPT0(0, "[ModelManager.cpp][DrawModelOnDepthMap]モデル存在しない,描画できない\n");
		return;
	}
	m_ModelSlot[IDNum]->DrawOnDepthMap(world, LightView,LightProje);
}

void CModelManager::DrawUseDepthMap(MODEL_ID IDNum, const D3DXMATRIX& world, const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje, LPDIRECT3DTEXTURE9 DepthTexture)
{
	if (IDNum == -1) return;

	if (m_ModelSlot[IDNum] == nullptr)
	{
		_RPT0(0, "[ModelManager.cpp][DrawUseDepthMap]モデル存在しない,描画できない\n");
		return;
	}
	m_ModelSlot[IDNum]->DrawUseDepthMap(world, LightView, LightProje, DepthTexture);
}

void CModelManager::DrawSpecular(MODEL_ID IDNum, const D3DXMATRIX& world)
{
	if (IDNum == -1) return;

	if (m_ModelSlot[IDNum] == nullptr)
	{
		_RPT0(0, "[ModelManager.cpp][DrawUseDepthMap]モデル存在しない,描画できない\n");
		return;
	}
	m_ModelSlot[IDNum]->DrawSpecular(world);
}

//モーションブラーを描画
void CModelManager::DrawMotionBlur(MODEL_ID IDNum, const D3DXMATRIX& worldMtx, const D3DXMATRIX& worldMtxOld, LPDIRECT3DTEXTURE9 OriginalTexture)
{
	if (IDNum == -1) return;

	if (m_ModelSlot[IDNum] == nullptr)
	{
		_RPT0(0, "[ModelManager.cpp][DrawMotionBlur]モデル存在しない,描画できない\n");
		return;
	}
	m_ModelSlot[IDNum]->DrawMotionBlur(worldMtx, worldMtxOld, OriginalTexture);
}

//モデルスロットにモデルを入れる
HRESULT CModelManager::CreateModelObj(MODEL_ID IDNum, const char *FilePass)
{
	if (m_ModelSlot[IDNum] != nullptr)
	{
		MessageBox(nullptr, "このモデルスロットもすでに使用されている", "エラー", MB_OK | MB_ICONHAND);
		return E_FAIL;
	}
	m_ModelSlot[IDNum] = new CModelObj;//実体化
	m_ModelSlot[IDNum]->SetModelPass(FilePass);//ファイルパスをセット
	m_ModelSlot[IDNum]->CreateModelObj();//モデルオブジェクト生成

	return S_OK;
}

const char* CModelManager::GetModelPass(MODEL_ID IDNum)
{
	if (nullptr == m_ModelSlot[IDNum]) return nullptr;
	return m_ModelSlot[IDNum]->GetModelPass();
}

LPD3DXMESH CModelManager::GetMesh(MODEL_ID IDNum)
{
	if (nullptr == m_ModelSlot[IDNum]) {
		return nullptr;
	}
	return m_ModelSlot[IDNum]->GetMesh();
}

void CModelManager::SetAlpha(MODEL_ID IDNum, float Alpha) {
	if (nullptr == m_ModelSlot[IDNum]) {
		return;
	}
	m_ModelSlot[IDNum]->SetAlpha(Alpha);
}