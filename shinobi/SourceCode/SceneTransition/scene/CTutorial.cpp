#include "CTutorial.h"
#include "main.h"

#include "CModelStorage.h"
#include "CScene2DStorage.h"
#include "CScene3DStorage.h"
#include "CLightManager.h"
#include "CMeshFieldManager.h"
#include "CRenderer.h"
#include "SceneTransition.h"
#include "CGame.h"

/**************************SCENE_TUTORIAL部分*****************************/
bool CTutorial::Init(HWND hWnd)
{
	CScene2DStorage::InitAll();
	CScene2DStorage::CreateObject(D3DXVECTOR3(SCREEN_WIDTH*0.5, SCREEN_HEIGHT*0.5, 0.0f), D3DXVECTOR3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f), 0, TUTORIAL_TEX);

	hWnd;			//ダミー参照

	return true;
}
void CTutorial::Uninit(void)
{
	CScene2DStorage::UninitAll();
}

void CTutorial::Update(void)
{
	if (CSceneTransition::GetInputKeyboard()->GetKeyTrigger(DIK_RETURN) || CSceneTransition::GetXInput()->GetTriggerButton(VK_PAD_X)) {
		CSceneTransition::SetScene(CSceneTransition::FADE_OUT, new CGame());
	}
}

void CTutorial::Draw(void)
{
	CModelStorage::DrawAll();
	CScene3DStorage::DrawAll();
	CMeshFieldManager::DrawAll();
	CScene2DStorage::DrawAll();
}