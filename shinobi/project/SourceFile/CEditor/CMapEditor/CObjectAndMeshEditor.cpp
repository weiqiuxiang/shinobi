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

//���W�ړ��J�[�\���̃}�N��
#define SCL_IS_ONE_BASE_DIST (30.0f)			//���W�ړ��J�[�\���̂̃X�P�[����1.0�̋���
#define NO_AXIS_BE_SELECTED (-1)
#define AXIS_MOVE_COEFFICIENT (0.02f)			//���ړ��W��(�g���� : ���ړ��W�� * �}�E�X�̑��Έړ���)
#define HOLD_MOUSE_RIGHT_TIME (100)				//�E�N���b�N�����������锻��p����

CObjectAndMeshEditor::CObjectAndMeshEditor()
{
	m_EditMode = CObjectAndMeshEditorUI::EDIT_MESH;
	m_pCScene3D = nullptr;
	m_pModel = nullptr;
	m_bClickFlag = false;
	m_AxisSelected = NO_AXIS_BE_SELECTED;				//�I������Ă��鎲
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

//������
bool CObjectAndMeshEditor::Init(HWND hWnd)
{
	hWnd;				//�_�~�[�Q��

	//�N���X�����o�ϐ�������
	m_EditMode = CObjectAndMeshEditorUI::EDIT_MESH;
	m_pCScene3D = nullptr;
	m_pModel = nullptr;
	m_SetObjChildModeNum = SET_OBJ_MODE_NONE;
	m_MouseRightButtonFrameTime = timeGetTime();
	m_MouseRightButtonPushTime = 0;
	m_MoveObjsFlag = false;

	//�I�u�W�F�N�g������
	CModelStorage::InitAll();

	//���b�V���f�[�^������
	CMeshFieldManager::InitAll();

	//���b�V���ɂ���}�E�X�ʒu�̏�����
	m_MouseEditMeshField.Init(5.0f, {100,0,0,200});

	//�f�[�^�ǂݍ���
	CMeshField* pMeshField = m_MeshDataIO.ImportMeshData("data/MESH_DATA/EDIT_MESH_SAVE.txt");
	m_MeshDataIO.SetMesh(pMeshField);

	m_Coordinate.Init(D3DXVECTOR3(0.0f,0.0f,0.0f));			//���W�ړ��J�[�\��������
	
	InitAndCreateLight();									//���C�g������

	//�ݒu���郂�f����������
	if (LoadModels("data/MAP_EDITOR/LoadModelFilePass.txt") == false) 
	{
		return false;
	}

	//UI������
	m_UI.Init();
	//UI�̃��j���[�Ƀ��f���ݒ�
	for (auto it = m_ModelList.begin(); it != m_ModelList.end(); ++it) {
		m_UI.DrawOnTexture( (*it) );
	}

	m_ModelDataIO.ImportObj3DData("data/3D_OBJ_DATA/3D_OBJ_SAVE_DATA.txt");

	m_SelectMoreObjMode.Init();

	return true;
}

//�I������
void CObjectAndMeshEditor::Uninit(void)
{
	//�I�u�W�F�N�g���
	CScene2DStorage::UninitAll();
	CScene3DStorage::UninitAll();
	CModelStorage::UninitAll();

	//���b�V���f�[�^���
	CMeshFieldManager::UninitAll();

	//���b�V���ɂ���}�E�X�ʒu�̉��
	m_MouseEditMeshField.Uninit();

	m_Coordinate.Uninit();			//���W�ړ��J�[�\�����

	m_UI.Uninit();

	m_SelectMoreObjMode.Uninit();
}

//�X�V����
void CObjectAndMeshEditor::Update(void)
{
	m_UI.Update();		//UI�̍X�V
	UpdateEdit();		//�G�f�B�b�g�̍X�V

	//�E�N���b�N�̃^�C�}�[�֘A
	UpdateRightButtonClickTimer();

	CScene3DStorage::UpdateAll();
	CScene2DStorage::UpdateAll();
	CModelStorage::UpdateAll();

	CMeshFieldManager::UpdateAll();					//���b�V���A�b�v�f�[�g
	CAllObj3DHitMesh::ColliAllObj3DAndAllMesh();	//3D�I�u�W�F�ƃ��b�V���̓��蔻��

	m_Coordinate.Update();							//���W�J�[�\���X�V

	SaveOrLoad();									//�f�[�^�̏����o���Ɠǂݍ��ݏ���

	MeshCreate();									//���b�V�������
}

//�G�f�B�^�[�����̍X�V
void CObjectAndMeshEditor::UpdateEdit(void)
{
	DetectModeChange();						//�ҏW���[�h�ύX
	ActionInEditMode();						//���[�h��̂̓���

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
	//�E�N���b�N�����n��
	if (CSceneTransition::GetInputMouse()->GetMouseTrigger(MOUSE_RIGHT)) {
		m_MouseRightButtonFrameTime = timeGetTime();
		m_MouseRightButtonPushTime = 0;
		return;
	}

	//�E�N���b�N�̗ݐώ���
	if (CSceneTransition::GetInputMouse()->GetMousePress(MOUSE_RIGHT)) {
		int TimeNow = timeGetTime();											//���ݎ����̎擾
		m_MouseRightButtonPushTime += TimeNow - m_MouseRightButtonFrameTime;	//�E�N���b�N�̎��ԗݐ�
		m_MouseRightButtonFrameTime = TimeNow;									//���ݎ����̕ۑ�
	}
}

//�`��
void CObjectAndMeshEditor::Draw(void)
{
	CMeshFieldManager::DrawAll();		//���b�V���`��

	m_Coordinate.Draw();			//���W�ړ��J�[�\���`��

	//3D�I�u�W�F�N�g�`��
	switch (m_EditMode) {
	case CObjectAndMeshEditorUI::EDIT_MESH:
		CModelStorage::DrawAll();
		break;
	case CObjectAndMeshEditorUI::SET_OBJ:
		CModelStorage::DrawAll();

		//�G�f�B�b�g�I�u�W�F�N�g�̕`��
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
			//���f���擾
			CModel* pModel = CModelStorage::GetObj(i);
			if (pModel == nullptr) {
				continue;
			}

			//�I�����ꂽ��A�I���}�e���A���ŕ`��
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
		m_MouseEditMeshField.Draw();			//���b�V���ɂ���}�E�X�ʒu�̕`��
	}

	if (nullptr != m_pCScene3D) {
		m_pCScene3D->Draw();
	}

	m_SelectMoreObjMode.Draw();
}

//���C�g���
void CObjectAndMeshEditor::InitAndCreateLight(void)
{
	D3DLIGHT9 Light;
	ZeroMemory(&Light, sizeof(D3DLIGHT9));						//���C�g������
	Light.Type = D3DLIGHT_DIRECTIONAL;							//���C�g�̃^�C�v���w��
	Light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);			//�f�B���N�V���i�����C�g�̐F
	Light.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);			//�A���r�G���g���C�g
	D3DXVECTOR3 vecDir(1.0f, -1.0f, 1.0f);						//���C�g����
	D3DXVec3Normalize((D3DXVECTOR3*)&Light.Direction, &vecDir);	//���C�g������ݒ肵�����C�g�ɓ����
	CLightManager::CreateLight(Light);
}

