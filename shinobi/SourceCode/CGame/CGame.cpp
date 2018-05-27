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

//マグロ
#define FLOOR_BLOCK_HEIGHT (30.0f)

#define LOCK_ON_ICON_SIZE_X (4.0f)
#define LOCK_ON_ICON_SIZE_Y (4.0f)

#define DEFAULT_FIELD_COFFI (0.2f)		//デフォルト地面摩擦係数

#define CLEAR_TO_RESULT_TIME (180)

//ライフゲージのサイズ
#define LIFE_GAUGE_SIZEX (4.0f)
#define LIFE_GAUGE_SIZEY (0.5f)

#define DOG_LIFE_GAUGE_POSY_FROM_BODY (3.0f)		//ライフゲージの位置は、タイプは犬の場合、ボディの位置から離れる距離

CGame::CGame()
{
	m_GameUI = new CGameUI();
	m_PlayerControl = new CPlayerControl();
	m_Player = new CPlayer();

	//ロックオンエフェクト初期化
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
	hWnd;		//dummy参照

	InitStatic();		//static型の部分の初期化

	//コントロールユニット初期化
	m_PlayerControl->Init();				//プレーヤーコントローラの初期化
	CEnemyStorage::InitAll();

	//カメラ初期化
	CCameraManager::SetCameraPara(
		CCameraManager::CAMERA_PLAYER, 
		{ D3DXVECTOR3(45.0f,5.0f,75.0f) ,D3DXVECTOR3(20.0f,0.0f,25.0f) ,D3DXVECTOR3(0.0f, 1.0f, 0.0f) }
	);

	InitLight();		//ライト部分の初期化

	//メッシュフィールド
	m_MeshLoader.ImportMeshData(STAGE01_MESH_DATA);
	m_ModelLoader.ImportObj3DData(STAGE01_3DOBJ_DATA);

	//プレーヤーの初期化
	m_Player->Init(D3DXVECTOR3(35.0f,0.0,70.0f), D3DXVECTOR3(0.0f, D3DX_PI * 0.5f, 0.0f), MOTION_DATA_PLAYER);
	m_Player->SetEnvironment(1.0f,-0.03f, DEFAULT_FIELD_COFFI);

	//武器作る
	CWeapon *pWeapon01 = new CWeapon();
	pWeapon01->Init(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), MOTION_DATA_L_SWORD);
	pWeapon01->SetEnvironment(1.0f, 0.0f, 0.0f);
	CWeapon *pWeapon02 = new CWeapon();
	pWeapon02->Init(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), MOTION_DATA_R_SWORD);
	pWeapon02->SetEnvironment(1.0f, 0.0f, 0.0f);
	m_Player->SetWeaponLeft(pWeapon01,D3DXVECTOR3(-0.25f,0.0f,0.1f),5);
	m_Player->SetWeaponRight(pWeapon02, D3DXVECTOR3(0.25f, 0.0f, 0.1f), 8);

	//敵コンテナにプレーヤーを設置
	CEnemyStorage::SetPlayer(m_Player);
	CCollisionPlayerAndModel::Init(m_Player->GetBodyPos());

	InitEnemy();

	m_PlayerControl->SetPlayer(m_Player);			//プレーヤーコントローラのプレーヤーセット
	CCollisionJudge::SetPlayer(m_Player);			//当り判定のプレーヤーセット

	m_GameUI->InitUIObj();					//UIオブジェクト初期化

	m_SkyDoom.Init(300.0f,30,30, SKYDOME_TEX);
	m_SkyDoom2.Init(350.0f, 30, 30, SKYDOME2_TEX);
	m_SkyDoom2.SetRot(D3DXVECTOR3(D3DX_PI * 0.5f,0.0f,0.0f));


	//レンダーターゲットテスト
	SRP RenderTargetTest;
	RenderTargetTest.pos = D3DXVECTOR3(-5.0f,10.0f,-5.0f);
	RenderTargetTest.rot = D3DXVECTOR3(0.0f, -D3DX_PI*0.5, 0.0f);
	RenderTargetTest.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	//レンダーターゲット初期化
	m_SceneRenderTarget.Init(D3DXVECTOR3(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f), D3DFMT_A8B8G8R8);
	
	//クリア後、リザルトに行く時間初期化
	m_GoToResultTime = CLEAR_TO_RESULT_TIME;


	CAllObj3DHitMesh::ColliAllObj3DAndAllMesh();		//3Dオブジェクトと地面の当り判定
	Update();					//全て一回更新

	return true;
}

