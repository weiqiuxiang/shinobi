#ifndef CENEMY_DOG_RORATE_TO_PLAYER_STATE_H_
#define CENEMY_DOG_RORATE_TO_PLAYER_STATE_H_

#include "CEnemyState.h"

class CEnemyDogRorateToPlayerState : public CEnemyState
{
public:
	CEnemyDogRorateToPlayerState(CEnemy*, CEnemyStateManager*);
	~CEnemyDogRorateToPlayerState();

	virtual bool InitParameter(CEnemy*, CEnemyStateManager*);
	virtual int Update(CEnemy*);
	virtual void Draw(void);
	
private:
	CEnemyDogRorateToPlayerState() {}

	bool IsMissPlayer(CEnemy* pEnemy);
	bool RorateToPlayer(CEnemy* pEnemy);
};

#endif