#include "CResult.h"
#include "main.h"

#include "CModelStorage.h"
#include "CScene2DStorage.h"
#include "CScene3DStorage.h"
#include "CLightManager.h"
#include "CMeshFieldManager.h"
#include "SceneTransition.h"
#include "CTitle.h"
#include "CRenderer.h"

/**************************SCENE_RESULT部分*****************************/
bool CResult::Init(HWND hWnd)
{
	CScene2DStorage::InitAll();
	CScene2DStorage::CreateObject(D3DXVECTOR3(SCREEN_WIDTH*0.5, SCREEN_HEIGHT*0.5, 0.0f), D3DXVECTOR3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f), 0, RESULT_TEX);

	hWnd;		//ダミー参照

	return true;
}
void CResult::Uninit(void)
{
	CScene2DStorage::UninitAll();
}

void CResult::Update(void)
{
	if (CSceneTransition::GetInputKeyboard()->GetKeyTrigger(DIK_RETURN) || CSceneTransition::GetXInput()->GetTriggerButton(VK_PAD_X)) {
		CSceneTransition::SetScene(CSceneTransition::FADE_OUT, new CTitle());
	}
}

void CResult::Draw(void)
{
	CModelStorage::DrawAll();
	CScene3DStorage::DrawAll();
	CMeshFieldManager::DrawAll();

	CScene2DStorage::DrawAll();
}