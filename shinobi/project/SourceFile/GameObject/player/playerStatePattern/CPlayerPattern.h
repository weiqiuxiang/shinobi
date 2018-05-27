//���� : �v���[���[�̍s��state�p�^�[���C���^�t�F�[�X
#ifndef CPLAYER_PATTERN_H_
#define CPLAYER_PATTERN_H_

#include "main.h"
#include "CPlayerPatternManager.h"

//����錾
class CPlayer;

//�N���X��`
class CPlayerPattern
{
public:
	CPlayerPattern() {}
	virtual ~CPlayerPattern() {}
	virtual bool InitParameter(CPlayer* , CPlayerPatternManager*) { return true; }
	virtual void Update(CPlayer*) {}
	virtual void Draw(void) {}
protected:
	void PlayerGoalDir(CPlayer* pPlayer);
	void PlayerFaceDir(CPlayer* pPlayer);

protected:
	int			m_MotionNumOld;			//�O�̃��[�V�����ԍ�
	CPlayerPatternManager *m_PlayerPatternManager;
};

#endif