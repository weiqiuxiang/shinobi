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

//�O���[�o���ϐ�
CInputKeyboard *CSceneTransition::m_pInputKeyboard = nullptr;
CInputMouse *CSceneTransition::m_pInputMouse = nullptr;
XInputControl *CSceneTransition::m_pXInput = nullptr;

CScene2D CSceneTransition::m_Fade;
CSceneTransition::FADE_MODE CSceneTransition::m_FadeMode = CSceneTransition::FADE_NONE;
CSceneManager *CSceneTransition::m_StorageBeforeFade = nullptr;

//�R���X�g���N�^�ƃf�X�g���N�^
CSceneTransition::CSceneTransition()
{
	m_SceneManager = nullptr;
}

CSceneTransition::~CSceneTransition()
{
	
}

//Init,Uninit,Update,Draw�֐�
void CSceneTransition::Init(HINSTANCE hInst, HWND hWnd, BOOL bWindow)
{
	//���f���}�l�W���[������
	CModelManager::InitAll();

	//�e�N�X�`���}�l�[�W���[������
	CTexManager::InitAll();

	//Renderer������
	CRenderer::Init(hWnd, bWindow);

	//�f�o�b�N�t�H���g�̏�����
	CDebug::Init();

	InitSound(hWnd);				//�T�E���h������

	//���͑��u������
	m_pInputKeyboard = new CInputKeyboard();
	m_pInputKeyboard->Init(hInst, hWnd);
	m_pInputMouse = new CInputMouse();
	m_pInputMouse->Init(hInst, hWnd);
	m_pXInput = new XInputControl();
	m_pXInput->Init(true);

	//Scene�}�l�[�W���[�B������
	m_SceneManager = new CTitle();
	if (m_SceneManager != nullptr) {
		m_SceneManager->Init(hWnd);
	}

	//�t�F�[�h������
	m_Fade.Init(D3DXVECTOR3(SCREEN_WIDTH*0.5, SCREEN_HEIGHT*0.5, 0.0f), D3DXVECTOR3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f), nullptr);
	RGBA rgba = { 0,0,0,255 };
	m_Fade.SetRGBA(rgba);

	//���̕ϐ�������
	m_FadeMode = FADE_IN;

	//�f�o�b�N���b�Z�[�W��\�����
	CShowDebugFlag::SetDebugFlag(false);
	CShowDebugFlag::SetDebugFlagLayer2(false);

	//�V�F�[�_�[�}�l�[�W��������
	CShaderManager *pShaderManager = CShaderManager::GetInstance();
	if (pShaderManager != nullptr) {
		pShaderManager->CreateAllShader(hWnd);
	}

	//�[�x�����_�[�^�[�Q�b�g������
	CDepthRenderTarget* pDepthRenderTarget = CDepthRenderTarget::GetInstance();
	if (pDepthRenderTarget == nullptr) {
		MessageBox(nullptr, "pDepthRenderTarget�̏��������s", "[SceneTransition.cpp][Init]", MB_ICONHAND);
		return;
	}
	pDepthRenderTarget->Init();

	//�E�B���h�E�n���h����ۑ�
	m_Wnd = hWnd;

	srand((unsigned)time(nullptr));				//����
}

void CSceneTransition::Uninit(void)
{
	//�L�[�{�[�h�|�C���g�j��
	if (m_pInputKeyboard != nullptr)
	{
		m_pInputKeyboard->Uninit();
		delete m_pInputKeyboard;
		m_pInputKeyboard = nullptr;
	}

	//�}�E�X�j��
	if (m_pInputMouse != nullptr)
	{
		m_pInputMouse->Uninit();
		delete m_pInputMouse;
		m_pInputMouse = nullptr;
	}
	//�R���g���[���̔j��
	if (m_pXInput != nullptr)
	{
		delete m_pXInput;
		m_pXInput = nullptr;
	}

	//�eScene�}�l�[�W���[���
	if (nullptr != m_SceneManager)
	{
		m_SceneManager->Uninit();
		delete m_SceneManager;
		m_SceneManager = nullptr;
	}

	//���̃����o�ϐ����
	m_Fade.Uninit();
	CDebug::Uninit();				//�f�o�b�N�t�H���g�̉��
	CRenderer::Uninit();
	CTexManager::UninitAll();		//�e�N�X�`���}�l�[�W���[���
	CModelManager::UninitAll();		//���f���}�l�W���[���

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

	UninitSound();					//�T�E���h���
}

void CSceneTransition::Update(void)
{
	CDebug::ClearText();			//�f�o�b�N�t�H���g�̃N���A
	SceneManagerAndFadeUpdate();	//�V�[���}�l�[�W���[�ƃt�F�[�h�̍X�V
}

void CSceneTransition::Draw(void)
{
	//�o�b�N�o�b�t�@�N���A
	CRenderer::DrawClear();

	CRenderer::DrawBegin();
	if (m_SceneManager != nullptr) {
		m_SceneManager->Draw();
	}

	m_Fade.Draw();          //�t�F�[�h�`��

#ifdef _DEBUG
	if (CShowDebugFlag::GetShowDebugFlag() == true) {
		CFog::DisableFog();					//�t�H�O�𖳌��ɂ���
		CDebug::Draw();
	}
#endif
	CRenderer::DrawEnd();

	CRenderer::DrawBegin();
	ImGui::Render();						//imgui�����_�����O
	CRenderer::DrawEnd();

	//�`��I��
	CRenderer::DrawPresent();
}

void CSceneTransition::SceneManagerAndFadeUpdate(void)
{
	if (m_pInputKeyboard != nullptr) m_pInputKeyboard->Update();			//�L�[�{�[�h�̍X�V����
	if (m_pInputMouse != nullptr) m_pInputMouse->Update();					//�}�E�X�̍X�V����
	if (m_pXInput != nullptr) m_pXInput->Update();							//���͍X�V
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
		SetAllSoundVolum(1.0f - percent);					//�T�E���h�̃t�F�[�h�A�E�g
		if (AlphaValue == 255)
		{
			SetAllSoundVolum(0.0f);
			StopSound();					//�S�ẴT�E���h��~
			m_FadeMode = FADE_IN;
			SceneTransition();				//�V�[���J�ڊJ�n
		}
		break;
	}
	case FADE_IN:
	{
		int AlphaValue;
		m_Fade.ChangeAlpha(-FADE_SPEED);
		AlphaValue = m_Fade.GetLayerA();
		float percent = (float)AlphaValue / 255;
		SetAllSoundVolum(1.0f - percent);					//�T�E���h�̃t�F�[�h�C��
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
		if (m_pInputKeyboard->GetKeyTrigger(DIK_F1)) {						//�f�o�b�N���b�Z�[�W�̕\��/��\���̐؂�ւ�
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