#ifndef CENEMY_DOG_ATTACK_RECOVERY_STATE_H_
#define CENEMY_DOG_ATTACK_RECOVERY_STATE_H_

#include "CEnemyState.h"

class CEnemyDogAttackRecoveryState : public CEnemyState
{
public:
	CEnemyDogAttackRecoveryState(CEnemy*, CEnemyStateManager*);
	~CEnemyDogAttackRecoveryState();

	virtual bool InitParameter(CEnemy*, CEnemyStateManager*);
	virtual int Update(CEnemy*);
	virtual void Draw(void);

private:
	CEnemyDogAttackRecoveryState() {}

	bool ChangeState(CEnemy*);

private:
	int m_WaitTime;			//‘Ò‚¿ŽžŠÔ
};

#endif