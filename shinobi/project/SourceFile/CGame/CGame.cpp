#include "CGame.h"
#include "CTitle.h"
#include "CResult.h"
#include "main.h"
#include "CScene3D.h"

#include "CModelStorage.h"
#include "CScene2DStorage.h"
#include "CScene3DStorage.h"
#include "CBloodStorage.h"
#include "CLightManager.h"
#include "CMeshFieldManager.h"

#include "CCameraManager.h"
#include "ModelManager.h"
#include "TexManager.h"
#include "CAllObj3DHitMesh.h"

#include "CPlayer.h"
#include "CPlayerControl.h"
#include "CWeapon.h"

#include "CEnemy.h"
#include "CEnemyStorage.h"
#include "CLifeGaugeStorage.h"

#include "CCollisionPlayerAndEnemy.h"
#include "CFontUIScaleFactory.h"
#include "CLinkPlayerToUI.h"

#include "SceneTransition.h"
#include "CGameUI.h"
#include "CCollisionJudge.h"
#include "CRenderer.h"
#include "CShaderManager.h"
#include "CDepthRenderTarget.h"
#include "CCollisionPlayerAndModel.h"
#include "CPlayerPatternManager.h"
#include "CCreatureDataIO.h"

//�}�O��
#define FLOOR_BLOCK_HEIGHT (30.0f)

#define LOCK_ON_ICON_SIZE_X (4.0f)
#define LOCK_ON_ICON_SIZE_Y (4.0f)

#define DEFAULT_FIELD_COFFI (0.2f)		//�f�t�H���g�n�ʖ��C�W��

#define CLEAR_TO_RESULT_TIME (180)

//���C�t�Q�[�W�̃T�C�Y
#define LIFE_GAUGE_SIZEX (4.0f)
#define LIFE_GAUGE_SIZEY (0.5f)

#define DOG_LIFE_GAUGE_POSY_FROM_BODY (3.0f)		//���C�t�Q�[�W�̈ʒu�́A�^�C�v�͌��̏ꍇ�A�{�f�B�̈ʒu���痣��鋗��

CGame::CGame()
{
	m_GameUI = new CGameUI();
	m_PlayerControl = new CPlayerControl();
	m_Player = new CPlayer();

	//���b�N�I���G�t�F�N�g������
	m_LockOnEF.Init(D3DXVECTOR3(48.0f, 48.0f,0.0f), "data/TEXTURE/LockonEF.png");
	m_GoToResultTime = CLEAR_TO_RESULT_TIME;
}

CGame::~CGame() 
{
	if (nullptr != m_GameUI) {
		m_GameUI->UninitUIObj();
		delete m_GameUI;
		m_GameUI = nullptr;
	}
	if (nullptr != m_PlayerControl) {
		m_PlayerControl->Uninit();
		delete m_PlayerControl;
		m_PlayerControl = nullptr;
	}

	if (nullptr != m_Player) {
		m_Player->Uninit();
		delete m_Player;
		m_Player = nullptr;
	}

	m_LockOnEF.Uninit();
}

