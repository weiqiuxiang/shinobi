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
	static int MinDistance(float Dist[4]);		//l‚Â‚Ì‹——£‚Ì’†‚ÌÅ¬’l‚ğ‹‚ß‚é

private:
	static D3DXVECTOR3 m_PlayerBodyPosOld;
};

#endif