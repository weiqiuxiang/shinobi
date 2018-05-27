#include "SceneTransition.h"
#include "CTitle.h"
#include "CResult.h"
#include "CTutorial.h"
#include "CGame.h"
#include "CMapEditor.h"
#include "CScene2DStorage.h"
#include "CScene3DStorage.h"
#include "CModelStorage.h"
#include "CCameraManager.h"
#include "CRenderer.h"
#include "CMeshFieldManager.h"
#include "CLightManager.h"
#include "CPlayerControl.h"
#include "CDebug.h"
#include "TexManager.h"
#include "CFog.h"
#include "CShowDebugFlag.h"
#include "sound.h"
#include "CDepthRenderTarget.h"
#include <time.h>
#include <imgui.h>
#include "CShaderManager.h"
#include "ModelManager.h"

#define FADE_SPEED (5)

//グローバル変数
CInputKeyboard *CSceneTransition::m_pInputKeyboard = nullptr;
CInputMouse *CSceneTransition::m_pInputMouse = nullptr;
XInputControl *CSceneTransition::m_pXInput = nullptr;

CScene2D CSceneTransition::m_Fade;
CSceneTransition::FADE_MODE CSceneTransition::m_FadeMode = CSceneTransition::FADE_NONE;
CSceneManager *CSceneTransition::m_StorageBeforeFade = nullptr;

//コンストラクタとデストラクタ
CSceneTransition::CSceneTransition()
{
	m_SceneManager = nullptr;
}

CSceneTransition::~CSceneTransition()
{
	
}

//Init,Uninit,Update,Draw関数
void CSceneTransition::Init(HINSTANCE hInst, HWND hWnd, BOOL bWindow)
{
	//モデルマネジャー初期化
	CModelManager::InitAll();

	//テクスチャマネージャー初期化
	CTexManager::InitAll();

	//Renderer初期化
	CRenderer::Init(hWnd, bWindow);

	//デバックフォントの初期化
	CDebug::Init();

	InitSound(hWnd);				//サウンド初期化

	//入力装置初期化
	m_pInputKeyboard = new CInputKeyboard();
	m_pInputKeyboard->Init(hInst, hWnd);
	m_pInputMouse = new CInputMouse();
	m_pInputMouse->Init(hInst, hWnd);
	m_pXInput = new XInputControl();
	m_pXInput->Init(true);

	//Sceneマネージャー達初期化
	m_SceneManager = new CTitle();
	if (m_SceneManager != nullptr) {
		m_SceneManager->Init(hWnd);
	}

	//フェード初期化
	m_Fade.Init(D3DXVECTOR3(SCREEN_WIDTH*0.5, SCREEN_HEIGHT*0.5, 0.0f), D3DXVECTOR3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f), nullptr);
	RGBA rgba = { 0,0,0,255 };
	m_Fade.SetRGBA(rgba);

	//他の変数初期化
	m_FadeMode = FADE_IN;

	//デバックメッセージを表示状態
	CShowDebugFlag::SetDebugFlag(false);
	CShowDebugFlag::SetDebugFlagLayer2(false);

	//シェーダーマネージャ初期化
	CShaderManager *pShaderManager = CShaderManager::GetInstance();
	if (pShaderManager != nullptr) {
		pShaderManager->CreateAllShader(hWnd);
	}

	//深度レンダーターゲット初期化
	CDepthRenderTarget* pDepthRenderTarget = CDepthRenderTarget::GetInstance();
	if (pDepthRenderTarget == nullptr) {
		MessageBox(nullptr, "pDepthRenderTargetの初期化失敗", "[SceneTransition.cpp][Init]", MB_ICONHAND);
		return;
	}
	pDepthRenderTarget->Init();

	//ウィンドウハンドルを保存
	m_Wnd = hWnd;

	srand((unsigned)time(nullptr));				//乱数
}

