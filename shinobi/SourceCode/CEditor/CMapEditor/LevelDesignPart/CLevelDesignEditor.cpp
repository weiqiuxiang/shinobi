#include "CLevelDesignEditor.h"
#include "CPlayer.h"
#include "CEnemy.h"
#include "CEnemyStorage.h"
#include "SceneTransition.h"
#include "raypicktomesh.h"
#include "CCameraManager.h"
#include <imgui.h>
#include "CCreatureDataIO.h"
#include "CNavMeshManager.h"

#define SCL_IS_ONE_BASE_DIST (30.0f)	//座標軸の拡大率
#define DEFAULT_FIELD_COFFI (0.2f)		//デフォルト地面摩擦係数
#define HOLD_MOUSE_RIGHT_TIME (100)		//右クリックを押し続ける判定用時間
#define AXIS_MOVE_COEFFICIENT (0.02f)	//軸移動係数(使い方 : 軸移動係数 * マウスの相対移動量)

#define NO_AXIS_BE_SELECTED (-1)

CLevelDesignEditor::CLevelDesignEditor()
{
	m_pEnemy = nullptr;
	m_pPathPoint = nullptr;
	m_MouseRightButtonFrameTime = timeGetTime();
	m_MouseRightButtonPushTime = 0;

	m_SetCreatureMode = SET_CREATURE;
	m_SetEnemyPathMode = SELECT_CREATURE;
	m_AxisSelected = NO_AXIS_BE_SELECTED;

	m_SelectObjMtrl.Diffuse = { 0.0f,1.0f,0.0f,0.3f };
	m_SelectObjMtrl.Ambient = { 0.5f,0.5f,0.5f,0.0f };
	m_SelectObjMtrl.Specular = { 0.0f,0.0f,0.0f,0.0f };
	m_SelectObjMtrl.Emissive = { 0.0f,0.0f,0.0f,0.0f };
	m_SelectObjMtrl.Power = 6.0f;

	m_MouseInAxisMtrl.Diffuse = { 0.5f,0.5f,0.5f,1.0f };
	m_MouseInAxisMtrl.Ambient = { 0.5f,0.5f,0.5f,1.0f };
	m_MouseInAxisMtrl.Specular = { 0.0f,0.0f,0.0f,0.0f };
	m_MouseInAxisMtrl.Emissive = { 0.0f,0.0f,0.0f,0.0f };
	m_MouseInAxisMtrl.Power = 6.0f;

	m_SelectAxisMtrl.Diffuse = { 0.9f,0.9f,0.9f,1.0f };
	m_SelectAxisMtrl.Ambient = { 0.5f,0.5f,0.5f,1.0f };
	m_SelectAxisMtrl.Specular = { 0.0f,0.0f,0.0f,0.0f };
	m_SelectAxisMtrl.Emissive = { 0.0f,0.0f,0.0f,0.0f };
	m_SelectAxisMtrl.Power = 6.0f;
}

CLevelDesignEditor::~CLevelDesignEditor()
{
	
}

