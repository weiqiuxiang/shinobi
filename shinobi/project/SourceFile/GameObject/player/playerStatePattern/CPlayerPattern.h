//説明 : プレーヤーの行動stateパターンインタフェース
#ifndef CPLAYER_PATTERN_H_
#define CPLAYER_PATTERN_H_

#include "main.h"
#include "CPlayerPatternManager.h"

//先方宣言
class CPlayer;

//クラス定義
class CPlayerPattern
{
public:
	CPlayerPattern() {}
	virtual ~CPlayerPattern() {}
	virtual bool InitParameter(CPlayer* , CPlayerPatternManager*) { return true; }
	virtual void Update(CPlayer*) {}
	virtual void Draw(void) {}
protected:
	void PlayerGoalDir(CPlayer* pPlayer);
	void PlayerFaceDir(CPlayer* pPlayer);

protected:
	int			m_MotionNumOld;			//前のモーション番号
	CPlayerPatternManager *m_PlayerPatternManager;
};

#endif