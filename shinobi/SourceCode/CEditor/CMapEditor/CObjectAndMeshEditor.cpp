#include "CObjectAndMeshEditor.h"
#include "CModelStorage.h"
#include "CScene3DStorage.h"
#include "CScene2DStorage.h"
#include "CCameraManager.h"
#include "CMeshFieldManager.h"
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
#include "CTextLoad.h"
#include "ModelManager.h"
#include "CTitle.h"
#include <iterator>
#include <imgui.h>
#include "ModelManager.h"
#include "MousePosition.h"

//座標移動カーソルのマクロ
#define SCL_IS_ONE_BASE_DIST (30.0f)			//座標移動カーソルののスケールは1.0の距離
#define NO_AXIS_BE_SELECTED (-1)
#define AXIS_MOVE_COEFFICIENT (0.02f)			//軸移動係数(使い方 : 軸移動係数 * マウスの相対移動量)
#define HOLD_MOUSE_RIGHT_TIME (100)				//右クリックを押し続ける判定用時間

CObjectAndMeshEditor::CObjectAndMeshEditor()
{
	m_EditMode = CObjectAndMeshEditorUI::EDIT_MESH;
	m_pCScene3D = nullptr;
	m_pModel = nullptr;
	m_bClickFlag = false;
	m_AxisSelected = NO_AXIS_BE_SELECTED;				//選択されている軸
	m_SetObjChildModeNum = SET_OBJ_MODE_NONE;
	m_MouseRightButtonFrameTime = timeGetTime();
	m_MouseRightButtonPushTime = 0;
	m_MoveObjsFlag = false;

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

	m_ModelList.clear();
}

CObjectAndMeshEditor::~CObjectAndMeshEditor()
{
	if (nullptr != m_pCScene3D) {
		m_pCScene3D->Uninit();
		delete m_pCScene3D;
		m_pCScene3D = nullptr;
	}
	if (nullptr != m_pModel) {
		m_pModel->Uninit();
		delete m_pModel;
		m_pModel = nullptr;
	}
	m_Coordinate.Uninit();
}

//初期化
bool CObjectAndMeshEditor::Init(HWND hWnd)
{
	hWnd;				//ダミー参照

	//クラスメンバ変数初期化
	m_EditMode = CObjectAndMeshEditorUI::EDIT_MESH;
	m_pCScene3D = nullptr;
	m_pModel = nullptr;
	m_SetObjChildModeNum = SET_OBJ_MODE_NONE;
	m_MouseRightButtonFrameTime = timeGetTime();
	m_MouseRightButtonPushTime = 0;
	m_MoveObjsFlag = false;

	//オブジェクト初期化
	CModelStorage::InitAll();

	//メッシュデータ初期化
	CMeshFieldManager::InitAll();

	//メッシュにあるマウス位置の初期化
	m_MouseEditMeshField.Init(5.0f, {100,0,0,200});

	//データ読み込み
	CMeshField* pMeshField = m_MeshDataIO.ImportMeshData("data/MESH_DATA/EDIT_MESH_SAVE.txt");
	m_MeshDataIO.SetMesh(pMeshField);

	m_Coordinate.Init(D3DXVECTOR3(0.0f,0.0f,0.0f));			//座標移動カーソル初期化
	
	InitAndCreateLight();									//ライト初期化

	//設置するモデルを初期化
	if (LoadModels("data/MAP_EDITOR/LoadModelFilePass.txt") == false) 
	{
		return false;
	}

	//UI初期化
	m_UI.Init();
	//UIのメニューにモデル設定
	for (auto it = m_ModelList.begin(); it != m_ModelList.end(); ++it) {
		m_UI.DrawOnTexture( (*it) );
	}

	m_ModelDataIO.ImportObj3DData("data/3D_OBJ_DATA/3D_OBJ_SAVE_DATA.txt");

	m_SelectMoreObjMode.Init();

	return true;
}

//終了処理
void CObjectAndMeshEditor::Uninit(void)
{
	//オブジェクト解放
	CScene2DStorage::UninitAll();
	CScene3DStorage::UninitAll();
	CModelStorage::UninitAll();

	//メッシュデータ解放
	CMeshFieldManager::UninitAll();

	//メッシュにあるマウス位置の解放
	m_MouseEditMeshField.Uninit();

	m_Coordinate.Uninit();			//座標移動カーソル解放

	m_UI.Uninit();

	m_SelectMoreObjMode.Uninit();
}