//初期化
bool CLevelDesignEditor::Init(HWND hWnd)
{
	hWnd;				//ダミー参照

	m_UI.Init();
	m_pEnemy = nullptr;
	m_pPathPoint = nullptr;
	m_MouseRightButtonFrameTime = timeGetTime();
	m_MouseRightButtonPushTime = 0;

	//敵の初期化
	CEnemyStorage::InitAll();

	//メッシュにあるマウス位置の初期化
	m_MouseEditMeshField.Init(5.0f, { 100,0,0,200 });

	//座標系初期化
	m_Coordinate.Init(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	//敵データ読み取り
	CCreatureDataIO EnemyIO;
	EnemyIO.ImportEnemyObjData("data/CREATURE_DATA/EnemyDataOutput.txt");

	return true;
}

//終了処理
void CLevelDesignEditor::Uninit(void)
{
	m_UI.Uninit();

	//メッシュにあるマウス位置の解放
	m_MouseEditMeshField.Uninit();

	//敵の解放
	CEnemyStorage::UninitAll();

	//座標系解放
	m_Coordinate.Uninit();
}

//更新処理
void CLevelDesignEditor::Update(void)
{
	m_UI.Update();

	//座標系更新
	m_Coordinate.Update();

	//メッシュ上マウスの位置を計算する
	m_MouseEditMeshField.CalcMousePositionInMesh();

	//右クリックのタイマー計測
	UpdateRightButtonClickTimer();

	//全ての敵の更新
	CEnemyStorage::UpdateEditorAll();
	CEnemyStorage::UpdateAllEnemyPathPosition();

	//モードによる実行
	ModeAction();

	if (m_UI.SaveEnemyUI() == true) {
		CCreatureDataIO EnemyIO;
		EnemyIO.ExportEnemyObjData();
		MessageBox(nullptr, "敵テータ保存完了", "", MB_OK);
	}
}

//描画
void CLevelDesignEditor::Draw(void)
{
	//全敵描画
	CEnemyStorage::DrawAll();

	//モードによる実行
	switch (m_UI.GetMode()) {
	case CLevelDesignUI::SET_CREATURE:
		m_Coordinate.Draw();
		break;
	case CLevelDesignUI::SET_ENEMY_PATH:
		m_Coordinate.Draw();
		if (m_pEnemy != nullptr) {
			m_pEnemy->DrawPath();
		}
		break;
	case CLevelDesignUI::SET_NAV_MESH:
		break;
	}
}

void CLevelDesignEditor::DrawNavMesh(void)
{

}

//右クリックのタイマー関連
void CLevelDesignEditor::UpdateRightButtonClickTimer(void)
{
	//右クリック押し始め
	if (CSceneTransition::GetInputMouse()->GetMouseTrigger(MOUSE_RIGHT)) {
		m_MouseRightButtonFrameTime = timeGetTime();
		m_MouseRightButtonPushTime = 0;
		return;
	}

	//右クリックの累積時間
	if (CSceneTransition::GetInputMouse()->GetMousePress(MOUSE_RIGHT)) {
		int TimeNow = timeGetTime();											//現在時刻の取得
		m_MouseRightButtonPushTime += TimeNow - m_MouseRightButtonFrameTime;	//右クリックの時間累積
		m_MouseRightButtonFrameTime = TimeNow;									//現在時刻の保存
	}
}

//モード変更時の初期化
void CLevelDesignEditor::ModeChangeInit(void)
{
	static CLevelDesignUI::MODE S_ModeOld = CLevelDesignUI::SET_CREATURE;

	if (S_ModeOld == m_UI.GetMode()) {
		return;
	}

	switch (S_ModeOld) {
	case CLevelDesignUI::SET_CREATURE:
	{
		SET_CREATURE_MODE OldSetCreatureMode = m_SetCreatureMode;
		m_SetCreatureMode = SET_CREATURE;

		switch (OldSetCreatureMode) {
		case SET_CREATURE:
			if (m_pEnemy != nullptr) {
				CEnemyStorage::Delete(m_pEnemy->GetUseID());
				m_pEnemy = nullptr;
			}
			break;
		case MOVE_ROT_CREATURE:
			m_Coordinate.SetDrawUse(false);
			if (m_pEnemy != nullptr) {
				m_pEnemy->SetDrawType(CModel::DRAW_USE_MTRL_DEFALUT);
				m_pEnemy = nullptr;
			}
			break;
		}
		break;
	}

	case CLevelDesignUI::SET_ENEMY_PATH:
		if (m_pEnemy != nullptr) {
			m_Coordinate.SetDrawUse(false);
			if (m_pEnemy != nullptr) {
				m_pEnemy->SetDrawType(CModel::DRAW_USE_MTRL_DEFALUT);
				m_pEnemy = nullptr;
			}
		}
		break;

	case CLevelDesignUI::SET_NAV_MESH:
		break;
	}

	//モードの上書き
	S_ModeOld = m_UI.GetMode();
}

void CLevelDesignEditor::ModeAction(void)
{
	//モード変化時の初期化
	ModeChangeInit();

	switch (m_UI.GetMode()) {
	case CLevelDesignUI::SET_CREATURE:
	{
		static bool IsMouseLeftButtonHold = false;			//左ボタンをしたままなのか

		if (m_pEnemy == nullptr) {
			if (CreateCreature() == true) {
				m_SetCreatureMode = SET_CREATURE;
			}
			if (MouseSelectEnemy() == true) {
				m_SetCreatureMode = MOVE_ROT_CREATURE;
				m_Coordinate.SetDrawUse(true);
				IsMouseLeftButtonHold = true;
			}
		}
		else {
			switch (m_SetCreatureMode) {
			case SET_CREATURE:
				SetCreature();
				break;
			case MOVE_ROT_CREATURE:
				//マウス押し続けるなら、以下は実行しない
				if (CSceneTransition::GetInputMouse()->GetMouseRelease(MOUSE_LEFT)) {
					IsMouseLeftButtonHold = false;
				}
				if (IsMouseLeftButtonHold == true) {
					break;
				}

				CoordinateAction();
				MoveCreature();
				break;
			}
		}
		break;
	}

	case CLevelDesignUI::SET_ENEMY_PATH:
	{
		static bool IsMouseLeftButtonHold = false;			//左ボタンをしたままなのか

		switch (m_SetEnemyPathMode) {
		case SELECT_CREATURE:
			if (MouseSelectEnemy() == true) {
				m_SetEnemyPathMode = SELECT_CREATURE_PATH;
			}
			else {
				ImGui::Begin("MainMenu");
				ImGui::Text("please select a Enemy");
				ImGui::End();
			}
			break;
		case SELECT_CREATURE_PATH:
			//例外処理
			if (m_pEnemy == nullptr) {
				m_SetEnemyPathMode = SELECT_CREATURE;
				break;
			}

			//巡回ポイント追加
			AddPathPoint();				//巡回ポイントの追加

			if (m_pPathPoint == nullptr) {
				//敵選択をキャンセル
				CancelSelectEnemy();

				//巡回ポイント選択
				if (SelectPathPoint() == true) {
					IsMouseLeftButtonHold = true;
				}
			}

			else {
				if (CSceneTransition::GetInputMouse()->GetMouseRelease(MOUSE_LEFT)) {
					IsMouseLeftButtonHold = false;
				}
				if (IsMouseLeftButtonHold == true) {
					break;
				}

				//パス移動
				CoordinateAction();
				MovePathPoint();

				//巡回ポイントの挿入
				InsertPathPoint();

				//パス削除
				if (DeletePathPoint() == true) {
				}

				//待ち時間の設定
				SetPathPointStopFrame();
			}
			break;
		}

		break;
	}

	case CLevelDesignUI::SET_NAV_MESH:
		break;
	}
}

bool CLevelDesignEditor::CreateCreature(void)
{
	bool IsCreateEnemy = false;

	int EnemyNumber = m_UI.GetSelectEnemyType();

	//選択なし
	if (EnemyNumber == -1) {
		return false;
	}

	D3DXVECTOR3 MousePosition = m_MouseEditMeshField.GetMousePosInMesh();

	switch (EnemyNumber) {
	case 0:
		m_pEnemy = CEnemyStorage::Create(CEnemy::TYPE_DOG, MousePosition,D3DXVECTOR3(0,0,0), MOTION_DATA_ENEMYDOG, 1.0f, -0.1f, DEFAULT_FIELD_COFFI);
		IsCreateEnemy = true;
		break;
	}

	return IsCreateEnemy;
}

bool CLevelDesignEditor::MouseSelectEnemy(void)
{
	if (false == CSceneTransition::GetInputMouse()->GetMouseTrigger(MOUSE_LEFT)) {
		return false;
	}

	//ImGui使用中なら無効
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse) {
		return false;
	}

	//新しいモデルを選択する前のクリア設定
	if (m_pEnemy != nullptr) {
		m_pEnemy->SetDrawType(CModel::DRAW_USE_MTRL_DEFALUT);		//描画方式をデフォルトに戻る
		m_pEnemy = nullptr;											//何も選択されていない状態に戻る
	}

	//オブジェクトの選択
	D3DXVECTOR3 RayStartPoint = m_MouseEditMeshField.GetMouseRayStartPoint();		//レイの始点
	D3DXVECTOR3 RayEndPoint = m_MouseEditMeshField.GetMouseRayEndPoint();			//レイの終点
	float DistanceMin = D3DXVec3Length(&(RayEndPoint - RayStartPoint));			//最短距離

	//全ての敵に対してマウスレイとの当り判定を行う
	auto it = CEnemyStorage::GetEnemiesListBegin();
	auto itEnd = CEnemyStorage::GetEnemiesListEnd();
	for (; it != itEnd; ++it) {
		//オブジェクトはヌルだったら
		if (*it == nullptr) {
			continue;
		}

		D3DXVECTOR3 HitPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	//当り判定位置
		bool bHit = RayPickToMesh::RaypicktoMesh(				//マウスレイとの当り判定
			false,
			&HitPosition,
			RayEndPoint,
			RayStartPoint,
			(*it)->GetBodyMesh(),
			(*it)->GetBodyResultMtx()
		);

		//当たってないなら以下処理しない
		if (bHit == false) {
			continue;
		}

		//一番手前のオブジェクトを当る対象に設定
		float Distance = D3DXVec3Length(&(HitPosition - RayStartPoint));
		if (Distance < DistanceMin) {
			DistanceMin = Distance;			//最短距離更新
			m_pEnemy = (*it);				//モデル代入
		}
	}

	if (m_pEnemy != nullptr) {
		m_pEnemy->SetDrawType(CModel::DRAW_USE_MTRL_CUSTOMIZE);
		m_pEnemy->SetMaterial(m_SelectObjMtrl);
		m_Coordinate.SetPos(m_pEnemy->GetPos());
		return true;
	}

	return false;
}

