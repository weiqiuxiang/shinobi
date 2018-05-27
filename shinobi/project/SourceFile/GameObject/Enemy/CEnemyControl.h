#pragma once
#ifndef _CENEMY_ACTION_H_
#define _CENEMY_ACTION_H_

#include "CPlayer.h"
#include "CMeshField.h"
#include "CEnemy.h"
#include <list>

//�O���錾
class CEnemy;
class CPathPoint;

class CEnemyControl  //�v���[���[���R���g���[�����邽�߂̃N���X
{
public:
	typedef enum
	{
		MOTION_NEUTRAL = 0,
		MOTION_MOVE,
		MOTION_RUN,
		MOTION_ATTACK,
		MOTION_JUMPATTACK,
		MOTION_STUN,
		MOTION_DEAD
	}MOTION_MODE;			//���[�V����

	CEnemyControl();
	~CEnemyControl();

	void Init(void);						//������
	void EnemyControl(void);				//�v���[���[���R���g���[������(�܂Ƃ߃N���X)

	//�Z�b�^�[
	void SetEnemy(CEnemy *Enemy);
	void SetPlayer(CPlayer *Player);

	//�Q�b�^�[
	CPlayer* GetPlayer(void) { return m_Player; }

private:
	void EnemyFaceDir(void);			//AI�̌�������|�[�Y���Z�o
	void EnemyHitAllMesh(void);			//AI�ƃ��b�V���t�B�[���h�Ƃ̓��蔻��

	//�����o�ϐ�
	CEnemy*				m_AI;
	CPlayer*			m_Player;
};
#endif