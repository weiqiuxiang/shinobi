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
	
	void StunSetting(CPlayer*);				//�X�^�����̏ڍאݒ�
private:
	int m_StunMotionTime;					//�X�^������
	int m_DecoveryTime;						//�d������
};

#endif