//���f���̃e�L�X�g���烂�f����ǂݍ���ŏ���������
bool CObjectAndMeshEditor::LoadModels(const char* ModelTextPass)
{
	//���f�����X�g�̃f�[�^���N���A
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

	//���f���ǂݍ���ŏ���������
	FILE *fp;
	fp = fopen(ModelTextPass, "r");

	if (fp == nullptr)
	{
		MessageBox(nullptr, "���f���f�[�^�̃e�L�X�g�����݂��Ȃ�", "[CObjectAndMeshEditor][LoadModels]", MB_OK | MB_ICONHAND);
		fclose(fp);
		return false;
	}

	char buf[1024];		//�����o�b�t�@
	while (CTextLoad::GetStrToken(fp, TOKEN, buf) != -1) 
	{
		if (strcmp("MODEL_FILENAME=", buf) != 0) {
			continue;
		}

		//���f���̃t�@�C���p�X��ǂݍ���
		CTextLoad::GetStrToken(fp, TOKEN, buf);
		char ModelPass[1024];
		strcpy_s(ModelPass, _countof(ModelPass), buf);

		//���f�������
		CModel *pModel = new CModel;
		if (pModel != nullptr) {
			SRP srp;
			srp.pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			srp.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			srp.scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
			pModel->Init(srp, ModelPass);

			//���f�������X�g�ɒǉ�
			m_ModelList.push_back(pModel);
		}
	}

	fclose(fp);

	return true;
}