//更新処理
void CObjectAndMeshEditor::Update(void)
{
	m_UI.Update();		//UIの更新
	UpdateEdit();		//エディットの更新

	//右クリックのタイマー関連
	UpdateRightButtonClickTimer();

	CScene3DStorage::UpdateAll();
	CScene2DStorage::UpdateAll();
	CModelStorage::UpdateAll();

	CMeshFieldManager::UpdateAll();					//メッシュアップデート
	CAllObj3DHitMesh::ColliAllObj3DAndAllMesh();	//3Dオブジェとメッシュの当り判定

	m_Coordinate.Update();							//座標カーソル更新

	SaveOrLoad();									//データの書き出しと読み込み処理

	MeshCreate();									//メッシュを作る
}

//エディター部分の更新
void CObjectAndMeshEditor::UpdateEdit(void)
{
	DetectModeChange();						//編集モード変更
	ActionInEditMode();						//モード具体の動作

	if (nullptr != m_pCScene3D){
		CAllObj3DHitMesh::CScene3DHitAllMesh(m_pCScene3D);
		m_pCScene3D->Update();
	}
	if (nullptr != m_pModel) {
		CAllObj3DHitMesh::ModelHitAllMesh(m_pModel);
		m_pModel->Update();
	}
}

void CObjectAndMeshEditor::UpdateRightButtonClickTimer(void)
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

//描画
void CObjectAndMeshEditor::Draw(void)
{
	CMeshFieldManager::DrawAll();		//メッシュ描画

	m_Coordinate.Draw();			//座標移動カーソル描画

	//3Dオブジェクト描画
	switch (m_EditMode) {
	case CObjectAndMeshEditorUI::EDIT_MESH:
		CModelStorage::DrawAll();
		break;
	case CObjectAndMeshEditorUI::SET_OBJ:
		CModelStorage::DrawAll();

		//エディットオブジェクトの描画
		if (nullptr != m_pModel && m_SetObjChildModeNum == SET_OBJECT)
		{
			m_pModel->SetMtxSetting();
			m_pModel->Draw();
		}
		break;
	case CObjectAndMeshEditorUI::MOVE_OBJ:
	{
		int Amount = m_SelectMoreObjMode.GetSelectObjAmount();
		int GetNumber = 0;

		for (int i = 0; i < MODEL_OBJ_MAX; i++) {
			//モデル取得
			CModel* pModel = CModelStorage::GetObj(i);
			if (pModel == nullptr) {
				continue;
			}

			//選択されたら、選択マテリアルで描画
			if (i == m_SelectMoreObjMode.GetSelectObjNum(GetNumber) && Amount != 0) {
				Amount--;
				GetNumber++;
				pModel->SetMtxSetting();
				pModel->DrawUseMtrl(m_SelectObjMtrl);
			}
			else {
				pModel->SetMtxSetting();
				pModel->DrawDefault();
			}
		}
		break;
	}
	}

	if (m_EditMode == CObjectAndMeshEditorUI::EDIT_MESH) {
		m_MouseEditMeshField.Draw();			//メッシュにあるマウス位置の描画
	}

	if (nullptr != m_pCScene3D) {
		m_pCScene3D->Draw();
	}

	m_SelectMoreObjMode.Draw();
}

//ライト作る
void CObjectAndMeshEditor::InitAndCreateLight(void)
{
	D3DLIGHT9 Light;
	ZeroMemory(&Light, sizeof(D3DLIGHT9));						//ライト初期化
	Light.Type = D3DLIGHT_DIRECTIONAL;							//ライトのタイプを指定
	Light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);			//ディレクショナルライトの色
	Light.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);			//アンビエントライト
	D3DXVECTOR3 vecDir(1.0f, -1.0f, 1.0f);						//ライト方向
	D3DXVec3Normalize((D3DXVECTOR3*)&Light.Direction, &vecDir);	//ライト方向を設定したライトに入れる
	CLightManager::CreateLight(Light);
}

