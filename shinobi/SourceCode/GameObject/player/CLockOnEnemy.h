#pragma once
#ifndef CLOCKON_ENEMY_H_
#define CLOCKON_ENEMY_H_

#include "CEnemy.h"
#include "CPlayer.h"

#define LOCKON_DISTANCE (50.0f)					//ƒƒbƒNƒIƒ“‚Å‚«‚é‹——£

class CLockOnEnemy
{
public:
	static CEnemy* LockOnEnemy(CPlayer* pPlayer);
	static CEnemy* ChangeTarget(CPlayer* pPlayer, CEnemy *pEnemy, bool bDir);
private:
};

#endif