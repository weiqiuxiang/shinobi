#ifndef CPLAYER_STEP_PATTERN_H_
#define CPLAYER_STEP_PATTERN_H_

#include "CPlayerPattern.h"

class CPlayer;
class CPlayerStepPattern : public CPlayerPattern
{
public:
	CPlayerStepPattern(CPlayer*,CPlayerPatternManager*);
	~CPlayerStepPattern() {}

	virtual bool InitParameter(CPlayer*, CPlayerPatternManager*);
	virtual void Update(CPlayer*);
	virtual void Draw(void) {}
private:
	CPlayerStepPattern();

	void MotionSet(CPlayer*);
	bool ChangePattern(CPlayer*);
private:
	int		m_nMotionTime;			//ƒ‚[ƒVƒ‡ƒ“‚ÌŒp‘±ŠÔ
};

#endif