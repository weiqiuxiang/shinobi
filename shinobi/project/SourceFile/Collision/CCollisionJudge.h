#pragma once
#ifndef CCOLLISION_JUDGE_H_
#define CCOLLISION_JUDGE_H_

#include "CPlayer.h"
#include "CEnemy.h"

class CCollisionJudge 
{
public:
	static void Update(void);
	static void Clear(void) {
		m_Player = nullptr;
	}

	//セッター
	static void SetPlayer(CPlayer *pPlayer) { m_Player = pPlayer; }
private:
	static CPlayer *m_Player;
};

#endif