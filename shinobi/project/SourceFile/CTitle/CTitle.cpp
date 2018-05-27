#include "CTitle.h"
#include "main.h"
#include "CModelStorage.h"
#include "CScene2DStorage.h"
#include "CScene3DStorage.h"
#include "CLightManager.h"
#include "CMeshFieldManager.h"
#include "CFontUIScaleFactory.h"
#include "SceneTransition.h"
#include "sound.h"
#include "XInputControl.h"
#include "CGame.h"
#include "CMapEditor.h"
#include "sound.h"
#include "CMotionEditor.h"
#include "CTutorial.h"

//�}�N��
#define FONT_WIDTH (440.0f)			//�V�[���J�ڃt�H���g�̉���
#define FONT_HEIGHT (75.0f)			//�V�[���J�ڃt�H���g�̍���
#define INTERVAL_FONT (70.0f)		//�V�[���J�ڃt�H���g�̊Ԋu

#define SOULGEM_WIDTH (100.0f)
#define SOULGEM_HEIGHT (183.0f)
#define SOULGEM_ALPHA_MIN (70)
#define SOULGEM_ALPHA_MAX (120)

#define SELECTOR_INIT_VALUE (-1)

#define ALPHA_SPEED (1)				//�A���t�@�̕ύX���x

#define MOUSE_WHEEL_DEADZONE (0.2f)

//�O���[�o���ϐ�
HWND CTitle::m_hWnd;

CTitle::CTitle()
{
	for (int i = 0; i < (int)UI_MAX;i++) m_FontUIScale[i] = nullptr;
	for (int i = 0; i < (int)OBJ_MAX; i++) m_OtherObj[i] = nullptr;
}

CTitle::~CTitle()
{
	
}

bool CTitle::Init(HWND hWnd)
{
	InitTitleObj();
	CScene2DStorage::InitAll();
	CScene2DStorage::CreateObject(D3DXVECTOR3(SCREEN_WIDTH*0.5, SCREEN_HEIGHT*0.5, 0.0f), D3DXVECTOR3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f), 0, TITLE_BG_TEX);
	PlaySound(TITLE_BGM);				//BGM�Đ�
	
	hWnd;				//�_�~�[�Q��

	return true;
}

void CTitle::Uninit(void)
{
	CScene2DStorage::UninitAll();
	UninitTitleObj();				//�^�C�g�����g�̃����o�̉��
}

void CTitle::Update(void)
{
	ChangeSelector();				//�I������ύX
	UpdateTitleObj();				//�^�C�g�������o���g�̃A�b�v�f�[�g
	TransitionToOtherScene();		//�V�[���J��
}

void CTitle::Draw(void)
{
	CModelStorage::DrawAll();
	CScene3DStorage::DrawAll();
	CScene2DStorage::DrawAll();
	CMeshFieldManager::DrawAll();

	DrawTitleObj();					//�^�C�g�������o���g�̕`��
}