bool CGame::Init(HWND hWnd)
{
	hWnd;		//dummy�Q��

	InitStatic();		//static�^�̕����̏�����

	//�R���g���[�����j�b�g������
	m_PlayerControl->Init();				//�v���[���[�R���g���[���̏�����
	CEnemyStorage::InitAll();

	//�J����������
	CCameraManager::SetCameraPara(
		CCameraManager::CAMERA_PLAYER, 
		{ D3DXVECTOR3(45.0f,5.0f,75.0f) ,D3DXVECTOR3(20.0f,0.0f,25.0f) ,D3DXVECTOR3(0.0f, 1.0f, 0.0f) }
	);

	InitLight();		//���C�g�����̏�����

	//���b�V���t�B�[���h
	m_MeshLoader.ImportMeshData(STAGE01_MESH_DATA);
	m_ModelLoader.ImportObj3DData(STAGE01_3DOBJ_DATA);

	//�v���[���[�̏�����
	m_Player->Init(D3DXVECTOR3(35.0f,0.0,70.0f), D3DXVECTOR3(0.0f, D3DX_PI * 0.5f, 0.0f), MOTION_DATA_PLAYER);
	m_Player->SetEnvironment(1.0f,-0.03f, DEFAULT_FIELD_COFFI);

	//������
	CWeapon *pWeapon01 = new CWeapon();
	pWeapon01->Init(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), MOTION_DATA_L_SWORD);
	pWeapon01->SetEnvironment(1.0f, 0.0f, 0.0f);
	CWeapon *pWeapon02 = new CWeapon();
	pWeapon02->Init(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), MOTION_DATA_R_SWORD);
	pWeapon02->SetEnvironment(1.0f, 0.0f, 0.0f);
	m_Player->SetWeaponLeft(pWeapon01,D3DXVECTOR3(-0.25f,0.0f,0.1f),5);
	m_Player->SetWeaponRight(pWeapon02, D3DXVECTOR3(0.25f, 0.0f, 0.1f), 8);

	//�G�R���e�i�Ƀv���[���[��ݒu
	CEnemyStorage::SetPlayer(m_Player);
	CCollisionPlayerAndModel::Init(m_Player->GetBodyPos());

	InitEnemy();

	m_PlayerControl->SetPlayer(m_Player);			//�v���[���[�R���g���[���̃v���[���[�Z�b�g
	CCollisionJudge::SetPlayer(m_Player);			//���蔻��̃v���[���[�Z�b�g

	m_GameUI->InitUIObj();					//UI�I�u�W�F�N�g������

	m_SkyDoom.Init(300.0f,30,30, SKYDOME_TEX);
	m_SkyDoom2.Init(350.0f, 30, 30, SKYDOME2_TEX);
	m_SkyDoom2.SetRot(D3DXVECTOR3(D3DX_PI * 0.5f,0.0f,0.0f));


	//�����_�[�^�[�Q�b�g�e�X�g
	SRP RenderTargetTest;
	RenderTargetTest.pos = D3DXVECTOR3(-5.0f,10.0f,-5.0f);
	RenderTargetTest.rot = D3DXVECTOR3(0.0f, -D3DX_PI*0.5, 0.0f);
	RenderTargetTest.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	//�����_�[�^�[�Q�b�g������
	m_SceneRenderTarget.Init(D3DXVECTOR3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f), D3DFMT_A8B8G8R8);
	
	//�N���A��A���U���g�ɍs�����ԏ�����
	m_GoToResultTime = CLEAR_TO_RESULT_TIME;


	CAllObj3DHitMesh::ColliAllObj3DAndAllMesh();		//3D�I�u�W�F�N�g�ƒn�ʂ̓��蔻��
	Update();					//�S�Ĉ��X�V

	return true;
}

/*==============================================================================
	static�^�̃I�u�W�F�N�g�̏�����
===============================================================================*/
void CGame::InitStatic(void)
{
	CScene2DStorage::InitAll();
	CCameraManager::InitAll();
	CModelStorage::InitAll();
	CLightManager::InitAll();
	CMeshFieldManager::InitAll();
	CColliPlayerEnemy::Init();
	CCollisionJudge::Clear();
	CLifeGaugeStorage::InitAll();
	CBloodStorage::InitAll();
}

