#pragma once
#ifndef _CPLAYER_CONTROL_H_
#define _CPLAYER_CONTROL_H_

#include "CPlayer.h"
#include "CEnemy.h"
#include "CMeshField.h"
#include "CGame.h"
#include "CAfterimageEffect.h"

//����錾
class CCharacterCamera;

//�N���X��`
class CPlayerControl
{
public:
	void Init(void);					//������
	void Update(void);					//�v���[���[���R���g���[������(�܂Ƃ߃N���X)
	void SceneTraDecision(CGame::SCENE_TRA* pTraFlag);		//�V�[���J��
	void Uninit(void);					//���

	//�Z�b�^�[
	void SetPlayer(CPlayer *Player);
	
	//�Q�b�^�[
	inline bool GetLockOnFlag(void) {
		if (m_Player != nullptr) {
			return m_Player->GetLockOnFlag();
		}
		return false; 
	}
	D3DXVECTOR3 GetLockOnEnemyPos(void);
private:
	void PlayerHitAllMesh(void);				//�v���[���[�ƃ��b�V���t�B�[���h�Ƃ̓��蔻��
	void PlayerLockOnEnemy(void);				//�G�Ƀ��b�N�I��
	void LockOnTargetChange(void);				//���b�N�I�����Ă���W�I��ύX
	void RotateCamera(void);					//�J��������]����
	void PlayerCameraControl(void);				//�v���[���[�̃J�����Ǐ]
	void PlayerFaceDir(void);					//�v���[���[�̖ʂ̌�������p�����v�Z

private:
	CEnemy *m_Enemy;
	CPlayer *m_Player;
	bool m_bMotionChange;
	CPlayer::MOTION_MODE m_MotionMode;		//���s���郂�[�V�����ԍ�
	CPlayer::MOTION_MODE m_MotionModeOld;	//�O����s�������[�V�����ԍ�
	int m_nMotionTime;						//���[�V�����̌p�����Ԏ擾
	int m_nKeyFrameTime;					//1�L�[�t���[���̌p�����Ԏ擾
	D3DXVECTOR2 m_ControlerStickDir;		//�X�e�B�b�N�̉�������
	D3DXVECTOR2 m_ControlerRightStickDirOld;//�X�e�B�b�N�̉�������
	D3DXVECTOR3 m_ActionMoveDir;			//�v���[���[������̓�������鎞�̈ړ�����(�� : ���[�����O�A�X�e�b�v�A�G�̍U�����󂯎��̈ړ�����)
	float		m_MovePower;				//�v���[���[�̈ړ���
	int			m_KeyFrameBefore;			//�O�̃L�[�t���[��
	CCharacterCamera *m_PlayerCamera;
};

#endif