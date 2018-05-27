#include "CAIDogAction.h"

//�}�N��
#define WALK_POWER (0.02f)							//������
#define RUN_POWER (0.10f)							//�����
#define JUMP_ATTACK_POWER (1.2f)					//�W�����v�U���̈ړ���
#define JUMP_POWER (0.8f)							//�W�����v�U���̒�����
#define ATTACK_MOVE_POWER1 (0.6f)					//�U���ړ���1
#define ATTACK_MOVE_POWER2 (1.0f)					//�U���ړ���2

#define EYE_HALF_HORIANGLE (D3DX_PI * 0.25)			//���o�̔����p�x(����)
#define EYE_HALF_VERTIANGLE (D3DX_PI * 0.25)		//���o�̔����p�x(����)
#define EYE_DISTANCE (50.0f)						//���o�̋���
#define LISTEN_DISTANCE (20.0f)						//���o�̋���
#define ATTACK_CAN_START_DISTANCE (10.0f)			//�U���̋���
#define JUMPATTACK_CAN_START_DISTANCE (20.0f)		//��ъ|����U�����s�\����

#define DEAD_ZONE_DISTANCE_FROM_PATHPOINT (1.0f)	//����͊J�n/�I���ł��鋗��

CAIDogAction::CAIDogAction()
{
	m_pPlayer	= nullptr;
	m_pAI		= nullptr;

	//����֌W�̕ϐ�
	m_pPathPoint	= nullptr;
	m_PathDir		= false;
}

CAIDogAction::~CAIDogAction() 
{

}

void CAIDogAction::AIModeAction(void)
{
	if (nullptr == m_pAI) {
		return;
	}
	if (nullptr == m_pPlayer) {
		return;
	}

	switch (m_AIState) {		//AI�̏��
	case STATE_PATROL:
		if (m_Action != ACTION_NORMAL) m_Action = ACTION_NORMAL;	//�A�N�V�����ݒ�
		AIPatrol();							//����
		AISencePlayer();					//�v���[���[���m
		IfScenePlayerSetting();				//�����v���[���[�𔭌�
		break;

	case STATE_THINK:
		switch (m_ThinkState) {
		case TS_RORATE_TO_PLAYER:	//�v���[���[�̕��Ɍ���
		{
			m_Action = ACTION_NORMAL;
			m_MovePowerDir = CalcEnemyFaceToPlayerDir();	//�v���[���[�̕��Ɍ���
			if (acos( D3DXVec3Dot(&m_MovePowerDir, &m_pAI->GetFaceDir()) ) < D3DX_PI*0.1) {
				m_ThinkState = TS_THINK_NO_MOVE;		//�ړ����Ȃ��v�l�ɑJ��
			}
			break;
		}

		case TS_THINK_NO_MOVE:			//�ړ����Ȃ��v�l
		{
			m_nThinkTime--;
			if (m_nThinkTime > 0) break;
			DistToDecide(CalcDistPlayerAI());	//�v���[���[�Ƃ̋����œ���������
			break;
		}

		case TS_THINK_MOVE:
			break;

		case TS_RUN_TO_ATTACK_PLAYER:
			RunToAttack();
			break;

		case TS_JUMP_ATTACK:
			if (m_nMotionTime > 0) break;
			m_ThinkState = TS_RORATE_TO_PLAYER;
			m_nThinkTime = 30;
			m_pAI->SetCoffi(0.2);
			m_pAI->DisableHitCircle();			//�U�����蔻�苅�𖳌���
			break;
		}
		break;

	case STATE_DEAD:
		m_Action = ACTION_DEAD;
		break;
	}

	switch (m_Action) {		//AI�̃A�N�V����(AI�̏�Ԃ̒��Ɍ��߂�)
	case ACTION_NORMAL:		//�������܂�
		SetMotion(MOTION_NEUTRAL);
		m_pAI->SetGoalDir(m_MovePowerDir);
		break;
	case ACTION_WALK:		//����
		SetMotion(MOTION_MOVE);
		m_pAI->SetGoalDir(m_MovePowerDir);				//�������ړ������ɐݒ�
		m_pAI->AddPower(m_MovePowerDir*WALK_POWER);		//�ړ��͂�^����
		break;
	case ACTION_RUN:		//����
		SetMotion(MOTION_RUN);
		m_pAI->SetGoalDir(m_MovePowerDir);				//�������ړ������ɐݒ�
		m_pAI->AddPower(m_MovePowerDir*RUN_POWER);
		break;
	case ACTION_ATTACK:		//�U��
		SetMotion(MOTION_ATTACK);
		AttackAction();										//�U���̋�̓���
		m_nMotionTime = max(--m_nMotionTime,0);				//���[�V�������Ԍ���
		if (m_nMotionTime <= 0)m_pAI->DisableHitCircle();	//���蔻�苅�L����
		break;
	case ACTION_JUMPATTACK:	//��ъ|����U��
		SetMotion(MOTION_JUMPATTACK);
		JumpAttackAction();
		m_nMotionTime = max(--m_nMotionTime, 0);			//���[�V�������Ԍ���
		if(m_nMotionTime <= 0)m_pAI->DisableHitCircle();	//���蔻�苅�L������
		break;
	case ACTION_STUN:		//����(�X�^��)
		SetMotion(MOTION_STUN);
		m_nStunTime = max(--m_nStunTime, 0);		//�X�^�����Ԍ���
		break;
	case ACTION_DEAD:		//���S
		SetMotion(MOTION_DEAD);
		break;
	}

	MotionChangeCheck();				//���[�V�����ύX���L����(���̃��[�V�����Ɠ����������疳��)
}

//�v���[���[�̕��Ɍ���
D3DXVECTOR3 CAIDogAction::CalcEnemyFaceToPlayerDir(void)
{
	D3DXVECTOR3 PlayerPos = m_pPlayer->GetPos();            //�v���[���[�ʒu�擾
	D3DXVECTOR3 Enemy = m_pAI->GetPos();
	D3DXVECTOR3 MoveDirGoal = PlayerPos - Enemy;
	MoveDirGoal.y = 0;
	D3DXVec3Normalize(&MoveDirGoal, &MoveDirGoal);
	return MoveDirGoal;
}