/*==============================================================================
	static型のオブジェクトの初期化
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
	ライトの初期化
===============================================================================*/
void CGame::InitLight(void)
{
	//ライト
	D3DLIGHT9 Light;
	ZeroMemory(&Light, sizeof(D3DLIGHT9));                      //ライト初期化
	Light.Type = D3DLIGHT_DIRECTIONAL;                          //ライトのタイプを指定
	Light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);          //ディレクショナルライトの色
	Light.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);          //アンビエントライト
	D3DXVECTOR3 vecDir(-1.0f, -1.0f, -1.0f);                         //ライト方向
	D3DXVec3Normalize((D3DXVECTOR3*)&Light.Direction, &vecDir); //ライト方向を設定したライトに入れる
	CLightManager::CreateLight(Light);

	m_CameraLight.Init(D3DXVECTOR3(-50,-50,-50), (D3DXVECTOR3)Light.Direction, D3DXVECTOR3(0, 1, 0));
	m_CameraLight.SetCameraPosAndLightDir(D3DXVECTOR3(50, 50, 50), (D3DXVECTOR3)Light.Direction,100);
	m_CameraLight.SetFar(200.0f);
}

void CGame::InitEnemy(void)
{
	CCreatureDataIO EnemyIO;
	EnemyIO.ImportEnemyObjData("data/CREATURE_DATA/EnemyDataOutput.txt");

	//ライフゲージ初期化
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
	//更新
	CScene2DStorage::UpdateAll();
	CScene3DStorage::UpdateAll();
	CModelStorage::UpdateAll();
	CMeshFieldManager::UpdateAll();
	CEnemyStorage::UpdateAll();
	CEnemyStorage::UpdateAllCollisionToModel();
	CLifeGaugeStorage::UpdateAll();

	m_PlayerControl->Update();					//プレーヤーコントローラ更新
	m_Player->Update();							//プレーヤー更新
	CCollisionPlayerAndModel::Update(m_Player);	//プレーヤーとモデルの当り判定

	//ロックオン表示処理
	UpdateLockOnEF();

	//スカイドーム更新
	m_SkyDoom.Update();
	m_SkyDoom2.Update();

	//当り判定更新
	CCollisionJudge::SetPlayer(m_Player);
	CCollisionJudge::Update();
	CCollisionJudge::Clear();

	//攻撃の血しぶきに関する更新
	CColliPlayerEnemy::UpdateBlood();
	CBloodStorage::UpdateAll();

	//UI更新
	m_GameUI->UpdateUIObj();

	//テスト
	//m_FeedBackBlur.DrawBlurOnDrawTarget(this);

	//ライト側のカメラ更新
	D3DXVECTOR3 PlayerPos = m_Player->GetBodyPos();
	D3DXVECTOR3 LightDir = CLightManager::GetLightDirection(0);
	m_CameraLight.SetCameraLookAt(PlayerPos - 50.0f * LightDir, PlayerPos + 50.0f * LightDir);
	m_CameraLight.SetViewMtx();
	m_CameraLight.SetProjectionMtx();
	
	//シーン遷移判定更新
	m_PlayerControl->SceneTraDecision(&m_TraFlag);
	if (CEnemyStorage::EnemyAllDead()) {
		m_GoToResultTime--;
	}
	if (m_GoToResultTime <= 0) {
		m_TraFlag = TRA_TO_RESULT;
	}
	SceneTraDecision();				//フラグによるのシーン遷移判定
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
	//一旦シーンの描画を終了させます
	CRenderer::DrawEnd();

	//レンダーターゲットのポインタとデバイスの取得
	CDepthRenderTarget* pDepthRenderTarget = CDepthRenderTarget::GetInstance();
	if (pDepthRenderTarget == nullptr) {
		_RPT0(0, "[CGame.cpp][Draw]pDepthRenderTargetはヌル\n");
		return;
	}
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	//バックバッファのサーフェース取得
	LPDIRECT3DSURFACE9		BackBufferSurface;
	LPDIRECT3DSURFACE9		BackDepthSurface;
	pDevice->GetRenderTarget(0, &BackBufferSurface);
	pDevice->GetDepthStencilSurface(&BackDepthSurface);

	//レンダーターゲットを深度マップに切り替える
	pDevice->SetRenderTarget(0, pDepthRenderTarget->GetSurfacePointer());
	pDevice->SetDepthStencilSurface(pDepthRenderTarget->GetDepthSurfacePointer());
	pDevice->Clear(0, nullptr, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL), D3DCOLOR_RGBA(255, 255, 255, 255), 1.0f, 0);

	//描画再開
	CRenderer::DrawBegin();

	//描画
	D3DXMATRIX LightView = m_CameraLight.GetViewMtx();
	D3DXMATRIX LightProje = m_CameraLight.GetProjeMtx();
	DrawInDepthRenderTarget(LightView, LightProje);

	//レンダーターゲットの描画終了
	CRenderer::DrawEnd();

	//描画再開
	CRenderer::DrawBegin();

	//シーンの描画結果をテクスチャに書き込む
	pDevice->SetRenderTarget(0, m_SceneRenderTarget.GetSurfacePointer());
	pDevice->SetDepthStencilSurface(m_SceneRenderTarget.GetDepthSurfacePointer());
	pDevice->Clear(0, nullptr, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL), D3DCOLOR_RGBA(255, 255, 255, 255), 1.0f, 0);
	DrawSceneUseDepthMap();
	CBloodStorage::DrawAll();
	CLifeGaugeStorage::DrawAll();

	//描画終了
	CRenderer::DrawEnd();

	//バックバッファに戻る
	pDevice->SetRenderTarget(0, BackBufferSurface);
	pDevice->SetDepthStencilSurface(BackDepthSurface);
	pDevice->Clear(0, nullptr, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL), D3DCOLOR_RGBA(255, 255, 255, 255), 1.0f, 0);

	//描画再開
	CRenderer::DrawBegin();

	//描画
	m_SceneRenderTarget.DrawTest(D3DXVECTOR2(0,0),D3DXVECTOR2(SCREEN_WIDTH,SCREEN_HEIGHT));
	CScene2DStorage::DrawAll();
	m_GameUI->DrawUIObj();				//UI描画
	m_LockOnEF.Draw();					//ロックオン表示描画

	//描画終了
	CRenderer::DrawEnd();

	//動く物体に2.5Dモーションブラーをかける
	pDevice->Clear(0, nullptr, (D3DCLEAR_ZBUFFER), D3DCOLOR_RGBA(255, 255, 255, 255), 1.0f, 0);
	
	//描画再開
	CRenderer::DrawBegin();

	if (m_Player != nullptr) {
		if (m_Player->GetStateNow() == PLAYER_STEP_PATTERN) {
			m_Player->DrawMotionBlur(m_SceneRenderTarget.GetTexturePointer());
		}
	}

	//描画終了
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
	CCameraManager::ActiveCamera_ViewProMtxSet();	//描画用カメラ設定
	CLightManager::UseLight(0);						//0番ライト使用

	//スカイドーム描画
	m_SkyDoom.Draw();
	m_SkyDoom2.Draw();

	//メッシュフィールド描画
	CMeshFieldManager::DrawAll();

	m_Player->Draw();							//プレーヤー描画
	
	CEnemyStorage::DrawAll();					//敵の描画

	CModelStorage::DrawAll();

	CScene3DStorage::DrawAll();

	CScene2DStorage::DrawAll();
	m_GameUI->DrawUIObj();				//UI描画

	m_LockOnEF.Draw();					//ロックオン表示描画
}