//敵の位置設定
void CLevelDesignEditor::SetCreature(void)
{
	if (m_pEnemy == nullptr) {
		return;
	}

	//ImGui使用中なら無効
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse) {
		return;
	}

	D3DXVECTOR3 MousePosition = m_MouseEditMeshField.GetMousePosInMesh();

	m_pEnemy->SetPos(MousePosition);

	//敵を設置
	if (CSceneTransition::GetInputMouse()->GetMouseTrigger(MOUSE_LEFT)) {
		m_pEnemy = CEnemyStorage::Create(CEnemy::TYPE_DOG, MousePosition, D3DXVECTOR3(0, 0, 0), MOTION_DATA_ENEMYDOG, 1.0f, -0.1f, DEFAULT_FIELD_COFFI);
	}

	//敵設置をキャンセル
	if (CSceneTransition::GetInputMouse()->GetMouseRelease(MOUSE_RIGHT)) {
		if (m_MouseRightButtonPushTime < HOLD_MOUSE_RIGHT_TIME) {
			CEnemyStorage::Delete(m_pEnemy->GetUseID());
			m_pEnemy = nullptr;
		}
		m_MouseRightButtonPushTime = 0;					//マウスの右ボタンのタイムリセット
	}
}

void CLevelDesignEditor::MoveCreature(void)
{
	if (m_pEnemy == nullptr) {
		return;
	}
	
	D3DXVECTOR3 Dist = m_Coordinate.GetMoveDist();
	m_pEnemy->AddPos(Dist);

	//敵選択をキャンセル
	CancelSelectEnemy();

	//選択対象削除
	if (CSceneTransition::GetInputKeyboard()->GetKeyTrigger(DIK_DELETE)) {
		CEnemyStorage::Delete(m_pEnemy->GetUseID());
		m_Coordinate.SetDrawUse(false);
		m_pEnemy = nullptr;
	}
}

