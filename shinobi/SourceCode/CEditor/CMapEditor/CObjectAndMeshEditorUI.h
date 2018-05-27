/*==============================================================================
	�t�@�C������:
		�}�b�v�G�f�B�^�[��UI
===============================================================================*/
#ifndef OBJECT_AND_MESH_EDITOR_UI_H_
#define OBJECT_AND_MESH_EDITOR_UI_H_

/*==============================================================================
	�C���N���[�h�w�b�_
===============================================================================*/
#include "main.h"
#include "CModel.h"
#include "CRenderTarget2D.h"

/*==============================================================================
	�}�N��
===============================================================================*/
#define NO_PUSH_ANY_IMAGE_BUTTON (-1)
#define MODEL_MENU_OBJ_MAX (100)

/*==============================================================================
	�N���X��`
===============================================================================*/
class CObjectAndMeshEditorUI
{
public:
	CObjectAndMeshEditorUI();
	~CObjectAndMeshEditorUI() {}
	typedef enum
	{
		EDIT_MESH = 0,		//���b�V���̋N����ҏW���郂�[�h
		SET_OBJ,			//�I�u�W�F�N�g���Z�b�g
		MOVE_OBJ			//���[�h�̃e�X�g
		//TODO::�G�̔z�u�ƓG�̏���|�C���g�̔z�u
		//TODO::���蔻��̔z�u
	}MODE;

	bool Init(void);		//����������
	void Update(void);		//�X�V����
	void Uninit(void);		//�I������

	//�Q�b�^�[
	int GetPushButtonNumber(void);				//�������{�^���ԍ����擾���A�ԍ������Z�b�g
	MODE GetMode(void) { return m_Mode; }
	bool GetMouseUsing(void);
	bool GetSaveMeshFlag(void) { return m_IsSaveMeshData; }
	bool GetSaveModelFlag(void) { return m_IsSaveModelData; }
	bool GetCreateMeshData(D3DXVECTOR2*, D3DXVECTOR2*);
	int GetLockDistanceX(void) { return m_LockDistanceX; }
	int GetLockDistanceY(void) { return m_LockDistanceX; }
	bool GetLockFlagX(void) { return m_LockFlagXAxis; }
	bool GetLockFlagY(void) { return m_LockFlagYAxis; }
	bool GetResetAllModel(void);
	bool GetMoveSelectObjsFlag(void) { return m_IsMoveSelectObjs; }

	//���̊֐�
	void DrawOnTexture(CModel* pModel);			//�e�N�X�`���ɕ`�悷��

private:
	void ChangeModeUI(void);					//���[�h�̕ύXUI
	void ModelImageButtonUI(void);				//���f�����X�g�̃C���[�WUI
	bool ExitButtonUI(void);
	bool SaveMeshUI(void);
	bool SaveModelUI(void);
	void CreateMeshUI(void);
	void LockDistanceUI(void);
	void ResetAllObjUI(void);

	//MOVE_OBJ������UI
	void MoveObjsCheckBoxUI(void);
private:
	RENDER_TARGET_POINTER	m_RenderTargetPointerArray[MODEL_MENU_OBJ_MAX];
	int				m_PushButtonNumber;			//�����ꂽ�{�^���̔ԍ�
	MODE			m_Mode;						//���s���̃��[�h
	bool			m_IsSaveMeshData;			//���b�V���f�[�^�Z�[�u�t���O
	bool			m_IsSaveModelData;			//���f���f�[�^�Z�[�u�t���O
	bool			m_IsCreateMesh;				//���b�V���f�[�^�擾
	D3DXVECTOR2		m_MeshMasuSize;				//���b�V���}�X�T�C�Y
	D3DXVECTOR2		m_MeshFieldSize;			//���b�V���t�B�[���h�T�C�Y
	int				m_LockDistanceX;			//���f���z�u���̐����l�ړ�����
	int				m_LockDistanceY;			//���f���z�u���̐����l�ړ�����
	bool			m_LockFlagXAxis;			//
	bool			m_LockFlagYAxis;			//
	bool			m_ResetAllObj;				//�S�Ẵ��f���폜

	//MOVE_OBJ�����̕ϐ�
	bool			m_IsMoveSelectObjs;			//�I�u�W�F�N�g�ړ��t���O
};

#endif