//モデルのテキストからモデルを読み込んで初期化する
bool CObjectAndMeshEditor::LoadModels(const char* ModelTextPass)
{
	//モデルリストのデータをクリア
	if (m_ModelList.size() != 0) 
	{
		for (auto it = m_ModelList.begin(); it != m_ModelList.end(); ++it) {
			if ((*it) != nullptr) {
				(*it)->Uninit();
				delete (*it);
			}
		}
		m_ModelList.clear();
	}

	//モデル読み込んで初期化する
	FILE *fp;
	fp = fopen(ModelTextPass, "r");

	if (fp == nullptr)
	{
		MessageBox(nullptr, "モデルデータのテキストが存在しない", "[CObjectAndMeshEditor][LoadModels]", MB_OK | MB_ICONHAND);
		fclose(fp);
		return false;
	}

	char buf[1024];		//文字バッファ
	while (CTextLoad::GetStrToken(fp, TOKEN, buf) != -1) 
	{
		if (strcmp("MODEL_FILENAME=", buf) != 0) {
			continue;
		}

		//モデルのファイルパスを読み込む
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		char ModelPass[1024];
		strcpy_s(ModelPass, _countof(ModelPass), buf);

		//モデルを作る
		CModel *pModel = new CModel;
		if (pModel != nullptr) {
			SRP srp;
			srp.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			srp.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			srp.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
			pModel->Init(srp, ModelPass);

			//モデルをリストに追加
			m_ModelList.push_back(pModel);
		}
	}

	fclose(fp);

	return true;
}

//モードの変更を検知
void CObjectAndMeshEditor::DetectModeChange(void)
{
	CObjectAndMeshEditorUI::MODE modeNow = m_UI.GetMode();

	//前と同じだったら
	if (modeNow == m_EditMode) {
		return;
	}

	//モード遷移前の片付
	switch (m_EditMode)
	{
	case CObjectAndMeshEditorUI::EDIT_MESH:
		break;
	case CObjectAndMeshEditorUI::SET_OBJ:
		if (m_pModel != nullptr) {
			m_pModel->SetDrawType(CModel::DRAW_USE_MTRL_DEFALUT);		//描画方式をデフォルトに戻る
			m_pModel = nullptr;											//何も選択されていない状態に戻る
		}
		m_Coordinate.SetDrawUse(false);
		break;
	case CObjectAndMeshEditorUI::MOVE_OBJ:
		m_SelectMoreObjMode.ResetSelectObjAmount();
		break;
	}

	//遷移後の初期化
	switch(modeNow){
	case CObjectAndMeshEditorUI::EDIT_MESH:
		m_pModel = nullptr;
		break;
	case CObjectAndMeshEditorUI::SET_OBJ:
		break;
	case CObjectAndMeshEditorUI::MOVE_OBJ:
		m_MoveObjsFlag = false;
		break;
	}

	m_EditMode = modeNow;		//遷移後モード代入
}

//モデル回転
void CObjectAndMeshEditor::ModelRotate(void)
{
	if (nullptr == m_pModel) return;
	if (CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_A)) {
		m_pModel->AddRotateY(-0.1f);
	}
	if (CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_D)) {
		m_pModel->AddRotateY(0.1f);
	}
}

