#ifndef CENEMY_DOG_DEAD_STATE_H_
#define CENEMY_DOG_DEAD_STATE_H_

#include "CEnemyState.h"

class CEnemyDogDeadState : public CEnemyState
{
public:
	CEnemyDogDeadState(CEnemy*, CEnemyStateManager*);
	~CEnemyDogDeadState();

	virtual bool InitParameter(CEnemy*, CEnemyStateManager*);
	virtual int Update(CEnemy*);
	virtual void Draw(void);

private:
	CEnemyDogDeadState() {}
};

#endif