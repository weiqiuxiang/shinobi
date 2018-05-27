#ifndef CENEMY_DOG_JUMP_ATTACK_STATE_H_
#define CENEMY_DOG_JUMP_ATTACK_STATE_H_

#include "CEnemyState.h"
#include "main.h"

class CEnemyDogJumpAttackState : public CEnemyState
{
public:
	CEnemyDogJumpAttackState(CEnemy*, CEnemyStateManager*);
	~CEnemyDogJumpAttackState();

	virtual bool InitParameter(CEnemy*, CEnemyStateManager*);
	virtual int Update(CEnemy*);
	virtual void Draw(void);

private:
	CEnemyDogJumpAttackState() {}

	void AttackDetailAction(CEnemy*);
	D3DXVECTOR3 CalcDirectionToPlayer(CEnemy*);
	bool ChangeState(CEnemy*);
private:
	int m_KeyFrameOld;
	int m_MotionTime;
	int m_AttackRecovery;			//çUåÇçdíºéûä‘
};

#endif