//エディターの動作
void CObjectAndMeshEditor::ActionInEditMode(void)
{
	//クリックフラグを未設定状態にする
	m_bClickFlag = false;

	//フィールド上マウスの位置計算
	m_MouseEditMeshField.CalcMousePositionInMesh();

	D3DXVECTOR3 CoordinatePos = m_Coordinate.GetPos();
	float CoordinatePosY = CMeshFieldManager::CalcPosYValueInMeshField(CoordinatePos);
	CoordinatePos.y = CoordinatePosY;
	m_Coordinate.SetPos(CoordinatePos);

	//オブジェクトクリエイト
	switch (m_EditMode) {
	case CObjectAndMeshEditorUI::EDIT_MESH:
		m_MouseEditMeshField.EditHitFloor();
		break;
	case CObjectAndMeshEditorUI::SET_OBJ:
	{
		if (m_UI.GetResetAllModel() == true) {
			m_pModel = nullptr;
			CModelStorage::UninitAll();
		}

		static bool IsMouseLeftButtonHold = false;			//左ボタンをしたままなのか

		if (nullptr == m_pModel) {
			//子モード初期化
			m_SetObjChildModeNum = SET_OBJ_MODE_NONE;

			//新しいオブジェクトを作るなら
			if (SelectObject() == true) {
				m_SetObjChildModeNum = SET_OBJECT;
				break;
			}

			//フィールド上のオブジェクトを選択
			if (SelectModelObjOnFiled() == true) {
				m_Coordinate.SetDrawUse(true);
				m_SetObjChildModeNum = SELECT_OBJECT;
				IsMouseLeftButtonHold = true;
				break;
			}
		}

		else {
			m_pModel->SetPosition(m_Coordinate.GetPos());

			if (m_SetObjChildModeNum == SET_OBJECT) {
				SetObject();
			}
			else if (m_SetObjChildModeNum == SELECT_OBJECT) {
				if (CSceneTransition::GetInputMouse()->GetMouseRelease(MOUSE_LEFT)) {
					IsMouseLeftButtonHold = false;
				}
				if (IsMouseLeftButtonHold == true) {
					break;
				}

				CoordinateAxisSelect();				//座標移動カーソルの動作
				SelectedAxisAction();				//軸が選択されている状態の具体動作

				if (CSceneTransition::GetInputMouse()->GetMouseRelease(MOUSE_RIGHT)) {
					if (m_MouseRightButtonPushTime <= HOLD_MOUSE_RIGHT_TIME) {
						m_pModel->SetDrawType(CModel::DRAW_USE_MTRL_DEFALUT);
						m_pModel = nullptr;
						m_Coordinate.SetDrawUse(false);
					}
					m_MouseRightButtonPushTime = 0;
				}

				//オブジェクトの削除動作
				if (CSceneTransition::GetInputKeyboard()->GetKeyTrigger(DIK_DELETE)) {
					CModelStorage::Delete(m_pModel->GetUseID());
					m_pModel = nullptr;
					m_Coordinate.SetDrawUse(false);
				}
			}
		}
		break;
	}
	case CObjectAndMeshEditorUI::MOVE_OBJ:
	{
		//オブジェクトの移動モードなのか
		if (m_UI.GetMoveSelectObjsFlag() == false) {
			//フラグ管理
			m_MoveObjsFlag = false;
			m_Coordinate.SetDrawUse(false);

			//全てのオブジェクト削除
			if (CSceneTransition::GetInputKeyboard()->GetKeyTrigger(DIK_DELETE)) {
				DeleteObjs();
			}

			//画面範囲選択操作
			ImGuiIO& io = ImGui::GetIO();
			if (m_SelectMoreObjMode.GetUsingFlag() == false) {
				if (io.WantCaptureMouse == false) {
					m_SelectMoreObjMode.SelectObjs();
				}
			}
			else {
				m_SelectMoreObjMode.SelectObjs();
			}
		}

		else {
			MoveObjs();
			CoordinateAxisSelect();
			SelectedAxisAction();
		}
		
		break;
	}
	}
}

//フィールド上のオブジェクトを選択
bool CObjectAndMeshEditor::SelectModelObjOnFiled(void)
{
	if (false == CSceneTransition::GetInputMouse()->GetMouseTrigger(MOUSE_LEFT)) {
		return false;
	}

	//ImGui使用中は無効
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse) {
		return false;
	}

	//新しいモデルを選択する前のクリア設定
	if (m_pModel != nullptr) {
		m_pModel->SetDrawType(CModel::DRAW_USE_MTRL_DEFALUT);		//描画方式をデフォルトに戻る
		m_pModel = nullptr;											//何も選択されていない状態に戻る
	}

	//オブジェクトの選択
	D3DXVECTOR3 RayStartPoint = m_MouseEditMeshField.GetMouseRayStartPoint();		//レイの始点
	D3DXVECTOR3 RayEndPoint = m_MouseEditMeshField.GetMouseRayEndPoint();			//レイの終点
	float DistanceMin = D3DXVec3Length(&(RayEndPoint - RayStartPoint));			//最短距離
	
	for (int ObjNum = 0; ObjNum < MODEL_OBJ_MAX; ObjNum++) {
		CModel *pModel = CModelStorage::GetObj(ObjNum);
		if (pModel == nullptr) continue;							//オブジェクトはヌルだったら
		D3DXVECTOR3 HitPosition = D3DXVECTOR3(0.0f,0.0f,0.0f);	//当り判定位置
		bool bHit = RayPickToMesh::RaypicktoMesh(				//マウスレイとの当り判定
			false,
			&HitPosition,
			RayEndPoint,
			RayStartPoint,
			pModel->GetMesh(),
			pModel->GetResultMtx()
		);		//レイとメッシュの当り判定を取る

		if (bHit == false) continue;							//当たってないなら以下処理しない
		float Distance = D3DXVec3Length(&(HitPosition - RayStartPoint));	//レイ始点との距離を求め
		if (Distance < DistanceMin) {
			DistanceMin = Distance;			//最短距離更新
			m_pModel = pModel;				//モデル代入
		}
	}

	if (m_pModel != nullptr) {
		m_pModel->SetDrawType(CModel::DRAW_USE_MTRL_CUSTOMIZE);
		m_pModel->SetMaterial(m_SelectObjMtrl);
		m_Coordinate.SetPos(m_pModel->GetPosition());
		return true;
	}

	return false;
}

