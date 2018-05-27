//���� : �G���~�[�g�Ȑ���2DUI�A�Ȑ���Ԃ������̓~�T�C���̒e���Ɏg��
#pragma once
#ifndef HERMITIAN_H_
#define HERMITIAN_H_

#include "main.h"
#include "CFontUIScale.h"
#include <vector>

class CHermitian
{
public:
	typedef enum {
		LOCK_NONE = 0,
		LOCK_WINDOW,
		LOCK_START,
		LOCK_END
	}MOUSE_IN_OBJ;

	CHermitian();
	~CHermitian();

	void Init(const D3DXVECTOR2& DrawPos, const D3DXVECTOR2& StartDir, const D3DXVECTOR2& EndDir);
	void Update(void);
	void Draw(void);
	void Uninit(void);

	//�Q�b�^�[
	bool GetUsing(void) { return m_bUse; }
	bool GetMouseUsing(void) { return m_MouseUsing; }	//�}�E�X���g���Ă���t���O
	D3DXVECTOR2 GetStartDir(void) { return m_StartDir; }
	D3DXVECTOR2 GetEndDir(void) { return m_EndDir; }

	bool IsDragedDragPoint(void);						//�h���b�N�|�C���g�𑀍삵����


	//�Z�b�^�[
	void SetUsing(bool bUse) { m_bUse = bUse; }
	void SetPreview(bool bUse) { m_bPreView = bUse; }
	void SetStartDir(const D3DXVECTOR2& StartDir);
	void SetEndDir(const D3DXVECTOR2& EndDir);

	//���̊֐�
	void CurveReset(void);

public:
	static float GetChangeValuePercent(const D3DXVECTOR2& StartDir, const D3DXVECTOR2& EndDir, float t); //�ω��ʂ̃p�[�Z���e�[�W�擾

private:
	//�֐�
	void UpdateCoordinate(void);
	void UpdateCurve(void);			//�Ȑ��̃A�b�v�f�[�g�̕���

	void DrawCoordinate(void);		//���W���`��
	void DrawHermitianCurve(void);	//�G���~�[�g�Ȑ��`��
	void DrawVectorPoint(void);		//�n�_�I�_�̕����x�N�g���̏I�_�̈ʒu�`��

	void MouseControl(void);
	void LimitedPosition(void);		//�ʒu����
	void MouseHover(const D3DXVECTOR3& MousePos, MOUSE_IN_OBJ *pOut);
	void MousePress(const D3DXVECTOR3& MousePos, MOUSE_IN_OBJ PressFlags);
private:
	//�f�[�^���ϐ�
	D3DXVECTOR2 m_DrawPos;			//�Ȑ��S�̂̕`����W(����)
	D3DXVECTOR2 m_StartPos;			//�Ȑ����_�̐�΍��W
	D3DXVECTOR2 m_EndPos;			//�Ȑ��I�_�̐�΍��W
	D3DXVECTOR2 m_StartDir;			//�Ȑ��n�_�����x�N�g��
	D3DXVECTOR2 m_EndDir;			//�Ȑ��I�_�����x�N�g��
	bool		m_MouseUsing;		//�}�E�X�Ȃ��t���O
	MOUSE_IN_OBJ	m_MouseHoverFlags;		//�}�E�X�Ȃ�
	MOUSE_IN_OBJ	m_MousePressFlags;		//�}�E�X�N���b�N
	MOUSE_IN_OBJ	m_MousePressFlagsOld;	//�O�t���[���̃}�E�X�N���b�N�t���O
	D3DXVECTOR2		m_MousePressPos;
	bool			m_bUse;		//�`��ƍX�V������t���O
	bool			m_bPreView;	//true�Ȃ�Ȑ��̑��삪�ł��Ȃ�

	//�`�敔�ϐ�
	std::vector<D3DXVECTOR2> m_CurvePointList;		//�Ȑ���`��p���_�f�[�^
	CScene2D	*m_Background;		//�w�i
	CScene2D	*m_StartDirPoint;	//�n�_�����x�N�g���̃|�C���g
	CScene2D	*m_EndDirPoint;		//�I�_�����x�N�g���̃|�C���g
};

#endif