/*==============================================================================
	���C�g�̏�����
===============================================================================*/
void CGame::InitLight(void)
{
	//���C�g
	D3DLIGHT9 Light;
	ZeroMemory(&Light, sizeof(D3DLIGHT9));                      //���C�g������
	Light.Type = D3DLIGHT_DIRECTIONAL;                          //���C�g�̃^�C�v���w��
	Light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);          //�f�B���N�V���i�����C�g�̐F
	Light.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);          //�A���r�G���g���C�g
	D3DXVECTOR3 vecDir(-1.0f, -1.0f, -1.0f);                         //���C�g����
	D3DXVec3Normalize((D3DXVECTOR3*)&Light.Direction, &vecDir); //���C�g������ݒ肵�����C�g�ɓ����
	CLightManager::CreateLight(Light);

	m_CameraLight.Init(D3DXVECTOR3(-50,-50,-50), (D3DXVECTOR3)Light.Direction, D3DXVECTOR3(0, 1, 0));
	m_CameraLight.SetCameraPosAndLightDir(D3DXVECTOR3(50, 50, 50), (D3DXVECTOR3)Light.Direction,100);
	m_CameraLight.SetFar(200.0f);
}

void CGame::InitEnemy(void)
{
	CCreatureDataIO EnemyIO;
	EnemyIO.ImportEnemyObjData("data/CREATURE_DATA/EnemyDataOutput.txt");

	//���C�t�Q�[�W������
	auto it = CEnemyStorage::GetEnemiesListBegin();
	auto itEnd = CEnemyStorage::GetEnemiesListEnd();
	for (; it != itEnd; ++it) {
		if (*it != nullptr) {
			D3DXVECTOR3 BodyPos = (*it)->GetBodyPos();
			CEnemy::ENEMY_TYPE EnemyType = (*it)->GetEnemyType();
			switch (EnemyType) {
			case CEnemy::TYPE_DOG:
				BodyPos.y = BodyPos.y + DOG_LIFE_GAUGE_POSY_FROM_BODY;
				break;
			}

			CLifeGaugeStorage::CreateObject(BodyPos, D3DXVECTOR3(LIFE_GAUGE_SIZEX, LIFE_GAUGE_SIZEY, 0.0f), (*it)->GetLife());
		}
	}
}

void CGame::Uninit(void)
{
	CScene2DStorage::UninitAll();
	CScene3DStorage::UninitAll();
	CModelStorage::UninitAll();
	CLightManager::UninitAll();
	CMeshFieldManager::UninitAll();
	CEnemyStorage::UninitAll();
	CBloodStorage::UninitAll();

	m_GameUI->UninitUIObj();
	m_SkyDoom.Uninit();
	m_SkyDoom2.Uninit();

	m_SceneRenderTarget.Uninit();
}

void CGame::Update(void)
{
	//�X�V
	CScene2DStorage::UpdateAll();
	CScene3DStorage::UpdateAll();
	CModelStorage::UpdateAll();
	CMeshFieldManager::UpdateAll();
	CEnemyStorage::UpdateAll();
	CEnemyStorage::UpdateAllCollisionToModel();
	CLifeGaugeStorage::UpdateAll();

	m_PlayerControl->Update();					//�v���[���[�R���g���[���X�V
	m_Player->Update();							//�v���[���[�X�V
	CCollisionPlayerAndModel::Update(m_Player);	//�v���[���[�ƃ��f���̓��蔻��

	//���b�N�I���\������
	UpdateLockOnEF();

	//�X�J�C�h�[���X�V
	m_SkyDoom.Update();
	m_SkyDoom2.Update();

	//���蔻��X�V
	CCollisionJudge::SetPlayer(m_Player);
	CCollisionJudge::Update();
	CCollisionJudge::Clear();

	//�U���̌����Ԃ��Ɋւ���X�V
	CColliPlayerEnemy::UpdateBlood();
	CBloodStorage::UpdateAll();

	//UI�X�V
	m_GameUI->UpdateUIObj();

	//�e�X�g
	//m_FeedBackBlur.DrawBlurOnDrawTarget(this);

	//���C�g���̃J�����X�V
	D3DXVECTOR3 PlayerPos = m_Player->GetBodyPos();
	D3DXVECTOR3 LightDir = CLightManager::GetLightDirection(0);
	m_CameraLight.SetCameraLookAt(PlayerPos - 50.0f * LightDir, PlayerPos + 50.0f * LightDir);
	m_CameraLight.SetViewMtx();
	m_CameraLight.SetProjectionMtx();
	
	//�V�[���J�ڔ���X�V
	m_PlayerControl->SceneTraDecision(&m_TraFlag);
	if (CEnemyStorage::EnemyAllDead()) {
		m_GoToResultTime--;
	}
	if (m_GoToResultTime <= 0) {
		m_TraFlag = TRA_TO_RESULT;
	}
	SceneTraDecision();				//�t���O�ɂ��̃V�[���J�ڔ���
}

