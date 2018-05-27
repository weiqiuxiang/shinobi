#ifndef CENEMY_DOG_WALK_STATE_H_
#define CENEMY_DOG_WALK_STATE_H_

#include "CEnemyState.h"
#include "main.h"

class CEnemy;

class CEnemyDogWalkState : public CEnemyState
{
public:
	CEnemyDogWalkState(CEnemy*, CEnemyStateManager*);
	~CEnemyDogWalkState();

	virtual bool InitParameter(CEnemy*, CEnemyStateManager*);
	virtual int Update(CEnemy*);
	virtual void Draw(void);

private:
	CEnemyDogWalkState() {}

	D3DXVECTOR3 FindGoalPosition(CEnemy*);
	bool ChangeState(CEnemy*);

private:
	D3DXVECTOR3 m_GoalPos;
	bool		m_IsGetGoal;
};

#endif