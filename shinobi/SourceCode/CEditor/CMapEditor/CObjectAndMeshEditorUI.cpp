#include "CObjectAndMeshEditorUI.h"
#include "imgui.h"
#include "CRenderer.h"
#include "CCameraManager.h"
#include "CLightManager.h"
#include "SceneTransition.h"
#include <stdio.h>

/*==============================================================================
	マクロ
===============================================================================*/
#define UI_MAXOBJ_ONELINE (4)		//UI一行の最大表示オブジェクト数
#define IMAGE_BUTTON_SIZE (128)

/*==============================================================================
	コンストラクタ
===============================================================================*/
CObjectAndMeshEditorUI::CObjectAndMeshEditorUI() 
{
	for(int i = 0; i < MODEL_MENU_OBJ_MAX;i++){
		m_RenderTargetPointerArray[i].TexturePointer = nullptr;
		m_RenderTargetPointerArray[i].SurfacePointer = nullptr;
	}

	m_Mode = EDIT_MESH;
	m_PushButtonNumber = NO_PUSH_ANY_IMAGE_BUTTON;
	m_IsSaveMeshData = false;
	m_IsSaveModelData = false;

	m_MeshMasuSize = D3DXVECTOR2(1,1);
	m_MeshFieldSize = D3DXVECTOR2(1, 1);

	m_LockDistanceX = 0;
	m_LockDistanceY = 0;
	m_LockFlagXAxis = false;
	m_LockFlagYAxis = false;
	m_ResetAllObj = false;
}

/*==============================================================================
	初期化処理
===============================================================================*/
bool CObjectAndMeshEditorUI::Init(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	//リストがクリアしていないなら
	for (int i = 0; i < MODEL_MENU_OBJ_MAX; i++) {
		//ポインタクリア
		if (m_RenderTargetPointerArray[i].TexturePointer != nullptr) {
			m_RenderTargetPointerArray[i].TexturePointer->Release();
			m_RenderTargetPointerArray[i].TexturePointer = nullptr;
		}

		if (m_RenderTargetPointerArray[i].SurfacePointer != nullptr) {
			m_RenderTargetPointerArray[i].SurfacePointer->Release();
			m_RenderTargetPointerArray[i].SurfacePointer = nullptr;
		}

		if (m_RenderTargetPointerArray[i].DepthBufferPointer != nullptr) {
			m_RenderTargetPointerArray[i].DepthBufferPointer->Release();
			m_RenderTargetPointerArray[i].DepthBufferPointer = nullptr;
		}
		
		//空きテクスチャ生成
		HRESULT hr;
		hr = D3DXCreateTexture(pDevice, IMAGE_BUTTON_SIZE, IMAGE_BUTTON_SIZE,
			1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
			&m_RenderTargetPointerArray[i].TexturePointer
		);
		if (hr == E_FAIL)
		{
			MessageBox(nullptr, "レンダーターゲットのテクスチャ生成失敗", "[CObjectAndMeshEditorUI.cpp][Init]", MB_ICONHAND);
			return false;
		}

		//サーフェースポインタを取得
		m_RenderTargetPointerArray[i].TexturePointer->GetSurfaceLevel(0, &m_RenderTargetPointerArray[i].SurfacePointer);

		//空きZバッファ生成
		pDevice->CreateDepthStencilSurface(IMAGE_BUTTON_SIZE, IMAGE_BUTTON_SIZE, D3DFMT_D24S8, D3DMULTISAMPLE_NONE,
			0, FALSE, &m_RenderTargetPointerArray[i].DepthBufferPointer, nullptr);

		//未使用状態にする
		m_RenderTargetPointerArray[i].bUse = false;
	}

	m_Mode = EDIT_MESH;
	m_PushButtonNumber = NO_PUSH_ANY_IMAGE_BUTTON;
	m_IsSaveMeshData = false;
	m_IsSaveModelData = false;
	m_ResetAllObj = false;

	m_MeshMasuSize = D3DXVECTOR2(1, 1);
	m_MeshFieldSize = D3DXVECTOR2(1, 1);

	return true;
}

/*==============================================================================
	更新処理
===============================================================================*/
void CObjectAndMeshEditorUI::Update(void)
{
	ChangeModeUI();			//モードの変更のUI

	switch (m_Mode) {
	case EDIT_MESH:
		CreateMeshUI();
		break;
	case SET_OBJ:
		ModelImageButtonUI();
		LockDistanceUI();
		ResetAllObjUI();
		break;
	case MOVE_OBJ:
		MoveObjsCheckBoxUI();
		break;
	}

	m_IsSaveMeshData = SaveMeshUI();	//メッシュデータ保存
	m_IsSaveModelData = SaveModelUI();	//モデルデータ保存
	
}

