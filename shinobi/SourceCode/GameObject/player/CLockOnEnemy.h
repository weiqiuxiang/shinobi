#pragma once
#ifndef CLOCKON_ENEMY_H_
#define CLOCKON_ENEMY_H_

#include "CEnemy.h"
#include "CPlayer.h"

#define LOCKON_DISTANCE (50.0f)					//���b�N�I���ł��鋗��

class CLockOnEnemy
{
public:
	static CEnemy* LockOnEnemy(CPlayer* pPlayer);
	static CEnemy* ChangeTarget(CPlayer* pPlayer, CEnemy *pEnemy, bool bDir);
private:
};

#endif