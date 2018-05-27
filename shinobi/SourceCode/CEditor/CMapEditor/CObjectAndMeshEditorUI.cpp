#include "CObjectAndMeshEditorUI.h"
#include "imgui.h"
#include "CRenderer.h"
#include "CCameraManager.h"
#include "CLightManager.h"
#include "SceneTransition.h"
#include <stdio.h>

/*==============================================================================
	�}�N��
===============================================================================*/
#define UI_MAXOBJ_ONELINE (4)		//UI��s�̍ő�\���I�u�W�F�N�g��
#define IMAGE_BUTTON_SIZE (128)

/*==============================================================================
	�R���X�g���N�^
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
	����������
===============================================================================*/
bool CObjectAndMeshEditorUI::Init(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	//���X�g���N���A���Ă��Ȃ��Ȃ�
	for (int i = 0; i < MODEL_MENU_OBJ_MAX; i++) {
		//�|�C���^�N���A
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
		
		//�󂫃e�N�X�`������
		HRESULT hr;
		hr = D3DXCreateTexture(pDevice, IMAGE_BUTTON_SIZE, IMAGE_BUTTON_SIZE,
			1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT,
			&m_RenderTargetPointerArray[i].TexturePointer
		);
		if (hr == E_FAIL)
		{
			MessageBox(nullptr, "�����_�[�^�[�Q�b�g�̃e�N�X�`���������s", "[CObjectAndMeshEditorUI.cpp][Init]", MB_ICONHAND);
			return false;
		}

		//�T�[�t�F�[�X�|�C���^���擾
		m_RenderTargetPointerArray[i].TexturePointer->GetSurfaceLevel(0, &m_RenderTargetPointerArray[i].SurfacePointer);

		//��Z�o�b�t�@����
		pDevice->CreateDepthStencilSurface(IMAGE_BUTTON_SIZE, IMAGE_BUTTON_SIZE, D3DFMT_D24S8, D3DMULTISAMPLE_NONE,
			0, FALSE, &m_RenderTargetPointerArray[i].DepthBufferPointer, nullptr);

		//���g�p��Ԃɂ���
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
	�X�V����
===============================================================================*/
void CObjectAndMeshEditorUI::Update(void)
{
	ChangeModeUI();			//���[�h�̕ύX��UI

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

	m_IsSaveMeshData = SaveMeshUI();	//���b�V���f�[�^�ۑ�
	m_IsSaveModelData = SaveModelUI();	//���f���f�[�^�ۑ�
	
}

/*==============================================================================
	�I������
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
	�������{�^���ԍ����擾���A�ԍ������Z�b�g
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
	//�k���`�F�b�N
	if (pModel == nullptr) {
		return;
	}

	//���g�p�̃����_�[�^�[�Q�b�g�Ƀ��f����`�悷��
	for (int Count = 0; Count < MODEL_MENU_OBJ_MAX; Count++) 
	{
		//�|�C���^�k���`�F�b�N
		bool IsPointerNoNull =
			m_RenderTargetPointerArray[Count].TexturePointer != nullptr &&
			m_RenderTargetPointerArray[Count].SurfacePointer != nullptr &&
			m_RenderTargetPointerArray[Count].DepthBufferPointer != nullptr;

		//�����_�[�^�[�Q�b�g����
		LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();
		if (IsPointerNoNull && (m_RenderTargetPointerArray[Count].bUse == false) )
		{
			//�`�敽�ʂ�Z�o�b�t�@�̐ݒ�
			pDevice->SetRenderTarget(0, m_RenderTargetPointerArray[Count].SurfacePointer); 
			pDevice->SetDepthStencilSurface(m_RenderTargetPointerArray[Count].DepthBufferPointer);

			//�r���[�ϊ��s��ݒ�(���f����AABB�Ƃ̔��茋�ʂŃJ�����̈ʒu�ƒ����_�̈ʒu�����߂�)
			D3DXVECTOR2 MinXMaxX, MinYMaxY, MinZMaxZ;
			pModel->GetAABBValue(&MinXMaxX, &MinYMaxY, &MinZMaxZ);

			//�J�����ʒu�����߂�
			D3DXVECTOR3 Camera;
			Camera.x = (MinXMaxX.x + MinXMaxX.y) * 0.5f;
			Camera.y = (MinYMaxY.x + MinYMaxY.y) * 0.5f;
			float HalfHeight = (MinYMaxY.y - MinYMaxY.x) * 0.5f;
			Camera.z = fabsf(HalfHeight / tanf(D3DX_PI / 3.0f * 0.5f) ) + MinZMaxZ.y;

			//�����_�ʒu�����߂�
			D3DXVECTOR3 Lookat;
			Lookat.x = (MinXMaxX.x + MinXMaxX.y) * 0.5f;
			Lookat.y = (MinYMaxY.x + MinYMaxY.y) * 0.5f;
			Lookat.z = (MinZMaxZ.x + MinZMaxZ.y) * 0.5f;

			D3DXMATRIX ViewMtx;
			D3DXMatrixLookAtLH(&ViewMtx, &Camera, &Lookat, &D3DXVECTOR3(0.0f,1.0f,0.0f));
			pDevice->SetTransform(D3DTS_VIEW, &ViewMtx);

			//�v���W�F�N�V�����s��ݒ�(�A�X�y�N�g���1:1)
			D3DXMATRIX ProjeMtx;
			D3DXMatrixPerspectiveFovLH(&ProjeMtx, D3DX_PI / 3.0f, (float)IMAGE_BUTTON_SIZE / IMAGE_BUTTON_SIZE, 1.0f, 1000.0f);
			pDevice->SetTransform(D3DTS_PROJECTION, &ProjeMtx);      //�v���W�F�N�V�����ϊ��s��
			
			//���C�g�̐ݒ�
			D3DLIGHT9 Light;
			ZeroMemory(&Light, sizeof(D3DLIGHT9));						//���C�g������
			Light.Type = D3DLIGHT_DIRECTIONAL;							//���C�g�̃^�C�v���w��
			Light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);			//�f�B���N�V���i�����C�g�̐F
			Light.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);			//�A���r�G���g���C�g
			D3DXVECTOR3 vecDir(0.0f, 0.0f, -1.0f);						//���C�g����
			D3DXVec3Normalize((D3DXVECTOR3*)&Light.Direction, &vecDir);	//���C�g������ݒ肵�����C�g�ɓ����
			pDevice->SetLight(0, &Light);											//�f�o�C�X�̃��C�g�X���b�g0�ԂɊi�[
			pDevice->LightEnable(0, TRUE);											//���C�g�X���b�g0�ԃ��C�g�N�p
			pDevice->SetRenderState(D3DRS_AMBIENT, (D3DXCOLOR)Light.Ambient);		// �A���r�G���g���C�g�ݒ�

			//���f���`��
			CRenderer::DrawClear();
			CRenderer::DrawBegin();
			pModel->SetMtxSetting();
			pModel->Draw();
			CRenderer::DrawEnd();

			//�o�b�N�o�b�t�@�̕`�敽�ʂ�Z�o�b�t�@�ɖ߂�
			pDevice->SetRenderTarget(0, CRenderer::GetBackBufferSurface());
			pDevice->SetDepthStencilSurface(CRenderer::GetBackDepthSurface());

			//�g�p��Ԃɂ���
			m_RenderTargetPointerArray[Count].bUse = true;

			//�`��p�J�����̃r���[�}�g���N�X�ɖ߂�
			CCameraManager::ActiveCamera_ViewProMtxSet();
			CLightManager::UseLight(0);
			break;
		}
	}
}

/*==============================================================================
	�}�E�X�̎g�p�󋵊m�F
===============================================================================*/
bool CObjectAndMeshEditorUI::GetMouseUsing(void)
{
	ImGuiIO& io = ImGui::GetIO();
	return io.WantCaptureMouse;
}

/*==============================================================================
�V�������b�V������邽�߂̃f�[�^�擾
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

	//���[�h�ω�����������
	if (EditModeNow != (int)m_Mode)
	{
		m_Mode = (MODE)EditModeNow;
		return;
	}
}

/*==============================================================================
	���f�����X�g�̃C���[�WUI
===============================================================================*/
void CObjectAndMeshEditorUI::ModelImageButtonUI(void)
{
	ImGui::Begin("MainMenu");

	ImGui::BeginChild(ImGui::GetID((void*)0),
		ImVec2(IMAGE_BUTTON_SIZE * UI_MAXOBJ_ONELINE * 1.2f, IMAGE_BUTTON_SIZE * 3),
		ImGuiWindowFlags_NoTitleBar);

	for (int Count = 0; Count < MODEL_MENU_OBJ_MAX; Count++) {
		//�����s�Ȃ̂��𔻒�
		if (Count % UI_MAXOBJ_ONELINE != 0)
		{
			ImGui::SameLine();
		}

		//�|�C���^�k���`�F�b�N
		bool IsPointerNoNull = 
			m_RenderTargetPointerArray[Count].TexturePointer != nullptr &&
			m_RenderTargetPointerArray[Count].SurfacePointer != nullptr &&
			m_RenderTargetPointerArray[Count].DepthBufferPointer != nullptr;
		if (IsPointerNoNull && (m_RenderTargetPointerArray[Count].bUse == true)) {
			//GUI�C���[�W�{�^��
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

//�V�������b�V�������
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

	//X��
	ImGui::Checkbox("LockXAxis", &m_LockFlagXAxis); ImGui::SameLine();
	ImGui::PushItemWidth(150);
	ImGui::InputText("LockValueX", X, sizeof(X));
	ImGui::PopItemWidth();

	//Y��
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