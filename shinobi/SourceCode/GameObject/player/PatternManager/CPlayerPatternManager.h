//プレーヤーのパターンを管理するマネージャー
#ifndef CPLAYER_PATTERN_MANAGER_H_
#define CPLAYER_PATTERN_MANAGER_H_

#include "main.h"
#include <array>

class CPlayer;
class CPlayerPattern;

typedef enum
{
	PLAYER_NEUTRAL_PATTERN = 0,
	PLAYER_ATTACK01_PATTERN,
	PLAYER_ATTACK02_PATTERN,
	PLAYER_ATTACK03_PATTERN,
	PLAYER_ATTACK04_PATTERN,
	PLAYER_ROLLING_PATTERN,
	PLAYER_STEP_PATTERN,
	PLAYER_CHANGE_WEAPON_SHAPE_PATTERN,
	PLAYER_STUN_PATTERN,
	PLAYER_DEAD_PATTERN,
	PLAYER_PATTERN_MAX
}PLAYER_PATTERN;

class CPlayerPatternManager
{
public:
	CPlayerPatternManager();
	~CPlayerPatternManager();

	bool InitAllInstance(CPlayer* pPlayer);		//すべてのパターンインスタンス化
	CPlayerPattern* GetPattern(int PatternNum);	//パターン取得
	void UninitAllInstance(void);
private:
	std::array<CPlayerPattern*, PLAYER_PATTERN_MAX> m_PatternArray;
};

#endif