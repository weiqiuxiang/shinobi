#include "CMotionEditor.h"
#include "CModelStorage.h"
#include "CScene3DStorage.h"
#include "CScene2DStorage.h"
#include "CMeshFieldManager.h"
#include "CCameraManager.h"
#include "CMouseEditMeshField.h"
#include "CMeshField.h"
#include "CMeshDataIO.h"
#include "C_Obj3D_DataIO.h"
#include "CAllObj3DHitMesh.h"
#include "SceneTransition.h"
#include "CLightManager.h"
#include "SceneTransition.h"
#include "CScene3DFactory.h"
#include "CModelFactory.h"
#include "CDebug.h"
#include "raypicktomesh.h"
#include "CDebug.h"
#include <string.h>
#include <imgui.h>
#include "imgui_impl_dx9.h"
#include "CMotion.h"
#include "CTitle.h"
#include <time.h>

#define EDITCAMERA_MOVE_SPEED (10.0f)		//カメラ移動速度

//座標移動カーソルのマクロ
#define SCL_IS_ONE_BASE_DIST (30.0f)			//座標移動カーソルののスケールは1.0の距離
#define NOTHING_BE_SELECTED (-1)				//何も選択されてない時の表示
#define AXIS_MOVE_COEFFICIENT (0.02f)			//軸移動係数(使い方 : 軸移動係数 * マウスの相対移動量)

//エディターに関するマクロ
#define OFFSET_MOVE_VALUE (0.01f)				//オフセットの移動値

#define STUCK_MAX (20)							//スタックリストの削除

CMotionEditor::CMotionEditor()
{
	m_EditMode = EDIT_OFFSET;
	m_MotionEdit = MOTION_EDIT_MODE_EDIT_KEYFRAME;
	m_bEditModeChange = false;
	m_bMotionEditChange = false;
	m_bClickFlag = false;
	m_bMotionPlay = false;
	m_bKeyFramePreviewPlay = false;
	m_pSelectedPart = nullptr;
	m_pShowParent = nullptr;
	m_IsShowMotionList = false;
	m_IsShowPartList = false;

	m_SelectPartMtrl.Diffuse = { 0.0f,1.0f,0.0f,0.3f };
	m_SelectPartMtrl.Ambient = { 0.5f,0.5f,0.5f,0.0f };
	m_SelectPartMtrl.Specular = { 0.0f,0.0f,0.0f,0.0f };
	m_SelectPartMtrl.Emissive = { 0.0f,0.0f,0.0f,0.0f };
	m_SelectPartMtrl.Power = 6.0f;

	m_ShowParentMtrl.Diffuse = { 0.5f,0.5f,0.5f,0.3f };
	m_ShowParentMtrl.Ambient = { 0.5f,0.5f,0.5f,0.0f };
	m_ShowParentMtrl.Specular = { 0.0f,0.0f,0.0f,0.0f };
	m_ShowParentMtrl.Emissive = { 0.0f,0.0f,0.0f,0.0f };
	m_ShowParentMtrl.Power = 6.0f;

	m_KeyFramePercent = 0;

	m_pHermitian = nullptr;
}

CMotionEditor::~CMotionEditor()
{
	//スタックのデータを解放
	m_ModelMotion.Uninit();					//モデル及びモーションデータの解放
	m_PushReadyData.ModelMotion.Uninit();
	m_DefaultData.ModelMotion.Uninit();
	for (auto it = m_UndoRedoStuck.begin(); it != m_UndoRedoStuck.end(); ++it) {
		(*it)->ModelMotion.Uninit();
		delete (*it);
	}
	m_UndoRedoStuck.clear();
	MyNameSpace::DeleteInitialize(m_pHermitian);
}

bool CMotionEditor::Init(HWND hWnd)
{
	hWnd;				//ダミー参照

	//オブジェクト初期化
	CModelStorage::InitAll();

	//カメラ初期化
	CCameraManager::InitAll();											//全てのカメラマネージャー初期化
	CCameraManager::ChangeActiveCamera(CCameraManager::CAMERA_EDIT01);	//アクティブカメラをCAMERA_EDIT01に設定
	CAMERA_PARA CameraPara = {D3DXVECTOR3(0.0f,10.0f,10.0f),D3DXVECTOR3(0.0f,0.0f,0.0f) ,D3DXVECTOR3(0.0f,1.0f,0.0f) };
	CCameraManager::SetCameraPara(CCameraManager::CAMERA_EDIT01, CameraPara);

	//メッシュデータ初期化
	CMeshFieldManager::InitAll();

	//メッシュにあるマウス位置の初期化
	m_MouseEditMesh.Init(5.0f, {100,0,0,200});

	//データ読み込み
	m_MeshLoader.SetMesh(m_MeshLoader.ImportMeshData("data/MESH_DATA/MotionEditor_MeshData.txt") );
	
	InitAndCreateLight();									//ライト初期化

	//モーション部分初期化
	m_ModelMotion.LoadMotionDataToInit(MOTION_DATA_PLAYER_MOTION_EDITOR);				//モデルとモーションデータの読み込み
	for (int i = 0; i < m_ModelMotion.GetPartTotalNum(); i++) {
		m_CopyKeyFrame.m_SRPData[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_CopyKeyFrame.m_SRPData[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_CopyKeyFrame.m_SRPData[i].scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	}

	//プッシュデータ初期化
	m_PushReadyData.ModelMotion = m_ModelMotion;
	m_PushReadyData.EditMode = m_EditMode;
	m_PushReadyData.MotionEdit = m_MotionEdit;

	m_DefaultData = m_PushReadyData;	//初期データを保存

	//スタッククリア
	for (auto it = m_UndoRedoStuck.begin(); it != m_UndoRedoStuck.end(); ++it) {
		(*it)->ModelMotion.Uninit();
		delete (*it);
	}
	m_UndoRedoStuck.clear();

	//デフォルトデータをスタックの一番目に保存
	UNDO_REDO_STUCK* pStuckBegin = new UNDO_REDO_STUCK;
	*pStuckBegin = m_DefaultData;
	m_UndoRedoStuck.push_back(pStuckBegin);
	m_pPushPosition = pStuckBegin;					//位置ポインタを保存

	m_bMouseHoverAndClickInWindow = false;

	//曲線初期化
	MyNameSpace::DeleteInitialize(m_pHermitian);
	m_pHermitian = new CHermitian;
	m_pHermitian->Init(D3DXVECTOR2(900.0f,600.0f), D3DXVECTOR2(50.0f, -50.0f), D3DXVECTOR2(-50.0f, 50.0f));

	//スカイドーム初期化
	m_SkyDoom.Init(400.0f, 30, 30, SKYDOME_TEX);
	m_SkyDoom2.Init(450.0f, 30, 30, SKYDOME2_TEX);
	m_SkyDoom2.SetRot(D3DXVECTOR3(D3DX_PI * 0.5f, 0.0f, 0.0f));

	return true;
}

void CMotionEditor::Uninit(void)
{
	//オブジェクト解放
	CModelStorage::UninitAll();

	//メッシュデータ解放
	CMeshFieldManager::UninitAll();

	//メッシュにあるマウス位置の解放
	m_MouseEditMesh.Uninit();

	//スタックのデータを解放
	m_ModelMotion.Uninit();					//モデル及びモーションデータの解放
	m_PushReadyData.ModelMotion.Uninit();
	m_DefaultData.ModelMotion.Uninit();
	for (auto it = m_UndoRedoStuck.begin(); it != m_UndoRedoStuck.end(); ++it) {
		(*it)->ModelMotion.Uninit();
		delete (*it);
	}
	m_UndoRedoStuck.clear();

	MyNameSpace::DeleteInitialize(m_pHermitian);	//曲線解放

	//スカイドーム解放
	m_SkyDoom.Uninit();
	m_SkyDoom2.Uninit();
}

void CMotionEditor::Update(void)
{
	m_MouseEditMesh.CalcMousePositionInMesh();	//メッシュにあるマウス位置の計算
	CModelStorage::UpdateAll();					//
	CMeshFieldManager::UpdateAll();				//メッシュアップデート

	//エディターの更新
	UpdateEdit();

	//データの書き出し処理
	ImGui::Begin("SAVE_BUTTON");
	if (ImGui::Button("SAVE_BUTTON", ImVec2(100, 50) ))
	{
		SaveData();
	}
	ImGui::End();

	//スカイドーム更新
	m_SkyDoom.Update();
	m_SkyDoom2.Update();

	//シーン遷移判定
	if (ExitButtonUI() == true) {
		UINT nID1 = MessageBox(NULL, "タイトル画面に戻りますか?", "エディター終了", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);
		if (nID1 == IDYES) {
			CSceneTransition::SetScene(CSceneTransition::FADE_OUT, new CTitle());
		}
	}
}

void CMotionEditor::UpdateEdit(void)
{
	m_bClickFlag = false;					//クリックフラグを未設定状態にする
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse) {				//ImGuiのマウスクリックしたかを確認
		m_bClickFlag = true;
	}

	if (m_bMouseHoverAndClickInWindow == false) {
		DetectMouseClickInWindow();
	}
	else {
		DetectMouseRelease();
	}

	//GUI部分
	UndoRedoUI();							//UndoRedoUI
	ChangeEditModeUI();						//編集モード変更UI
	ShowPartDetailUI();						//選択されたパーツの詳細表示
	ImguiForEditor();						//GUI
	
	HermitianEdit();						//曲線編集
	ChangeEditMode();						//編集モード変更
	EditCameraControl();					//編集カメラのコントロール
	ActionInEditMode();						//編集モードの具体動作
}

//他のボタン
void CMotionEditor::OtherOptionButton(void)
{
	//頂点を線形補間に戻すボタン
	ImGui::Begin("MainMenu");
	
	ImGui::NewLine();

	//頂点を線形補間に戻すボタン
	if(ImGui::Button("CurveReset",ImVec2(100,50))){
		m_pHermitian->CurveReset();
	}

	//モーションのリスト表示非表示ボタン
	ImGui::SameLine();
	if (ImGui::Button("MotionList", ImVec2(100, 50))) {
		if (m_IsShowMotionList) {
			m_IsShowMotionList = false;
		}
		else {
			m_IsShowMotionList = true;
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("PartList", ImVec2(100, 50))) {
		if (m_IsShowPartList) {
			m_IsShowPartList = false;
		}
		else {
			m_IsShowPartList = true;
		}
	}

	ImGui::End();
}

void CMotionEditor::Draw(void)
{
	CCameraManager::ActiveCamera_ViewProMtxSet();	//描画用カメラ設定
	CLightManager::UseLight(0);						//0番ライト使用

	//3Dオブジェクト描画
	CModelStorage::DrawAll();
	CMeshFieldManager::DrawAll();		//メッシュ描画
	m_ModelMotion.Draw();				//モデルの描画
	if (nullptr != m_pHermitian) m_pHermitian->Draw();	//曲線描画

	m_SkyDoom.Draw();
	m_SkyDoom2.Draw();
}

void CMotionEditor::EditCameraControl(void)
{
	if (CCameraManager::GetActiveCameraName() == CCameraManager::CAMERA_PLAYER) {
		return;
	}
	if (CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_LSHIFT)) {
		return;
	}
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse) {
		return;
	}

	//カメラ操作
	DIMOUSESTATE *pMouseState = CSceneTransition::GetInputMouse()->GetMouseStates();

	if (CSceneTransition::GetInputMouse()->GetMousePress(MOUSE_RIGHT))
	{
		float Horizon = -pMouseState->lX*0.01f;
		float Vertical = pMouseState->lY*0.01f;
		CCameraManager::TpsRotation(Horizon, Vertical, D3DX_PI*0.5);
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

void CMotionEditor::InitAndCreateLight(void)
{
	D3DLIGHT9 Light;
	ZeroMemory(&Light, sizeof(D3DLIGHT9));							//ライト初期化
	Light.Type = D3DLIGHT_DIRECTIONAL;								//ライトのタイプを指定
	Light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);				//ディレクショナルライトの色
	Light.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);				//アンビエントライト
	D3DXVECTOR3 vecDir(-1.0f, -1.0f, -1.0f);						//ライト方向
	D3DXVec3Normalize((D3DXVECTOR3*)&Light.Direction, &vecDir);		//ライト方向を設定したライトに入れる
	CLightManager::CreateLight(Light);
}


