#ifndef CCOLLISION_ENEMY_AND_MODEL
#define CCOLLISION_ENEMY_AND_MODEL

#include "main.h"

class CEnemy;

class CCollisionEnemyAndModel
{
public:
	static void Collision(CEnemy*,D3DXVECTOR3* OldPos);
private:
	static int MinDistance(float Dist[4]);		//四つの距離の中の最小値を求める
};

#endif