//���[�h�̕ύX�����m
void CObjectAndMeshEditor::DetectModeChange(void)
{
	CObjectAndMeshEditorUI::MODE modeNow = m_UI.GetMode();

	//�O�Ɠ�����������
	if (modeNow == m_EditMode) {
		return;
	}

	//���[�h�J�ڑO�̕Еt
	switch (m_EditMode)
	{
	case CObjectAndMeshEditorUI::EDIT_MESH:
		break;
	case CObjectAndMeshEditorUI::SET_OBJ:
		if (m_pModel != nullptr) {
			m_pModel->SetDrawType(CModel::DRAW_USE_MTRL_DEFALUT);		//�`��������f�t�H���g�ɖ߂�
			m_pModel = nullptr;											//�����I������Ă��Ȃ���Ԃɖ߂�
		}
		m_Coordinate.SetDrawUse(false);
		break;
	case CObjectAndMeshEditorUI::MOVE_OBJ:
		m_SelectMoreObjMode.ResetSelectObjAmount();
		break;
	}

	//�J�ڌ�̏�����
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

	m_EditMode = modeNow;		//�J�ڌヂ�[�h���
}

//���f����]
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

//�G�f�B�^�[�̓���
void CObjectAndMeshEditor::ActionInEditMode(void)
{
	//�N���b�N�t���O�𖢐ݒ��Ԃɂ���
	m_bClickFlag = false;

	//�t�B�[���h��}�E�X�̈ʒu�v�Z
	m_MouseEditMeshField.CalcMousePositionInMesh();

	D3DXVECTOR3 CoordinatePos = m_Coordinate.GetPos();
	float CoordinatePosY = CMeshFieldManager::CalcPosYValueInMeshField(CoordinatePos);
	CoordinatePos.y = CoordinatePosY;
	m_Coordinate.SetPos(CoordinatePos);

	//�I�u�W�F�N�g�N���G�C�g
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

		static bool IsMouseLeftButtonHold = false;			//���{�^���������܂܂Ȃ̂�

		if (nullptr == m_pModel) {
			//�q���[�h������
			m_SetObjChildModeNum = SET_OBJ_MODE_NONE;

			//�V�����I�u�W�F�N�g�����Ȃ�
			if (SelectObject() == true) {
				m_SetObjChildModeNum = SET_OBJECT;
				break;
			}

			//�t�B�[���h��̃I�u�W�F�N�g��I��
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

				CoordinateAxisSelect();				//���W�ړ��J�[�\���̓���
				SelectedAxisAction();				//�����I������Ă����Ԃ̋�̓���

				if (CSceneTransition::GetInputMouse()->GetMouseRelease(MOUSE_RIGHT)) {
					if (m_MouseRightButtonPushTime <= HOLD_MOUSE_RIGHT_TIME) {
						m_pModel->SetDrawType(CModel::DRAW_USE_MTRL_DEFALUT);
						m_pModel = nullptr;
						m_Coordinate.SetDrawUse(false);
					}
					m_MouseRightButtonPushTime = 0;
				}

				//�I�u�W�F�N�g�̍폜����
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
		//�I�u�W�F�N�g�̈ړ����[�h�Ȃ̂�
		if (m_UI.GetMoveSelectObjsFlag() == false) {
			//�t���O�Ǘ�
			m_MoveObjsFlag = false;
			m_Coordinate.SetDrawUse(false);

			//�S�ẴI�u�W�F�N�g�폜
			if (CSceneTransition::GetInputKeyboard()->GetKeyTrigger(DIK_DELETE)) {
				DeleteObjs();
			}

			//��ʔ͈͑I�𑀍�
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

//�t�B�[���h��̃I�u�W�F�N�g��I��
bool CObjectAndMeshEditor::SelectModelObjOnFiled(void)
{
	if (false == CSceneTransition::GetInputMouse()->GetMouseTrigger(MOUSE_LEFT)) {
		return false;
	}

	//ImGui�g�p���͖���
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse) {
		return false;
	}

	//�V�������f����I������O�̃N���A�ݒ�
	if (m_pModel != nullptr) {
		m_pModel->SetDrawType(CModel::DRAW_USE_MTRL_DEFALUT);		//�`��������f�t�H���g�ɖ߂�
		m_pModel = nullptr;											//�����I������Ă��Ȃ���Ԃɖ߂�
	}

	//�I�u�W�F�N�g�̑I��
	D3DXVECTOR3 RayStartPoint = m_MouseEditMeshField.GetMouseRayStartPoint();		//���C�̎n�_
	D3DXVECTOR3 RayEndPoint = m_MouseEditMeshField.GetMouseRayEndPoint();			//���C�̏I�_
	float DistanceMin = D3DXVec3Length(&(RayEndPoint - RayStartPoint));			//�ŒZ����
	
	for (int ObjNum = 0; ObjNum < MODEL_OBJ_MAX; ObjNum++) {
		CModel *pModel = CModelStorage::GetObj(ObjNum);
		if (pModel == nullptr) continue;							//�I�u�W�F�N�g�̓k����������
		D3DXVECTOR3 HitPosition = D3DXVECTOR3(0.0f,0.0f,0.0f);	//���蔻��ʒu
		bool bHit = RayPickToMesh::RaypicktoMesh(				//�}�E�X���C�Ƃ̓��蔻��
			false,
			&HitPosition,
			RayEndPoint,
			RayStartPoint,
			pModel->GetMesh(),
			pModel->GetResultMtx()
		);		//���C�ƃ��b�V���̓��蔻������

		if (bHit == false) continue;							//�������ĂȂ��Ȃ�ȉ��������Ȃ�
		float Distance = D3DXVec3Length(&(HitPosition - RayStartPoint));	//���C�n�_�Ƃ̋���������
		if (Distance < DistanceMin) {
			DistanceMin = Distance;			//�ŒZ�����X�V
			m_pModel = pModel;				//���f�����
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
	int SelectedModelNum = m_UI.GetPushButtonNumber();		//�I�����ꂽ���f���ԍ��擾
	if (SelectedModelNum == NO_PUSH_ANY_IMAGE_BUTTON) {		//���I����ԂȂ�I��
		return false;
	}

	auto it = m_ModelList.begin();
	m_pModel = (CModel*)(*std::next(it, SelectedModelNum));

	return true;
}

//�I�u�W�F�N�g�I��
void CObjectAndMeshEditor::SetObject(void)
{
	//�k���`�F�b�N
	if (m_pModel == nullptr) {
		return;
	}

	//UI�̕����̓}�E�X�g���Ă��邩
	if (m_UI.GetMouseUsing() == true) {
		return;
	}
	
	//���b�V����̃}�E�X�̈ʒu���擾
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

	//�g�嗦�ݒ�
	D3DXVECTOR3 CameraPos = CCameraManager::GetActiveCameraPos();
	float Camera_SelectedModel_Dist = D3DXVec3Length( &(CameraPos - m_Coordinate.GetPos()) );
	float Scl = Camera_SelectedModel_Dist / SCL_IS_ONE_BASE_DIST;				//�J�����������قǑI���J�[�\���̑傫���͂ł���
	m_Coordinate.SetScl(D3DXVECTOR3(Scl, Scl, Scl));							//�X�P�[���ݒ�

	//���蔻������
	D3DXVECTOR3 RayStartPoint = m_MouseEditMeshField.GetMouseRayStartPoint();		//���C�̎n�_
	D3DXVECTOR3 RayEndPoint = m_MouseEditMeshField.GetMouseRayEndPoint();			//���C�̏I�_
	float DistanceMin = D3DXVec3Length(&(RayEndPoint - RayStartPoint));			//�ŒZ����

	//�}�E�X�J�[�\���Ǝ����d�˂Ă��邩�̔���
	int MouseOnAxis = NO_AXIS_BE_SELECTED;
	for (int ObjNum = 0; ObjNum < 4; ObjNum++) {
		D3DXVECTOR3 HitPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	//���蔻��ʒu
		bool bHit = RayPickToMesh::RaypicktoMesh(				//�}�E�X���C�Ƃ̓��蔻��
			false,
			&HitPosition,
			RayEndPoint,
			RayStartPoint,
			m_Coordinate.GetMesh(ObjNum),
			m_Coordinate.GetMatrix(ObjNum)
		);		//���C�ƃ��b�V���̓��蔻������

		if (bHit == false) continue;										//�������ĂȂ��Ȃ�ȉ��������Ȃ�
		float Distance = D3DXVec3Length(&(HitPosition - RayStartPoint));	//���C�n�_�Ƃ̋���������
		if (Distance < DistanceMin) {
			DistanceMin = Distance;			//�ŒZ�����X�V
			MouseOnAxis = ObjNum;			//
		}
	}

	//�}�E�X���N���b�N�t���O
	bool bMouseLeftClick = CSceneTransition::GetInputMouse()->GetMousePress(MOUSE_LEFT);

	if (MouseOnAxis != NO_AXIS_BE_SELECTED) {
		for (int ObjNum = 0; ObjNum < 4; ObjNum++) {
			if(ObjNum != MouseOnAxis) m_Coordinate.SetDrawType(ObjNum, CModel::DRAW_USE_MTRL_DEFALUT);	//�`��^�C�v�����ɖ߂���
		}

		if (bMouseLeftClick) {	//�N���b�N����
			m_Coordinate.SetMaterial((CCoordinate::OBJ_NAME)MouseOnAxis, m_SelectAxisMtrl);					//�}�e���A���ݒ�
			m_AxisSelected = MouseOnAxis;			//�I�����ꂽ���ɐݒ�
			m_bClickFlag = true;					//�}�E�X���łɃN���b�N����
		}
		else {
			m_Coordinate.SetMaterial((CCoordinate::OBJ_NAME)MouseOnAxis, m_MouseInAxisMtrl);				//�}�e���A���ݒ�
		}
		m_Coordinate.SetDrawType((CCoordinate::OBJ_NAME)MouseOnAxis, CModel::DRAW_USE_MTRL_CUSTOMIZE);	//�`��^�C�v�̕ύX�ݒ�
	}
}

//����I��
void CObjectAndMeshEditor::SelectedAxisAction(void)
{
	if (false == CSceneTransition::GetInputMouse()->GetMousePress(MOUSE_LEFT)) {	//�N���b�N���������ςȂ����Ă��Ȃ�
		m_AxisSelected = NO_AXIS_BE_SELECTED;		//���I������Ă��Ȃ���Ԃɖ߂�
		return;
	}
	if (NO_AXIS_BE_SELECTED == m_AxisSelected) return;		//���I������Ă��Ȃ�

	//�g�嗦�ݒ�
	D3DXVECTOR3 CameraPos = CCameraManager::GetActiveCameraPos();
	D3DXVECTOR3 CameraModelVec = m_Coordinate.GetPos() - CameraPos;
	float Camera_SelectedModel_Dist = D3DXVec3Length(&CameraModelVec);
	float Scl = Camera_SelectedModel_Dist / SCL_IS_ONE_BASE_DIST;				//�J�����������قǑI���J�[�\���̑傫���͂ł���
	m_Coordinate.SetScl(D3DXVECTOR3(Scl, Scl, Scl));							//�X�P�[���ݒ�

	//���ɉ���������
	switch (m_AxisSelected) {
	case CCoordinate::OBJ_X_AXIS:
	{
		D3DXVECTOR3 CameraToLookat = CCameraManager::GetDirCameraToLookat(CCameraManager::GetActiveCameraName());
		D3DXVECTOR3 CrossVec;
		D3DXVECTOR3 CameraToLookatXZDir = D3DXVECTOR3(CameraToLookat.x, 0.0f, CameraToLookat.z);
		D3DXVec3Normalize(&CameraToLookatXZDir,&CameraToLookatXZDir);
		D3DXVec3Cross(&CrossVec, &CameraToLookatXZDir, &D3DXVECTOR3(1.0f, 0.0f, 0.0f));		//X���ƃJ���������_��XZ���ʂ̃x�N�g��������
		DIMOUSESTATE *pMouseState = CSceneTransition::GetInputMouse()->GetMouseStates();	//�}�E�X�X�e�[�g�擾
		if (CrossVec.y >= 0) {		//XZ���ʏ�AX���̓J���������_��XZ���ʂ̃x�N�g���̉E�ɂ���
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
		D3DXVec3Cross(&CrossVec, &CameraToLookatXZDir, &D3DXVECTOR3(0.0f, 0.0f, 1.0f));		//Z���ƃJ���������_��XZ���ʂ̃x�N�g��������
		DIMOUSESTATE *pMouseState = CSceneTransition::GetInputMouse()->GetMouseStates();	//�}�E�X�X�e�[�g�擾
		if (CrossVec.y >= 0) {		//XZ���ʏ�AX���̓J���������_��XZ���ʂ̃x�N�g���̉E�ɂ���
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
	//�I�����ꂽ�I�u�W�F�N�g�͂��Ȃ�
	int SelectObjsAmount = m_SelectMoreObjMode.GetSelectObjAmount();
	if (SelectObjsAmount <= 0) {
		return;
	}

	//���߂ă��[�h�ɓ��鎞�̏���
	if (m_MoveObjsFlag == false && m_UI.GetMoveSelectObjsFlag() == true) {
		m_MoveObjsFlag = true;

		//�I�����ꂽ�I�u�W�F�N�g�����̒��S���W�����߂�
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

	//�I�u�W�F�N�g�̈ړ�����
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

//�Z�[�u�ƃ��[�h
void CObjectAndMeshEditor::SaveOrLoad(void)
{
	if (m_UI.GetSaveMeshFlag()) {
		m_MeshDataIO.ExportMeshData();
		MessageBox(nullptr,"���b�V���f�[�^��ۑ����܂���","",MB_OK);
	}
	if (m_UI.GetSaveModelFlag()) {
		m_ModelDataIO.ExportObj3DData();
		MessageBox(nullptr, "���f���f�[�^��ۑ����܂���", "", MB_OK);
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

	//�Â����b�V�����폜
	CMeshFieldManager::UninitAll();

	int VertexNum = ((int)FieldSize.x + 1) * ((int)FieldSize.y + 1);		//���_�����v�Z
	double *pMeshRelativeHeight = new double[VertexNum];		//�W���|�C���^
	
	for (int y = 0; y <= FieldSize.y; y++){
		for (int x = 0; x <= FieldSize.x; x++) {
			int k = y * ((int)FieldSize.x + 1) + x;
			pMeshRelativeHeight[k] = 0.0f;
		}
	}

	CMeshField *pMesh = CMeshFieldManager::CreateMeshField(D3DXVECTOR3(0,0,0), CINDEX_SIZE(FieldSize, MasuSize), FLOOR001_TEX);
	pMesh->LoadRelativeHigh(pMeshRelativeHeight);
	m_MeshDataIO.SetMesh(pMesh);

	//���
	delete[] pMeshRelativeHeight;
}