void CLevelDesignEditor::CancelSelectEnemy(void)
{
	if (CSceneTransition::GetInputMouse()->GetMouseRelease(MOUSE_RIGHT)) {
		if (m_MouseRightButtonPushTime < HOLD_MOUSE_RIGHT_TIME) {
			m_Coordinate.SetDrawUse(false);
			m_pEnemy->SetDrawType(CModel::DRAW_USE_MTRL_DEFALUT);
			m_pEnemy = nullptr;
		}
		m_MouseRightButtonPushTime = 0;
	}
}

//軸のアクション
void CLevelDesignEditor::CoordinateAction(void)
{
	CoordinateAxisSelect();
	SelectedAxisAction();
}

//軸選択
void CLevelDesignEditor::CoordinateAxisSelect(void)
{
	//ImGui使用中なら無効
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse) {
		return;
	}

	//軸選択する状態なら入らない
	if (NO_AXIS_BE_SELECTED != m_AxisSelected) {
		return;
	}

	//拡大率設定
	D3DXVECTOR3 CameraPos = CCameraManager::GetActiveCameraPos();
	float Camera_SelectedModel_Dist = D3DXVec3Length(&(CameraPos - m_Coordinate.GetPos()));
	float Scl = Camera_SelectedModel_Dist / SCL_IS_ONE_BASE_DIST;				//カメラが遠いほど選択カーソルの大きさはでかい
	m_Coordinate.SetScl(D3DXVECTOR3(Scl, Scl, Scl));							//スケール設定

	//当り判定を取る
	D3DXVECTOR3 RayStartPoint = m_MouseEditMeshField.GetMouseRayStartPoint();		//レイの始点
	D3DXVECTOR3 RayEndPoint = m_MouseEditMeshField.GetMouseRayEndPoint();			//レイの終点
	float DistanceMin = D3DXVec3Length(&(RayEndPoint - RayStartPoint));			//最短距離

																				//マウスカーソルと軸が重ねているかの判定
	int MouseOnAxis = NO_AXIS_BE_SELECTED;
	for (int ObjNum = 0; ObjNum < 4; ObjNum++) {
		D3DXVECTOR3 HitPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	//当り判定位置
		bool bHit = RayPickToMesh::RaypicktoMesh(				//マウスレイとの当り判定
			false,
			&HitPosition,
			RayEndPoint,
			RayStartPoint,
			m_Coordinate.GetMesh(ObjNum),
			m_Coordinate.GetMatrix(ObjNum)
		);		//レイとメッシュの当り判定を取る

		if (bHit == false) continue;										//当たってないなら以下処理しない
		float Distance = D3DXVec3Length(&(HitPosition - RayStartPoint));	//レイ始点との距離を求め
		if (Distance < DistanceMin) {
			DistanceMin = Distance;			//最短距離更新
			MouseOnAxis = ObjNum;			//
		}
	}

	//マウス左クリックフラグ
	bool bMouseLeftClick = CSceneTransition::GetInputMouse()->GetMousePress(MOUSE_LEFT);

	if (MouseOnAxis != NO_AXIS_BE_SELECTED) {
		for (int ObjNum = 0; ObjNum < 4; ObjNum++) {
			if (ObjNum != MouseOnAxis) m_Coordinate.SetDrawType(ObjNum, CModel::DRAW_USE_MTRL_DEFALUT);	//描画タイプを元に戻して
		}

		if (bMouseLeftClick) {	//クリックした
			m_Coordinate.SetMaterial((CCoordinate::OBJ_NAME)MouseOnAxis, m_SelectAxisMtrl);					//マテリアル設定
			m_AxisSelected = MouseOnAxis;			//選択された軸に設定
		}
		else {
			m_Coordinate.SetMaterial((CCoordinate::OBJ_NAME)MouseOnAxis, m_MouseInAxisMtrl);				//マテリアル設定
		}
		m_Coordinate.SetDrawType((CCoordinate::OBJ_NAME)MouseOnAxis, CModel::DRAW_USE_MTRL_CUSTOMIZE);	//描画タイプの変更設定
	}
}

