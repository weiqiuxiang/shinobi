#pragma once
#ifndef CCOLLISION_PLAYER_AND_ENEMY
#define CCOLLISION_PLAYER_AND_ENEMY

#include "main.h"
#include "CPlayer.h"
#include "CEnemy.h"

//�}�N��
#define BLOOD_POSITION_MAX (30)

//�O���錾
class CCollisionCircle;

typedef struct 
{
	D3DXVECTOR3			AttackCircleOldPos;	//�U�����̌Â����W
	D3DXVECTOR3			DamageCircleOldPos;	//�_�[���W���̌Â����W
	CCollisionCircle*	AttackCircle;		//�U�����̋�
	CCollisionCircle*	DamageCircle;		//�_�[���W���̋�
	int					Time;				//�v�Z��
	bool				IsUse;				//�g�p���ꂽ��
}BLOOD_POSITION;

class CColliPlayerEnemy
{
public:
	static void Init(void);
	static void UpdateBlood(void);
	static void GetPlayerEnemyDataAndColliCheck(void);				//�v���[���[��G�̃f�[�^(�|�C���^)�擾�Ɠ��蔻��
	static void GetEnemiesDataAndColliCheck(void);					//�G�ƓG�̃f�[�^(�|�C���^)�擾�Ɠ��蔻��

	//�Z�b�^�[
	/*test*/
	static void SetPlayer(CPlayer *Player);
	static void SetEnemy(CEnemy *Enemy);
	static void SetEnemyOther(CEnemy *Enemy);
	/*test*/
private:
	//�����o�֐�

	//�v���[���[�ƓG�̓��蔻��֐�
	static bool CanCalcPlayerEnemy(void);
	static void BodyCollisionPlayerEnemy(void);						//�v���[���[�ƓG�̏Փ˔���
	static void PlayerHitEnemy(void);								//�v���[���[�͓G�Ƀq�b�g���Ă��邪�ǂ���
	static void EnemyHitPlayer(void);								//�G�̓v���[���[�Ƀq�b�g���Ă��邪�ǂ���

	//�G�ƓG�̏Փ˔���
	static void BodyCollisionEnemies(void);							//�G�ƓG�̏Փ˔���
	static void CreateBloodPosition(CCollisionCircle* Attack, CCollisionCircle* Damage);

	//�����o�ϐ�
	static CPlayer* m_Player;
	static CEnemy* m_Enemy;
	static CEnemy* m_EnemyOtherEnemy;
	static float m_CoffiPushOut;									//�����o���W��
	static BLOOD_POSITION m_BloodPosition[BLOOD_POSITION_MAX];		//�����Ԃ��ʒu
};

#endif