void CGame::UpdateLockOnEF(void)
{
	if (m_PlayerControl->GetLockOnFlag()){
		m_LockOnEF.SetUse(true);
		m_LockOnEF.SetPos(m_PlayerControl->GetLockOnEnemyPos());
	}

	else {
		m_LockOnEF.SetUse(false);
	}

	m_LockOnEF.Update();
}

void CGame::Draw(void)
{
	//��U�V�[���̕`����I�������܂�
	CRenderer::DrawEnd();

	//�����_�[�^�[�Q�b�g�̃|�C���^�ƃf�o�C�X�̎擾
	CDepthRenderTarget* pDepthRenderTarget = CDepthRenderTarget::GetInstance();
	if (pDepthRenderTarget == nullptr) {
		_RPT0(0, "[CGame.cpp][Draw]pDepthRenderTarget�̓k��\n");
		return;
	}
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	//�o�b�N�o�b�t�@�̃T�[�t�F�[�X�擾
	LPDIRECT3DSURFACE9		BackBufferSurface;
	LPDIRECT3DSURFACE9		BackDepthSurface;
	pDevice->GetRenderTarget(0, &BackBufferSurface);
	pDevice->GetDepthStencilSurface(&BackDepthSurface);

	//�����_�[�^�[�Q�b�g��[�x�}�b�v�ɐ؂�ւ���
	pDevice->SetRenderTarget(0, pDepthRenderTarget->GetSurfacePointer());
	pDevice->SetDepthStencilSurface(pDepthRenderTarget->GetDepthSurfacePointer());
	pDevice->Clear(0, nullptr, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL), D3DCOLOR_RGBA(255, 255, 255, 255), 1.0f, 0);

	//�`��ĊJ
	CRenderer::DrawBegin();

	//�`��
	D3DXMATRIX LightView = m_CameraLight.GetViewMtx();
	D3DXMATRIX LightProje = m_CameraLight.GetProjeMtx();
	DrawInDepthRenderTarget(LightView, LightProje);

	//�����_�[�^�[�Q�b�g�̕`��I��
	CRenderer::DrawEnd();

	//�`��ĊJ
	CRenderer::DrawBegin();

	//�V�[���̕`�挋�ʂ��e�N�X�`���ɏ�������
	pDevice->SetRenderTarget(0, m_SceneRenderTarget.GetSurfacePointer());
	pDevice->SetDepthStencilSurface(m_SceneRenderTarget.GetDepthSurfacePointer());
	pDevice->Clear(0, nullptr, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL), D3DCOLOR_RGBA(255, 255, 255, 255), 1.0f, 0);
	DrawSceneUseDepthMap();
	CBloodStorage::DrawAll();
	CLifeGaugeStorage::DrawAll();

	//�`��I��
	CRenderer::DrawEnd();

	//�o�b�N�o�b�t�@�ɖ߂�
	pDevice->SetRenderTarget(0, BackBufferSurface);
	pDevice->SetDepthStencilSurface(BackDepthSurface);
	pDevice->Clear(0, nullptr, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL), D3DCOLOR_RGBA(255, 255, 255, 255), 1.0f, 0);

	//�`��ĊJ
	CRenderer::DrawBegin();

	//�`��
	m_SceneRenderTarget.DrawTest(D3DXVECTOR2(0,0),D3DXVECTOR2(SCREEN_WIDTH,SCREEN_HEIGHT));
	CScene2DStorage::DrawAll();
	m_GameUI->DrawUIObj();				//UI�`��
	m_LockOnEF.Draw();					//���b�N�I���\���`��

	//�`��I��
	CRenderer::DrawEnd();

	//�������̂�2.5D���[�V�����u���[��������
	pDevice->Clear(0, nullptr, (D3DCLEAR_ZBUFFER), D3DCOLOR_RGBA(255, 255, 255, 255), 1.0f, 0);
	
	//�`��ĊJ
	CRenderer::DrawBegin();

	if (m_Player != nullptr) {
		if (m_Player->GetStateNow() == PLAYER_STEP_PATTERN) {
			m_Player->DrawMotionBlur(m_SceneRenderTarget.GetTexturePointer());
		}
	}

	//�`��I��
	CRenderer::DrawEnd();
}
void CGame::SceneTraDecision(void)
{
	if (CGame::TRA_TO_NONE == m_TraFlag) {
		return;
	}

	switch (m_TraFlag) {
	case CGame::TRA_TO_TITLE:
		CSceneTransition::SetScene(CSceneTransition::FADE_OUT, new CTitle());
		break;
	case CGame::TRA_TO_RESULT:
		CSceneTransition::SetScene(CSceneTransition::FADE_OUT, new CResult());
		break;
	}

	m_TraFlag = CGame::TRA_TO_NONE;
}