//軸選択後の動作
void CLevelDesignEditor::SelectedAxisAction(void)
{
	if (false == CSceneTransition::GetInputMouse()->GetMousePress(MOUSE_LEFT)) {	//クリックを押しっぱなししていない
		m_AxisSelected = NO_AXIS_BE_SELECTED;		//軸選択されていない状態に戻る
		return;
	}
	if (NO_AXIS_BE_SELECTED == m_AxisSelected) return;		//軸選択されていない

															//拡大率設定
	D3DXVECTOR3 CameraPos = CCameraManager::GetActiveCameraPos();
	D3DXVECTOR3 CameraModelVec = m_Coordinate.GetPos() - CameraPos;
	float Camera_SelectedModel_Dist = D3DXVec3Length(&CameraModelVec);
	float Scl = Camera_SelectedModel_Dist / SCL_IS_ONE_BASE_DIST;				//カメラが遠いほど選択カーソルの大きさはでかい
	m_Coordinate.SetScl(D3DXVECTOR3(Scl, Scl, Scl));							//スケール設定

																				//軸に応じた動作
	switch (m_AxisSelected) {
	case CCoordinate::OBJ_X_AXIS:
	{
		D3DXVECTOR3 CameraToLookat = CCameraManager::GetDirCameraToLookat(CCameraManager::GetActiveCameraName());
		D3DXVECTOR3 CrossVec;
		D3DXVECTOR3 CameraToLookatXZDir = D3DXVECTOR3(CameraToLookat.x, 0.0f, CameraToLookat.z);
		D3DXVec3Normalize(&CameraToLookatXZDir, &CameraToLookatXZDir);
		D3DXVec3Cross(&CrossVec, &CameraToLookatXZDir, &D3DXVECTOR3(1.0f, 0.0f, 0.0f));		//X軸とカメラ注視点のXZ平面のベクトルを求め
		DIMOUSESTATE *pMouseState = CSceneTransition::GetInputMouse()->GetMouseStates();	//マウスステート取得
		if (CrossVec.y >= 0) {		//XZ平面上、X軸はカメラ注視点のXZ平面のベクトルの右にある
			m_Coordinate.AddPosX(pMouseState->lX*AXIS_MOVE_COEFFICIENT*fabs(CrossVec.y));
		}
		else {
			m_Coordinate.AddPosX(-pMouseState->lX*AXIS_MOVE_COEFFICIENT*fabs(CrossVec.y));
		}
		break;
	}
	case CCoordinate::OBJ_Y_AXIS:
		break;
	case CCoordinate::OBJ_Z_AXIS:
	{
		D3DXVECTOR3 CameraToLookat = CCameraManager::GetDirCameraToLookat(CCameraManager::GetActiveCameraName());
		D3DXVECTOR3 CrossVec;
		D3DXVECTOR3 CameraToLookatXZDir = D3DXVECTOR3(CameraToLookat.x, 0.0f, CameraToLookat.z);
		D3DXVec3Normalize(&CameraToLookatXZDir, &CameraToLookatXZDir);
		D3DXVec3Cross(&CrossVec, &CameraToLookatXZDir, &D3DXVECTOR3(0.0f, 0.0f, 1.0f));		//Z軸とカメラ注視点のXZ平面のベクトルを求め
		DIMOUSESTATE *pMouseState = CSceneTransition::GetInputMouse()->GetMouseStates();	//マウスステート取得
		if (CrossVec.y >= 0) {		//XZ平面上、X軸はカメラ注視点のXZ平面のベクトルの右にある
			m_Coordinate.AddPosZ(pMouseState->lX*AXIS_MOVE_COEFFICIENT*fabs(CrossVec.y));
		}
		else {
			m_Coordinate.AddPosZ(-pMouseState->lX*AXIS_MOVE_COEFFICIENT*fabs(CrossVec.y));
		}
		break;
	}
	case CCoordinate::OBJ_BOX:
	{
		D3DXVECTOR3 MousePos = m_MouseEditMeshField.GetMousePosInMesh();
		m_Coordinate.SetPos(MousePos);

		break;
	}
	default:
		return;
		break;
	}
}

