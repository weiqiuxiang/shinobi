#ifndef CENEMY_DOG_ATTACK_STATE_H_
#define CENEMY_DOG_ATTACK_STATE_H_

#include "CEnemyState.h"

class CEnemyDogAttackModeState : public CEnemyState
{
public:
	CEnemyDogAttackModeState(CEnemy*, CEnemyStateManager*);
	~CEnemyDogAttackModeState();

	virtual bool InitParameter(CEnemy*, CEnemyStateManager*);
	virtual int Update(CEnemy*);
	virtual void Draw(void);

private:
	CEnemyDogAttackModeState() {}

private:
	bool ChangeState(CEnemy*);
};

#endif