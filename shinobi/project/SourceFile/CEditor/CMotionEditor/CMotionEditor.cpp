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

#define EDITCAMERA_MOVE_SPEED (10.0f)		//�J�����ړ����x

//���W�ړ��J�[�\���̃}�N��
#define SCL_IS_ONE_BASE_DIST (30.0f)			//���W�ړ��J�[�\���̂̃X�P�[����1.0�̋���
#define NOTHING_BE_SELECTED (-1)				//�����I������ĂȂ����̕\��
#define AXIS_MOVE_COEFFICIENT (0.02f)			//���ړ��W��(�g���� : ���ړ��W�� * �}�E�X�̑��Έړ���)

//�G�f�B�^�[�Ɋւ���}�N��
#define OFFSET_MOVE_VALUE (0.01f)				//�I�t�Z�b�g�̈ړ��l

#define STUCK_MAX (20)							//�X�^�b�N���X�g�̍폜

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
	//�X�^�b�N�̃f�[�^�����
	m_ModelMotion.Uninit();					//���f���y�у��[�V�����f�[�^�̉��
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
	hWnd;				//�_�~�[�Q��

	//�I�u�W�F�N�g������
	CModelStorage::InitAll();

	//�J����������
	CCameraManager::InitAll();											//�S�ẴJ�����}�l�[�W���[������
	CCameraManager::ChangeActiveCamera(CCameraManager::CAMERA_EDIT01);	//�A�N�e�B�u�J������CAMERA_EDIT01�ɐݒ�
	CAMERA_PARA CameraPara = {D3DXVECTOR3(0.0f,10.0f,10.0f),D3DXVECTOR3(0.0f,0.0f,0.0f) ,D3DXVECTOR3(0.0f,1.0f,0.0f) };
	CCameraManager::SetCameraPara(CCameraManager::CAMERA_EDIT01, CameraPara);

	//���b�V���f�[�^������
	CMeshFieldManager::InitAll();

	//���b�V���ɂ���}�E�X�ʒu�̏�����
	m_MouseEditMesh.Init(5.0f, {100,0,0,200});

	//�f�[�^�ǂݍ���
	m_MeshLoader.SetMesh(m_MeshLoader.ImportMeshData("data/MESH_DATA/MotionEditor_MeshData.txt") );
	
	InitAndCreateLight();									//���C�g������

	//���[�V��������������
	m_ModelMotion.LoadMotionDataToInit(MOTION_DATA_PLAYER_MOTION_EDITOR);				//���f���ƃ��[�V�����f�[�^�̓ǂݍ���
	for (int i = 0; i < m_ModelMotion.GetPartTotalNum(); i++) {
		m_CopyKeyFrame.m_SRPData[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_CopyKeyFrame.m_SRPData[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		m_CopyKeyFrame.m_SRPData[i].scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	}

	//�v�b�V���f�[�^������
	m_PushReadyData.ModelMotion = m_ModelMotion;
	m_PushReadyData.EditMode = m_EditMode;
	m_PushReadyData.MotionEdit = m_MotionEdit;

	m_DefaultData = m_PushReadyData;	//�����f�[�^��ۑ�

	//�X�^�b�N�N���A
	for (auto it = m_UndoRedoStuck.begin(); it != m_UndoRedoStuck.end(); ++it) {
		(*it)->ModelMotion.Uninit();
		delete (*it);
	}
	m_UndoRedoStuck.clear();

	//�f�t�H���g�f�[�^���X�^�b�N�̈�Ԗڂɕۑ�
	UNDO_REDO_STUCK* pStuckBegin = new UNDO_REDO_STUCK;
	*pStuckBegin = m_DefaultData;
	m_UndoRedoStuck.push_back(pStuckBegin);
	m_pPushPosition = pStuckBegin;					//�ʒu�|�C���^��ۑ�

	m_bMouseHoverAndClickInWindow = false;

	//�Ȑ�������
	MyNameSpace::DeleteInitialize(m_pHermitian);
	m_pHermitian = new CHermitian;
	m_pHermitian->Init(D3DXVECTOR2(900.0f,600.0f), D3DXVECTOR2(50.0f, -50.0f), D3DXVECTOR2(-50.0f, 50.0f));

	//�X�J�C�h�[��������
	m_SkyDoom.Init(400.0f, 30, 30, SKYDOME_TEX);
	m_SkyDoom2.Init(450.0f, 30, 30, SKYDOME2_TEX);
	m_SkyDoom2.SetRot(D3DXVECTOR3(D3DX_PI * 0.5f, 0.0f, 0.0f));

	return true;
}

void CMotionEditor::Uninit(void)
{
	//�I�u�W�F�N�g���
	CModelStorage::UninitAll();

	//���b�V���f�[�^���
	CMeshFieldManager::UninitAll();

	//���b�V���ɂ���}�E�X�ʒu�̉��
	m_MouseEditMesh.Uninit();

	//�X�^�b�N�̃f�[�^�����
	m_ModelMotion.Uninit();					//���f���y�у��[�V�����f�[�^�̉��
	m_PushReadyData.ModelMotion.Uninit();
	m_DefaultData.ModelMotion.Uninit();
	for (auto it = m_UndoRedoStuck.begin(); it != m_UndoRedoStuck.end(); ++it) {
		(*it)->ModelMotion.Uninit();
		delete (*it);
	}
	m_UndoRedoStuck.clear();

	MyNameSpace::DeleteInitialize(m_pHermitian);	//�Ȑ����

	//�X�J�C�h�[�����
	m_SkyDoom.Uninit();
	m_SkyDoom2.Uninit();
}

void CMotionEditor::Update(void)
{
	m_MouseEditMesh.CalcMousePositionInMesh();	//���b�V���ɂ���}�E�X�ʒu�̌v�Z
	CModelStorage::UpdateAll();					//
	CMeshFieldManager::UpdateAll();				//���b�V���A�b�v�f�[�g

	//�G�f�B�^�[�̍X�V
	UpdateEdit();

	//�f�[�^�̏����o������
	ImGui::Begin("SAVE_BUTTON");
	if (ImGui::Button("SAVE_BUTTON", ImVec2(100, 50) ))
	{
		SaveData();
	}
	ImGui::End();

	//�X�J�C�h�[���X�V
	m_SkyDoom.Update();
	m_SkyDoom2.Update();

	//�V�[���J�ڔ���
	if (ExitButtonUI() == true) {
		UINT nID1 = MessageBox(NULL, "�^�C�g����ʂɖ߂�܂���?", "�G�f�B�^�[�I��", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);
		if (nID1 == IDYES) {
			CSceneTransition::SetScene(CSceneTransition::FADE_OUT, new CTitle());
		}
	}
}

void CMotionEditor::UpdateEdit(void)
{
	m_bClickFlag = false;					//�N���b�N�t���O�𖢐ݒ��Ԃɂ���
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse) {				//ImGui�̃}�E�X�N���b�N���������m�F
		m_bClickFlag = true;
	}

	if (m_bMouseHoverAndClickInWindow == false) {
		DetectMouseClickInWindow();
	}
	else {
		DetectMouseRelease();
	}

	//GUI����
	UndoRedoUI();							//UndoRedoUI
	ChangeEditModeUI();						//�ҏW���[�h�ύXUI
	ShowPartDetailUI();						//�I�����ꂽ�p�[�c�̏ڍו\��
	ImguiForEditor();						//GUI
	
	HermitianEdit();						//�Ȑ��ҏW
	ChangeEditMode();						//�ҏW���[�h�ύX
	EditCameraControl();					//�ҏW�J�����̃R���g���[��
	ActionInEditMode();						//�ҏW���[�h�̋�̓���
}