bool CLevelDesignEditor::SelectPathPoint(void)
{
	bool IsSelectPathPoint = false;

	//ImGui使用中なら無効
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse) {
		return false;
	}

	//ヌルチェック
	if (m_pEnemy == nullptr) {
		return false;
	}

	if (CSceneTransition::GetInputMouse()->GetMouseTrigger(MOUSE_LEFT) == false) {
		return false;
	}

	//巡回ポイント選択
	//新しいモデルを選択する前のクリア設定
	if (m_pPathPoint != nullptr) {
		m_pPathPoint->SetDrawType(CPathPoint::DRAW_USE_MTRL_DEFALUT);	//描画方式をデフォルトに戻る
		m_pPathPoint = nullptr;											//何も選択されていない状態に戻る
	}

	//オブジェクトの選択
	D3DXVECTOR3 RayStartPoint = m_MouseEditMeshField.GetMouseRayStartPoint();		//レイの始点
	D3DXVECTOR3 RayEndPoint = m_MouseEditMeshField.GetMouseRayEndPoint();			//レイの終点
	float DistanceMin = D3DXVec3Length(&(RayEndPoint - RayStartPoint));			//最短距離

	//全ての敵に対してマウスレイとの当り判定を行う
	auto it = m_pEnemy->GetPathPointListBegin();
	auto itEnd = m_pEnemy->GetPathPointListEnd();

	//0番目の巡回ポイントを除く,1番目以降の巡回ポイントは選択対象
	for (++it; it != itEnd; ++it) {
		//オブジェクトはヌルだったら
		if (*it == nullptr) {
			continue;
		}

		D3DXVECTOR3 HitPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	//当り判定位置
		bool bHit = RayPickToMesh::RaypicktoMesh(				//マウスレイとの当り判定
			false,
			&HitPosition,
			RayEndPoint,
			RayStartPoint,
			(*it)->GetMesh(),
			(*it)->GetWorldMtx()
		);

		//当たってないなら以下処理しない
		if (bHit == false) {
			continue;
		}

		//一番手前のオブジェクトを当る対象に設定
		float Distance = D3DXVec3Length(&(HitPosition - RayStartPoint));
		if (Distance < DistanceMin) {
			DistanceMin = Distance;			//最短距離更新
			m_pPathPoint = (*it);			//モデル代入
		}
	}

	if (m_pPathPoint != nullptr) {
		m_pPathPoint->SetDrawType(CPathPoint::DRAW_USE_MTRL_SELECTED);
		m_Coordinate.SetPos(m_pPathPoint->GetPos());
		m_Coordinate.SetDrawUse(true);
		IsSelectPathPoint = true;
		return true;
	}

	return IsSelectPathPoint;
}