/*==============================================================================
	曲線編集
===============================================================================*/
void CMotionEditor::HermitianEdit(void)
{
	//ポインタチェック
	if (m_pHermitian == nullptr) {
		return;
	}


	//曲線はモーション編集時しか表示/操作できない
	if (m_EditMode != EDIT_MOTION) {
		m_pHermitian->SetUsing(false);
	}
	else {
		if (m_MotionEdit != MOTION_EDIT_MODE_EDIT_KEYFRAME) {
			m_pHermitian->SetPreview(true);
		}
		else {
			m_pHermitian->SetPreview(false);
		}
		m_pHermitian->SetUsing(true);
	}

	if (m_pHermitian->GetUsing()){
		if (m_pHermitian->GetMouseUsing()) {
			m_bClickFlag = true;
		}
	}

	//曲線更新
	if (m_pHermitian != nullptr) {
		m_pHermitian->Update();
	}

	//変更検出
	if (m_pHermitian != nullptr) {
		bool IsControlHermitian = false;
		IsControlHermitian = m_pHermitian->IsDragedDragPoint();
		if (IsControlHermitian == true) {
			DetectDataChangeAndPushData();
		}
	}
}

void CMotionEditor::ChangeEditMode(void)
{
	if (false == m_bEditModeChange) return;

	EDIT_MODE OldMode = m_EditMode;

	//遷移前の片付
	switch (OldMode) 
	{
	case EDIT_OFFSET:
		break;
	case EDIT_LINK:
		break;
	case EDIT_MOTION:
		break;
	}

	//遷移後の初期化
	switch(m_EditMode){
	case EDIT_OFFSET:
		break;
	case EDIT_LINK:
		break;
	case EDIT_MOTION:
	{
		m_KeyFramePercent = 0;
		D3DXVECTOR2 StartDir, EndDir;
		m_ModelMotion.GetKeyFrameHerminateDirNow(&StartDir, &EndDir);
		if (m_pHermitian != nullptr) {		//曲線設定
			m_pHermitian->SetStartDir(StartDir);
			m_pHermitian->SetEndDir(EndDir);
		}
		break;
	}
	}
	m_bEditModeChange = false;
}

void CMotionEditor::ChangeEditModeUI(void)
{
	int EditModeNow = (int)m_EditMode;

	ImGui::SetNextWindowSize(ImVec2(400, 600), ImGuiSetCond_Once);
	ImGui::Begin("MainMenu");

	ImGui::RadioButton("OFFSET_MODE", &EditModeNow, (int)EDIT_OFFSET); ImGui::SameLine();
	ImGui::RadioButton("LINK_MODE", &EditModeNow, (int)EDIT_LINK); ImGui::SameLine();
	ImGui::RadioButton("MOTION_MODE", &EditModeNow, (int)EDIT_MOTION);

	ImGui::End();

	//モード変化があったら
	if (EditModeNow != (int)m_EditMode)
	{
		m_EditMode = (EDIT_MODE)EditModeNow;
		m_bEditModeChange = true;
	}
}

void CMotionEditor::ShowPartDetailUI(void) 
{
	if (nullptr == m_pSelectedPart) {
		return;
	}

	SRP SRPNow = m_pSelectedPart->GetSRPNow();
	ImGui::SetNextWindowSize(ImVec2(350, 200), ImGuiSetCond_Once);
	ImGui::Begin("SELECTED PARTS DETAIL");
	ImGui::Text("Index : %d", m_pSelectedPart->m_PartID);
	ImGui::Text("Parent : %d", m_pSelectedPart->m_ParentID);
	ImGui::Text("translation : %.2f %.2f %.2f", SRPNow.pos.x, SRPNow.pos.y, SRPNow.pos.z);
	ImGui::Text("rotation    : %.2f %.2f %.2f", SRPNow.rot.x, SRPNow.rot.y, SRPNow.rot.z);
	ImGui::End();
}

