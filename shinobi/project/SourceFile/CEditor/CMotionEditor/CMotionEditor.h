#pragma once
//���� : ���[�V�����G�f�B�^�[
#ifndef _CMOTION_EDITOR_H_
#define _CMOTION_EDITOR_H_

#include "SceneManager.h"
#include "CModel.h"
#include "CScene3D.h"
#include "CFontUIScale.h"
#include "CCoordinate.h"
#include "CPart.h"
#include "CEditorMotion.h"
#include "CMotion.h"
#include "CHermitian.h"
#include <list>
#include "CSkyDome.h"
#include "CMouseEditMeshField.h"
#include "CMeshDataIO.h"

class CMotionEditor : public CSceneManager
{
public:
	typedef enum {
		EDIT_OFFSET = 0,			//�I�t�Z�b�g�ݒ�
		EDIT_LINK,					//���̃p�[�c�Ƃ̃����O
		EDIT_MOTION					//���[�V�����ҏW
	}EDIT_MODE;			//�ҏW���[�h

	typedef enum {
		MOTION_EDIT_MODE_EDIT_KEYFRAME = 0,		//�L�[�t���[���̒ǉ��폜�A�ύX,�L�[�t���[���̌p���t���[�����A���[�v�t���O�ݒ�
		MOTION_EDIT_MODE_MOTION_PREVIEW,		//���[�V�����̃v���r���[
		MOTION_EDIT_MODE_KEYFRAME_PREVIEW,		//�w��L�[�t���[�����玟�̃L�[�t���[���܂ł̃v���r���[
	}MOTION_EDIT_MODE;				//EDIT_MOTION�̎q�K�w

	typedef struct {
		CEditorMotion		ModelMotion;			//���f�����[�V����
		EDIT_MODE			EditMode;				//�G�f�B�b�g���[�h
		MOTION_EDIT_MODE	MotionEdit;				//���[�V�����G�f�B�b�g���[�h
	}UNDO_REDO_STUCK;

	CMotionEditor();
	~CMotionEditor();

	bool Init(HWND hWnd);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
	//�������֐�
	void InitAndCreateLight(void);

	//�X�V�֐�
	void UpdateEdit(void);

	//���̊֐�
	void HermitianEdit(void);						//�Ȑ��ҏW
	void ChangeEditMode(void);						//�ҏW���[�h�ύX
	void ChangeEditModeUI(void);					//�ҏW���[�h�ύX�Ή�UI
	void ChangeEditModeMotion(void);				//���[�V�����ҏW���[�h�ύX
	void ChangeEditModeMotionUI(void);				//���[�V�����ҏW���[�h�ύXUI
	void EditChangeMotionUI(void);					//���[�V�����ύX
	void MotionListUI(void);						//���[�V�����̃��X�gUI
	void PartListUI(void);							//�p�[�c�̃��X�gUI
	void PartListUIInEditKeyFrame(void);			//�p�[�c�̃��X�gUI
	void SelectParentListUI(void);					//�p�[�c�̐e�I�����̃p�[�c���X�g
	void EditChangeKeyFrameUI(void);				//�L�[�t���[���ύX
	void EditLoopFlagUI(void);						//���[�v�t���O�̐ݒ�
	void EditChangeFrameUI(void);					//�L�[�t���[���̃t���[�����̕ύX
	void CopyPastKeyFrameUI(void);					//�L�[�t���[���̕����Ɠ\��t��
	void EditKeyFrameUI(void);						//�L�[�t���[���̕ҏW
	void ChangeOffset(void);						//�I�t�Z�b�g�ύX
	void ShowPartDetailUI(void);					//�p�[�c�̏ڍו\��(SRP�ANAME�AINDEX�APARENT)
	void EditCameraControl(void);					//�G�f�B�b�g���[�h�̃J�����R���g���[��
	void ImguiForEditor(void);						//���b�Z�[�W�\��
	void SelectPartObj(void);						//�p�[�c�̑I��
	void OtherOptionButton(void);					//���̃{�^��

