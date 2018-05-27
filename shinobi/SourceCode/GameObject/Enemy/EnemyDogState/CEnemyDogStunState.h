#ifndef CENEMY_DOG_STUN_STATE_H_
#define CENEMY_DOG_STUN_STATE_H_

#include "CEnemyState.h"

class CEnemyDogStunState : public CEnemyState
{
public:
	CEnemyDogStunState(CEnemy*, CEnemyStateManager*);
	~CEnemyDogStunState();

	virtual bool InitParameter(CEnemy*, CEnemyStateManager*);
	virtual int Update(CEnemy*);
	virtual void Draw(void);

private:
	CEnemyDogStunState() {}
	
	void StunSetting(CEnemy*);				//スタン時の詳細設定
private:
	int m_StunMotionTime;					//スタン時間
	int m_DecoveryTime;						//硬直時間
};

#endif