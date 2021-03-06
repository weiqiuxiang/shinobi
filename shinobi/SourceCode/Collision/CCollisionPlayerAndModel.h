#ifndef CCOLLISION_PLAYER_AND_MODEL
#define CCOLLISION_PLAYER_AND_MODEL

#include "main.h"

class CPlayer;

class CCollisionPlayerAndModel
{
public:
	static void Init(const D3DXVECTOR3& PlayerBodyPosOld);
	static void Update(CPlayer*);

private:
	static int MinDistance(float Dist[4]);		//四つの距離の中の最小値を求める

private:
	static D3DXVECTOR3 m_PlayerBodyPosOld;
};

#endif