void CGame::DrawInDepthRenderTarget(D3DXMATRIX ViewMtx, D3DXMATRIX ProjeMtx)
{
	CRenderer::GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE,FALSE);

	//メッシュフィールド描画
	CMeshFieldManager::DrawAllOnDepthMap(ViewMtx, ProjeMtx);

	//プレーヤー描画
	m_Player->DrawOnDepthMap(ViewMtx, ProjeMtx);

	//敵の描画
	CEnemyStorage::DrawAllOnDepthMap(ViewMtx, ProjeMtx);

	//モデルの描画
	CModelStorage::DrawAllOnDepthMap(ViewMtx, ProjeMtx);

	CRenderer::GetDevice()->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
}

void CGame::DrawSceneUseDepthMap(void)
{
	D3DXMATRIX LightView = m_CameraLight.GetViewMtx();
	D3DXMATRIX LightProje = m_CameraLight.GetProjeMtx();

	CDepthRenderTarget* pDepthRenderTarget = CDepthRenderTarget::GetInstance();
	if (pDepthRenderTarget == nullptr) {
		_RPT0(0, "[CGame.cpp][Draw]pDepthRenderTargetはヌル\n");
		return;
	}

	CCameraManager::ActiveCamera_ViewProMtxSet();	//描画用カメラ設定
	CLightManager::UseLight(0);						//0番ライト使用

	//スカイドーム描画
	m_SkyDoom.Draw();
	m_SkyDoom2.Draw();

	//メッシュフィールド描画
	CMeshFieldManager::DrawUseDepthMap(LightView, LightProje, pDepthRenderTarget->GetTexturePointer());

	m_Player->DrawSpecular( );

	CEnemyStorage::DrawSpecular();

	CModelStorage::DrawSpecular();
}