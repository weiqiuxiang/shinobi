#ifndef CENEMY_DOG_TOP_STATE_H_
#define CENEMY_DOG_TOP_STATE_H_

#include "CEnemyState.h"

class CEnemyDogTopState : public CEnemyState
{
public:
	CEnemyDogTopState(CEnemy*, CEnemyStateManager*);
	~CEnemyDogTopState();

	virtual bool InitParameter(CEnemy*, CEnemyStateManager*);
	virtual int Update(CEnemy*);
	virtual void Draw(void);

private:
	CEnemyDogTopState() {}
};

#endif