#ifndef CENEMY_STATE_MANAGER_H_
#define CENEMY_STATE_MANAGER_H_

#include <array>

class CEnemy;
class CEnemyState;

#define ENEMY_NULL_STATE (-1)

class CEnemyStateManager
{
public:
	CEnemyStateManager() {}
	~CEnemyStateManager() {}

	virtual bool InitAllInstance(CEnemy* pEnemy) = 0;		//すべてのパターンインスタンス化
	virtual CEnemyState* GetState(int StateNum) = 0;	//パターン取得
	virtual void UninitAllInstance(void) = 0;

private:
};

#endif