bool CObjectAndMeshEditor::SelectObject(void)
{
	int SelectedModelNum = m_UI.GetPushButtonNumber();		//選択されたモデル番号取得
	if (SelectedModelNum == NO_PUSH_ANY_IMAGE_BUTTON) {		//未選択状態なら終了
		return false;
	}

	auto it = m_ModelList.begin();
	m_pModel = (CModel*)(*std::next(it, SelectedModelNum));

	return true;
}

//オブジェクト選択
void CObjectAndMeshEditor::SetObject(void)
{
	//ヌルチェック
	if (m_pModel == nullptr) {
		return;
	}

	//UIの部分はマウス使っているか
	if (m_UI.GetMouseUsing() == true) {
		return;
	}
	
	//メッシュ上のマウスの位置を取得
	D3DXVECTOR3 MousePos = m_MouseEditMeshField.GetMousePosInMesh();
	if (m_UI.GetLockFlagX() == true) {
		int XValue = (int)MousePos.x;
		MousePos.x = (float)XValue / m_UI.GetLockDistanceX() * m_UI.GetLockDistanceX();
	}
	if (m_UI.GetLockFlagY() == true) {
		int ZValue = (int)MousePos.z;
		MousePos.z = (float)ZValue / m_UI.GetLockDistanceY() * m_UI.GetLockDistanceY();
	}

	m_pModel->SetPosition(MousePos);

	if(CSceneTransition::GetInputMouse()->GetMouseRelease(MOUSE_LEFT)){
		SRP Srp;
		Srp.pos = m_pModel->GetPosition();
		Srp.rot = m_pModel->GetRot();
		Srp.scl = D3DXVECTOR3(1.0f,1.0f,1.0f);
		CModelStorage::CreateObject(Srp, CModelManager::GetModelPass(m_pModel->GetModelID()) );
	}

	if (CSceneTransition::GetInputMouse()->GetMouseRelease(MOUSE_RIGHT)) {
		if (m_MouseRightButtonPushTime <= HOLD_MOUSE_RIGHT_TIME) {
			m_pModel = nullptr;
		}
		m_MouseRightButtonPushTime = 0;
	}
}

void CObjectAndMeshEditor::CoordinateAxisSelect(void)
{
	if (m_bClickFlag) return;
	if (NO_AXIS_BE_SELECTED != m_AxisSelected) return;

	//拡大率設定
	D3DXVECTOR3 CameraPos = CCameraManager::GetActiveCameraPos();
	float Camera_SelectedModel_Dist = D3DXVec3Length( &(CameraPos - m_Coordinate.GetPos()) );
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
			if(ObjNum != MouseOnAxis) m_Coordinate.SetDrawType(ObjNum, CModel::DRAW_USE_MTRL_DEFALUT);	//描画タイプを元に戻して
		}

		if (bMouseLeftClick) {	//クリックした
			m_Coordinate.SetMaterial((CCoordinate::OBJ_NAME)MouseOnAxis, m_SelectAxisMtrl);					//マテリアル設定
			m_AxisSelected = MouseOnAxis;			//選択された軸に設定
			m_bClickFlag = true;					//マウスすでにクリックした
		}
		else {
			m_Coordinate.SetMaterial((CCoordinate::OBJ_NAME)MouseOnAxis, m_MouseInAxisMtrl);				//マテリアル設定
		}
		m_Coordinate.SetDrawType((CCoordinate::OBJ_NAME)MouseOnAxis, CModel::DRAW_USE_MTRL_CUSTOMIZE);	//描画タイプの変更設定
	}
}

//軸を選択
void CObjectAndMeshEditor::SelectedAxisAction(void)
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
		D3DXVec3Normalize(&CameraToLookatXZDir,&CameraToLookatXZDir);
		D3DXVec3Cross(&CrossVec, &CameraToLookatXZDir, &D3DXVECTOR3(1.0f, 0.0f, 0.0f));		//X軸とカメラ注視点のXZ平面のベクトルを求め
		DIMOUSESTATE *pMouseState = CSceneTransition::GetInputMouse()->GetMouseStates();	//マウスステート取得
		if (CrossVec.y >= 0) {		//XZ平面上、X軸はカメラ注視点のXZ平面のベクトルの右にある
			m_Coordinate.AddPosX(pMouseState->lX*AXIS_MOVE_COEFFICIENT*fabs(CrossVec.y) );
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
		if (m_UI.GetLockFlagX() == true) {
			int XValue = (int)MousePos.x;
			MousePos.x = (float)XValue / m_UI.GetLockDistanceX() * m_UI.GetLockDistanceX();
		}
		if (m_UI.GetLockFlagY() == true) {
			int ZValue = (int)MousePos.z;
			MousePos.z = (float)ZValue / m_UI.GetLockDistanceY() * m_UI.GetLockDistanceY();
		}
		m_Coordinate.SetPos(MousePos);

		break;
	}
	default:
		return;
		break;
	}
}