//���̃{�^��
void CMotionEditor::OtherOptionButton(void)
{
	//���_����`��Ԃɖ߂��{�^��
	ImGui::Begin("MainMenu");
	
	ImGui::NewLine();

	//���_����`��Ԃɖ߂��{�^��
	if(ImGui::Button("CurveReset",ImVec2(100,50))){
		m_pHermitian->CurveReset();
	}

	//���[�V�����̃��X�g�\����\���{�^��
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
	CCameraManager::ActiveCamera_ViewProMtxSet();	//�`��p�J�����ݒ�
	CLightManager::UseLight(0);						//0�ԃ��C�g�g�p

	//3D�I�u�W�F�N�g�`��
	CModelStorage::DrawAll();
	CMeshFieldManager::DrawAll();		//���b�V���`��
	m_ModelMotion.Draw();				//���f���̕`��
	if (nullptr != m_pHermitian) m_pHermitian->Draw();	//�Ȑ��`��

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

	//�J��������
	DIMOUSESTATE *pMouseState = CSceneTransition::GetInputMouse()->GetMouseStates();

	if (CSceneTransition::GetInputMouse()->GetMousePress(MOUSE_RIGHT))
	{
		float Horizon = -pMouseState->lX*0.01f;
		float Vertical = pMouseState->lY*0.01f;
		CCameraManager::TpsRotation(Horizon, Vertical, D3DX_PI*0.5);
	}

	if (CSceneTransition::GetInputMouse()->GetMousePress(MOUSE_CENTER))
	{
		//�J�����ƒ����_�̋����擾
		float Distance = CCameraManager::GetDistance(CCameraManager::GetActiveCameraName());
		Distance = Distance * 0.05f;

		//�}�E�XX�����̈ړ�
		float CameraHoriAngle = CCameraManager::GetHoriAngle(CCameraManager::GetActiveCameraName());
		CameraHoriAngle = CameraHoriAngle + D3DX_PI*0.5f;
		D3DXVECTOR3 CameraMoveDir;                   //�J�����̈ړ�����
		CameraMoveDir.x = cos(CameraHoriAngle);
		CameraMoveDir.z = sin(CameraHoriAngle);
		CameraMoveDir.y = 0.0f;
		float Horizon = -pMouseState->lX*0.01f;
		CCameraManager::Move3D(CameraMoveDir, Horizon*EDITCAMERA_MOVE_SPEED*Distance);    //�ړ�

																					  //�}�E�XY�����̈ړ�
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
	ZeroMemory(&Light, sizeof(D3DLIGHT9));							//���C�g������
	Light.Type = D3DLIGHT_DIRECTIONAL;								//���C�g�̃^�C�v���w��
	Light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);				//�f�B���N�V���i�����C�g�̐F
	Light.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);				//�A���r�G���g���C�g
	D3DXVECTOR3 vecDir(-1.0f, -1.0f, -1.0f);						//���C�g����
	D3DXVec3Normalize((D3DXVECTOR3*)&Light.Direction, &vecDir);		//���C�g������ݒ肵�����C�g�ɓ����
	CLightManager::CreateLight(Light);
}


