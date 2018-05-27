#ifndef CPLAYER_NEUTRAL_PATTERN_H_
#define CPLAYER_NEUTRAL_PATTERN_H_

#include "CPlayerPattern.h"

class CPlayerNeutralPattern : public CPlayerPattern
{
public:
	CPlayerNeutralPattern(CPlayer*,CPlayerPatternManager*);
	virtual ~CPlayerNeutralPattern() {}
	virtual bool InitParameter(CPlayer* pPlayer, CPlayerPatternManager*);
	virtual void Update(CPlayer*);
	virtual void Draw(void) {}

private:
	CPlayerNeutralPattern();

	bool CalcStickValueFromKeyboard(void);
	bool CalcStickValueFromController(void);
	bool IsRun(void);
	float CalcMovePower(void);
	void PushRunButtonTime(void);							//走るボタン押しっぱなしのフレーム数
	void MoveNoLockOn(CPlayer*, float MovePower);			//ロックオンしてない時の移動
	void MoveLockOn(CPlayer*, float MovePower);				//ロックオンしている時の移動
	void CalcFaceGoalDirNoLockOn(CPlayer*);					//ロックオンしない時のプレーヤーの目標向きベクトルを計算
	void CalcFaceGoalDirLockOn(CPlayer*);					//ロックオンしる時のプレーヤーの目標向きベクトルを計算
	void CalcFaceDir(CPlayer*);								//向きの計算
	void ChangeMotion(CPlayer*, float MovePower);			//モーションの変化

	bool ChangePattern(CPlayer*);							//パターンの変化判定
	bool ChangePatternToAttack(CPlayer*);					//攻撃パターンに変更
	bool ChangePatternToRolling(CPlayer*);					//ローリングパターンに変更
	bool ChangePatternToStep(CPlayer*);						//ステップパターンに変更
	bool ChangePatternToChangeWeaponShape(CPlayer*);		//武器変形パターンに変更

private:
	D3DXVECTOR2 m_StickValue;			//スティックの値
	int			m_PushRunButtonTime;	//走るボタン押しっぱなしの時間
	bool		m_IsRun;				//走っているか
};

#endif
