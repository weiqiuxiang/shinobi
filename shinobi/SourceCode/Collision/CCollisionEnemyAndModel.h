#ifndef CCOLLISION_ENEMY_AND_MODEL
#define CCOLLISION_ENEMY_AND_MODEL

#include "main.h"

class CEnemy;

class CCollisionEnemyAndModel
{
public:
	static void Collision(CEnemy*,D3DXVECTOR3* OldPos);
private:
	static int MinDistance(float Dist[4]);		//l‚Â‚Ì‹——£‚Ì’†‚ÌÅ¬’l‚ğ‹‚ß‚é
};

#endif