#include "CMapEditor.h"
#include "main.h"
#include <imgui.h>
#include "SceneTransition.h"
#include "CTitle.h"
#include "CCameraManager.h"
#include "CLightManager.h"
#include "CModelStorage.h"
#include "CRenderer.h"

//�J�����ړ����x
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

	//�����̃����o�ϐ�
	m_IsExit = false;
	m_IsShowAABB = false;
	m_Mode = OBJECT_AND_MESH_EDITOR;

	m_SkyDoom.Init(400.0f, 30, 30, SKYDOME_TEX);
	m_SkyDoom2.Init(450.0f, 30, 30, SKYDOME2_TEX);
	m_SkyDoom2.SetRot(D3DXVECTOR3(D3DX_PI * 0.5f, 0.0f, 0.0f));

	//�J����������
	CCameraManager::InitAll();											//�S�ẴJ�����}�l�[�W���[������
	CCameraManager::ChangeActiveCamera(CCameraManager::CAMERA_EDIT01);	//�A�N�e�B�u�J������CAMERA_EDIT01�ɐݒ�
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
	//�J��������
	EditCameraControl();

	//���[�h�I��
	ModeSelectUI();
	ImGui::Begin("MainMenu");
	ImGui::SameLine();
	ImGui::End();
	ShowAABBUI();

	//���[�h�ɂ����s
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

	//�V�[���J�ڔ���
	m_IsExit = ExitButtonUI();
	if (m_IsExit == true) {
		UINT nID1 = MessageBox(NULL, "�^�C�g����ʂɖ߂�܂���?", "�G�f�B�^�[�I��", MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);
		if (nID1 == IDYES) {
			CSceneTransition::SetScene(CSceneTransition::FADE_OUT, new CTitle());
		}
	}
}

void CMapEditor::Draw(void)
{
	CCameraManager::ActiveCamera_ViewProMtxSet();	//�`��p�J�����ݒ�
	CLightManager::UseLight(0);						//0�ԃ��C�g�g�p

	//�X�J�C�h�[���`��
	m_SkyDoom.Draw();
	m_SkyDoom2.Draw();

	m_ObjAndMeshEditor.Draw();
	m_LevelDesignEditor.Draw();

	//AABB�{�b�N�X�̕`��
	if (m_IsShowAABB == true) {
		CModelStorage::DrawAllAABBTest();
	}

	m_LevelDesignEditor.DrawNavMesh();
}

void CMapEditor::EditCameraControl(void)
{
	//IMGUI���쒆
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse) {
		return;
	}

	if (CCameraManager::GetActiveCameraName() == CCameraManager::CAMERA_PLAYER) {
		return;      //�쓮���Ă���J�����̖��O�擾
	}
	if (CSceneTransition::GetInputKeyboard()->GetKeyPress(DIK_LSHIFT)) {
		return;
	}

	//�J��������
	DIMOUSESTATE *pMouseState = CSceneTransition::GetInputMouse()->GetMouseStates();

	if (CSceneTransition::GetInputMouse()->GetMousePress(MOUSE_RIGHT))
	{
		float Horizon = -pMouseState->lX*0.01f;
		float Vertical = pMouseState->lY*0.01f;
		CCameraManager::TpsRotation(Horizon, Vertical, D3DX_PI*0.5f);
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

	//���[�h�ω�����������
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