/*==============================================================================
	終了処理
===============================================================================*/
void CObjectAndMeshEditorUI::Uninit(void)
{
	for (int i = 0; i < MODEL_MENU_OBJ_MAX; i++) {
		if (m_RenderTargetPointerArray[i].TexturePointer != nullptr) {
			m_RenderTargetPointerArray[i].TexturePointer->Release();
			m_RenderTargetPointerArray[i].TexturePointer = nullptr;
		}

		if (m_RenderTargetPointerArray[i].SurfacePointer != nullptr) {
			m_RenderTargetPointerArray[i].SurfacePointer->Release();
			m_RenderTargetPointerArray[i].SurfacePointer = nullptr;
		}

		if (m_RenderTargetPointerArray[i].DepthBufferPointer != nullptr) {
			m_RenderTargetPointerArray[i].DepthBufferPointer->Release();
			m_RenderTargetPointerArray[i].DepthBufferPointer = nullptr;
		}
	}
}

/*==============================================================================
	押したボタン番号を取得し、番号をリセット
===============================================================================*/
int CObjectAndMeshEditorUI::GetPushButtonNumber(void)
{
	int Number = m_PushButtonNumber;
	m_PushButtonNumber = NO_PUSH_ANY_IMAGE_BUTTON;
	return Number;
}

bool CObjectAndMeshEditorUI::GetResetAllModel(void)
{
	bool Flag = m_ResetAllObj;
	m_ResetAllObj = false;
	return Flag;
}

void CObjectAndMeshEditorUI::DrawOnTexture(CModel* pModel)
{
	//ヌルチェック
	if (pModel == nullptr) {
		return;
	}

	//未使用のレンダーターゲットにモデルを描画する
	for (int Count = 0; Count < MODEL_MENU_OBJ_MAX; Count++) 
	{
		//ポインタヌルチェック
		bool IsPointerNoNull =
			m_RenderTargetPointerArray[Count].TexturePointer != nullptr &&
			m_RenderTargetPointerArray[Count].SurfacePointer != nullptr &&
			m_RenderTargetPointerArray[Count].DepthBufferPointer != nullptr;

		//レンダーターゲットする
		LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();
		if (IsPointerNoNull && (m_RenderTargetPointerArray[Count].bUse == false) )
		{
			//描画平面とZバッファの設定
			pDevice->SetRenderTarget(0, m_RenderTargetPointerArray[Count].SurfacePointer); 
			pDevice->SetDepthStencilSurface(m_RenderTargetPointerArray[Count].DepthBufferPointer);

			//ビュー変換行列設定(モデルのAABBとの判定結果でカメラの位置と注視点の位置を決める)
			D3DXVECTOR2 MinXMaxX, MinYMaxY, MinZMaxZ;
			pModel->GetAABBValue(&MinXMaxX, &MinYMaxY, &MinZMaxZ);

			//カメラ位置を求める
			D3DXVECTOR3 Camera;
			Camera.x = (MinXMaxX.x + MinXMaxX.y) * 0.5f;
			Camera.y = (MinYMaxY.x + MinYMaxY.y) * 0.5f;
			float HalfHeight = (MinYMaxY.y - MinYMaxY.x) * 0.5f;
			Camera.z = fabsf(HalfHeight / tanf(D3DX_PI / 3.0f * 0.5f) ) + MinZMaxZ.y;

			//注視点位置を求める
			D3DXVECTOR3 Lookat;
			Lookat.x = (MinXMaxX.x + MinXMaxX.y) * 0.5f;
			Lookat.y = (MinYMaxY.x + MinYMaxY.y) * 0.5f;
			Lookat.z = (MinZMaxZ.x + MinZMaxZ.y) * 0.5f;

			D3DXMATRIX ViewMtx;
			D3DXMatrixLookAtLH(&ViewMtx, &Camera, &Lookat, &D3DXVECTOR3(0.0f,1.0f,0.0f));
			pDevice->SetTransform(D3DTS_VIEW, &ViewMtx);

			//プロジェクション行列設定(アスペクト比は1:1)
			D3DXMATRIX ProjeMtx;
			D3DXMatrixPerspectiveFovLH(&ProjeMtx, D3DX_PI / 3.0f, (float)IMAGE_BUTTON_SIZE / IMAGE_BUTTON_SIZE, 1.0f, 1000.0f);
			pDevice->SetTransform(D3DTS_PROJECTION, &ProjeMtx);      //プロジェクション変換行列
			
			//ライトの設定
			D3DLIGHT9 Light;
			ZeroMemory(&Light, sizeof(D3DLIGHT9));						//ライト初期化
			Light.Type = D3DLIGHT_DIRECTIONAL;							//ライトのタイプを指定
			Light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);			//ディレクショナルライトの色
			Light.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);			//アンビエントライト
			D3DXVECTOR3 vecDir(0.0f, 0.0f, -1.0f);						//ライト方向
			D3DXVec3Normalize((D3DXVECTOR3*)&Light.Direction, &vecDir);	//ライト方向を設定したライトに入れる
			pDevice->SetLight(0, &Light);											//デバイスのライトスロット0番に格納
			pDevice->LightEnable(0, TRUE);											//ライトスロット0番ライト起用
			pDevice->SetRenderState(D3DRS_AMBIENT, (D3DXCOLOR)Light.Ambient);		// アンビエントライト設定

			//モデル描画
			CRenderer::DrawClear();
			CRenderer::DrawBegin();
			pModel->SetMtxSetting();
			pModel->Draw();
			CRenderer::DrawEnd();

			//バックバッファの描画平面とZバッファに戻す
			pDevice->SetRenderTarget(0, CRenderer::GetBackBufferSurface());
			pDevice->SetDepthStencilSurface(CRenderer::GetBackDepthSurface());

			//使用状態にする
			m_RenderTargetPointerArray[Count].bUse = true;

			//描画用カメラのビューマトリクスに戻す
			CCameraManager::ActiveCamera_ViewProMtxSet();
			CLightManager::UseLight(0);
			break;
		}
	}
}

