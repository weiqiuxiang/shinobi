#ifndef CPLAYER_ROLLING_PATTERN_H_
#define CPLAYER_ROLLING_PATTERN_H_

#include "CPlayerPattern.h"

class CPlayer;
class CPlayerRollingPattern : public CPlayerPattern
{
public:
	CPlayerRollingPattern(CPlayer*,CPlayerPatternManager*);
	~CPlayerRollingPattern() {}

	virtual bool InitParameter(CPlayer*, CPlayerPatternManager*);
	virtual void Update(CPlayer*);
	virtual void Draw(void) {}
private:
	CPlayerRollingPattern();
	bool ChangePattern(CPlayer*);

private:
	int		m_KeyFrameBefore;		//前のキーフレーム
	int		m_nMotionTime;			//モーションの継続時間
};

#endif