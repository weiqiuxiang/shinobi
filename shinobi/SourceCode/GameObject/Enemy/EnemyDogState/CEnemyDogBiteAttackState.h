#ifndef CENEMY_DOG_BITE_ATTACK_STATE_H_
#define CENEMY_DOG_BITE_ATTACK_STATE_H_

#include "main.h"
#include "CEnemyState.h"

class CEnemyDogBiteAttackState : public CEnemyState
{
public:
	CEnemyDogBiteAttackState(CEnemy*, CEnemyStateManager*);
	~CEnemyDogBiteAttackState();

	virtual bool InitParameter(CEnemy*, CEnemyStateManager*);
	virtual int Update(CEnemy*);
	virtual void Draw(void);

private:
	CEnemyDogBiteAttackState() {}

	D3DXVECTOR3 CalcDirectionToPlayer(CEnemy* pEnemy);
	float CalcDistFromPlayer(CEnemy*);
	void AttackActionDetail(CEnemy*);
private:
	int m_BiteAttackMotionTime;				//噛み攻撃のモーション時間
	int m_AttackKeyFrameOld;				//前の攻撃モーション中のキーフレーム
	int m_AttackRecovery;					//攻撃硬直時間
};

#endif