void CLevelDesignEditor::MovePathPoint(void)
{
	//ヌルチェック
	if (m_pPathPoint == nullptr) {
		return;
	}

	D3DXVECTOR3 Dist = m_Coordinate.GetMoveDist();
	m_pPathPoint->AddPos(Dist);

	if (CSceneTransition::GetInputMouse()->GetMouseRelease(MOUSE_RIGHT)) {
		if (m_MouseRightButtonPushTime < HOLD_MOUSE_RIGHT_TIME) {
			m_Coordinate.SetDrawUse(false);
			m_pPathPoint->SetDrawType(CPathPoint::DRAW_USE_MTRL_DEFALUT);
			m_pPathPoint = nullptr;
		}
		m_MouseRightButtonPushTime = 0;
	}
}

void CLevelDesignEditor::SetPathPointStopFrame(void)
{
	//ヌルチェック
	if (m_pPathPoint == nullptr) {
		return;
	}

	int StopFrame = m_pPathPoint->GetStopFrame();
	bool IsInput = m_UI.SetIntputStopFrame(&StopFrame);

	if (IsInput == true) {
		m_pPathPoint->SetStopFrame(StopFrame);
	}
}

void CLevelDesignEditor::AddPathPoint(void)
{
	//ヌルチェック
	if (m_pEnemy == nullptr) {
		return;
	}

	if (m_UI.AddPathPointUI() == true) {
		int Size = m_pEnemy->GetPathPointListSize();
		auto it = m_pEnemy->GetPathPointListNumber(Size - 1);
		D3DXVECTOR3 Position = (*it)->GetPos();
		Position.x += 4.0f;
		m_pEnemy->AddPathPoint(Position, 0);
	}
}

bool CLevelDesignEditor::DeletePathPoint(void)
{
	//ヌルチェック
	if (m_pEnemy == nullptr) {
		return false;
	}

	bool IsDelete = (m_UI.DeletePathPointUI() == true) || CSceneTransition::GetInputKeyboard()->GetKeyTrigger(DIK_DELETE);

	if (IsDelete) {
		m_pEnemy->DeletePathPointUsePointer(m_pPathPoint);
		m_pPathPoint = nullptr;
		return true;
	}

	return false;
}

void CLevelDesignEditor::InsertPathPoint(void)
{
	//ヌルチェック
	if (m_pEnemy == nullptr) {
		return;
	}
	if (m_pPathPoint == nullptr) {
		return;
	}

	if (m_UI.InsertPathPointUI() == true) {
		D3DXVECTOR3 Position = m_pPathPoint->GetPos();
		Position.x += 4.0f;
		m_pEnemy->InsertPathPointUsePointer(Position, 0, m_pPathPoint);
	}
}