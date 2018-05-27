#ifndef CENEMY_STATE_H_
#define CENEMY_STATE_H_

//����錾
class CEnemy;
class CEnemyStateManager;

class CEnemyState
{
public:
	CEnemyState(CEnemy*, CEnemyStateManager*) {
	}
	~CEnemyState() {}

	//�I�[�o�[���C�h����
	virtual bool InitParameter(CEnemy*, CEnemyStateManager*) { return true; }
	virtual int Update(CEnemy*) { return -1; }			//�߂�l : �e�I�u�W�F�N�g�̑J�ڐ�X�e�[�g�̔ԍ���Ԃ��A�J�ڂ͂Ȃ��ꍇ-1��Ԃ�
	virtual void Draw(void) {}

	//�Z�b�^�[
	inline void SetState(CEnemy* pEnemy, CEnemyStateManager* pEnemyStateManager,CEnemyState* pState) {
		m_ChildState = pState;					//�����̎q�Ƃ��ɐݒ�
		if (m_ChildState != nullptr) {
			m_ChildState->InitParameter(pEnemy, pEnemyStateManager);
			m_ChildState->SetParentState(this);	//�q���̐e�������ɐݒ�
		}
	}
	inline void SetParentState(CEnemyState* pState) {
		m_ParentState = pState;
	}

protected:
	CEnemyState() {}

protected:
	CEnemyState*			m_ParentState;
	CEnemyState*			m_ChildState;
	CEnemyStateManager*		m_EnemyStateManager;
	int						m_TransitionStateNum;		//�J�ڐ�X�e�[�g�ԍ�
};

#endif