void CTitle::InitTitleObj(void)
{
	for (int i = 0; i < (int)UI_MAX; i++) {
		if(nullptr != m_FontUIScale[i]){
			m_FontUIScale[i]->Uninit();
			delete m_FontUIScale[i];
			m_FontUIScale[i] = nullptr;
		}
		
		m_FontUIScale[i] = CFontUIScaleFactory::Create(
			D3DXVECTOR3(SCREEN_WIDTH * 0.5f,SCREEN_HEIGHT * 0.6f + INTERVAL_FONT * i,0.0f),
			D3DXVECTOR3(FONT_WIDTH, FONT_HEIGHT, 0.0f),
			D3DXVECTOR3(SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.6f + INTERVAL_FONT * i, 0.0f),
			D3DXVECTOR2(0.0f,i * 0.25f),
			D3DXVECTOR2(1.0f, 0.25f),
			{0,0,0,255},
			false,
			TITLE_SELECT_FONT_TEX
		);

		m_FontUIScale[i]->Update();
	}

	m_Selector = SELECTOR_INIT_VALUE;
	m_SoulJamFade = false;

	//���
	for (int i = 0; i < (int)OBJ_MAX; i++) {
		if (nullptr != m_OtherObj[i]) {
			m_OtherObj[i]->Uninit();
			delete m_OtherObj[i];
			m_OtherObj[i] = nullptr;
		}
	}

	//���S
	m_OtherObj[OBJ_LOGO] = CFontUIScaleFactory::Create(
		D3DXVECTOR3(SCREEN_WIDTH*0.5, SCREEN_HEIGHT*0.3, 0.0f),
		D3DXVECTOR3(800.0f, 177.0f, 0.0f),
		D3DXVECTOR3(SCREEN_WIDTH*0.5, SCREEN_HEIGHT*0.3, 0.0f),
		D3DXVECTOR2(0.0f, 0.0f),
		D3DXVECTOR2(1.0f, 1.0f),
		{ 255,255,255,255 },
		false,
		TITLE_LOGO_TEX
	);
	m_OtherObj[OBJ_LOGO]->Update();

	//�\�[���W�F��
	m_OtherObj[OBJ_SOULGEM] = CFontUIScaleFactory::Create(
		D3DXVECTOR3(SCREEN_WIDTH*0.58f, SCREEN_HEIGHT*0.3f, 0.0f),
		D3DXVECTOR3(SOULGEM_WIDTH, SOULGEM_HEIGHT, 0.0f),
		D3DXVECTOR3(SCREEN_WIDTH*0.58f, SCREEN_HEIGHT*0.3f, 0.0f),
		D3DXVECTOR2(0.0f, 0.0f),
		D3DXVECTOR2(1.0f, 1.0f),
		{ 255,255,255,255 },
		false,
		SOUL_GEM_TEX
	);
	m_OtherObj[OBJ_SOULGEM]->Update();

	//�\�[���W�F���G�t�F�N�g
	m_OtherObj[OBJ_SOULGEM_EF] = CFontUIScaleFactory::Create(
		D3DXVECTOR3(SCREEN_WIDTH*0.58f, SCREEN_HEIGHT*0.3f, 0.0f),
		D3DXVECTOR3(SOULGEM_WIDTH, SOULGEM_HEIGHT, 0.0f),
		D3DXVECTOR3(SCREEN_WIDTH*0.58f, SCREEN_HEIGHT*0.3f, 0.0f),
		D3DXVECTOR2(0.0f, 0.0f),
		D3DXVECTOR2(1.0f, 1.0f),
		{ 250,31,227,20 },
		true,
		SOUL_GEM_EF_TEX
	);
	m_OtherObj[OBJ_SOULGEM_EF]->SetScale(1.0f);
	m_OtherObj[OBJ_SOULGEM_EF]->Update();
}

void CTitle::UninitTitleObj(void)
{
	for (int i = 0; i < (int)UI_MAX; i++) {
		if (nullptr != m_FontUIScale[i]) {
			m_FontUIScale[i]->Uninit();
			delete m_FontUIScale[i];
			m_FontUIScale[i] = nullptr;
		}
	}
	for (int i = 0; i < (int)OBJ_MAX; i++) {
		if (nullptr != m_OtherObj[i]) {
			m_OtherObj[i]->Uninit();
			delete m_OtherObj[i];
			m_OtherObj[i] = nullptr;
		}
	}
}

void CTitle::UpdateTitleObj(void)
{
	SoulGemFadeEffect();					//�\�[���W�F���G�t�F�N�g

	//�V�[���J�ڃI�u�W�F
	for (int i = 0; i < (int)UI_MAX; i++) {
		if (nullptr != m_FontUIScale[i]) {
			if (i != (int)m_Selector) m_FontUIScale[i]->SetScale(1.0f);
			else m_FontUIScale[i]->SetScale(1.2f);
			m_FontUIScale[i]->Update();
		}
	}
	//���h���I�u�W�F
	for (int i = 0; i < (int)OBJ_MAX; i++) {
		if (nullptr != m_OtherObj[i]) {
			m_OtherObj[i]->Update();
		}
	}
}

void CTitle::DrawTitleObj(void)
{
	//�V�[���J�ڃI�u�W�F
	for (int i = 0; i < (int)UI_MAX; i++) {
		if (nullptr != m_FontUIScale[i]) {
			m_FontUIScale[i]->Draw();
		}
	}
	//���h���I�u�W�F
	for (int i = 0; i < (int)OBJ_MAX; i++) {
		if (nullptr != m_OtherObj[i]) {
			m_OtherObj[i]->Draw();
		}
	}
}