void CMotionEditor::ActionInEditMode(void)
{
	//オブジェクトクリエイト
	switch (m_EditMode) {
	case EDIT_OFFSET:			//パーツのオフセット設置
		m_ModelMotion.UpdateOffset();		//オフセットの更新
		SelectPartObj();					//パーツ選択
		break;
	case EDIT_LINK:				//別のパーツとの関連付け
		m_ModelMotion.UpdateOffset();					//オフセットの更新
		if(nullptr == m_pSelectedPart) SelectPartObj();	//パーツ選択
		else EditLinkParent();							//親選択
		break;
	case EDIT_MOTION:			//キーフレームのステータスを変更し
		ChangeEditModeMotion();		//モーション編集モードの変更
		switch (m_MotionEdit) {
		case MOTION_EDIT_MODE_EDIT_KEYFRAME:
			m_ModelMotion.UpdateKeyFrame();		//キーフレームの更新
			SelectPartObj();					//パーツ選択
			if (nullptr != m_pHermitian) {
				m_pHermitian->Update();	//曲線更新
			}
			//キーフレームの曲線情報更新
			m_ModelMotion.SetKeyFrameHerminateDir(m_pHermitian->GetStartDir(), m_pHermitian->GetEndDir());
			break;
		case MOTION_EDIT_MODE_MOTION_PREVIEW:
			if(m_bMotionPlay) m_ModelMotion.UpdateMotion();		//モーションの更新
			else m_ModelMotion.UpdateKeyFrame();
			break;
		case MOTION_EDIT_MODE_KEYFRAME_PREVIEW:
			if (m_bKeyFramePreviewPlay == true) 
			{
				float PercentChangeSpeed = m_ModelMotion.GetKeyFrameChangeSpeed();
				m_KeyFramePercent += PercentChangeSpeed;
			}

			m_KeyFramePercent = min(1.0f, m_KeyFramePercent);

			if (m_pHermitian != nullptr)
			{
				//変化量のパーセンテージ取得
				D3DXVECTOR2 StartDir, EndDir;
				m_ModelMotion.GetKeyFrameHerminateDirNow(&StartDir, &EndDir);
				float Percent = CHermitian::GetChangeValuePercent(StartDir, EndDir, m_KeyFramePercent);
				Percent = max(0.0f, Percent);
				Percent = min(1.0f, Percent);
				m_ModelMotion.UpdateKeyFramePreview(Percent);		//キーフレームプレビューの更新
			}
			break;
		}
		break;
	}
}

void CMotionEditor::ChangeEditModeMotion(void)
{
	if (false == m_bMotionEditChange) {
		return;
	}

	MOTION_EDIT_MODE OldMode = m_MotionEdit;

	//遷移前の片付
	switch (OldMode)
	{
	case MOTION_EDIT_MODE_EDIT_KEYFRAME:
		break;
	case MOTION_EDIT_MODE_MOTION_PREVIEW:
		break;
	case MOTION_EDIT_MODE_KEYFRAME_PREVIEW:
		break;
	}

	//遷移後の初期化
	switch (m_MotionEdit) {
	case MOTION_EDIT_MODE_EDIT_KEYFRAME:
		break;
	case MOTION_EDIT_MODE_MOTION_PREVIEW:
		break;
	case MOTION_EDIT_MODE_KEYFRAME_PREVIEW:
		break;
	}
	m_bMotionEditChange = false;
}

void CMotionEditor::ChangeEditModeMotionUI(void)
{
	int EditMode = (int)m_MotionEdit;

	ImGui::Begin("MainMenu");

	ImGui::RadioButton("EDIT_KEYFRAME", &EditMode, (int)MOTION_EDIT_MODE_EDIT_KEYFRAME); ImGui::SameLine();
	ImGui::RadioButton("KEYFRAME_PREVIEW", &EditMode, (int)MOTION_EDIT_MODE_KEYFRAME_PREVIEW); ImGui::SameLine();
	ImGui::RadioButton("MOTION_PREVIEW", &EditMode, (int)MOTION_EDIT_MODE_MOTION_PREVIEW);

	ImGui::NewLine();		//改行

	ImGui::End();

	if (m_MotionEdit != (MOTION_EDIT_MODE)EditMode)
	{
		m_MotionEdit = (MOTION_EDIT_MODE)EditMode;
		m_bMotionEditChange = true;
	}
}

void CMotionEditor::EditLinkParent(void)
{
	if (nullptr == m_pSelectedPart) return;
	if (m_bClickFlag) return;

	if (m_pShowParent != nullptr) {
		m_pShowParent->m_Model.SetDrawType(CModel::DRAW_USE_MTRL_DEFALUT);
		m_pShowParent = nullptr;
	}

	//親オブジェクトの選択
	D3DXVECTOR3 RayStartPoint = m_MouseEditMesh.GetMouseRayStartPoint();		//レイの始点
	D3DXVECTOR3 RayEndPoint = m_MouseEditMesh.GetMouseRayEndPoint();			//レイの終点
	float DistanceMin = D3DXVec3Length(&(RayEndPoint - RayStartPoint));			//最短距離

	CPart *pParentPart = nullptr;
	for (int ObjNum = 0; ObjNum < m_ModelMotion.GetPartTotalNum(); ObjNum++) {
		CPart *pPart = m_ModelMotion.GetPartPointer(ObjNum);
		if (pPart == nullptr) continue;							//オブジェクトはヌルだったら
		D3DXVECTOR3 HitPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	//当り判定位置
		bool bHit = RayPickToMesh::RaypicktoMesh(				//マウスレイとの当り判定
			false,
			&HitPosition,
			RayEndPoint,
			RayStartPoint,
			pPart->GetMesh(),
			pPart->GetNowMtx()
		);		//レイとメッシュの当り判定を取る

		if (bHit == false) continue;							//当たってないなら以下処理しない
		float Distance = D3DXVec3Length(&(HitPosition - RayStartPoint));	//レイ始点との距離を求め
		if (Distance < DistanceMin) {
			DistanceMin = Distance;					//最短距離更新
			pParentPart = pPart;					//パーツ設定
		}
	}

	//マウスが翳しているパーツの色変化
	if ( (pParentPart != nullptr) && (pParentPart != m_pSelectedPart) ) {	//マウス翳しているパーツの色変化
		m_pShowParent = pParentPart;					//マウスを翳しているパーツを保存
		m_pShowParent->m_Model.SetDrawType(CModel::DRAW_USE_MTRL_CUSTOMIZE);
		m_pShowParent->m_Model.SetMaterial(m_ShowParentMtrl);
	}

	//入力確認
	if (false == CSceneTransition::GetInputMouse()->GetMouseTrigger(MOUSE_LEFT)) return;
	m_bClickFlag = true;

	//例外処理
	if ( (pParentPart == m_pSelectedPart) || (pParentPart == nullptr)) { //選択しているパーツはヌルもしくは選択しているパーツは自分自身だったら
		m_pSelectedPart->m_Model.SetDrawType(CModel::DRAW_USE_MTRL_DEFALUT);
		m_pSelectedPart = nullptr;
		return;
	}
	if (m_pSelectedPart->IsLowerHierarchy(pParentPart)) { //自分の下の階層のパーツだったら
		MessageBox(nullptr, "自分より下の階層を親にすることはできない", "WARNING", MB_ICONHAND);
		return;
	}

	if(pParentPart != nullptr) m_pSelectedPart->SetParent(pParentPart);			//親設定

	//解放
	m_pSelectedPart->m_Model.SetDrawType(CModel::DRAW_USE_MTRL_DEFALUT);
	m_pSelectedPart = nullptr;
	m_pShowParent->m_Model.SetDrawType(CModel::DRAW_USE_MTRL_DEFALUT);
	m_pShowParent = nullptr;
	return;
}

bool CMotionEditor::ExitButtonUI(void)
{
	bool IsPushExitButton = false;

	ImGui::Begin("ExitButton");

	if (ImGui::Button("EXIT", ImVec2(100, 50))) {
		IsPushExitButton = true;
	}

	ImGui::End();

	return IsPushExitButton;
}

void CMotionEditor::EditChangeMotionUI(void)
{
	bool bMotionBefore = false;
	bool bMotionNext = false;

	int nTotalMoTion = m_ModelMotion.GetTotalMotionNum();
	int nMotionNow = m_ModelMotion.GetMotionNow();

	ImGui::Begin("MainMenu");

	//モーションの変更
	ImGui::Text("MOTION : ");
	ImGui::SameLine();
	if (ImGui::Button("<<", ImVec2(20, 25))) {
		bMotionBefore = true;
	}
	ImGui::SameLine();
	ImGui::Text("%d / %d", nMotionNow, nTotalMoTion - 1);
	ImGui::SameLine();
	if (ImGui::Button(">>", ImVec2(20, 25))) {
		bMotionNext = true;
	}

	//モーションの増加と削除
	if (m_MotionEdit == MOTION_EDIT_MODE_EDIT_KEYFRAME)
	{
		ImGui::SameLine();
		if (ImGui::Button("Add", ImVec2(70, 25))) {
			m_ModelMotion.AddMotion();
		}
		ImGui::SameLine();
		if (ImGui::Button("Insert", ImVec2(70, 25))) {
			m_ModelMotion.InsertMotion();
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete", ImVec2(70, 25))) {
			m_ModelMotion.DeleteMotionNow();
		}
	}
	ImGui::End();

	if (bMotionBefore) {
		int BeforeMotion = NOTHING_BE_SELECTED;
		if (nMotionNow == 0) BeforeMotion = nTotalMoTion - 1;
		else BeforeMotion = nMotionNow - 1;
		m_ModelMotion.SetMotionNow(BeforeMotion);
		return;
	}

	if (bMotionNext) {
		int NextMotion = NOTHING_BE_SELECTED;
		if (nMotionNow == nTotalMoTion - 1) NextMotion = 0;
		else NextMotion = nMotionNow + 1;
		m_ModelMotion.SetMotionNow(NextMotion);
	}

	//曲線設定
	D3DXVECTOR2 StartDir, EndDir;
	m_ModelMotion.GetKeyFrameHerminateDirNow(&StartDir, &EndDir);
	if (m_pHermitian != nullptr) {
		m_pHermitian->SetStartDir(StartDir);
		m_pHermitian->SetEndDir(EndDir);
	}
}

