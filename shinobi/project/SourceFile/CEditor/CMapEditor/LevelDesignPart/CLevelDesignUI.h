/*==============================================================================
	�t�@�C������:
		�}�b�v�G�f�B�^�[��UI
===============================================================================*/
#ifndef LEVEL_DESIGN_UI_H_
#define LEVEL_DESIGN_UI_H_

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

/*==============================================================================
	�O���錾
===============================================================================*/
class CHuman;

/*==============================================================================
	�N���X��`
===============================================================================*/
class CLevelDesignUI 
{
public:
	CLevelDesignUI();
	~CLevelDesignUI();

	typedef enum
	{
		SET_CREATURE = 0,
		SET_ENEMY_PATH,
		SET_NAV_MESH
	}MODE;

	bool Init(void);		//����������
	void Update(void);		//�X�V����
	void Uninit(void);		//�I������

	bool AddPathPointUI(void);
	bool DeletePathPointUI(void);
	bool InsertPathPointUI(void);

	//�Z�b�^�[
	bool SetIntputStopFrame(int* pStopFrameNow);

	//�Q�b�^�[
	MODE GetMode(void) { return m_Mode; }
	int GetSelectEnemyType(void);
	
	bool CLevelDesignUI::SaveEnemyUI(void);
private:
	void ModeSelectUI(void);
	void EnemyTypeUI(void);

private:
	int				m_PushEnemyButtonNumber;	//�����ꂽ�G�{�^���̔ԍ�
	MODE			m_Mode;						//���s���̃��[�h
};

#endif