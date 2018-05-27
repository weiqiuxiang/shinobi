#ifndef CENEMY_DOG_RUN_STATE_H_
#define CENEMY_DOG_RUN_STATE_H_

#include "CEnemyState.h"
#include "main.h"

class CEnemyDogRunState : public CEnemyState
{
public:
	CEnemyDogRunState(CEnemy*, CEnemyStateManager*);
	~CEnemyDogRunState();

	virtual bool InitParameter(CEnemy*, CEnemyStateManager*);
	virtual int Update(CEnemy*);
	virtual void Draw(void);

private:
	CEnemyDogRunState() {}

	D3DXVECTOR3 CalcDirectionToPlayer(CEnemy*);
	float CalcDistFromPlayer(CEnemy*);

	bool ChangeState(CEnemy*);

private:
	bool m_IsCanDoJumpAttack;
};

#endif