//モーションリストUI
void CMotionEditor::MotionListUI(void) 
{
	int MotionNum = m_ModelMotion.GetTotalMotionNum();

	ImGui::Begin("MotionList");

	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(350, 200), ImGuiWindowFlags_NoTitleBar);
	for (int i = 0; i < MotionNum; ++i) {
		CMotion* pMotion = m_ModelMotion.GetMotion(i);
		if (pMotion != nullptr)
		{
			//インプットテキスト
			char buf[64] = {'\0'};
			sprintf(buf,"Motion%d",i);
			ImGui::PushItemWidth(150);
			ImGui::InputText(buf, pMotion->m_Name, sizeof(pMotion->m_Name));
			ImGui::PopItemWidth();

			//ボタン
			char ButtonName[64] = "GoTo";
			strcat_s(ButtonName,64, buf);
			ImGui::SameLine();
			if ( ImGui::Button(ButtonName, ImVec2(100,20)) )
			{
				m_ModelMotion.SetMotionNow(i);
				m_ModelMotion.SetKeyFrameNow(0);
			}
		}
	}
	ImGui::EndChild();

	ImGui::End();
}

//パーツのリストUI
void CMotionEditor::PartListUI(void)
{
	const int PartNum = m_ModelMotion.GetPartTotalNum();		//パーツの数

	ImGui::Begin("MainMenu");

	ImGui::Text("PartList");

	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(350, 200), ImGuiWindowFlags_NoTitleBar);
	for (int i = 0; i < PartNum; ++i) {
		CPart* pPart = m_ModelMotion.GetPartPointer(i);
		if (pPart != nullptr)
		{
			//インプットテキスト
			char buf[64] = { '\0' };
			sprintf(buf, "Part%d", i);
			ImGui::PushItemWidth(150);
			ImGui::InputText(buf, pPart->m_Name, sizeof(pPart->m_Name));
			ImGui::PopItemWidth();

			//ボタン
			char ButtonName[64] = "GoTo";
			strcat_s(ButtonName, 64, buf);
			ImGui::SameLine();
			if (ImGui::Button(ButtonName, ImVec2(100, 20)))
			{
				if (m_pSelectedPart != nullptr) {
					m_pSelectedPart->m_Model.SetDrawType(CModel::DRAW_USE_MTRL_DEFALUT);
				}

				m_pSelectedPart = pPart;
				m_pSelectedPart->m_Model.SetDrawType(CModel::DRAW_USE_MTRL_CUSTOMIZE);
				m_pSelectedPart->m_Model.SetMaterial(m_SelectPartMtrl);
			}
		}
	}
	ImGui::EndChild();

	ImGui::End();
}

void CMotionEditor::PartListUIInEditKeyFrame(void)
{
	const int PartNum = m_ModelMotion.GetPartTotalNum();		//パーツの数

	ImGui::Begin("PartList");

	ImGui::Text("PartList");

	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(350, 200), ImGuiWindowFlags_NoTitleBar);
	for (int i = 0; i < PartNum; ++i) {
		CPart* pPart = m_ModelMotion.GetPartPointer(i);
		if (pPart != nullptr)
		{
			//インプットテキスト
			char buf[64] = { '\0' };
			sprintf(buf, "Part%d", i);
			ImGui::PushItemWidth(150);
			ImGui::InputText(buf, pPart->m_Name, sizeof(pPart->m_Name));
			ImGui::PopItemWidth();

			//ボタン
			char ButtonName[64] = "GoTo";
			strcat_s(ButtonName, 64, buf);
			ImGui::SameLine();
			if (ImGui::Button(ButtonName, ImVec2(100, 20)))
			{
				if (m_pSelectedPart != nullptr) {
					m_pSelectedPart->m_Model.SetDrawType(CModel::DRAW_USE_MTRL_DEFALUT);
				}

				m_pSelectedPart = pPart;
				m_pSelectedPart->m_Model.SetDrawType(CModel::DRAW_USE_MTRL_CUSTOMIZE);
				m_pSelectedPart->m_Model.SetMaterial(m_SelectPartMtrl);
			}
		}
	}
	ImGui::EndChild();

	ImGui::End();
}

//パーツの親選択時のパーツリスト
void CMotionEditor::SelectParentListUI(void)
{
	const int PartNum = m_ModelMotion.GetPartTotalNum();		//パーツの数

	ImGui::Begin("MainMenu");
	ImGui::NewLine();
	ImGui::Text("ParentList");

	ImGui::BeginChild(ImGui::GetID((void*)1), ImVec2(350, 200), ImGuiWindowFlags_NoTitleBar);
	for (int i = 0; i < PartNum; ++i) {
		CPart* pPart = m_ModelMotion.GetPartPointer(i);
		if (pPart != nullptr)
		{
			//インプットテキスト
			ImGui::Text("NAME:"); ImGui::SameLine();
			ImGui::Text(pPart->m_Name);

			//ボタン
			char buf[64] = { '\0' };
			sprintf(buf, "Parent%d", i);
			char ButtonName[64] = "Select";
			strcat_s(ButtonName, 64, buf);
			ImGui::SameLine();
			if (ImGui::Button(ButtonName, ImVec2(120, 20)))
			{
				if (m_pSelectedPart->IsLowerHierarchy(pPart)) { //自分の下の階層のパーツだったら
					MessageBox(nullptr, "自分より下の階層を親にすることはできない", "WARNING", MB_ICONHAND);
				}
				else{
					m_pSelectedPart->SetParent(pPart);
				}
				m_pSelectedPart->m_Model.SetDrawType(CModel::DRAW_USE_MTRL_DEFALUT);
				m_pSelectedPart = nullptr;
			}
		}
	}
	ImGui::EndChild();

	ImGui::End();
}

void CMotionEditor::EditChangeKeyFrameUI(void)
{
	bool bKeyFrameBefore = false;
	bool bKeyFrameNext = false;

	int nTotalKeyFrame = m_ModelMotion.GetTotalKeyFrame(m_ModelMotion.GetMotionNow());
	int nKeyFrameNow = m_ModelMotion.GetKeyFrameNow();

	ImGui::Begin("MainMenu");

	//キーフレームの選択
	ImGui::Text("KEYFRAME : ");
	ImGui::SameLine();
	if (ImGui::Button("<", ImVec2(15, 25))) {
		bKeyFrameBefore = true;
	}
	ImGui::SameLine();
	ImGui::Text("%d / %d", nKeyFrameNow, nTotalKeyFrame - 1);
	ImGui::SameLine();
	if (ImGui::Button(">", ImVec2(15, 25))) {
		bKeyFrameNext = true;
	}

	if (m_MotionEdit == MOTION_EDIT_MODE_EDIT_KEYFRAME)
	{
		//キーフレームの増加と削除
		ImGui::SameLine();
		if (ImGui::Button("AddKey", ImVec2(70, 25))) {
			m_ModelMotion.AddKeyFrame();
		}
		ImGui::SameLine();
		if (ImGui::Button("InsertKey", ImVec2(70, 25))) {
			m_ModelMotion.InsertKeyFrame();
		}
		ImGui::SameLine();
		if (ImGui::Button("DeleteKey", ImVec2(70, 25))) {
			m_ModelMotion.DeleteKeyFrameNow();
		}
	}

	ImGui::End();

	if (bKeyFrameBefore) {
		int BeforeKeyFrame = NOTHING_BE_SELECTED;
		if (nKeyFrameNow == 0) BeforeKeyFrame = nTotalKeyFrame - 1;
		else BeforeKeyFrame = nKeyFrameNow - 1;
		m_ModelMotion.SetKeyFrameNow(BeforeKeyFrame);
		m_KeyFramePercent = 0;								//キーフレームプレビューのpercentを0にする
	}

	if (bKeyFrameNext) {
		int NextKeyFrame = NOTHING_BE_SELECTED;
		if (nKeyFrameNow == nTotalKeyFrame - 1) NextKeyFrame = 0;
		else NextKeyFrame = nKeyFrameNow + 1;
		m_ModelMotion.SetKeyFrameNow(NextKeyFrame);
		m_KeyFramePercent = 0;								//キーフレームプレビューのpercentを0にする
	}

	//曲線設定
	D3DXVECTOR2 StartDir, EndDir;
	m_ModelMotion.GetKeyFrameHerminateDirNow(&StartDir, &EndDir);
	if (m_pHermitian != nullptr) {
		m_pHermitian->SetStartDir(StartDir);
		m_pHermitian->SetEndDir(EndDir);
	}
}

