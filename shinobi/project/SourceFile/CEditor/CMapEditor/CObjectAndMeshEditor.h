#pragma once

#ifndef OBJECT_AND_MESH_EDITOR_H_
#define OBJECT_AND_MESH_EDITOR_H_

#include "SceneManager.h"
#include "CModel.h"
#include "CScene3D.h"
#include "CFontUIScale.h"
#include "CCoordinate.h"
#include "CSkyDome.h"
#include "CMeshDataIO.h"
#include "CMouseEditMeshField.h"
#include "CObjectAndMeshEditorUI.h"
#include "C_Obj3D_DataIO.h"
#include "CModelStorage.h"
#include "CSelectMoreObj.h"
#include <list>

class CObjectAndMeshEditor
{
public:
	CObjectAndMeshEditor();
	~CObjectAndMeshEditor();

	typedef enum 
	{
		SET_OBJ_MODE_NONE = 0,
		SET_OBJECT,
		SELECT_OBJECT
	}SET_OBJ_MODE;

	bool Init(HWND hWnd);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
	//�������֐�
	void InitAndCreateLight(void);
	bool LoadModels(const char* ModelTextPass);			//���f���̃e�L�X�g���烂�f����ǂݍ���ŏ���������

	//�X�V�֐�
	void UpdateEdit(void);
	void UpdateRightButtonClickTimer(void);			//�E�N���b�N�̃^�C�}�[�֘A

	//���̊֐�
	void DetectModeChange(void);					//�ҏW���[�h�ύX���o
	void ModelRotate(void);							//�z�u�����f������]�ł���
	void ShowCoordinateUpdate(void);				//���W���\�����X�V����
	void SaveOrLoad(void);							//�Z�[�u�ƃ��[�h
	void MeshCreate(void);							//�V�������b�V�������

	//�e�ҏW���[�h�̋�̓���
	void ActionInEditMode(void);					//�e�ҏW���[�h�̋�̓�����܂Ƃ߂�֐�
	bool SelectObject(void);						//�I�u�W�F�N�g��I��
	void SetObject(void);							//�I�u�W�F�N�g��ݒu
	bool SelectModelObjOnFiled(void);				//�ݒu���ꂽ�I�u�W�F�N�g��I������
	
	//���W�ړ��J�[�\���̊֐�
	void CoordinateAxisSelect(void);				//���W�ړ��J�[�\���̎��I��
	void SelectedAxisAction(void);					//�I�����ꂽ���̃A�N�V����

	//MOVE_OBJ���[�h�̊֐�
	void DeleteObjs(void);							//�����I�u�W�F�N�g�̍폜
	void MoveObjs(void);							//�����I�u�W�F�N�g�̈ړ�

private:
	//�����o�ϐ�
	CObjectAndMeshEditorUI::MODE	m_EditMode;		//�O����[�h
	CScene3D*			m_pCScene3D = nullptr;		//�G�f�B�b�g�I�u�W�F�N�g
	CModel*				m_pModel = nullptr;			//�G�f�B�b�g�I�u�W�F�N�g
	D3DMATERIAL9		m_SelectObjMtrl;			//�I�����Ă���I�u�W�F�N�g�̃}�e���A��
	D3DMATERIAL9		m_MouseInAxisMtrl;			//�}�E�X���Ȃ��Ă��鎲�̃}�e���A��
	D3DMATERIAL9		m_SelectAxisMtrl;			//�}�E�X�Ńh���b�N���Ă��鎲�̃}�e���A��
	CCoordinate			m_Coordinate;				//���W�ړ��J�[�\���̓���
	int					m_AxisSelected;				//�I�����ꂽ���ԍ�
	bool				m_bClickFlag;				//�N���b�N�����t���O(�ʂ̂��̂�I���������Փ˂��Ȃ��悤�ɁA�Ⴆ��UI���N���b�N������A3D�I�u�W�F��I�������Ȃ��悤��)

	CMeshDataIO				m_MeshDataIO;				//���b�V���f�[�^�̓ǂݍ��݂Ə����o����
	C_Obj3D_DataIO			m_ModelDataIO;				//���f���f�[�^�̓ǂݍ��݂Ə����o����
	CMouseEditMeshField		m_MouseEditMeshField;		//���b�V����̃}�E�X���W
	CObjectAndMeshEditorUI	m_UI;						//�}�b�v�G�f�B�^�[UI
	std::list<CModel*>		m_ModelList;
	SET_OBJ_MODE			m_SetObjChildModeNum;			//SET_OBJ���[�h�̎q���[�h�ԍ�
	int						m_MouseRightButtonFrameTime;	//�E�{�^�����������ςȂ��̎�,������ۑ��p�ϐ�
	int						m_MouseRightButtonPushTime;		//�E�{�^�����������ςȂ��̗ݐώ���

	//�����I�u�W�F�N�g�I���֘A�ϐ�
	bool				m_MoveObjsFlag;
	CSelectMoreObj		m_SelectMoreObjMode;
};

#endif