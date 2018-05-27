#include "CMapEditor.h"
#include "main.h"
#include <imgui.h>
#include "SceneTransition.h"
#include "CTitle.h"
#include "CCameraManager.h"
#include "CLightManager.h"
#include "CModelStorage.h"
#include "CRenderer.h"

//カメラ移動速度
#define EDITCAMERA_MOVE_SPEED (10.0f)

CMapEditor::CMapEditor()
{
	m_IsExit = false;
	m_IsShowAABB = false;
	m_Mode = OBJECT_AND_MESH_EDITOR;
}

CMapEditor::~CMapEditor()
{
	
}

bool CMapEditor::Init(HWND hWnd)
{
	m_ObjAndMeshEditor.Init(hWnd);
	m_ObjAndMeshEditor.Update();
	m_LevelDesignEditor.Init(hWnd);
	m_LevelDesignEditor.Update();

	//自分のメンバ変数
	m_IsExit = false;
	m_IsShowAABB = false;
	m_Mode = OBJECT_AND_MESH_EDITOR;

	m_SkyDoom.Init(400.0f, 30, 30, SKYDOME_TEX);
	m_SkyDoom2.Init(450.0f, 30, 30, SKYDOME2_TEX);
	m_SkyDoom2.SetRot(D3DXVECTOR3(D3DX_PI * 0.5f, 0.0f, 0.0f));

	//カメラ初期化
	CCameraManager::InitAll();											//全てのカメラマネージャー初期化
	CCameraManager::ChangeActiveCamera(CCameraManager::CAMERA_EDIT01);	//アクティブカメラをCAMERA_EDIT01に設定
	CAMERA_PARA CameraPara = {D3DXVECTOR3(20.0f,20.0f,20.0f),D3DXVECTOR3(0.0f,0.0f,0.0f) ,D3DXVECTOR3(0.0f,1.0f,0.0f) };
	CCameraManager::SetCameraPara(CCameraManager::CAMERA_EDIT01, CameraPara);

	return true;
}
void CMapEditor::Uninit(void)
{
	m_ObjAndMeshEditor.Uninit();
	m_LevelDesignEditor.Uninit();

	m_SkyDoom.Uninit();
	m_SkyDoom2.Uninit();
}

void CMapEditor::Update(void)
{
	//カメラ操作
	EditCameraControl();

	//モード選択
	ModeSelectUI();
	ImGui::Begin("MainMenu");
	ImGui::SameLine();
	ImGui::End();
	ShowAABBUI();

	//モードによる実行
	switch(m_Mode){
	case OBJECT_AND_MESH_EDITOR:
		m_ObjAndMeshEditor.Update();
		break;
	case LEVEL_EDITOR:
		m_LevelDesignEditor.Update();
		break;
	}

	m_SkyDoom.Update();
	m_SkyDoom2.Update();

	//シーン遷移判定
	m_IsExit = ExitButtonUI();
	if (m_IsExit == true) {
		UINT nID1 = MessageBox(NULL, "タイトル画面に戻りますか?", "エディター終了", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);
		if (nID1 == IDYES) {
			CSceneTransition::SetScene(CSceneTransition::FADE_OUT, new CTitle());
		}
	}
}

void CMapEditor::Draw(void)
{
	CCameraManager::ActiveCamera_ViewProMtxSet();	//描画用カメラ設定
	CLightManager::UseLight(0);						//0番ライト使用

	//スカイドーム描画
	m_SkyDoom.Draw();
	m_SkyDoom2.Draw();

	m_ObjAndMeshEditor.Draw();
	m_LevelDesignEditor.Draw();

	//AABBボックスの描画
	if (m_IsShowAABB == true) {
		CModelStorage::DrawAllAABBTest();
	}

	m_LevelDesignEditor.DrawNavMesh();
}

void CMapEditor::EditCameraControl(void)
{
	//IMGUI操作中
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse) {
		return;
	}

	if (CCameraManager::GetActiveCameraName() == CCameraManager::CAMERA_PLAYER) {
		return;      //作動しているカメラの名前取得
	}
	if (CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_LSHIFT)) {
		return;
	}

	//カメラ操作
	DIMOUSESTATE *pMouseState = CSceneTransition::GetInputMouse()->GetMouseStates();

	if (CSceneTransition::GetInputMouse()->GetMousePress(MOUSE_RIGHT))
	{
		float Horizon = -pMouseState->lX*0.01f;
		float Vertical = pMouseState->lY*0.01f;
		CCameraManager::TpsRotation(Horizon, Vertical, D3DX_PI*0.5f);
	}

	if (CSceneTransition::GetInputMouse()->GetMousePress(MOUSE_CENTER))
	{
		//カメラと注視点の距離取得
		float Distance = CCameraManager::GetDistance(CCameraManager::GetActiveCameraName());
		Distance = Distance * 0.05f;

		//マウスX方向の移動
		float CameraHoriAngle = CCameraManager::GetHoriAngle(CCameraManager::GetActiveCameraName());
		CameraHoriAngle = CameraHoriAngle + D3DX_PI*0.5f;
		D3DXVECTOR3 CameraMoveDir;                   //カメラの移動方向
		CameraMoveDir.x = cos(CameraHoriAngle);
		CameraMoveDir.z = sin(CameraHoriAngle);
		CameraMoveDir.y = 0.0f;
		float Horizon = -pMouseState->lX*0.01f;
		CCameraManager::Move3D(CameraMoveDir, Horizon*EDITCAMERA_MOVE_SPEED*Distance);    //移動

																						  //マウスY方向の移動
		CameraHoriAngle = CCameraManager::GetHoriAngle(CCameraManager::GetActiveCameraName());
		float CameraVertiAngle = CCameraManager::GetVertiAngle(CCameraManager::GetActiveCameraName());
		CameraVertiAngle = CameraVertiAngle + D3DX_PI*0.5f;
		CameraMoveDir.x = cos(CameraHoriAngle) * cos(CameraVertiAngle);
		CameraMoveDir.z = sin(CameraHoriAngle) * cos(CameraVertiAngle);
		CameraMoveDir.y = sin(CameraVertiAngle);
		float Vertical = pMouseState->lY*0.01f;
		CCameraManager::Move3D(CameraMoveDir, Vertical*EDITCAMERA_MOVE_SPEED*Distance);
	}

	float ZoomValue = pMouseState->lZ*0.01f;
	CCameraManager::Zoom(ZoomValue);
}

bool CMapEditor::ExitButtonUI(void)
{
	bool IsPushButton = false;

	ImGui::Begin("Exit");

	if (ImGui::Button("EXIT", ImVec2(100, 50))) {
		IsPushButton = true;
	}

	ImGui::End();

	return IsPushButton;
}

void CMapEditor::ModeSelectUI(void)
{
	int EditModeNow = (int)m_Mode;

	ImGui::Begin("MainMenu");

	ImGui::RadioButton("OBJ_MESH_EDITOR", &EditModeNow, (int)OBJECT_AND_MESH_EDITOR); ImGui::SameLine();
	ImGui::RadioButton("LEVEL_EDITOR", &EditModeNow, (int)LEVEL_EDITOR);

	ImGui::End();

	//モード変化があったら
	if (EditModeNow != (int)m_Mode)
	{
		m_Mode = (MODE)EditModeNow;
		return;
	}
}

void CMapEditor::ShowAABBUI(void)
{
	ImGui::Begin("MainMenu");

	ImGui::Checkbox("ShowAABB", &m_IsShowAABB);

	ImGui::End();
}