//ループフラグの変更
void CMotionEditor::EditLoopFlagUI(void)
{
	CMotion *pMotion = m_ModelMotion.GetMotion(m_ModelMotion.GetMotionNow());
	if (pMotion == nullptr)
	{
		return;
	}

	bool LoopFlagNow = pMotion->m_bLoop;
	int LoopNow = 0;
	if (LoopFlagNow) {
		LoopNow = 1;
	}

	ImGui::Begin("MainMenu");
	ImGui::RadioButton("LOOPYES", &LoopNow, 1); ImGui::SameLine();
	ImGui::RadioButton("LOOPNO", &LoopNow, 0);
	ImGui::End();

	if (LoopNow == 1 && LoopFlagNow == false) {
		pMotion->m_bLoop = true;
	}
	if (LoopNow == 0 && LoopFlagNow == true) {
		pMotion->m_bLoop = false;
	}
}

//キーフレームのフレーム数の変更
void CMotionEditor::EditChangeFrameUI(void)
{
	static char textBuf[4] = { '\0' };
	ImGui::Begin("MainMenu");
	ImGui::NewLine();
	ImGui::Text("Frame : %d", m_ModelMotion.GetFrameOfKeyFrame
	(m_ModelMotion.GetMotionNow(), m_ModelMotion.GetKeyFrameNow()));
	ImGui::InputText("InputFrame", textBuf, sizeof(textBuf));
	ImGui::End();

	ImGui::Begin("MainMenu");
	if(ImGui::Button("InputFrameButton") )
	{
		//入力は数字のみので、文字列に対してフィルタリングを行う
		bool IsCanWriteIn = false;			//「フレーム数を上書き可」フラグ
		if (textBuf[0] != '\0')
		{
			//文字数を数える
			int TextSize = sizeof(textBuf);
			int FontNum = 0;		//文字数
			for (int k = 0; k < TextSize; k++)
			{
				if (textBuf[k] == '\0')
				{
					FontNum = k;
					break;
				}
			}

			//文字列は数字のみかを検出
			int i = 0;
			for (i = 0; i < FontNum; i++)
			{
				//入力の数値は0-9ではない
				if (textBuf[i] < 48 || textBuf[i] > 57)
				{
					break;
				}
			}
			//全部の文字は数字
			if (i == FontNum && FontNum != 0)
			{
				IsCanWriteIn = true;
			}
			else
			{
				MessageBox(nullptr, "数字のみ入力できる!", "WARNING", MB_OK);
			}
		}

		if (IsCanWriteIn == true)
		{
			int Number = atoi(textBuf);
			m_ModelMotion.SetFrameOfKeyFrame
			(m_ModelMotion.GetMotionNow(), m_ModelMotion.GetKeyFrameNow(), Number);
		}
	}
	ImGui::End();
}

//キーフレームの複製と貼り付け
void CMotionEditor::CopyPastKeyFrameUI(void)
{
	CMotion* pMotionNow = m_ModelMotion.GetMotion(m_ModelMotion.GetMotionNow());	//モーション取得
	CKeyFrame *pKeyFrame = pMotionNow->GetKeyFrameData(m_ModelMotion.GetKeyFrameNow());		//キーフレームデータ取得

	ImGui::Begin("MainMenu");

	ImGui::NewLine();

	//キーフレームのコピー&ペスト
	if (ImGui::Button("CopyKeyFrame", ImVec2(100, 25))) {
		m_CopyKeyFrame = *pKeyFrame;
	}
	ImGui::SameLine();
	if (ImGui::Button("PastKeyFrame", ImVec2(100, 25))) {
		*pKeyFrame = m_CopyKeyFrame;
	}
	
	ImGui::End();
}

//キーフレームの編集
void CMotionEditor::EditKeyFrameUI(void)
{
	CMotion* pMotionNow = m_ModelMotion.GetMotion(m_ModelMotion.GetMotionNow());		//モーション取得
	CKeyFrame *pKeyFrame = pMotionNow->GetKeyFrameData(m_ModelMotion.GetKeyFrameNow());	//キーフレームデータ取得
	int nPartID = m_pSelectedPart->m_PartID;

	ImGui::Begin("MainMenu");

	ImGui::NewLine();
	ImGui::Text("EditKeyFrame");

	ImGui::Text("rotation");

	//X回転
	ImGui::DragFloat("XRot", &pKeyFrame->m_SRPData[nPartID].rot.x, OFFSET_MOVE_VALUE, 0.0f, 0.0f, "X Value : %.2f");
	ImGui::SameLine();
	if (ImGui::Button("XRotReset",ImVec2(70,20))) {
		pKeyFrame->m_SRPData[nPartID].rot.x = 0;
	}
	//Y回転
	ImGui::DragFloat("YRot", &pKeyFrame->m_SRPData[nPartID].rot.y, OFFSET_MOVE_VALUE, 0.0f, 0.0f, "Y Value : %.2f");
	ImGui::SameLine();
	if (ImGui::Button("YRotReset", ImVec2(70, 20))) {
		pKeyFrame->m_SRPData[nPartID].rot.y = 0;
	}
	//Z回転
	ImGui::DragFloat("ZRot", &pKeyFrame->m_SRPData[nPartID].rot.z, OFFSET_MOVE_VALUE, 0.0f, 0.0f, "Z Value : %.2f");
	ImGui::SameLine();
	if (ImGui::Button("ZRotReset", ImVec2(70, 20))) {
		pKeyFrame->m_SRPData[nPartID].rot.z = 0;
	}

	ImGui::Text("translation");
	//X移動
	ImGui::DragFloat("XPos", &pKeyFrame->m_SRPData[nPartID].pos.x, OFFSET_MOVE_VALUE, 0.0f, 0.0f, "X Value : %.2f");
	ImGui::SameLine();
	if (ImGui::Button("XPosReset", ImVec2(70, 20))) {
		pKeyFrame->m_SRPData[nPartID].pos.x = 0;
	}

	ImGui::DragFloat("YPos", &pKeyFrame->m_SRPData[nPartID].pos.y, OFFSET_MOVE_VALUE, 0.0f, 0.0f, "Y Value : %.2f");
	ImGui::SameLine();
	if (ImGui::Button("YPosReset", ImVec2(70, 20))) {
		pKeyFrame->m_SRPData[nPartID].pos.y = 0;
	}

	ImGui::DragFloat("ZPos", &pKeyFrame->m_SRPData[nPartID].pos.z, OFFSET_MOVE_VALUE, 0.0f, 0.0f, "Z Value : %.2f");
	ImGui::SameLine();
	if (ImGui::Button("ZPosReset", ImVec2(70, 20))) {
		pKeyFrame->m_SRPData[nPartID].pos.z = 0;
	}
	ImGui::End();

	//回転値を-PI ～ PIの範囲内固定
	if (pKeyFrame->m_SRPData[nPartID].rot.x > D3DX_PI) { pKeyFrame->m_SRPData[nPartID].rot.x -= D3DX_PI * 2; }
	if (pKeyFrame->m_SRPData[nPartID].rot.x < -D3DX_PI) { pKeyFrame->m_SRPData[nPartID].rot.x += D3DX_PI * 2; }
	if (pKeyFrame->m_SRPData[nPartID].rot.y > D3DX_PI) { pKeyFrame->m_SRPData[nPartID].rot.y -= D3DX_PI * 2; }
	if (pKeyFrame->m_SRPData[nPartID].rot.y < -D3DX_PI) { pKeyFrame->m_SRPData[nPartID].rot.y += D3DX_PI * 2; }
	if (pKeyFrame->m_SRPData[nPartID].rot.z > D3DX_PI) { pKeyFrame->m_SRPData[nPartID].rot.z -= D3DX_PI * 2; }
	if (pKeyFrame->m_SRPData[nPartID].rot.z < -D3DX_PI) { pKeyFrame->m_SRPData[nPartID].rot.z += D3DX_PI * 2; }
}

