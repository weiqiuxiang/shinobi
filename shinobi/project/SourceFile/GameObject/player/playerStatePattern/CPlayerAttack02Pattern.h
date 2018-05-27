#ifndef CPLAYER_ATTACK02_PATTERN_H_
#define CPLAYER_ATTACK02_PATTERN_H_

#include "CPlayerPattern.h"
#include "CAfterimageEffect.h"

class CPlayer;
class CPlayerAttack02Pattern : public CPlayerPattern
{
public:
	CPlayerAttack02Pattern(CPlayer* , CPlayerPatternManager*);
	~CPlayerAttack02Pattern();
	virtual bool InitParameter(CPlayer* , CPlayerPatternManager*);
	virtual void Update(CPlayer*);
	virtual void Draw(void);
private:
	CPlayerAttack02Pattern();

	void UpdateAttackEffectPosition(CPlayer*);	//武器の軌跡エフェクトの更新
	void UpdateWSwordAttackEffect(CPlayer*);	//両刀の軌跡エフェクト
	void UpdateClaymoreAttackEffect(CPlayer*);	//大剣の軌跡エフェクト
	void StopWeaponEffect(void);				//武器エフェクト停止

	void AttackActionDetail(CPlayer*);			//武器ごとの攻撃モーションの具体動作

	bool ChangePattern(CPlayer*);				//パターンの変更
	void ChangeDirWhenAttack(CPlayer*);			//攻撃時の方向転換
	D3DXVECTOR2 KeyboardValue(void);
private:
	CAfterimageEffect	m_LeftWeaponATKEffect;
	CAfterimageEffect	m_RightWeaponATKEffect;
	int					m_nMotionTime;				//モーションの継続時間
};

#endif