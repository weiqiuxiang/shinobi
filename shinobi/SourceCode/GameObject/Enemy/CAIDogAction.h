#pragma once
#ifndef CAI_DOG_ACTION_H_
#define CAI_DOG_ACTION_H_

//�C���N���[�h�w�b�_
#include "CAIActionInterface.h"
#include "CPlayer.h"
#include "CEnemy.h"

class CAIDogAction : public CAIActionInterface
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

	CAIDogAction();
	~CAIDogAction();

	virtual void AIModeAction(void);	//�������Update�֐�

	//�Z�b�^�[
	virtual void SetPlayer(CPlayer *pPlayer) { m_pPlayer = pPlayer; }
	virtual void SetAI(CEnemy *pAI) {m_pAI = pAI; }

	//�Q�b�^�[
	virtual CPlayer* GetPlayer(void) { return m_pPlayer; }
	virtual CEnemy* GetAI(void) { return m_pAI; }

private:
	//�����o�֐�
	void IfScenePlayerSetting(void);	//��ԑJ�ڑO�̏���
	D3DXVECTOR3 CalcEnemyFaceToPlayerDir(void);//�v���[���[�̕����Ɍ���(�߂�l�̓v���[���[�̕���)
	void DistToDecide(float dist);

	//�A�N�V�����ύX�̎��̐ݒ�֐�
	void ChangeActionToAttackSet(void);
	void ChangeActionToJumpAttackSet(void);
	void ChangeActionToStun(const D3DXVECTOR3& GoalFaceDir/*����*/,int StunTime/*�d������*/, float MovePower/*�ړ��p���[*/);

	//�퓬�n�֐�
	void AttackAction(void);
	void JumpAttackAction(void);
	void RunToAttack(void);

	//AI���[�h�Ɋ֌W�ϐ�
	CPlayer*		m_pPlayer;
	CEnemy*			m_pAI;

	//����֌W�ϐ�
	bool			m_PathDir;					//����|�C���g����鏇��(false�͐���,true�͋t��)
	CPathPoint*		m_pPathPoint;				//CPathPoint�̃|�C���^�[
};

#endif