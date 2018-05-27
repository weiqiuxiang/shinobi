#ifndef CPLAYER_CHANGE_WEAPON_SHAPE_PATTERN_H_
#define CPLAYER_CHANGE_WEAPON_SHAPE_PATTERN_H_

#include "CPlayerPattern.h"

class CPlayer;
class CPlayerChangeWeaponShapePattern : public CPlayerPattern
{
public:
	CPlayerChangeWeaponShapePattern(CPlayer*, CPlayerPatternManager*);
	~CPlayerChangeWeaponShapePattern() {}

	virtual bool InitParameter(CPlayer*, CPlayerPatternManager*);
	virtual void Update(CPlayer*);
	virtual void Draw(void) {}
private:
	CPlayerChangeWeaponShapePattern();
	bool ChangePattern(CPlayer*);				//�p�^�[���̕ύX

private:
	int		m_nMotionTime;			//���[�V�����p������
	int		m_KeyFrameBefore;		//�O�̃L�[�t���[��
};

#endif