void CMotionEditor::ChangeOffset(void)
{
	if (nullptr == m_pSelectedPart) return;

	ImGui::Begin("MainMenu");
	ImGui::Text("ChangeOffset");

	//X方向移動
	ImGui::DragFloat("X", &m_pSelectedPart->m_SRPOffset.pos.x, OFFSET_MOVE_VALUE, 0.0f, 0.0f, "X Axis : %.2f");
	ImGui::SameLine();
	if (ImGui::Button("XValueReset", ImVec2(90, 20))) {
		m_pSelectedPart->m_SRPOffset.pos.x = 0;
	}
	//Y方向移動
	ImGui::DragFloat("Y", &m_pSelectedPart->m_SRPOffset.pos.y, OFFSET_MOVE_VALUE, 0.0f, 0.0f, "Y Axis : %.2f");
	ImGui::SameLine();
	if (ImGui::Button("YValueReset", ImVec2(90, 20))) {
		m_pSelectedPart->m_SRPOffset.pos.y = 0;
	}
	//Z方向移動
	ImGui::DragFloat("Z", &m_pSelectedPart->m_SRPOffset.pos.z, OFFSET_MOVE_VALUE, 0.0f, 0.0f, "Z Axis : %.2f");
	ImGui::SameLine();
	if (ImGui::Button("ZValueReset", ImVec2(90, 20))) {
		m_pSelectedPart->m_SRPOffset.pos.z = 0;
	}
	ImGui::End();
}

void CMotionEditor::ImguiForEditor(void)
{
	switch (m_EditMode) {
	case EDIT_OFFSET:			//パーツのオフセット設置
		PartListUI();
		ChangeOffset();
		break;
	case EDIT_LINK:				//別のパーツとの関連付け
		PartListUI();
		if (m_pSelectedPart != nullptr) {
			SelectParentListUI();
		}
		break;
	case EDIT_MOTION:			//キーフレームのステータスを変更し
		ChangeEditModeMotionUI();
		switch (m_MotionEdit) {
		case MOTION_EDIT_MODE_EDIT_KEYFRAME:
		{
			EditLoopFlagUI();
			EditChangeMotionUI();
			EditChangeKeyFrameUI();
			CopyPastKeyFrameUI();
			EditChangeFrameUI();
			OtherOptionButton();

			//キーフレーム編集
			if (m_pSelectedPart != nullptr) {
				EditKeyFrameUI();
			}

			//モーションリストの表示
			if (m_IsShowMotionList) {
				MotionListUI();
			}

			if (m_IsShowPartList) {
				PartListUIInEditKeyFrame();
			}

			break;
		}
		case MOTION_EDIT_MODE_MOTION_PREVIEW:
			EditChangeMotionUI();
			ImGui::Begin("MainMenu");
			if (ImGui::Checkbox("MotionPlay", &m_bMotionPlay)) {
				m_ModelMotion.SetKeyFrameNow(0);
			}
			ImGui::End();
			break;
		case MOTION_EDIT_MODE_KEYFRAME_PREVIEW:
			EditChangeMotionUI();
			EditChangeKeyFrameUI();
			ImGui::Begin("MainMenu");
			ImGui::Text("Frame : %d", m_ModelMotion.GetFrameOfKeyFrame				//フレーム数表示
			(m_ModelMotion.GetMotionNow(), m_ModelMotion.GetKeyFrameNow()));
			if (ImGui::Checkbox("KeyFramePlay", &m_bKeyFramePreviewPlay)) {			//キーフレーム再生チェックボックス
				m_KeyFramePercent = 0.0f;
			}
			ImGui::SliderFloat("KeyFramePercent", &m_KeyFramePercent, 0.0f, 1.0f);	//キーフレームパーセンテージ
			ImGui::End();
			break;
		}
		break;
	}
}

void CMotionEditor::SelectPartObj(void)
{
	if (false == CSceneTransition::GetInputMouse()->GetMouseTrigger(MOUSE_LEFT)) return;
	if (m_bClickFlag) return;
	m_bClickFlag = true;

	//新しいモデルを選択する前のクリア設定
	if (m_pSelectedPart != nullptr) {
		m_pSelectedPart->m_Model.SetDrawType(CModel::DRAW_USE_MTRL_DEFALUT);		//描画方式をデフォルトに戻る
		m_pSelectedPart = nullptr;											//何も選択されていない状態に戻る
	}

	//オブジェクトの選択
	D3DXVECTOR3 RayStartPoint = m_MouseEditMesh.GetMouseRayStartPoint();		//レイの始点
	D3DXVECTOR3 RayEndPoint = m_MouseEditMesh.GetMouseRayEndPoint();			//レイの終点
	float DistanceMin = D3DXVec3Length(&(RayEndPoint - RayStartPoint));			//最短距離
	
	for (int ObjNum = 0; ObjNum < m_ModelMotion.GetPartTotalNum(); ObjNum++) {
		CPart *pPart = m_ModelMotion.GetPartPointer(ObjNum);
		if (pPart == nullptr) continue;							//オブジェクトはヌルだったら
		D3DXVECTOR3 HitPosition = D3DXVECTOR3(0.0f,0.0f,0.0f);	//当り判定位置
		bool bHit = RayPickToMesh::RaypicktoMesh(				//マウスレイとの当り判定
			false,
			&HitPosition,
			RayEndPoint,
			RayStartPoint,
			pPart->GetMesh(),
			pPart->GetNowMtx()
		);		//レイとメッシュの当り判定を取る

		if (bHit == false) continue;							//当たってないなら以下処理しない
		float Distance = D3DXVec3Length(&(HitPosition - RayStartPoint));	//レイ始点との距離を求め
		if (Distance < DistanceMin) {
			DistanceMin = Distance;			//最短距離更新
			m_pSelectedPart = pPart;		//パーツ代入
		}
	}

	if (m_pSelectedPart != nullptr) {
		m_pSelectedPart->m_Model.SetDrawType(CModel::DRAW_USE_MTRL_CUSTOMIZE);
		m_pSelectedPart->m_Model.SetMaterial(m_SelectPartMtrl);
	}
}

//デバックメッセージ
void CMotionEditor::ShowUndoRedoStuckSize(void)
{
	int StuckSize = m_UndoRedoStuck.size();
	int Position = 0;

	auto ListPositionNow = std::find(m_UndoRedoStuck.begin(), m_UndoRedoStuck.end(), m_pPushPosition);
	for (auto it = m_UndoRedoStuck.begin(); it != m_UndoRedoStuck.end(); ++it) {
		Position++;
		if (it == ListPositionNow) break;
	}

	ImGui::Begin("UndoRedo");
	ImGui::Text("StuckSize     : %d", StuckSize);
	ImGui::Text("StuckPosition : %d", Position);
	ImGui::End();
}

void CMotionEditor::DetectMouseClickInWindow(void)
{
	ImGuiIO& io = ImGui::GetIO();
	bool IsHoverWindow = io.WantCaptureMouse;			//マウスがウィンドウの所にいるか
	bool IsClick = CSceneTransition::GetInputMouse()->GetMousePress(MOUSE_LEFT);		//クリックしたか

	if (IsHoverWindow && IsClick) {					//判定があれば今のデータを一時的に保存
		m_PushReadyData.ModelMotion = m_ModelMotion;
		m_PushReadyData.EditMode = m_EditMode;
		m_PushReadyData.MotionEdit = m_MotionEdit;
		m_bMouseHoverAndClickInWindow = true;
	}
}

void CMotionEditor::DetectMouseRelease(void)
{
	bool IsClick = CSceneTransition::GetInputMouse()->GetMousePress(MOUSE_LEFT);		//クリックしたか
	if (IsClick) {
		return;
	}

	m_bMouseHoverAndClickInWindow = false;		//フラグ還元
	DetectDataChangeAndPushData();				//データの変化を検知し、データが変化がある場合データをスタックにプッシュ
}