/*==============================================================================
	マウスの使用状況確認
===============================================================================*/
bool CObjectAndMeshEditorUI::GetMouseUsing(void)
{
	ImGuiIO& io = ImGui::GetIO();
	return io.WantCaptureMouse;
}

/*==============================================================================
新しいメッシュを作るためのデータ取得
===============================================================================*/
bool CObjectAndMeshEditorUI::GetCreateMeshData(D3DXVECTOR2* pFieldSize, D3DXVECTOR2* pMasuSize)
{
	if (m_IsCreateMesh == false) {
		return false;
	}

	*pFieldSize = m_MeshFieldSize;
	*pMasuSize = m_MeshMasuSize;

	return true;
}

/*==============================================================================
	ChangeModeUI
===============================================================================*/
void CObjectAndMeshEditorUI::ChangeModeUI(void)
{
	int EditModeNow = (int)m_Mode;

	ImGui::Begin("MainMenu");

	ImGui::RadioButton("EDIT_MESH", &EditModeNow, (int)EDIT_MESH); ImGui::SameLine();
	ImGui::RadioButton("SET_OBJ", &EditModeNow, (int)SET_OBJ); ImGui::SameLine();
	ImGui::RadioButton("MOVE_OBJ", &EditModeNow, (int)MOVE_OBJ);

	ImGui::End();

	//モード変化があったら
	if (EditModeNow != (int)m_Mode)
	{
		m_Mode = (MODE)EditModeNow;
		return;
	}
}

/*==============================================================================
	モデルリストのイメージUI
===============================================================================*/
void CObjectAndMeshEditorUI::ModelImageButtonUI(void)
{
	ImGui::Begin("MainMenu");

	ImGui::BeginChild(ImGui::GetID((void*)0),
		ImVec2(IMAGE_BUTTON_SIZE * UI_MAXOBJ_ONELINE * 1.2f, IMAGE_BUTTON_SIZE * 3),
		ImGuiWindowFlags_NoTitleBar);

	for (int Count = 0; Count < MODEL_MENU_OBJ_MAX; Count++) {
		//同じ行なのかを判定
		if (Count % UI_MAXOBJ_ONELINE != 0)
		{
			ImGui::SameLine();
		}

		//ポインタヌルチェック
		bool IsPointerNoNull = 
			m_RenderTargetPointerArray[Count].TexturePointer != nullptr &&
			m_RenderTargetPointerArray[Count].SurfacePointer != nullptr &&
			m_RenderTargetPointerArray[Count].DepthBufferPointer != nullptr;
		if (IsPointerNoNull && (m_RenderTargetPointerArray[Count].bUse == true)) {
			//GUIイメージボタン
			if (ImGui::ImageButton((void*)m_RenderTargetPointerArray[Count].TexturePointer, ImVec2(IMAGE_BUTTON_SIZE, IMAGE_BUTTON_SIZE))) {
				m_PushButtonNumber = Count;
			}
		}
	}

	ImGui::EndChild();
	ImGui::End();
}

