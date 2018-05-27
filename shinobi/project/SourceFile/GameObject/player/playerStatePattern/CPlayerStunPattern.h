#ifndef CPLAYER_STUN_PATTERN_H_
#define CPLAYER_STUN_PATTERN_H_

#include "CPlayerPattern.h"

class CPlayer;
class CPlayerStunPattern : public CPlayerPattern
{
public:
	CPlayerStunPattern(CPlayer*, CPlayerPatternManager*);
	~CPlayerStunPattern();

	virtual bool InitParameter(CPlayer*, CPlayerPatternManager*);
	virtual void Update(CPlayer*);
	virtual void Draw(void);

private:
	CPlayerStunPattern() {}
	
	void StunSetting(CPlayer*);				//スタン時の詳細設定
private:
	int m_StunMotionTime;					//スタン時間
	int m_DecoveryTime;						//硬直時間
};

#endif