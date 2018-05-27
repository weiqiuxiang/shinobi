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
	
	void StunSetting(CEnemy*);				//�X�^�����̏ڍאݒ�
private:
	int m_StunMotionTime;					//�X�^������
	int m_DecoveryTime;						//�d������
};

#endif