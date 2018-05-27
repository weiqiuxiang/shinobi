#ifndef OLEVEL_DESIGN_EDITOR_H_
#define OLEVEL_DESIGN_EDITOR_H_

#include "CLevelDesignUI.h"
#include "CEnemy.h"
#include "CPlayer.h"
#include "CMouseEditMeshField.h"
#include "CCoordinate.h"
#include "CPathPoint.h"

class CLevelDesignEditor
{
public:
	CLevelDesignEditor();
	~CLevelDesignEditor();

	typedef enum 
	{
		SET_CREATURE = 0,
		MOVE_ROT_CREATURE
	}SET_CREATURE_MODE;

	typedef enum
	{
		SELECT_CREATURE = 0,
		SELECT_CREATURE_PATH
	}SET_ENEMY_PATH_MODE;

	bool Init(HWND hWnd);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void DrawNavMesh(void);

private:
	void UpdateRightButtonClickTimer(void);	//�E�N���b�N�̃^�C�}�[�֘A
	void ModeChangeInit(void);				//���[�h�ύX���̏�����
	void ModeAction(void);					//�e���[�h�̋�̓I�ȓ���
	bool CreateCreature(void);				//�G�̃Z�b�g�y�шʒu�̈ړ��A��]�Ȃ�
	bool MouseSelectEnemy(void);			//�}�E�X�Ŕz�u�ς݂̓G��I������
	void SetCreature(void);					//�G�̐ݒu
	void MoveCreature(void);				//�G�̈ړ�
	void CancelSelectEnemy(void);			//�G�̑I����Ԃ�����

	//���W�n�Ɋւ���֐�
	void CoordinateAction(void);					//���̃A�N�V����
	void CoordinateAxisSelect(void);				//���W�ړ��J�[�\���̎��I��
	void SelectedAxisAction(void);					//�I�����ꂽ���̃A�N�V����

	//����|�C���g�Ɋւ���֐�
	bool SelectPathPoint(void);				//����|�C���g�̏���
	void MovePathPoint(void);				//����|�C���g�̈ړ�
	void SetPathPointStopFrame(void);		//����|�C���g�̒�~�t���[����ݒ�
	void AddPathPoint(void);				//����|�C���g�̒ǉ�
	bool DeletePathPoint(void);				//����|�C���g�̍폜
	void InsertPathPoint(void);				//����|�C���g�̑}��

private:
	CMouseEditMeshField		m_MouseEditMeshField;
	CLevelDesignUI			m_UI;
	CEnemy*					m_pEnemy;
	CPathPoint*				m_pPathPoint;
	CPlayer					m_PlayerInstance;
	int						m_MouseRightButtonFrameTime;//�E�{�^�����������ςȂ��̎�,������ۑ��p�ϐ�
	int						m_MouseRightButtonPushTime;	//�E�{�^�����������ςȂ��̗ݐώ���

	//�}�e���A��
	D3DMATERIAL9			m_SelectObjMtrl;			//�I�����Ă���I�u�W�F�N�g�̃}�e���A��
	D3DMATERIAL9			m_MouseInAxisMtrl;			//�}�E�X���Ȃ��Ă��鎲�̃}�e���A��
	D3DMATERIAL9			m_SelectAxisMtrl;			//�}�E�X�Ńh���b�N���Ă��鎲�̃}�e���A��

	//���W���Ɋւ������
	CCoordinate				m_Coordinate;			//���W�n�I�u�W�F�N�g
	int						m_AxisSelected;				//�I�����ꂽ���ԍ�

	//�q���[�h
	SET_CREATURE_MODE		m_SetCreatureMode;				//�q���[�h
	SET_ENEMY_PATH_MODE		m_SetEnemyPathMode;				//�q���[�h
};

#endif