#pragma once
#ifndef _CGAME_H_
#define _CGAME_H_

//�C���N���[�h�w�b�_
#include "SceneManager.h"
#include "CLockOnEF.h"
#include "CSkyDome.h"
#include "CFeedBackBlur.h"
#include "CMeshDataIO.h"
#include "C_Obj3D_DataIO.h"
#include "CCameraLight.h"
#include "CRenderTarget2D.h"

//����錾
class CGameUI;
class CPlayerControl;
class CPlayer;

//�N���X
class CGame : public CSceneManager
{
public:
	typedef enum {
		TRA_TO_NONE = 0,
		TRA_TO_TITLE,
		TRA_TO_RESULT,
	}SCENE_TRA;

	CGame();
	~CGame();

	bool Init(HWND hWnd);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void DrawInRenderTarget(void);
	void DrawInDepthRenderTarget(D3DXMATRIX ViewMtx, D3DXMATRIX ProjeMtx);
	void DrawSceneUseDepthMap(void);

	void UpdateLockOnEF(void);

	//�Z�b�^�[
	void SceneTraDecision(void);
private:
	void InitStatic(void);		//static�^�̃I�u�W�F�N�g�̏�����
	void InitLight(void);		//���C�g�̏�����
	void InitEnemy(void);		//�G�̏�����

private:
	SCENE_TRA		m_TraFlag;				//�V�[���J�ڃt���O
	CGameUI*		m_GameUI;				//�Q�[��UI
	CPlayer*		m_Player;				//�v���[���[
	CPlayerControl*	m_PlayerControl;		//�v���[���[�R���g���[��
	CMeshDataIO		m_MeshLoader;			//���b�V���ǂݍ��ݖ�
	C_Obj3D_DataIO	m_ModelLoader;			//���f���ǂݍ��ݖ�
	CLockOnEF		m_LockOnEF;				//���b�N�I���\��
	CSkyDome		m_SkyDoom;				//�X�J�C�h�[��1
	CSkyDome		m_SkyDoom2;				//�X�J�C�h�[��2
	CCameraLight	m_CameraLight;			//���C�g���̃J����
	int				m_GoToResultTime;		//�G��S�|���̌�̃��U���g�ɍs������

	CRenderTarget2D m_SceneRenderTarget;			//�V�[���̃����_�[�^�[�Q�b�g
};

#endif