void CObjectAndMeshEditor::DeleteObjs(void)
{
	if (m_SelectMoreObjMode.GetSelectObjAmount() <= 0) {
		return;
	}

	for (int i = 0; i < m_SelectMoreObjMode.GetSelectObjAmount(); i++) {
		int Number = m_SelectMoreObjMode.GetSelectObjNum(i);
		CModel* pModel = CModelStorage::GetObj(Number);
		CModelStorage::Delete(pModel->GetUseID());
	}
}

void CObjectAndMeshEditor::MoveObjs(void)
{
	//選択されたオブジェクトはいない
	int SelectObjsAmount = m_SelectMoreObjMode.GetSelectObjAmount();
	if (SelectObjsAmount <= 0) {
		return;
	}

	//初めてモードに入る時の処理
	if (m_MoveObjsFlag == false && m_UI.GetMoveSelectObjsFlag() == true) {
		m_MoveObjsFlag = true;

		//選択されたオブジェクトたちの中心座標を求める
		D3DXVECTOR3 CenterPos = D3DXVECTOR3(0,0,0);
		for (int i = 0; i < SelectObjsAmount; i++) {
			int Number = m_SelectMoreObjMode.GetSelectObjNum(i);
			CModel* pModel = CModelStorage::GetObj(Number);
			if (pModel != nullptr) {
				CenterPos += pModel->GetPosition();
			}
		}
		CenterPos = CenterPos / (float)SelectObjsAmount;

		m_Coordinate.SetPos(CenterPos);
		m_Coordinate.SetDrawUse(true);
	}

	//オブジェクトの移動処理
	D3DXVECTOR3 MoveDist = m_Coordinate.GetMoveDist();
	for (int i = 0; i < SelectObjsAmount; i++) {
		int Number = m_SelectMoreObjMode.GetSelectObjNum(i);
		CModel* pModel = CModelStorage::GetObj(Number);
		if (pModel != nullptr) {
			pModel->MoveX(MoveDist.x);
			pModel->MoveZ(MoveDist.z);
		}
	}
}

void CObjectAndMeshEditor::ShowCoordinateUpdate(void)
{

}

//セーブとロード
void CObjectAndMeshEditor::SaveOrLoad(void)
{
	if (m_UI.GetSaveMeshFlag()) {
		m_MeshDataIO.ExportMeshData();
		MessageBox(nullptr,"メッシュデータを保存しました","",MB_OK);
	}
	if (m_UI.GetSaveModelFlag()) {
		m_ModelDataIO.ExportObj3DData();
		MessageBox(nullptr, "モデルデータを保存しました", "", MB_OK);
	}
}

void CObjectAndMeshEditor::MeshCreate(void)
{
	D3DXVECTOR2 FieldSize(0, 0);
	D3DXVECTOR2 MasuSize(0, 0);

	bool IsCreateMesh = m_UI.GetCreateMeshData(&FieldSize, &MasuSize);
	if (IsCreateMesh == false) {
		return;
	}

	//古いメッシュ情報削除
	CMeshFieldManager::UninitAll();

	int VertexNum = ((int)FieldSize.x + 1) * ((int)FieldSize.y + 1);		//頂点数を計算
	double *pMeshRelativeHeight = new double[VertexNum];		//標高ポインタ
	
	for (int y = 0; y <= FieldSize.y; y++){
		for (int x = 0; x <= FieldSize.x; x++) {
			int k = y * ((int)FieldSize.x + 1) + x;
			pMeshRelativeHeight[k] = 0.0f;
		}
	}

	CMeshField *pMesh = CMeshFieldManager::CreateMeshField(D3DXVECTOR3(0,0,0), CINDEX_SIZE(FieldSize, MasuSize), FLOOR001_TEX);
	pMesh->LoadRelativeHigh(pMeshRelativeHeight);
	m_MeshDataIO.SetMesh(pMesh);

	//解放
	delete[] pMeshRelativeHeight;
}