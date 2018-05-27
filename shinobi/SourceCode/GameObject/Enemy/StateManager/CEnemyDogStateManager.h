//�v���[���[�̃p�^�[�����Ǘ�����}�l�[�W���[
#ifndef CENEMY_DOG_STATE_MANAGER_H_
#define CENEMY_DOG_STATE_MANAGER_H_

#include "main.h"
#include <array>
#include "CEnemyStateManager.h"

class CEnemy;
class CEnemyState;

typedef enum
{
	ENEMY_DOG_TOP_STATE = 0,
	ENEMY_DOG_ATTACK_MODE_STATE,
	ENEMY_DOG_BITE_ATTACK_STATE,
	ENEMY_DOG_DEAD_STATE,
	ENEMY_DOG_RORATE_TO_PLAYER_STATE,
	ENEMY_DOG_JUMPATTACK_STATE,
	ENEMY_DOG_ATTACK_RECOVERY_STATE,
	ENEMY_DOG_PATROL_STATE,
	ENEMY_DOG_RUN_STATE,
	ENEMY_DOG_STUN_STATE,
	ENEMY_DOG_WAIT_STATE,
	ENEMY_DOG_WALK_STATE,
	ENEMY_DOG_STATE_MAX
}ENEMY_DOG_STATE;

class CEnemyDogStateManager : public CEnemyStateManager
{
public:
	CEnemyDogStateManager();
	~CEnemyDogStateManager();

	bool InitAllInstance(CEnemy* pEnemy);		//���ׂẴp�^�[���C���X�^���X��
	CEnemyState* GetState(int StateNum);		//�p�^�[���擾
	void UninitAllInstance(void);

private:
	void LinkParentAndChild(void);
private:
	std::array<CEnemyState*, ENEMY_DOG_STATE_MAX> m_StateArray;
};

#endif