void CMotionEditor::DetectDataChangeAndPushData(void)
{
	//データの変化があるかを検知
	bool bChangeFlag = false;
	int MotionNow = m_ModelMotion.GetMotionNow();
	CMotion *pNowMotion = m_ModelMotion.GetMotion(MotionNow);
	CMotion *pOldMotion = m_PushReadyData.ModelMotion.GetMotion(MotionNow);

	//パーツデータ変化の検知
	for (int PartNum = 0; PartNum < m_ModelMotion.GetPartTotalNum(); PartNum++) {
		//データ取得
		CPart* pNowMotionPart = m_ModelMotion.GetPartPointer(PartNum);
		CPart* pOldMotionPart = m_PushReadyData.ModelMotion.GetPartPointer(PartNum);

		//親の変化を検知
		CPart* PartNow = pNowMotionPart->GetParent();
		CPart* PartOld = pOldMotionPart->GetParent();
		if (PartNow == nullptr) {
			if (PartNow != PartOld) {
				bChangeFlag = true;
				break;
			}
		}
		else {
			if (PartOld == nullptr) {
				bChangeFlag = true;
				break;
			}
			if (pNowMotionPart->GetParent()->m_PartID != pOldMotionPart->GetParent()->m_PartID) {
				bChangeFlag = true;
				break;
			}
		}

		//オフセットの変化を検知
		if (pNowMotionPart->m_SRPOffset.pos != pOldMotionPart->m_SRPOffset.pos) {
			bChangeFlag = true;
			break;
		}

		//パーツのキーフレームデータの変化の検知
		CKeyFrame* pNowKeyFrame = pNowMotion->GetKeyFrameData(m_ModelMotion.GetKeyFrameNow());
		CKeyFrame* pOldKeyFrame = pOldMotion->GetKeyFrameData(m_PushReadyData.ModelMotion.GetKeyFrameNow());
		SRP SRPNow = pNowKeyFrame->m_SRPData[PartNum];
		SRP SRPOld = pOldKeyFrame->m_SRPData[PartNum];
		if ((SRPNow.pos != SRPOld.pos) || (SRPNow.rot != SRPOld.rot)) {
			bChangeFlag = true;
			break;
		}
	}

	//今のモーションのキーフレームの比較
	if (bChangeFlag == false) {
		const int NowKeyFrameNum = pNowMotion->GetTotalKeyFrame();
		const int OldKeyFrameNum = pOldMotion->GetTotalKeyFrame();
		if (NowKeyFrameNum != OldKeyFrameNum) {
			bChangeFlag = true;
		}
		else{
			CKeyFrame* pNowKeyFrame = pNowMotion->GetKeyFrameData(m_ModelMotion.GetKeyFrameNow());
			CKeyFrame* pOldKeyFrame = pOldMotion->GetKeyFrameData(m_PushReadyData.ModelMotion.GetKeyFrameNow());

			//キーフレームの継続フレーム数を比較
			if (pNowKeyFrame->m_TotalFrame != pOldKeyFrame->m_TotalFrame){
				bChangeFlag = true;
			}
			//キーフレームの曲線データの比較
			if(pNowKeyFrame->m_HermitianStartDir != pOldKeyFrame->m_HermitianStartDir){
				bChangeFlag = true;
			}
			if (pNowKeyFrame->m_HermitianEndDir != pOldKeyFrame->m_HermitianEndDir) {
				bChangeFlag = true;
			}
		}
	}

	//モーション総数の比較
	if (bChangeFlag == false) {
		if (m_ModelMotion.GetTotalMotionNum() != m_PushReadyData.ModelMotion.GetTotalMotionNum()) {
			bChangeFlag = true;
		}
	}

	//ループフラグ比較
	if (bChangeFlag == false) {
		if (pNowMotion->m_bLoop != pOldMotion->m_bLoop)
		bChangeFlag = true;
	}

	//変化があった場合データを保存
	if (bChangeFlag) {
		PushData();
	}
}

void CMotionEditor::PushData(void)
{
	//データをコピー
	UNDO_REDO_STUCK* pData = new UNDO_REDO_STUCK;
	pData->ModelMotion = m_ModelMotion;
	pData->EditMode = m_EditMode;
	pData->MotionEdit = m_MotionEdit;

	//データを検索
	auto ListPositionNow = std::find(m_UndoRedoStuck.begin(), m_UndoRedoStuck.end(), m_pPushPosition);
	if (ListPositionNow == m_UndoRedoStuck.end()) {	//データが存在しない
		m_UndoRedoStuck.push_back(pData);	//データをスタックに入れる
		m_pPushPosition = pData;			//データを保存
		return;
	}
	else {	//データが存在する
		for (;;) {
			ListPositionNow = std::find(m_UndoRedoStuck.begin(), m_UndoRedoStuck.end(), m_pPushPosition);
			auto DeleteIt = ListPositionNow;		//コピー
			++DeleteIt;

			if (DeleteIt == m_UndoRedoStuck.end()) break;	//最後尾かどうか

			//データ削除
			(*DeleteIt)->ModelMotion.Uninit();
			delete (*DeleteIt);
			m_UndoRedoStuck.erase(DeleteIt);
		}
	}

	//データをスタックに入れる
	m_UndoRedoStuck.push_back(pData);					//データを保存
	if (m_UndoRedoStuck.size() > STUCK_MAX) {			//データが最大スタック数を超えたか
		//先頭データを削除
		(*m_UndoRedoStuck.begin())->ModelMotion.Uninit();
		delete (*m_UndoRedoStuck.begin());
		m_UndoRedoStuck.pop_front();
	}

	//データ反映
	m_pPushPosition = pData;				//位置記録
	m_PushReadyData = *pData;				//データをプッシュ予備データに反映
}

void CMotionEditor::Undo(void)
{
	//データを検索
	auto ListPositionNow = std::find(m_UndoRedoStuck.begin(), m_UndoRedoStuck.end(), m_pPushPosition);
	if (ListPositionNow == m_UndoRedoStuck.end()) return;
	if (ListPositionNow == m_UndoRedoStuck.begin()) return;

	--ListPositionNow;

	//データ記録
	m_pPushPosition = *ListPositionNow;					//位置記録
	m_PushReadyData = *(*ListPositionNow);				//データをプッシュ予備データに反映
	m_ModelMotion = (*ListPositionNow)->ModelMotion;	//データを今のモーションデータに反映
	m_EditMode = (*ListPositionNow)->EditMode;
	m_MotionEdit = (*ListPositionNow)->MotionEdit;
}

void CMotionEditor::Redo(void)
{
	//データを検索
	auto ListPositionNow = std::find(m_UndoRedoStuck.begin(), m_UndoRedoStuck.end(), m_pPushPosition);
	if (ListPositionNow == m_UndoRedoStuck.end()) return;

	//最後尾かどうか
	++ListPositionNow;
	if (ListPositionNow == m_UndoRedoStuck.end()) return;

	//データ記録
	m_pPushPosition = *ListPositionNow;					//位置記録
	m_PushReadyData = *(*ListPositionNow);				//データをプッシュ予備データに反映
	m_ModelMotion = (*ListPositionNow)->ModelMotion;	//データを今のモーションデータに反映
	m_EditMode = (*ListPositionNow)->EditMode;
	m_MotionEdit = (*ListPositionNow)->MotionEdit;
}

