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

#define SCL_IS_ONE_BASE_DIST (30.0f)	//���W���̊g�嗦
#define DEFAULT_FIELD_COFFI (0.2f)		//�f�t�H���g�n�ʖ��C�W��
#define HOLD_MOUSE_RIGHT_TIME (100)		//�E�N���b�N�����������锻��p����
#define AXIS_MOVE_COEFFICIENT (0.02f)	//���ړ��W��(�g���� : ���ړ��W�� * �}�E�X�̑��Έړ���)

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

//������
bool CLevelDesignEditor::Init(HWND hWnd)
{
	hWnd;				//�_�~�[�Q��

	m_UI.Init();
	m_pEnemy = nullptr;
	m_pPathPoint = nullptr;
	m_MouseRightButtonFrameTime = timeGetTime();
	m_MouseRightButtonPushTime = 0;

	//�G�̏�����
	CEnemyStorage::InitAll();

	//���b�V���ɂ���}�E�X�ʒu�̏�����
	m_MouseEditMeshField.Init(5.0f, { 100,0,0,200 });

	//���W�n������
	m_Coordinate.Init(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	//�G�f�[�^�ǂݎ��
	CCreatureDataIO EnemyIO;
	EnemyIO.ImportEnemyObjData("data/CREATURE_DATA/EnemyDataOutput.txt");

	return true;
}

//�I������
void CLevelDesignEditor::Uninit(void)
{
	m_UI.Uninit();

	//���b�V���ɂ���}�E�X�ʒu�̉��
	m_MouseEditMeshField.Uninit();

	//�G�̉��
	CEnemyStorage::UninitAll();

	//���W�n���
	m_Coordinate.Uninit();
}

//�X�V����
void CLevelDesignEditor::Update(void)
{
	m_UI.Update();

	//���W�n�X�V
	m_Coordinate.Update();

	//���b�V����}�E�X�̈ʒu���v�Z����
	m_MouseEditMeshField.CalcMousePositionInMesh();

	//�E�N���b�N�̃^�C�}�[�v��
	UpdateRightButtonClickTimer();

	//�S�Ă̓G�̍X�V
	CEnemyStorage::UpdateEditorAll();
	CEnemyStorage::UpdateAllEnemyPathPosition();

	//���[�h�ɂ����s
	ModeAction();

	if (m_UI.SaveEnemyUI() == true) {
		CCreatureDataIO EnemyIO;
		EnemyIO.ExportEnemyObjData();
		MessageBox(nullptr, "�G�e�[�^�ۑ�����", "", MB_OK);
	}
}

//�`��
void CLevelDesignEditor::Draw(void)
{
	//�S�G�`��
	CEnemyStorage::DrawAll();

	//���[�h�ɂ����s
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

//�E�N���b�N�̃^�C�}�[�֘A
void CLevelDesignEditor::UpdateRightButtonClickTimer(void)
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

//���[�h�ύX���̏�����
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

	//���[�h�̏㏑��
	S_ModeOld = m_UI.GetMode();
}

void CLevelDesignEditor::ModeAction(void)
{
	//���[�h�ω����̏�����
	ModeChangeInit();

	switch (m_UI.GetMode()) {
	case CLevelDesignUI::SET_CREATURE:
	{
		static bool IsMouseLeftButtonHold = false;			//���{�^���������܂܂Ȃ̂�

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
				//�}�E�X����������Ȃ�A�ȉ��͎��s���Ȃ�
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
		static bool IsMouseLeftButtonHold = false;			//���{�^���������܂܂Ȃ̂�

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
			//��O����
			if (m_pEnemy == nullptr) {
				m_SetEnemyPathMode = SELECT_CREATURE;
				break;
			}

			//����|�C���g�ǉ�
			AddPathPoint();				//����|�C���g�̒ǉ�

			if (m_pPathPoint == nullptr) {
				//�G�I�����L�����Z��
				CancelSelectEnemy();

				//����|�C���g�I��
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

				//�p�X�ړ�
				CoordinateAction();
				MovePathPoint();

				//����|�C���g�̑}��
				InsertPathPoint();

				//�p�X�폜
				if (DeletePathPoint() == true) {
				}

				//�҂����Ԃ̐ݒ�
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

	//�I���Ȃ�
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

	//ImGui�g�p���Ȃ疳��
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse) {
		return false;
	}

	//�V�������f����I������O�̃N���A�ݒ�
	if (m_pEnemy != nullptr) {
		m_pEnemy->SetDrawType(CModel::DRAW_USE_MTRL_DEFALUT);		//�`��������f�t�H���g�ɖ߂�
		m_pEnemy = nullptr;											//�����I������Ă��Ȃ���Ԃɖ߂�
	}

	//�I�u�W�F�N�g�̑I��
	D3DXVECTOR3 RayStartPoint = m_MouseEditMeshField.GetMouseRayStartPoint();		//���C�̎n�_
	D3DXVECTOR3 RayEndPoint = m_MouseEditMeshField.GetMouseRayEndPoint();			//���C�̏I�_
	float DistanceMin = D3DXVec3Length(&(RayEndPoint - RayStartPoint));			//�ŒZ����

	//�S�Ă̓G�ɑ΂��ă}�E�X���C�Ƃ̓��蔻����s��
	auto it = CEnemyStorage::GetEnemiesListBegin();
	auto itEnd = CEnemyStorage::GetEnemiesListEnd();
	for (; it != itEnd; ++it) {
		//�I�u�W�F�N�g�̓k����������
		if (*it == nullptr) {
			continue;
		}

		D3DXVECTOR3 HitPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	//���蔻��ʒu
		bool bHit = RayPickToMesh::RaypicktoMesh(				//�}�E�X���C�Ƃ̓��蔻��
			false,
			&HitPosition,
			RayEndPoint,
			RayStartPoint,
			(*it)->GetBodyMesh(),
			(*it)->GetBodyResultMtx()
		);

		//�������ĂȂ��Ȃ�ȉ��������Ȃ�
		if (bHit == false) {
			continue;
		}

		//��Ԏ�O�̃I�u�W�F�N�g�𓖂�Ώۂɐݒ�
		float Distance = D3DXVec3Length(&(HitPosition - RayStartPoint));
		if (Distance < DistanceMin) {
			DistanceMin = Distance;			//�ŒZ�����X�V
			m_pEnemy = (*it);				//���f�����
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

//�G�̈ʒu�ݒ�
void CLevelDesignEditor::SetCreature(void)
{
	if (m_pEnemy == nullptr) {
		return;
	}

	//ImGui�g�p���Ȃ疳��
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse) {
		return;
	}

	D3DXVECTOR3 MousePosition = m_MouseEditMeshField.GetMousePosInMesh();

	m_pEnemy->SetPos(MousePosition);

	//�G��ݒu
	if (CSceneTransition::GetInputMouse()->GetMouseTrigger(MOUSE_LEFT)) {
		m_pEnemy = CEnemyStorage::Create(CEnemy::TYPE_DOG, MousePosition, D3DXVECTOR3(0, 0, 0), MOTION_DATA_ENEMYDOG, 1.0f, -0.1f, DEFAULT_FIELD_COFFI);
	}

	//�G�ݒu���L�����Z��
	if (CSceneTransition::GetInputMouse()->GetMouseRelease(MOUSE_RIGHT)) {
		if (m_MouseRightButtonPushTime < HOLD_MOUSE_RIGHT_TIME) {
			CEnemyStorage::Delete(m_pEnemy->GetUseID());
			m_pEnemy = nullptr;
		}
		m_MouseRightButtonPushTime = 0;					//�}�E�X�̉E�{�^���̃^�C�����Z�b�g
	}
}

void CLevelDesignEditor::MoveCreature(void)
{
	if (m_pEnemy == nullptr) {
		return;
	}
	
	D3DXVECTOR3 Dist = m_Coordinate.GetMoveDist();
	m_pEnemy->AddPos(Dist);

	//�G�I�����L�����Z��
	CancelSelectEnemy();

	//�I��Ώۍ폜
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

//���̃A�N�V����
void CLevelDesignEditor::CoordinateAction(void)
{
	CoordinateAxisSelect();
	SelectedAxisAction();
}

//���I��
void CLevelDesignEditor::CoordinateAxisSelect(void)
{
	//ImGui�g�p���Ȃ疳��
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse) {
		return;
	}

	//���I�������ԂȂ����Ȃ�
	if (NO_AXIS_BE_SELECTED != m_AxisSelected) {
		return;
	}

	//�g�嗦�ݒ�
	D3DXVECTOR3 CameraPos = CCameraManager::GetActiveCameraPos();
	float Camera_SelectedModel_Dist = D3DXVec3Length(&(CameraPos - m_Coordinate.GetPos()));
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
			if (ObjNum != MouseOnAxis) m_Coordinate.SetDrawType(ObjNum, CModel::DRAW_USE_MTRL_DEFALUT);	//�`��^�C�v�����ɖ߂���
		}

		if (bMouseLeftClick) {	//�N���b�N����
			m_Coordinate.SetMaterial((CCoordinate::OBJ_NAME)MouseOnAxis, m_SelectAxisMtrl);					//�}�e���A���ݒ�
			m_AxisSelected = MouseOnAxis;			//�I�����ꂽ���ɐݒ�
		}
		else {
			m_Coordinate.SetMaterial((CCoordinate::OBJ_NAME)MouseOnAxis, m_MouseInAxisMtrl);				//�}�e���A���ݒ�
		}
		m_Coordinate.SetDrawType((CCoordinate::OBJ_NAME)MouseOnAxis, CModel::DRAW_USE_MTRL_CUSTOMIZE);	//�`��^�C�v�̕ύX�ݒ�
	}
}

//���I����̓���
void CLevelDesignEditor::SelectedAxisAction(void)
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
		D3DXVec3Normalize(&CameraToLookatXZDir, &CameraToLookatXZDir);
		D3DXVec3Cross(&CrossVec, &CameraToLookatXZDir, &D3DXVECTOR3(1.0f, 0.0f, 0.0f));		//X���ƃJ���������_��XZ���ʂ̃x�N�g��������
		DIMOUSESTATE *pMouseState = CSceneTransition::GetInputMouse()->GetMouseStates();	//�}�E�X�X�e�[�g�擾
		if (CrossVec.y >= 0) {		//XZ���ʏ�AX���̓J���������_��XZ���ʂ̃x�N�g���̉E�ɂ���
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

	//ImGui�g�p���Ȃ疳��
	ImGuiIO& io = ImGui::GetIO();
	if (io.WantCaptureMouse) {
		return false;
	}

	//�k���`�F�b�N
	if (m_pEnemy == nullptr) {
		return false;
	}

	if (CSceneTransition::GetInputMouse()->GetMouseTrigger(MOUSE_LEFT) == false) {
		return false;
	}

	//����|�C���g�I��
	//�V�������f����I������O�̃N���A�ݒ�
	if (m_pPathPoint != nullptr) {
		m_pPathPoint->SetDrawType(CPathPoint::DRAW_USE_MTRL_DEFALUT);	//�`��������f�t�H���g�ɖ߂�
		m_pPathPoint = nullptr;											//�����I������Ă��Ȃ���Ԃɖ߂�
	}

	//�I�u�W�F�N�g�̑I��
	D3DXVECTOR3 RayStartPoint = m_MouseEditMeshField.GetMouseRayStartPoint();		//���C�̎n�_
	D3DXVECTOR3 RayEndPoint = m_MouseEditMeshField.GetMouseRayEndPoint();			//���C�̏I�_
	float DistanceMin = D3DXVec3Length(&(RayEndPoint - RayStartPoint));			//�ŒZ����

	//�S�Ă̓G�ɑ΂��ă}�E�X���C�Ƃ̓��蔻����s��
	auto it = m_pEnemy->GetPathPointListBegin();
	auto itEnd = m_pEnemy->GetPathPointListEnd();

	//0�Ԗڂ̏���|�C���g������,1�Ԗڈȍ~�̏���|�C���g�͑I��Ώ�
	for (++it; it != itEnd; ++it) {
		//�I�u�W�F�N�g�̓k����������
		if (*it == nullptr) {
			continue;
		}

		D3DXVECTOR3 HitPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);	//���蔻��ʒu
		bool bHit = RayPickToMesh::RaypicktoMesh(				//�}�E�X���C�Ƃ̓��蔻��
			false,
			&HitPosition,
			RayEndPoint,
			RayStartPoint,
			(*it)->GetMesh(),
			(*it)->GetWorldMtx()
		);

		//�������ĂȂ��Ȃ�ȉ��������Ȃ�
		if (bHit == false) {
			continue;
		}

		//��Ԏ�O�̃I�u�W�F�N�g�𓖂�Ώۂɐݒ�
		float Distance = D3DXVec3Length(&(HitPosition - RayStartPoint));
		if (Distance < DistanceMin) {
			DistanceMin = Distance;			//�ŒZ�����X�V
			m_pPathPoint = (*it);			//���f�����
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
	//�k���`�F�b�N
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
	//�k���`�F�b�N
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
	//�k���`�F�b�N
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
	//�k���`�F�b�N
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
	//�k���`�F�b�N
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