bool CObjectAndMeshEditorUI::SaveMeshUI(void)
{
	bool IsPushButton = false;

	ImGui::Begin("Save");

	if (ImGui::Button("SAVE MESH DATA", ImVec2(130, 50))) {
		IsPushButton = true;
	}

	ImGui::End();

	return IsPushButton;
}

bool CObjectAndMeshEditorUI::SaveModelUI(void)
{
	bool IsPushButton = false;

	ImGui::Begin("Save");

	if (ImGui::Button("SAVE MODEL DATA", ImVec2(130, 50))) {
		IsPushButton = true;
	}

	ImGui::End();

	return IsPushButton;
}

//新しいメッシュを作る
void CObjectAndMeshEditorUI::CreateMeshUI(void)
{
	char MeshFieldSizeX[32] = { '\0' };
	char MeshFieldSizeY[32] = { '\0' };
	char MeshMasuSizeX[32] = { '\0' };
	char MeshMasuSizeY[32] = { '\0' };

	sprintf_s(MeshFieldSizeX,"%.2f",m_MeshFieldSize.x);
	sprintf_s(MeshFieldSizeY, "%.2f", m_MeshFieldSize.y);
	sprintf_s(MeshMasuSizeX, "%.2f", m_MeshMasuSize.x);
	sprintf_s(MeshMasuSizeY, "%.2f", m_MeshMasuSize.y);

	ImGui::Begin("MainMenu");

	ImGui::Text("CreateNewMesh");

	ImGui::PushItemWidth(150);
	ImGui::InputText("FieldSizeX", MeshFieldSizeX, sizeof(MeshFieldSizeX)); ImGui::SameLine();
	ImGui::InputText("FieldSizeY", MeshFieldSizeY, sizeof(MeshFieldSizeY));
	ImGui::InputText("MasuSizeX", MeshMasuSizeX, sizeof(MeshMasuSizeX)); ImGui::SameLine();
	ImGui::InputText("MasuSizeY", MeshMasuSizeY, sizeof(MeshMasuSizeY));
	ImGui::PopItemWidth();

	ImGui::End();

	m_MeshFieldSize.x = (FLOAT)atof(MeshFieldSizeX);
	m_MeshFieldSize.y = (FLOAT)atof(MeshFieldSizeY);
	m_MeshMasuSize.x = (FLOAT)atof(MeshMasuSizeX);
	m_MeshMasuSize.y = (FLOAT)atof(MeshMasuSizeY);

	bool IsPushCreateButton = false;

	ImGui::Begin("MainMenu");

	if(ImGui::Button("CreateMesh", ImVec2(100, 40)) ){
		IsPushCreateButton = true;
	}

	ImGui::End();

	m_IsCreateMesh = IsPushCreateButton;
}

void CObjectAndMeshEditorUI::LockDistanceUI(void)
{
	char X[32] = { '\0' };
	char Y[32] = { '\0' };

	sprintf_s(X, "%d", m_LockDistanceX);
	sprintf_s(Y, "%d", m_LockDistanceY);

	ImGui::Begin("MainMenu");

	//X軸
	ImGui::Checkbox("LockXAxis", &m_LockFlagXAxis); ImGui::SameLine();
	ImGui::PushItemWidth(150);
	ImGui::InputText("LockValueX", X, sizeof(X));
	ImGui::PopItemWidth();

	//Y軸
	ImGui::Checkbox("LockYAxis", &m_LockFlagYAxis); ImGui::SameLine();
	ImGui::PushItemWidth(150);
	ImGui::InputText("LockValueY", Y, sizeof(Y));
	ImGui::PopItemWidth();

	ImGui::End();

	m_LockDistanceX = atoi(X);
	m_LockDistanceY = atoi(Y);
}

void CObjectAndMeshEditorUI::ResetAllObjUI(void)
{
	ImGui::Begin("MainMenu");

	if (ImGui::Button("ResetAllModel", ImVec2(100, 40)))
	{
		m_ResetAllObj = true;
	}

	ImGui::End();
}

void CObjectAndMeshEditorUI::MoveObjsCheckBoxUI(void)
{
	ImGui::Begin("MainMenu");
	ImGui::Checkbox("MoveObjs", &m_IsMoveSelectObjs);
	ImGui::End();
}