/*==============================================================================
	�Ȑ��ҏW
===============================================================================*/
void CMotionEditor::HermitianEdit(void)
{
	//�|�C���^�`�F�b�N
	if (m_pHermitian == nullptr) {
		return;
	}


	//�Ȑ��̓��[�V�����ҏW�������\��/����ł��Ȃ�
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

	//�Ȑ��X�V
	if (m_pHermitian != nullptr) {
		m_pHermitian->Update();
	}

	//�ύX���o
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

	//�J�ڑO�̕Еt
	switch (OldMode) 
	{
	case EDIT_OFFSET:
		break;
	case EDIT_LINK:
		break;
	case EDIT_MOTION:
		break;
	}

	//�J�ڌ�̏�����
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
		if (m_pHermitian != nullptr) {		//�Ȑ��ݒ�
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

	//���[�h�ω�����������
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
	//�I�u�W�F�N�g�N���G�C�g
	switch (m_EditMode) {
	case EDIT_OFFSET:			//�p�[�c�̃I�t�Z�b�g�ݒu
		m_ModelMotion.UpdateOffset();		//�I�t�Z�b�g�̍X�V
		SelectPartObj();					//�p�[�c�I��
		break;
	case EDIT_LINK:				//�ʂ̃p�[�c�Ƃ̊֘A�t��
		m_ModelMotion.UpdateOffset();					//�I�t�Z�b�g�̍X�V
		if(nullptr == m_pSelectedPart) SelectPartObj();	//�p�[�c�I��
		else EditLinkParent();							//�e�I��
		break;
	case EDIT_MOTION:			//�L�[�t���[���̃X�e�[�^�X��ύX��
		ChangeEditModeMotion();		//���[�V�����ҏW���[�h�̕ύX
		switch (m_MotionEdit) {
		case MOTION_EDIT_MODE_EDIT_KEYFRAME:
			m_ModelMotion.UpdateKeyFrame();		//�L�[�t���[���̍X�V
			SelectPartObj();					//�p�[�c�I��
			if (nullptr != m_pHermitian) {
				m_pHermitian->Update();	//�Ȑ��X�V
			}
			//�L�[�t���[���̋Ȑ����X�V
			m_ModelMotion.SetKeyFrameHerminateDir(m_pHermitian->GetStartDir(), m_pHermitian->GetEndDir());
			break;
		case MOTION_EDIT_MODE_MOTION_PREVIEW:
			if(m_bMotionPlay) m_ModelMotion.UpdateMotion();		//���[�V�����̍X�V
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
				//�ω��ʂ̃p�[�Z���e�[�W�擾
				D3DXVECTOR2 StartDir, EndDir;
				m_ModelMotion.GetKeyFrameHerminateDirNow(&StartDir, &EndDir);
				float Percent = CHermitian::GetChangeValuePercent(StartDir, EndDir, m_KeyFramePercent);
				Percent = max(0.0f, Percent);
				Percent = min(1.0f, Percent);
				m_ModelMotion.UpdateKeyFramePreview(Percent);		//�L�[�t���[���v���r���[�̍X�V
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

	//�J�ڑO�̕Еt
	switch (OldMode)
	{
	case MOTION_EDIT_MODE_EDIT_KEYFRAME:
		break;
	case MOTION_EDIT_MODE_MOTION_PREVIEW:
		break;
	case MOTION_EDIT_MODE_KEYFRAME_PREVIEW:
		break;
	}

	//�J�ڌ�̏�����
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

	ImGui::NewLine();		//���s

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

	//�e�I�u�W�F�N�g�̑I��
	D3DXVECTOR3 RayStartPoint = m_MouseEditMesh.GetMouseRayStartPoint();		//���C�̎n�_
	D3DXVECTOR3 RayEndPoint = m_MouseEditMesh.GetMouseRayEndPoint();			//���C�̏I�_
	float DistanceMin = D3DXVec3Length(&(RayEndPoint - RayStartPoint));			//�ŒZ����

	CPart *pParentPart = nullptr;
	for (int ObjNum = 0; ObjNum < m_ModelMotion.GetPartTotalNum(); ObjNum++) {
		CPart *pPart = m_ModelMotion.GetPartPointer(ObjNum);
		if (pPart == nullptr) continue;							//�I�u�W�F�N�g�̓k����������
		D3DXVECTOR3 HitPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	//���蔻��ʒu
		bool bHit = RayPickToMesh::RaypicktoMesh(				//�}�E�X���C�Ƃ̓��蔻��
			false,
			&HitPosition,
			RayEndPoint,
			RayStartPoint,
			pPart->GetMesh(),
			pPart->GetNowMtx()
		);		//���C�ƃ��b�V���̓��蔻������

		if (bHit == false) continue;							//�������ĂȂ��Ȃ�ȉ��������Ȃ�
		float Distance = D3DXVec3Length(&(HitPosition - RayStartPoint));	//���C�n�_�Ƃ̋���������
		if (Distance < DistanceMin) {
			DistanceMin = Distance;					//�ŒZ�����X�V
			pParentPart = pPart;					//�p�[�c�ݒ�
		}
	}

	//�}�E�X���Ȃ��Ă���p�[�c�̐F�ω�
	if ( (pParentPart != nullptr) && (pParentPart != m_pSelectedPart) ) {	//�}�E�X�Ȃ��Ă���p�[�c�̐F�ω�
		m_pShowParent = pParentPart;					//�}�E�X���Ȃ��Ă���p�[�c��ۑ�
		m_pShowParent->m_Model.SetDrawType(CModel::DRAW_USE_MTRL_CUSTOMIZE);
		m_pShowParent->m_Model.SetMaterial(m_ShowParentMtrl);
	}

	//���͊m�F
	if (false == CSceneTransition::GetInputMouse()->GetMouseTrigger(MOUSE_LEFT)) return;
	m_bClickFlag = true;

	//��O����
	if ( (pParentPart == m_pSelectedPart) || (pParentPart == nullptr)) { //�I�����Ă���p�[�c�̓k���������͑I�����Ă���p�[�c�͎������g��������
		m_pSelectedPart->m_Model.SetDrawType(CModel::DRAW_USE_MTRL_DEFALUT);
		m_pSelectedPart = nullptr;
		return;
	}
	if (m_pSelectedPart->IsLowerHierarchy(pParentPart)) { //�����̉��̊K�w�̃p�[�c��������
		MessageBox(nullptr, "������艺�̊K�w��e�ɂ��邱�Ƃ͂ł��Ȃ�", "WARNING", MB_ICONHAND);
		return;
	}

	if(pParentPart != nullptr) m_pSelectedPart->SetParent(pParentPart);			//�e�ݒ�

	//���
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

	//���[�V�����̕ύX
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

	//���[�V�����̑����ƍ폜
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

	//�Ȑ��ݒ�
	D3DXVECTOR2 StartDir, EndDir;
	m_ModelMotion.GetKeyFrameHerminateDirNow(&StartDir, &EndDir);
	if (m_pHermitian != nullptr) {
		m_pHermitian->SetStartDir(StartDir);
		m_pHermitian->SetEndDir(EndDir);
	}
}

//���[�V�������X�gUI
void CMotionEditor::MotionListUI(void) 
{
	int MotionNum = m_ModelMotion.GetTotalMotionNum();

	ImGui::Begin("MotionList");

	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(350, 200), ImGuiWindowFlags_NoTitleBar);
	for (int i = 0; i < MotionNum; ++i) {
		CMotion* pMotion = m_ModelMotion.GetMotion(i);
		if (pMotion != nullptr)
		{
			//�C���v�b�g�e�L�X�g
			char buf[64] = {'\0'};
			sprintf(buf,"Motion%d",i);
			ImGui::PushItemWidth(150);
			ImGui::InputText(buf, pMotion->m_Name, sizeof(pMotion->m_Name));
			ImGui::PopItemWidth();

			//�{�^��
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

//�p�[�c�̃��X�gUI
void CMotionEditor::PartListUI(void)
{
	const int PartNum = m_ModelMotion.GetPartTotalNum();		//�p�[�c�̐�

	ImGui::Begin("MainMenu");

	ImGui::Text("PartList");

	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(350, 200), ImGuiWindowFlags_NoTitleBar);
	for (int i = 0; i < PartNum; ++i) {
		CPart* pPart = m_ModelMotion.GetPartPointer(i);
		if (pPart != nullptr)
		{
			//�C���v�b�g�e�L�X�g
			char buf[64] = { '\0' };
			sprintf(buf, "Part%d", i);
			ImGui::PushItemWidth(150);
			ImGui::InputText(buf, pPart->m_Name, sizeof(pPart->m_Name));
			ImGui::PopItemWidth();

			//�{�^��
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
	const int PartNum = m_ModelMotion.GetPartTotalNum();		//�p�[�c�̐�

	ImGui::Begin("PartList");

	ImGui::Text("PartList");

	ImGui::BeginChild(ImGui::GetID((void*)0), ImVec2(350, 200), ImGuiWindowFlags_NoTitleBar);
	for (int i = 0; i < PartNum; ++i) {
		CPart* pPart = m_ModelMotion.GetPartPointer(i);
		if (pPart != nullptr)
		{
			//�C���v�b�g�e�L�X�g
			char buf[64] = { '\0' };
			sprintf(buf, "Part%d", i);
			ImGui::PushItemWidth(150);
			ImGui::InputText(buf, pPart->m_Name, sizeof(pPart->m_Name));
			ImGui::PopItemWidth();

			//�{�^��
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

//�p�[�c�̐e�I�����̃p�[�c���X�g
void CMotionEditor::SelectParentListUI(void)
{
	const int PartNum = m_ModelMotion.GetPartTotalNum();		//�p�[�c�̐�

	ImGui::Begin("MainMenu");
	ImGui::NewLine();
	ImGui::Text("ParentList");

	ImGui::BeginChild(ImGui::GetID((void*)1), ImVec2(350, 200), ImGuiWindowFlags_NoTitleBar);
	for (int i = 0; i < PartNum; ++i) {
		CPart* pPart = m_ModelMotion.GetPartPointer(i);
		if (pPart != nullptr)
		{
			//�C���v�b�g�e�L�X�g
			ImGui::Text("NAME:"); ImGui::SameLine();
			ImGui::Text(pPart->m_Name);

			//�{�^��
			char buf[64] = { '\0' };
			sprintf(buf, "Parent%d", i);
			char ButtonName[64] = "Select";
			strcat_s(ButtonName, 64, buf);
			ImGui::SameLine();
			if (ImGui::Button(ButtonName, ImVec2(120, 20)))
			{
				if (m_pSelectedPart->IsLowerHierarchy(pPart)) { //�����̉��̊K�w�̃p�[�c��������
					MessageBox(nullptr, "������艺�̊K�w��e�ɂ��邱�Ƃ͂ł��Ȃ�", "WARNING", MB_ICONHAND);
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

	//�L�[�t���[���̑I��
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
		//�L�[�t���[���̑����ƍ폜
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
		m_KeyFramePercent = 0;								//�L�[�t���[���v���r���[��percent��0�ɂ���
	}

	if (bKeyFrameNext) {
		int NextKeyFrame = NOTHING_BE_SELECTED;
		if (nKeyFrameNow == nTotalKeyFrame - 1) NextKeyFrame = 0;
		else NextKeyFrame = nKeyFrameNow + 1;
		m_ModelMotion.SetKeyFrameNow(NextKeyFrame);
		m_KeyFramePercent = 0;								//�L�[�t���[���v���r���[��percent��0�ɂ���
	}

	//�Ȑ��ݒ�
	D3DXVECTOR2 StartDir, EndDir;
	m_ModelMotion.GetKeyFrameHerminateDirNow(&StartDir, &EndDir);
	if (m_pHermitian != nullptr) {
		m_pHermitian->SetStartDir(StartDir);
		m_pHermitian->SetEndDir(EndDir);
	}
}

//���[�v�t���O�̕ύX
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

//�L�[�t���[���̃t���[�����̕ύX
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
		//���͂͐����݂̂̂ŁA������ɑ΂��ăt�B���^�����O���s��
		bool IsCanWriteIn = false;			//�u�t���[�������㏑���v�t���O
		if (textBuf[0] != '\0')
		{
			//�������𐔂���
			int TextSize = sizeof(textBuf);
			int FontNum = 0;		//������
			for (int k = 0; k < TextSize; k++)
			{
				if (textBuf[k] == '\0')
				{
					FontNum = k;
					break;
				}
			}

			//������͐����݂̂������o
			int i = 0;
			for (i = 0; i < FontNum; i++)
			{
				//���͂̐��l��0-9�ł͂Ȃ�
				if (textBuf[i] < 48 || textBuf[i] > 57)
				{
					break;
				}
			}
			//�S���̕����͐���
			if (i == FontNum && FontNum != 0)
			{
				IsCanWriteIn = true;
			}
			else
			{
				MessageBox(nullptr, "�����̂ݓ��͂ł���!", "WARNING", MB_OK);
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

//�L�[�t���[���̕����Ɠ\��t��
void CMotionEditor::CopyPastKeyFrameUI(void)
{
	CMotion* pMotionNow = m_ModelMotion.GetMotion(m_ModelMotion.GetMotionNow());	//���[�V�����擾
	CKeyFrame *pKeyFrame = pMotionNow->GetKeyFrameData(m_ModelMotion.GetKeyFrameNow());		//�L�[�t���[���f�[�^�擾

	ImGui::Begin("MainMenu");

	ImGui::NewLine();

	//�L�[�t���[���̃R�s�[&�y�X�g
	if (ImGui::Button("CopyKeyFrame", ImVec2(100, 25))) {
		m_CopyKeyFrame = *pKeyFrame;
	}
	ImGui::SameLine();
	if (ImGui::Button("PastKeyFrame", ImVec2(100, 25))) {
		*pKeyFrame = m_CopyKeyFrame;
	}
	
	ImGui::End();
}

//�L�[�t���[���̕ҏW
void CMotionEditor::EditKeyFrameUI(void)
{
	CMotion* pMotionNow = m_ModelMotion.GetMotion(m_ModelMotion.GetMotionNow());		//���[�V�����擾
	CKeyFrame *pKeyFrame = pMotionNow->GetKeyFrameData(m_ModelMotion.GetKeyFrameNow());	//�L�[�t���[���f�[�^�擾
	int nPartID = m_pSelectedPart->m_PartID;

	ImGui::Begin("MainMenu");

	ImGui::NewLine();
	ImGui::Text("EditKeyFrame");

	ImGui::Text("rotation");

	//X��]
	ImGui::DragFloat("XRot", &pKeyFrame->m_SRPData[nPartID].rot.x, OFFSET_MOVE_VALUE, 0.0f, 0.0f, "X Value : %.2f");
	ImGui::SameLine();
	if (ImGui::Button("XRotReset",ImVec2(70,20))) {
		pKeyFrame->m_SRPData[nPartID].rot.x = 0;
	}
	//Y��]
	ImGui::DragFloat("YRot", &pKeyFrame->m_SRPData[nPartID].rot.y, OFFSET_MOVE_VALUE, 0.0f, 0.0f, "Y Value : %.2f");
	ImGui::SameLine();
	if (ImGui::Button("YRotReset", ImVec2(70, 20))) {
		pKeyFrame->m_SRPData[nPartID].rot.y = 0;
	}
	//Z��]
	ImGui::DragFloat("ZRot", &pKeyFrame->m_SRPData[nPartID].rot.z, OFFSET_MOVE_VALUE, 0.0f, 0.0f, "Z Value : %.2f");
	ImGui::SameLine();
	if (ImGui::Button("ZRotReset", ImVec2(70, 20))) {
		pKeyFrame->m_SRPData[nPartID].rot.z = 0;
	}

	ImGui::Text("translation");
	//X�ړ�
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

	//��]�l��-PI �` PI�͈͓̔��Œ�
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

	//X�����ړ�
	ImGui::DragFloat("X", &m_pSelectedPart->m_SRPOffset.pos.x, OFFSET_MOVE_VALUE, 0.0f, 0.0f, "X Axis : %.2f");
	ImGui::SameLine();
	if (ImGui::Button("XValueReset", ImVec2(90, 20))) {
		m_pSelectedPart->m_SRPOffset.pos.x = 0;
	}
	//Y�����ړ�
	ImGui::DragFloat("Y", &m_pSelectedPart->m_SRPOffset.pos.y, OFFSET_MOVE_VALUE, 0.0f, 0.0f, "Y Axis : %.2f");
	ImGui::SameLine();
	if (ImGui::Button("YValueReset", ImVec2(90, 20))) {
		m_pSelectedPart->m_SRPOffset.pos.y = 0;
	}
	//Z�����ړ�
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
	case EDIT_OFFSET:			//�p�[�c�̃I�t�Z�b�g�ݒu
		PartListUI();
		ChangeOffset();
		break;
	case EDIT_LINK:				//�ʂ̃p�[�c�Ƃ̊֘A�t��
		PartListUI();
		if (m_pSelectedPart != nullptr) {
			SelectParentListUI();
		}
		break;
	case EDIT_MOTION:			//�L�[�t���[���̃X�e�[�^�X��ύX��
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

			//�L�[�t���[���ҏW
			if (m_pSelectedPart != nullptr) {
				EditKeyFrameUI();
			}

			//���[�V�������X�g�̕\��
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
			ImGui::Text("Frame : %d", m_ModelMotion.GetFrameOfKeyFrame				//�t���[�����\��
			(m_ModelMotion.GetMotionNow(), m_ModelMotion.GetKeyFrameNow()));
			if (ImGui::Checkbox("KeyFramePlay", &m_bKeyFramePreviewPlay)) {			//�L�[�t���[���Đ��`�F�b�N�{�b�N�X
				m_KeyFramePercent = 0.0f;
			}
			ImGui::SliderFloat("KeyFramePercent", &m_KeyFramePercent, 0.0f, 1.0f);	//�L�[�t���[���p�[�Z���e�[�W
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

	//�V�������f����I������O�̃N���A�ݒ�
	if (m_pSelectedPart != nullptr) {
		m_pSelectedPart->m_Model.SetDrawType(CModel::DRAW_USE_MTRL_DEFALUT);		//�`��������f�t�H���g�ɖ߂�
		m_pSelectedPart = nullptr;											//�����I������Ă��Ȃ���Ԃɖ߂�
	}

	//�I�u�W�F�N�g�̑I��
	D3DXVECTOR3 RayStartPoint = m_MouseEditMesh.GetMouseRayStartPoint();		//���C�̎n�_
	D3DXVECTOR3 RayEndPoint = m_MouseEditMesh.GetMouseRayEndPoint();			//���C�̏I�_
	float DistanceMin = D3DXVec3Length(&(RayEndPoint - RayStartPoint));			//�ŒZ����
	
	for (int ObjNum = 0; ObjNum < m_ModelMotion.GetPartTotalNum(); ObjNum++) {
		CPart *pPart = m_ModelMotion.GetPartPointer(ObjNum);
		if (pPart == nullptr) continue;							//�I�u�W�F�N�g�̓k����������
		D3DXVECTOR3 HitPosition = D3DXVECTOR3(0.0f,0.0f,0.0f);	//���蔻��ʒu
		bool bHit = RayPickToMesh::RaypicktoMesh(				//�}�E�X���C�Ƃ̓��蔻��
			false,
			&HitPosition,
			RayEndPoint,
			RayStartPoint,
			pPart->GetMesh(),
			pPart->GetNowMtx()
		);		//���C�ƃ��b�V���̓��蔻������

		if (bHit == false) continue;							//�������ĂȂ��Ȃ�ȉ��������Ȃ�
		float Distance = D3DXVec3Length(&(HitPosition - RayStartPoint));	//���C�n�_�Ƃ̋���������
		if (Distance < DistanceMin) {
			DistanceMin = Distance;			//�ŒZ�����X�V
			m_pSelectedPart = pPart;		//�p�[�c���
		}
	}

	if (m_pSelectedPart != nullptr) {
		m_pSelectedPart->m_Model.SetDrawType(CModel::DRAW_USE_MTRL_CUSTOMIZE);
		m_pSelectedPart->m_Model.SetMaterial(m_SelectPartMtrl);
	}
}

//�f�o�b�N���b�Z�[�W
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
	bool IsHoverWindow = io.WantCaptureMouse;			//�}�E�X���E�B���h�E�̏��ɂ��邩
	bool IsClick = CSceneTransition::GetInputMouse()->GetMousePress(MOUSE_LEFT);		//�N���b�N������

	if (IsHoverWindow && IsClick) {					//���肪����΍��̃f�[�^���ꎞ�I�ɕۑ�
		m_PushReadyData.ModelMotion = m_ModelMotion;
		m_PushReadyData.EditMode = m_EditMode;
		m_PushReadyData.MotionEdit = m_MotionEdit;
		m_bMouseHoverAndClickInWindow = true;
	}
}

void CMotionEditor::DetectMouseRelease(void)
{
	bool IsClick = CSceneTransition::GetInputMouse()->GetMousePress(MOUSE_LEFT);		//�N���b�N������
	if (IsClick) {
		return;
	}

	m_bMouseHoverAndClickInWindow = false;		//�t���O�Ҍ�
	DetectDataChangeAndPushData();				//�f�[�^�̕ω������m���A�f�[�^���ω�������ꍇ�f�[�^���X�^�b�N�Ƀv�b�V��
}

void CMotionEditor::DetectDataChangeAndPushData(void)
{
	//�f�[�^�̕ω������邩�����m
	bool bChangeFlag = false;
	int MotionNow = m_ModelMotion.GetMotionNow();
	CMotion *pNowMotion = m_ModelMotion.GetMotion(MotionNow);
	CMotion *pOldMotion = m_PushReadyData.ModelMotion.GetMotion(MotionNow);

	//�p�[�c�f�[�^�ω��̌��m
	for (int PartNum = 0; PartNum < m_ModelMotion.GetPartTotalNum(); PartNum++) {
		//�f�[�^�擾
		CPart* pNowMotionPart = m_ModelMotion.GetPartPointer(PartNum);
		CPart* pOldMotionPart = m_PushReadyData.ModelMotion.GetPartPointer(PartNum);

		//�e�̕ω������m
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

		//�I�t�Z�b�g�̕ω������m
		if (pNowMotionPart->m_SRPOffset.pos != pOldMotionPart->m_SRPOffset.pos) {
			bChangeFlag = true;
			break;
		}

		//�p�[�c�̃L�[�t���[���f�[�^�̕ω��̌��m
		CKeyFrame* pNowKeyFrame = pNowMotion->GetKeyFrameData(m_ModelMotion.GetKeyFrameNow());
		CKeyFrame* pOldKeyFrame = pOldMotion->GetKeyFrameData(m_PushReadyData.ModelMotion.GetKeyFrameNow());
		SRP SRPNow = pNowKeyFrame->m_SRPData[PartNum];
		SRP SRPOld = pOldKeyFrame->m_SRPData[PartNum];
		if ((SRPNow.pos != SRPOld.pos) || (SRPNow.rot != SRPOld.rot)) {
			bChangeFlag = true;
			break;
		}
	}

	//���̃��[�V�����̃L�[�t���[���̔�r
	if (bChangeFlag == false) {
		const int NowKeyFrameNum = pNowMotion->GetTotalKeyFrame();
		const int OldKeyFrameNum = pOldMotion->GetTotalKeyFrame();
		if (NowKeyFrameNum != OldKeyFrameNum) {
			bChangeFlag = true;
		}
		else{
			CKeyFrame* pNowKeyFrame = pNowMotion->GetKeyFrameData(m_ModelMotion.GetKeyFrameNow());
			CKeyFrame* pOldKeyFrame = pOldMotion->GetKeyFrameData(m_PushReadyData.ModelMotion.GetKeyFrameNow());

			//�L�[�t���[���̌p���t���[�������r
			if (pNowKeyFrame->m_TotalFrame != pOldKeyFrame->m_TotalFrame){
				bChangeFlag = true;
			}
			//�L�[�t���[���̋Ȑ��f�[�^�̔�r
			if(pNowKeyFrame->m_HermitianStartDir != pOldKeyFrame->m_HermitianStartDir){
				bChangeFlag = true;
			}
			if (pNowKeyFrame->m_HermitianEndDir != pOldKeyFrame->m_HermitianEndDir) {
				bChangeFlag = true;
			}
		}
	}

	//���[�V���������̔�r
	if (bChangeFlag == false) {
		if (m_ModelMotion.GetTotalMotionNum() != m_PushReadyData.ModelMotion.GetTotalMotionNum()) {
			bChangeFlag = true;
		}
	}

	//���[�v�t���O��r
	if (bChangeFlag == false) {
		if (pNowMotion->m_bLoop != pOldMotion->m_bLoop)
		bChangeFlag = true;
	}

	//�ω����������ꍇ�f�[�^��ۑ�
	if (bChangeFlag) {
		PushData();
	}
}

void CMotionEditor::PushData(void)
{
	//�f�[�^���R�s�[
	UNDO_REDO_STUCK* pData = new UNDO_REDO_STUCK;
	pData->ModelMotion = m_ModelMotion;
	pData->EditMode = m_EditMode;
	pData->MotionEdit = m_MotionEdit;

	//�f�[�^������
	auto ListPositionNow = std::find(m_UndoRedoStuck.begin(), m_UndoRedoStuck.end(), m_pPushPosition);
	if (ListPositionNow == m_UndoRedoStuck.end()) {	//�f�[�^�����݂��Ȃ�
		m_UndoRedoStuck.push_back(pData);	//�f�[�^���X�^�b�N�ɓ����
		m_pPushPosition = pData;			//�f�[�^��ۑ�
		return;
	}
	else {	//�f�[�^�����݂���
		for (;;) {
			ListPositionNow = std::find(m_UndoRedoStuck.begin(), m_UndoRedoStuck.end(), m_pPushPosition);
			auto DeleteIt = ListPositionNow;		//�R�s�[
			++DeleteIt;

			if (DeleteIt == m_UndoRedoStuck.end()) break;	//�Ō�����ǂ���

			//�f�[�^�폜
			(*DeleteIt)->ModelMotion.Uninit();
			delete (*DeleteIt);
			m_UndoRedoStuck.erase(DeleteIt);
		}
	}

	//�f�[�^���X�^�b�N�ɓ����
	m_UndoRedoStuck.push_back(pData);					//�f�[�^��ۑ�
	if (m_UndoRedoStuck.size() > STUCK_MAX) {			//�f�[�^���ő�X�^�b�N���𒴂�����
		//�擪�f�[�^���폜
		(*m_UndoRedoStuck.begin())->ModelMotion.Uninit();
		delete (*m_UndoRedoStuck.begin());
		m_UndoRedoStuck.pop_front();
	}

	//�f�[�^���f
	m_pPushPosition = pData;				//�ʒu�L�^
	m_PushReadyData = *pData;				//�f�[�^���v�b�V���\���f�[�^�ɔ��f
}

void CMotionEditor::Undo(void)
{
	//�f�[�^������
	auto ListPositionNow = std::find(m_UndoRedoStuck.begin(), m_UndoRedoStuck.end(), m_pPushPosition);
	if (ListPositionNow == m_UndoRedoStuck.end()) return;
	if (ListPositionNow == m_UndoRedoStuck.begin()) return;

	--ListPositionNow;

	//�f�[�^�L�^
	m_pPushPosition = *ListPositionNow;					//�ʒu�L�^
	m_PushReadyData = *(*ListPositionNow);				//�f�[�^���v�b�V���\���f�[�^�ɔ��f
	m_ModelMotion = (*ListPositionNow)->ModelMotion;	//�f�[�^�����̃��[�V�����f�[�^�ɔ��f
	m_EditMode = (*ListPositionNow)->EditMode;
	m_MotionEdit = (*ListPositionNow)->MotionEdit;
}

void CMotionEditor::Redo(void)
{
	//�f�[�^������
	auto ListPositionNow = std::find(m_UndoRedoStuck.begin(), m_UndoRedoStuck.end(), m_pPushPosition);
	if (ListPositionNow == m_UndoRedoStuck.end()) return;

	//�Ō�����ǂ���
	++ListPositionNow;
	if (ListPositionNow == m_UndoRedoStuck.end()) return;

	//�f�[�^�L�^
	m_pPushPosition = *ListPositionNow;					//�ʒu�L�^
	m_PushReadyData = *(*ListPositionNow);				//�f�[�^���v�b�V���\���f�[�^�ɔ��f
	m_ModelMotion = (*ListPositionNow)->ModelMotion;	//�f�[�^�����̃��[�V�����f�[�^�ɔ��f
	m_EditMode = (*ListPositionNow)->EditMode;
	m_MotionEdit = (*ListPositionNow)->MotionEdit;
}

/*==============================================================================
	�f�[�^�̕ۑ�
===============================================================================*/
void CMotionEditor::SaveData(void)
{
	//���ݎ����擾���A�����z��ɋl�ߍ���
	char FilePass[256] = { '\0' };
	strcat_s(FilePass, 256, "data/MOTION_DATA/MotionEditor/Save/motion_");	//�����o���t�@�C������w��
	GetTimeNow(&FilePass[0]);			//���Ԃ���������
	strcat_s(FilePass, 256,".txt");		//.txt�g���q��������
	
	//�t�@�C���쐬
	FILE *fp;
	fp = fopen(FilePass,"w");
	if (fp == nullptr) 
	{
		MessageBox(nullptr, "�t�@�C�����J���܂���", "[CMotionEditor.cpp][Function:SaveData]", MB_ICONHAND);
		fclose(fp);
		return;
	}

	fprintf(fp, "#------------------------------------------------------------------------------\n");
	fprintf(fp, "# ���[�V�����f�[�^�̊J�n\n");
	fprintf(fp, "#------------------------------------------------------------------------------\n");
	fprintf(fp, "SCRIPT\t\t\t# ���̍s�͐�Ώ����Ȃ����ƁI\n\n");
	fprintf(fp, "#------------------------------------------------------------------------------\n");
	fprintf(fp, "# ���f����\n");
	fprintf(fp, "#------------------------------------------------------------------------------\n");

	//���f���������o��
	const int TotalPartNum = m_ModelMotion.GetPartTotalNum();	//�p�[�c���擾
	fprintf(fp, "NUM_MODEL= %d\n", TotalPartNum);

	fprintf(fp, "#------------------------------------------------------------------------------\n");
	fprintf(fp, "# ���f���t�@�C����\n");
	fprintf(fp, "#------------------------------------------------------------------------------\n");

	//�p�[�c�t�@�C���p�X�����o��
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
	fprintf(fp, "# �L�����N�^�[���\n");
	fprintf(fp, "#------------------------------------------------------------------------------\n");
	fprintf(fp, "CHARACTERSET\n");
	fprintf(fp, "\tNUM_PARTS= %d\n\n", TotalPartNum);

	//�p�[�c�I�t�Z�b�g�̏����o��
	for (int i = 0; i < TotalPartNum; i++)
	{
		fprintf(fp, "\tPARTSSET\n");
		CPart* pPart = m_ModelMotion.GetPartPointer(i);
		if (pPart != nullptr) 
		{
			fprintf(fp, "\t\tNAME= %s\n", pPart->m_Name);		//�����̖��O�����o��
			fprintf(fp, "\t\tINDEX= %d\n", pPart->m_PartID);	//������ID�����o��
			fprintf(fp, "\t\tPARENT= %d\n", pPart->m_ParentID);	//�e��ID�����o��
			fprintf(fp, "\t\tPOS= %.3f %.3f %.3f\n", pPart->m_SRPOffset.pos.x, pPart->m_SRPOffset.pos.y, pPart->m_SRPOffset.pos.z);
			fprintf(fp, "\t\tROT= %.3f %.3f %.3f\n", pPart->m_SRPOffset.rot.x, pPart->m_SRPOffset.rot.y, pPart->m_SRPOffset.rot.z);
		}
		fprintf(fp, "\tEND_PARTSSET\n\n");
	}
	fprintf(fp, "END_CHARACTERSET\n");

	fprintf(fp, "#------------------------------------------------------------------------------\n");
	fprintf(fp, "# ���[�V�������\n");
	fprintf(fp, "#------------------------------------------------------------------------------\n");

	//���[�V������񏑂��o��
	const int MotionNum = m_ModelMotion.GetTotalMotionNum();	//���[�V�������擾
	for (int i = 0; i < MotionNum; i++)
	{
		//���[�V�����擾
		CMotion* pMotion = m_ModelMotion.GetMotion(i);
		if (pMotion == nullptr) {
			continue;
		}

		fprintf(fp, "#------------------------------------------------------------------------------\n");
		fprintf(fp, "# [%d]%s\n", i, pMotion->m_Name);
		fprintf(fp, "#------------------------------------------------------------------------------\n");
		fprintf(fp, "MOTIONSET\n");

		//���[�V�����̖��O�ƃ��[�v�t���O�ƃL�[���̏����o��
		fprintf(fp, "\tNAME= %s\n", pMotion->m_Name);
		int nLoop = 0;
		(pMotion->m_bLoop == false) ? (nLoop = 0) : (nLoop = 1);
		fprintf(fp, "\tLOOP= %d\n", nLoop);
		const int TotalKeyFrame = pMotion->m_TotalKeyFrame;	//�L�[�t���[�������擾
		fprintf(fp, "\tNUM_KEY= %d\n\n", TotalKeyFrame);

		//�L�[�t���[���f�[�^�̏����o��
		for (int KeyFrameNum = 0; KeyFrameNum < TotalKeyFrame; KeyFrameNum++)
		{
			fprintf(fp, "\tKEYSET\t\t\t");
			fprintf(fp, "# --- << KEY : %d / %d >> ---\n", KeyFrameNum, TotalKeyFrame - 1);
			CKeyFrame* pKeyFrame = pMotion->GetKeyFrameData(KeyFrameNum);
			if (pKeyFrame == nullptr) {
				continue;
			}

			//�L�[�t���[���̃t���[�������������o��
			fprintf(fp, "\t\tFRAME= %d\n", pKeyFrame->m_TotalFrame);

			//�G���~�[�g�Ȑ��̋N�_�x�N�g���ƏI�_�x�N�g���̏����o��
			fprintf(fp, "\t\tSTART_DIR= %.3f %.3f\n", pKeyFrame->m_HermitianStartDir.x, pKeyFrame->m_HermitianStartDir.y);
			fprintf(fp, "\t\tEND_DIR= %.3f %.3f\n\n", pKeyFrame->m_HermitianEndDir.x, pKeyFrame->m_HermitianEndDir.y);

			//�p�[�c���Ƃ̃L�[�t���[���f�[�^�������o��
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

	fprintf(fp, "END_SCRIPT\t\t# ���̍s�͐�Ώ����Ȃ����ƁI");

	fclose(fp);

	MessageBox(nullptr, "���[�V�����f�[�^�Z�[�u����", "SUCCESS", MB_OK);
}

/*==============================================================================
	�����z��Ɍ��݂̎������l�ߍ���
===============================================================================*/
void CMotionEditor::GetTimeNow(char* pChar)
{
	//�k���`�F�b�N
	if (pChar == nullptr) {
		MessageBox(nullptr, "[CMotionEditor.cpp][Function:GetTimeNow]����pChar�̓k��", "WARNING", MB_ICONHAND);
		return;
	}

	//���ݎ����擾
	time_t t = time(nullptr);
	tm TimeNow;
	localtime_s(&TimeNow, &t);

	//�N�����f�[�^�𕶎���ɋl�ߍ���
	int year = TimeNow.tm_year + 1900;
	char Yeah[5];
	for (int i = 0; i < 4; i++) {
		int Num = year % 10;
		year /= 10;
		Yeah[4 - 1 - i] = (char)(Num + 48);		//�����𕶎��ɕϊ�
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

	//�����b�f�[�^�𕶎���ɋl�ߍ���
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