/*==============================================================================
	データの保存
===============================================================================*/
void CMotionEditor::SaveData(void)
{
	//現在時刻取得し、文字配列に詰め込む
	char FilePass[256] = { '\0' };
	strcat_s(FilePass, 256, "data/MOTION_DATA/MotionEditor/Save/motion_");	//書き出すファイル先を指定
	GetTimeNow(&FilePass[0]);			//時間をくっつける
	strcat_s(FilePass, 256,".txt");		//.txt拡張子くっつける
	
	//ファイル作成
	FILE *fp;
	fp = fopen(FilePass,"w");
	if (fp == nullptr) 
	{
		MessageBox(nullptr, "ファイルを開けません", "[CMotionEditor.cpp][Function:SaveData]", MB_ICONHAND);
		fclose(fp);
		return;
	}

	fprintf(fp, "#------------------------------------------------------------------------------\n");
	fprintf(fp, "# モーションデータの開始\n");
	fprintf(fp, "#------------------------------------------------------------------------------\n");
	fprintf(fp, "SCRIPT\t\t\t# この行は絶対消さないこと！\n\n");
	fprintf(fp, "#------------------------------------------------------------------------------\n");
	fprintf(fp, "# モデル数\n");
	fprintf(fp, "#------------------------------------------------------------------------------\n");

	//モデル数書き出す
	const int TotalPartNum = m_ModelMotion.GetPartTotalNum();	//パーツ数取得
	fprintf(fp, "NUM_MODEL= %d\n", TotalPartNum);

	fprintf(fp, "#------------------------------------------------------------------------------\n");
	fprintf(fp, "# モデルファイル名\n");
	fprintf(fp, "#------------------------------------------------------------------------------\n");

	//パーツファイルパス書き出す
	for (int i = 0; i < TotalPartNum; i++)
	{
		std::string buf;
		buf.clear();
		buf += "MODEL_FILENAME= ";
		buf += m_ModelMotion.GetPartFilePass(i);
		fprintf(fp, "%s\t\t\t", buf.c_str());
		fprintf(fp, "#[%d]\n",i);
	}
	fprintf(fp,"\n");

	fprintf(fp, "#------------------------------------------------------------------------------\n");
	fprintf(fp, "# キャラクター情報\n");
	fprintf(fp, "#------------------------------------------------------------------------------\n");
	fprintf(fp, "CHARACTERSET\n");
	fprintf(fp, "\tNUM_PARTS= %d\n\n", TotalPartNum);

	//パーツオフセットの書き出し
	for (int i = 0; i < TotalPartNum; i++)
	{
		fprintf(fp, "\tPARTSSET\n");
		CPart* pPart = m_ModelMotion.GetPartPointer(i);
		if (pPart != nullptr) 
		{
			fprintf(fp, "\t\tNAME= %s\n", pPart->m_Name);		//自分の名前書き出す
			fprintf(fp, "\t\tINDEX= %d\n", pPart->m_PartID);	//自分のID書き出す
			fprintf(fp, "\t\tPARENT= %d\n", pPart->m_ParentID);	//親のID書き出す
			fprintf(fp, "\t\tPOS= %.3f %.3f %.3f\n", pPart->m_SRPOffset.pos.x, pPart->m_SRPOffset.pos.y, pPart->m_SRPOffset.pos.z);
			fprintf(fp, "\t\tROT= %.3f %.3f %.3f\n", pPart->m_SRPOffset.rot.x, pPart->m_SRPOffset.rot.y, pPart->m_SRPOffset.rot.z);
		}
		fprintf(fp, "\tEND_PARTSSET\n\n");
	}
	fprintf(fp, "END_CHARACTERSET\n");

	fprintf(fp, "#------------------------------------------------------------------------------\n");
	fprintf(fp, "# モーション情報\n");
	fprintf(fp, "#------------------------------------------------------------------------------\n");

	//モーション情報書き出す
	const int MotionNum = m_ModelMotion.GetTotalMotionNum();	//モーション数取得
	for (int i = 0; i < MotionNum; i++)
	{
		//モーション取得
		CMotion* pMotion = m_ModelMotion.GetMotion(i);
		if (pMotion == nullptr) {
			continue;
		}

		fprintf(fp, "#------------------------------------------------------------------------------\n");
		fprintf(fp, "# [%d]%s\n", i, pMotion->m_Name);
		fprintf(fp, "#------------------------------------------------------------------------------\n");
		fprintf(fp, "MOTIONSET\n");

		//モーションの名前とループフラグとキー数の書き出す
		fprintf(fp, "\tNAME= %s\n", pMotion->m_Name);
		int nLoop = 0;
		(pMotion->m_bLoop == false) ? (nLoop = 0) : (nLoop = 1);
		fprintf(fp, "\tLOOP= %d\n", nLoop);
		const int TotalKeyFrame = pMotion->m_TotalKeyFrame;	//キーフレーム総数取得
		fprintf(fp, "\tNUM_KEY= %d\n\n", TotalKeyFrame);

		//キーフレームデータの書き出す
		for (int KeyFrameNum = 0; KeyFrameNum < TotalKeyFrame; KeyFrameNum++)
		{
			fprintf(fp, "\tKEYSET\t\t\t");
			fprintf(fp, "# --- << KEY : %d / %d >> ---\n", KeyFrameNum, TotalKeyFrame - 1);
			CKeyFrame* pKeyFrame = pMotion->GetKeyFrameData(KeyFrameNum);
			if (pKeyFrame == nullptr) {
				continue;
			}

			//キーフレームのフレーム総数を書き出す
			fprintf(fp, "\t\tFRAME= %d\n", pKeyFrame->m_TotalFrame);

			//エルミート曲線の起点ベクトルと終点ベクトルの書き出す
			fprintf(fp, "\t\tSTART_DIR= %.3f %.3f\n", pKeyFrame->m_HermitianStartDir.x, pKeyFrame->m_HermitianStartDir.y);
			fprintf(fp, "\t\tEND_DIR= %.3f %.3f\n\n", pKeyFrame->m_HermitianEndDir.x, pKeyFrame->m_HermitianEndDir.y);

			//パーツごとのキーフレームデータを書き出す
			for (int PartNum = 0; PartNum < TotalPartNum; PartNum++) 
			{
				fprintf(fp, "\t\tKEY # ----- [ %d ] -----\n", PartNum);
				SRP KeyFrameSRP = pKeyFrame->m_SRPData[PartNum];
				fprintf(fp, "\t\t\tPOS= %.3f %.3f %.3f\n", KeyFrameSRP.pos.x, KeyFrameSRP.pos.y, KeyFrameSRP.pos.z);
				fprintf(fp, "\t\t\tROT= %.3f %.3f %.3f\n", KeyFrameSRP.rot.x, KeyFrameSRP.rot.y, KeyFrameSRP.rot.z);
				(PartNum != TotalPartNum - 1)? 
					fprintf(fp, "\t\tEND_KEY\n\n") : fprintf(fp, "\t\tEND_KEY\n");
			}

			(KeyFrameNum != TotalKeyFrame - 1) ?
				fprintf(fp, "\tEND_KEYSET\n\n") : fprintf(fp, "\tEND_KEYSET\n");
		}

		fprintf(fp, "END_MOTIONSET\n\n");

	}

	fprintf(fp, "END_SCRIPT\t\t# この行は絶対消さないこと！");

	fclose(fp);

	MessageBox(nullptr, "モーションデータセーブ完了", "SUCCESS", MB_OK);
}

/*==============================================================================
	文字配列に現在の時刻を詰め込む
===============================================================================*/
void CMotionEditor::GetTimeNow(char* pChar)
{
	//ヌルチェック
	if (pChar == nullptr) {
		MessageBox(nullptr, "[CMotionEditor.cpp][Function:GetTimeNow]引数pCharはヌル", "WARNING", MB_ICONHAND);
		return;
	}

	//現在時刻取得
	time_t t = time(nullptr);
	tm TimeNow;
	localtime_s(&TimeNow, &t);

	//年月日データを文字列に詰め込む
	int year = TimeNow.tm_year + 1900;
	char Yeah[5];
	for (int i = 0; i < 4; i++) {
		int Num = year % 10;
		year /= 10;
		Yeah[4 - 1 - i] = (char)(Num + 48);		//数字を文字に変換
	}
	Yeah[4] = '\0';
	strcat_s(pChar,256, Yeah);

	int mouth = TimeNow.tm_mon + 1;
	char Mouth[3];
	Mouth[0] = (char)(mouth / 10 + 48);
	Mouth[1] = (char)(mouth % 10 + 48);
	Mouth[2] = '\0';
	strcat_s(pChar, 256, Mouth);

	int day = TimeNow.tm_mday;
	char Day[4];
	Day[0] = (char)(day / 10 + 48);
	Day[1] = (char)(day % 10 + 48);
	Day[2] = '_';
	Day[3] = '\0';
	strcat_s(pChar, 256, Day);

	//時分秒データを文字列に詰め込む
	int hour = TimeNow.tm_hour;
	char Hour[3];
	Hour[0] = (char)(hour / 10 + 48);
	Hour[1] = (char)(hour % 10 + 48);
	Hour[2] = '\0';
	strcat_s(pChar, 256, Hour);

	int min = TimeNow.tm_min;
	char Min[3];
	Min[0] = (char)(min / 10 + 48);
	Min[1] = (char)(min % 10 + 48);
	Min[2] = '\0';
	strcat_s(pChar, 256, Min);

	int sec = TimeNow.tm_sec;
	char Sec[3];
	Sec[0] = (char)(sec / 10 + 48);
	Sec[1] = (char)(sec % 10 + 48);
	Sec[2] = '\0';
	strcat_s(pChar, 256, Sec);
}

void CMotionEditor::UndoRedoUI(void)
{
	ImGui::Begin("UndoRedo");
	if(ImGui::Button("Undo")) Undo();
	ImGui::SameLine();
	if (ImGui::Button("Redo")) Redo();
	ImGui::End();

	ShowUndoRedoStuckSize();
}