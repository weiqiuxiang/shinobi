#ifndef CENEMY_DOG_WAIT_STATE_H_
#define CENEMY_DOG_WAIT_STATE_H_

#include "CEnemyState.h"

class CEnemyDogWaitState : public CEnemyState
{
public:
	CEnemyDogWaitState(CEnemy*, CEnemyStateManager*);
	~CEnemyDogWaitState();

	virtual bool InitParameter(CEnemy*, CEnemyStateManager*);
	virtual int Update(CEnemy*);
	virtual void Draw(void);

private:
	CEnemyDogWaitState() {}

	bool ChangeState(CEnemy*);

private:
	int m_WaitTime;			//‘Ò‚¿ŽžŠÔ
};

#endif