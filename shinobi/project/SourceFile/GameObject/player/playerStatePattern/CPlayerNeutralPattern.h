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
	void PushRunButtonTime(void);							//����{�^���������ςȂ��̃t���[����
	void MoveNoLockOn(CPlayer*, float MovePower);			//���b�N�I�����ĂȂ����̈ړ�
	void MoveLockOn(CPlayer*, float MovePower);				//���b�N�I�����Ă��鎞�̈ړ�
	void CalcFaceGoalDirNoLockOn(CPlayer*);					//���b�N�I�����Ȃ����̃v���[���[�̖ڕW�����x�N�g�����v�Z
	void CalcFaceGoalDirLockOn(CPlayer*);					//���b�N�I�����鎞�̃v���[���[�̖ڕW�����x�N�g�����v�Z
	void CalcFaceDir(CPlayer*);								//�����̌v�Z
	void ChangeMotion(CPlayer*, float MovePower);			//���[�V�����̕ω�

	bool ChangePattern(CPlayer*);							//�p�^�[���̕ω�����
	bool ChangePatternToAttack(CPlayer*);					//�U���p�^�[���ɕύX
	bool ChangePatternToRolling(CPlayer*);					//���[�����O�p�^�[���ɕύX
	bool ChangePatternToStep(CPlayer*);						//�X�e�b�v�p�^�[���ɕύX
	bool ChangePatternToChangeWeaponShape(CPlayer*);		//����ό`�p�^�[���ɕύX

private:
	D3DXVECTOR2 m_StickValue;			//�X�e�B�b�N�̒l
	int			m_PushRunButtonTime;	//����{�^���������ςȂ��̎���
	bool		m_IsRun;				//�����Ă��邩
};

#endif