void CTitle::ChangeSelector(void)
{
	//�L�[�{�[�h�̑���
	if (CSceneTransition::GetInputKeyboard()->GetKeyTrigger(DIK_UP)) {
		if (m_Selector == SELECTOR_INIT_VALUE) m_Selector = 0;
		else if (--m_Selector < 0) m_Selector = (int)UI_MAX - 1;
		PlaySound(TITLE_SE_CURSOR);
	}
	if (CSceneTransition::GetInputKeyboard()->GetKeyTrigger(DIK_DOWN)){
		if (m_Selector == SELECTOR_INIT_VALUE) m_Selector = 0;
		else m_Selector = ++m_Selector % UI_MAX;
		PlaySound(TITLE_SE_CURSOR);
	}

	//�R���g���[���̑���
	if (CSceneTransition::GetXInput()->GetTriggerButton(XINPUT_GAMEPAD_DPAD_UP)) {
		if (m_Selector == SELECTOR_INIT_VALUE) m_Selector = 0;
		else if (--m_Selector < 0) m_Selector = (int)UI_MAX - 1;
		PlaySound(TITLE_SE_CURSOR);
	}
	if (CSceneTransition::GetXInput()->GetTriggerButton(XINPUT_GAMEPAD_DPAD_DOWN)) {
		if (m_Selector == SELECTOR_INIT_VALUE) m_Selector = 0;
		else m_Selector = ++m_Selector % UI_MAX;
		PlaySound(TITLE_SE_CURSOR);
	}

	//�}�E�X�z�C�[���̑���
	DIMOUSESTATE* pMouseState = CSceneTransition::GetInputMouse()->GetMouseStates();
	LONG MouseWheelValue = pMouseState->lZ;
	if (MouseWheelValue > MOUSE_WHEEL_DEADZONE) {
		if (m_Selector == SELECTOR_INIT_VALUE) m_Selector = 0;
		else if (--m_Selector < 0) m_Selector = (int)UI_MAX - 1;
		PlaySound(TITLE_SE_CURSOR);
	}
	if (MouseWheelValue < -MOUSE_WHEEL_DEADZONE) {
		if (m_Selector == SELECTOR_INIT_VALUE) m_Selector = 0;
		else m_Selector = ++m_Selector % UI_MAX;
		PlaySound(TITLE_SE_CURSOR);
	}
}

void CTitle::TransitionToOtherScene(void)
{
	bool bController = CSceneTransition::GetXInput()->GetTriggerButton(XINPUT_GAMEPAD_A);
	bool bKeyBoard = CSceneTransition::GetInputKeyboard()->GetKeyTrigger(DIK_RETURN);
	bool bMouse = CSceneTransition::GetInputMouse()->GetMouseTrigger(MOUSE_LEFT);
	//���̓f�o�C�X����̓��͂Ȃ��Ȃ�return
	if (!bKeyBoard && !bController && !bMouse) {
		return;
	}
	//���I����ԂȂ�return
	if (m_Selector == SELECTOR_INIT_VALUE) {
		return;
	}

	//�I��SE���Đ�
	PlaySound(TITLE_SE_DECISION);

	//�I�����ɂ��J��
	switch(m_Selector){
	case UI_START:
		CSceneTransition::SetScene(CSceneTransition::FADE_OUT, new CTutorial());
		break;
	case UI_MAP_EDITOR:
		CSceneTransition::SetScene(CSceneTransition::FADE_OUT, new CMapEditor());
		break;
	case UI_MOTION_EDITOR:
		CSceneTransition::SetScene(CSceneTransition::FADE_OUT, new CMotionEditor());
		break;
	case UI_EXIT:
		DestroyWindow(m_hWnd);
		break;
	default:
		MessageBox(nullptr, "CTitle.cpp�̃��\�b�hTransitionToOtherScene�G���[", "", MB_ICONHAND);
		break;
	}
}

void CTitle::SoulGemFadeEffect(void)
{
	if (nullptr == m_OtherObj[OBJ_SOULGEM_EF]) return;

	if(false == m_SoulJamFade){
		m_OtherObj[OBJ_SOULGEM_EF]->AddLayerA(ALPHA_SPEED);
		int Alpha = m_OtherObj[OBJ_SOULGEM_EF]->GetLayerA();
		if(Alpha >= SOULGEM_ALPHA_MAX){
			m_OtherObj[OBJ_SOULGEM_EF]->SetLayerA(SOULGEM_ALPHA_MAX);
			m_SoulJamFade = true;
		}
	}

	else{
		m_OtherObj[OBJ_SOULGEM_EF]->AddLayerA(-ALPHA_SPEED);
		int Alpha = m_OtherObj[OBJ_SOULGEM_EF]->GetLayerA();
		if (Alpha <= SOULGEM_ALPHA_MIN) {
			m_OtherObj[OBJ_SOULGEM_EF]->SetLayerA(SOULGEM_ALPHA_MIN);
			m_SoulJamFade = false;
		}
	}
}