	//Undo�ARedo�Ɋւ���֐�
	void DetectMouseClickInWindow(void);
	void DetectMouseRelease(void);
	void DetectDataChangeAndPushData(void);			//�f�[�^�̕ω������m���A�f�[�^���ω�������ꍇ�f�[�^���X�^�b�N�Ƀv�b�V��
	void PushData(void);							//�f�[�^���X�^�b�N�Ƀv�b�V��
	void UndoRedoUI(void);							//
	void Undo(void);								//���ɖ߂�
	void Redo(void);								//��蒼��
	void ShowUndoRedoStuckSize(void);				//�f�o�b�N���b�Z�[�W

	//Save�ALoad�Ɋւ���֐�
	void SaveData(void);							//�f�[�^�̕ۑ�
	void GetTimeNow(char*);							//�����z��Ɍ��݂̎������l�ߍ���

	void ActionInEditMode(void);					//�e�ҏW���[�h�̋�̓���
	void EditLinkParent(void);						//�e�ݒ�

	//���̊֐�
	bool ExitButtonUI(void);						//���[�h�I���{�^���������ꂽ���𔻒�

private:
	//�����o�ϐ�
	EDIT_MODE			m_EditMode;					//�G�f�B�b�g���[�h
	MOTION_EDIT_MODE	m_MotionEdit;				//���[�V�����G�f�B�b�g���[�h
	bool				m_bEditModeChange;			//�G�f�B�b�g���[�h�`�F���W�̃t���O
	bool				m_bMotionEditChange;		//���[�V�����ҏW���[�h�`�F���W�̃t���O
	bool				m_bMotionPlay;				//���[�V�������Đ�����t���O
	bool				m_bKeyFramePreviewPlay;		//�L�[�t���[���Đ�
	CPart*				m_pSelectedPart;			//�I�����ꂽ�p�[�c
	CPart*				m_pShowParent;				//�}�E�X���Ȃ��Ă���p�[�c
	D3DMATERIAL9		m_SelectPartMtrl;			//�I�����Ă���p�[�c�̃}�e���A��
	D3DMATERIAL9		m_ShowParentMtrl;			//�I�����Ă���p�[�c�̃}�e���A��
	bool				m_bClickFlag;				//�N���b�N�����t���O(�ʂ̂��̂�I���������Փ˂��Ȃ��悤�ɁA�Ⴆ��UI���N���b�N������A3D�I�u�W�F��I�������Ȃ��悤��)
	float				m_KeyFramePercent;			//�L�[�t���[���v���r���[���A���̃L�[�t���[�����玟�̃L�[�t���[���ɕ�Ԃ���p�[�Z���e�[�W
	CKeyFrame			m_CopyKeyFrame;				//�R�s�[���ꂽ�L�[�t���[��
	CEditorMotion		m_ModelMotion;				//���[�V�������j�b�g
	CHermitian			*m_pHermitian;				//�G���~�[�g�Ȑ�
	bool				m_IsShowMotionList;			//���[�V�������X�g�\���t���O
	bool				m_IsShowPartList;			//�p�[�c���X�g�̕\��

	//Undo�ARedo�ϐ�
	UNDO_REDO_STUCK					m_PushReadyData;	//�v�b�V���\���f�[�^
	std::list<UNDO_REDO_STUCK*>		m_UndoRedoStuck;	//Undo��Redo�̃X�^�b�N
	UNDO_REDO_STUCK*				m_pPushPosition;	//���ҏW���Ă��郊�X�g�̃C�e���[�^�[�ʒu
	UNDO_REDO_STUCK					m_DefaultData;		//�ŏ��̃f�[�^
	bool m_bMouseHoverAndClickInWindow;					//�}�E�X���E�B���h�E�̏��ɃN���b�N�����̂�

	CMouseEditMeshField m_MouseEditMesh;				//�v�Z���}�E�X�̃��C���擾�p
	CMeshDataIO			m_MeshLoader;					//���b�V���f�[�^�ǂݍ��ݗp
	CSkyDome		m_SkyDoom;
	CSkyDome		m_SkyDoom2;
};

#endif