void CGame::DrawInRenderTarget(void)
{
	CCameraManager::ActiveCamera_ViewProMtxSet();	//�`��p�J�����ݒ�
	CLightManager::UseLight(0);						//0�ԃ��C�g�g�p

	//�X�J�C�h�[���`��
	m_SkyDoom.Draw();
	m_SkyDoom2.Draw();

	//���b�V���t�B�[���h�`��
	CMeshFieldManager::DrawAll();

	m_Player->Draw();							//�v���[���[�`��
	
	CEnemyStorage::DrawAll();					//�G�̕`��

	CModelStorage::DrawAll();

	CScene3DStorage::DrawAll();

	CScene2DStorage::DrawAll();
	m_GameUI->DrawUIObj();				//UI�`��

	m_LockOnEF.Draw();					//���b�N�I���\���`��
}

void CGame::DrawInDepthRenderTarget(D3DXMATRIX ViewMtx, D3DXMATRIX ProjeMtx)
{
	CRenderer::GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);

	//���b�V���t�B�[���h�`��
	CMeshFieldManager::DrawAllOnDepthMap(ViewMtx, ProjeMtx);

	//�v���[���[�`��
	m_Player->DrawOnDepthMap(ViewMtx, ProjeMtx);

	//�G�̕`��
	CEnemyStorage::DrawAllOnDepthMap(ViewMtx, ProjeMtx);

	//���f���̕`��
	CModelStorage::DrawAllOnDepthMap(ViewMtx, ProjeMtx);

	CRenderer::GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
}

void CGame::DrawSceneUseDepthMap(void)
{
	D3DXMATRIX LightView = m_CameraLight.GetViewMtx();
	D3DXMATRIX LightProje = m_CameraLight.GetProjeMtx();

	CDepthRenderTarget* pDepthRenderTarget = CDepthRenderTarget::GetInstance();
	if (pDepthRenderTarget == nullptr) {
		_RPT0(0, "[CGame.cpp][Draw]pDepthRenderTarget�̓k��\n");
		return;
	}

	CCameraManager::ActiveCamera_ViewProMtxSet();	//�`��p�J�����ݒ�
	CLightManager::UseLight(0);						//0�ԃ��C�g�g�p

	//�X�J�C�h�[���`��
	m_SkyDoom.Draw();
	m_SkyDoom2.Draw();

	//���b�V���t�B�[���h�`��
	CMeshFieldManager::DrawUseDepthMap(LightView, LightProje, pDepthRenderTarget->GetTexturePointer());

	m_Player->DrawSpecular( );

	CEnemyStorage::DrawSpecular();

	CModelStorage::DrawSpecular();
}