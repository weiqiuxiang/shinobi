#ifndef CENEMY_DOG_PATROL_STATE_H_
#define CENEMY_DOG_PATROL_STATE_H_

#include "CEnemyState.h"

class CEnemyDogPatrolState : public CEnemyState
{
public:
	CEnemyDogPatrolState(CEnemy*, CEnemyStateManager*);
	~CEnemyDogPatrolState();

	virtual bool InitParameter(CEnemy*, CEnemyStateManager*);
	virtual int Update(CEnemy*);
	virtual void Draw(void);

private:
	CEnemyDogPatrolState() {}

	bool EyeSence(CEnemy*);					//目探知
	bool EarSence(CEnemy*);					//耳探知
	bool ChangeState(CEnemy*);				//自分のステートを変更

private:

};

#endif