void CSceneTransition::Uninit(void)
{
	//キーボードポイント破棄
	if (m_pInputKeyboard != nullptr)
	{
		m_pInputKeyboard->Uninit();
		delete m_pInputKeyboard;
		m_pInputKeyboard = nullptr;
	}

	//マウス破棄
	if (m_pInputMouse != nullptr)
	{
		m_pInputMouse->Uninit();
		delete m_pInputMouse;
		m_pInputMouse = nullptr;
	}
	//コントローラの破棄
	if (m_pXInput != nullptr)
	{
		delete m_pXInput;
		m_pXInput = nullptr;
	}

	//各Sceneマネージャー解放
	if (nullptr != m_SceneManager)
	{
		m_SceneManager->Uninit();
		delete m_SceneManager;
		m_SceneManager = nullptr;
	}

	//他のメンバ変数解放
	m_Fade.Uninit();
	CDebug::Uninit();				//デバックフォントの解放
	CRenderer::Uninit();
	CTexManager::UninitAll();		//テクスチャマネージャー解放
	CModelManager::UninitAll();		//モデルマネジャー解放

	CShaderManager *pShaderManager = CShaderManager::GetInstance();
	if (pShaderManager != nullptr) {
		pShaderManager->DeleteAllShader();
		pShaderManager->DeleteInstance();
	}

	CDepthRenderTarget* pDepthRenderTarget = CDepthRenderTarget::GetInstance();
	if (pDepthRenderTarget != nullptr) {
		pDepthRenderTarget->Uninit();
		pDepthRenderTarget->DeleteInstance();
	}

	UninitSound();					//サウンド解放
}

void CSceneTransition::Update(void)
{
	CDebug::ClearText();			//デバックフォントのクリア
	SceneManagerAndFadeUpdate();	//シーンマネージャーとフェードの更新
}

void CSceneTransition::Draw(void)
{
	//バックバッファクリア
	CRenderer::DrawClear();

	CRenderer::DrawBegin();
	if (m_SceneManager != nullptr) {
		m_SceneManager->Draw();
	}

	m_Fade.Draw();          //フェード描画

#ifdef _DEBUG
	if (CShowDebugFlag::GetShowDebugFlag() == true) {
		CFog::DisableFog();					//フォグを無効にする
		CDebug::Draw();
	}
#endif
	CRenderer::DrawEnd();

	CRenderer::DrawBegin();
	ImGui::Render();						//imguiレンダリング
	CRenderer::DrawEnd();

	//描画終了
	CRenderer::DrawPresent();
}

void CSceneTransition::SceneManagerAndFadeUpdate(void)
{
	if (m_pInputKeyboard != nullptr) m_pInputKeyboard->Update();			//キーボードの更新処理
	if (m_pInputMouse != nullptr) m_pInputMouse->Update();					//マウスの更新処理
	if (m_pXInput != nullptr) m_pXInput->Update();							//入力更新
	m_pXInput->RenderInput();

	m_Fade.Update();
	switch (m_FadeMode)
	{
	case FADE_OUT:
	{
		int AlphaValue;
		m_Fade.ChangeAlpha(FADE_SPEED);
		AlphaValue = m_Fade.GetLayerA();
		float percent = (float)AlphaValue / 255;
		SetAllSoundVolum(1.0f - percent);					//サウンドのフェードアウト
		if (AlphaValue == 255)
		{
			SetAllSoundVolum(0.0f);
			StopSound();					//全てのサウンド停止
			m_FadeMode = FADE_IN;
			SceneTransition();				//シーン遷移開始
		}
		break;
	}
	case FADE_IN:
	{
		int AlphaValue;
		m_Fade.ChangeAlpha(-FADE_SPEED);
		AlphaValue = m_Fade.GetLayerA();
		float percent = (float)AlphaValue / 255;
		SetAllSoundVolum(1.0f - percent);					//サウンドのフェードイン
		if (AlphaValue == 0){
			SetAllSoundVolum(1.0f);
			m_FadeMode = FADE_NONE;
		}
		break;
	}
	default:
		//CSceneTraDecision::SceneTraCheck();
		if (m_SceneManager != nullptr) m_SceneManager->Update();

#ifdef _DEBUG
		if (m_pInputKeyboard->GetKeyTrigger(DIK_F1)) {						//デバックメッセージの表示/非表示の切り替え
			if (CShowDebugFlag::GetShowDebugFlag() == false) CShowDebugFlag::SetDebugFlag(true);
			else CShowDebugFlag::SetDebugFlag(false);
		}
		if (m_pInputKeyboard->GetKeyTrigger(DIK_F2)) {
			if (CShowDebugFlag::GetShowDebugFlagLayer2() == false) CShowDebugFlag::SetDebugFlagLayer2(true);
			else CShowDebugFlag::SetDebugFlagLayer2(false);
		}
#endif
		break;
	}
}

void CSceneTransition::SceneTransition(void)
{

	if (nullptr != m_SceneManager)
	{
		m_SceneManager->Uninit();
		delete m_SceneManager;
		m_SceneManager = nullptr;
	}

	m_SceneManager = m_StorageBeforeFade;
	
	if (nullptr != m_SceneManager)
	{
		m_SceneManager->Init(m_Wnd);
	}
}

void CSceneTransition::SetScene(FADE_MODE fadeMode, CSceneManager *pMode)
{
	m_FadeMode = fadeMode;
	m_StorageBeforeFade = pMode;
}