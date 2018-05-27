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
	bool ChangePattern(CPlayer*);				//パターンの変更

private:
	int		m_nMotionTime;			//モーション継続時間
	int		m_KeyFrameBefore;		//前のキーフレーム
};

#endif