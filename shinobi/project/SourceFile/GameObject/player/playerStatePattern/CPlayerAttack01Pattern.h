#ifndef CPLAYER_ATTACK01_PATTERN_H_
#define CPLAYER_ATTACK01_PATTERN_H_

#include "CPlayerPattern.h"
#include "CAfterimageEffect.h"

class CPlayer;
class CPlayerAttack01Pattern : public CPlayerPattern
{
public:
	CPlayerAttack01Pattern(CPlayer* , CPlayerPatternManager*);
	~CPlayerAttack01Pattern();
	virtual bool InitParameter(CPlayer* , CPlayerPatternManager*);
	virtual void Update(CPlayer*);
	virtual void Draw(void);
private:
	CPlayerAttack01Pattern();

	void UpdateAttackEffectPosition(CPlayer*);	//����̋O�ՃG�t�F�N�g�̍X�V
	void UpdateWSwordAttackEffect(CPlayer*);	//�����̋O�ՃG�t�F�N�g
	void UpdateClaymoreAttackEffect(CPlayer*);	//�匕�̋O�ՃG�t�F�N�g
	void StopWeaponEffect(void);				//����G�t�F�N�g��~

	void AttackActionDetail(CPlayer*);			//���킲�Ƃ̍U�����[�V�����̋�̓���

	bool ChangePattern(CPlayer*);				//�p�^�[���̕ύX
	void ChangeDirWhenAttack(CPlayer*);			//�U�����̕����]��
	D3DXVECTOR2 KeyboardValue(void);

private:
	CAfterimageEffect	m_LeftWeaponATKEffect;
	CAfterimageEffect	m_RightWeaponATKEffect;
	int					m_nMotionTime;				//���[�V�����̌p������
};

#endif