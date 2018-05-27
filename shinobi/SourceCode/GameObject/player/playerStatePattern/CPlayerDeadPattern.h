#ifndef CPLAYER_DEAD_PATTERN_H_
#define CPLAYER_DEAD_PATTERN_H_

#include "CPlayerPattern.h"

class CPlayerDeadPattern : public CPlayerPattern
{
public:
	CPlayerDeadPattern(CPlayer*, CPlayerPatternManager*);
	~CPlayerDeadPattern();

	virtual bool InitParameter(CPlayer*, CPlayerPatternManager*);
	virtual void Update(CPlayer*);
	virtual void Draw(void);

private:
	CPlayerDeadPattern() {}
};

#endif