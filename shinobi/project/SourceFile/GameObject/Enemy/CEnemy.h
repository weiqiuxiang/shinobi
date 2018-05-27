#pragma once
#ifndef _CENEMY_H_
#define _CENEMY_H_

//���l(�G)�N���X
#include "CHuman.h"
#include "CPathPoint.h"
#include <list>
#include <algorithm>
#include "common.h"

class CPlayer;
class CEnemyControl;
class CEnemyState;
class CEnemyStateManager;

class CEnemy : public CHuman
{
public:
	typedef enum 
	{
		TYPE_DOG = 0,			//��
		TYPE_VILLAGER,			//���l
		TYPE_BIGMAN,			//��j
	}ENEMY_TYPE;

	CEnemy();
	~CEnemy();

	virtual void Init(ENEMY_TYPE type,const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const char* MotionPass);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void UpdateEditor(void);
	virtual void Draw(void);
	virtual void DrawPath(void);
	virtual void DrawOnDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje);
	void DrawLineBetweenPathPoint(void);

	//�Z�b�^�[
	void SetPlayer(CPlayer* pPlayer);
	void SetPathNumberNow(CPathPoint* pPathPoint) { m_PathNumberNow = pPathPoint; }
	void SetFlag_IsPathPointIsAscendingOrder(bool Flag) { m_IsPathPointIsAscendingOrder = Flag; }	//true�Ȃ珸����\��

	//�Z�b�^�[(�X�^���֘A)
	void SetNoDamageTime(int Time) { m_NoDamageTime = Time; }
	void SetStunDir(const D3DXVECTOR3& Stundir) { m_StunDir = Stundir; }
	void SetAttackType(ATTACK_TYPE_ENEMY attackType) { m_AttackType = attackType; }
	void SetDamageType(ATTACK_TYPE_PLAYER DamageType) { m_DamageType = DamageType; }
	void SetIsBeAttackedFlag(bool flag) { m_IsBeAttacked = flag; }
	void SetUseID(int ID) { m_UseID = ID; }

	//�Q�b�^�[
	int GetLife(void) { return m_nLife; }
	bool GetDeadFlag(void) { return m_DeadFlag; }
	CPathPoint* GetPathNumberNow(void) { return m_PathNumberNow; }
	CPlayer* GetPlayer(void);
	std::list<CPathPoint*>::iterator GetPathPointListBegin(void) { return m_PathPoints.begin(); }
	std::list<CPathPoint*>::iterator GetPathPointListEnd(void) { return m_PathPoints.end(); }
	std::list<CPathPoint*>::iterator GetPathPointListNumber(int Number);
	
	int GetPathPointListSize(void) { return m_PathPoints.size(); }
	std::list<CPathPoint*>::iterator GetPathPointListPrev(CPathPoint* pPoint, int Num);			//���̃|�C���^�̑O
	std::list<CPathPoint*>::iterator GetPathPointListNext(CPathPoint* pPoint, int Num);			//���̃|�C���^�̌��
	bool GetFlag_IsPathPointIsAscendingOrder(void) { return m_IsPathPointIsAscendingOrder; }

	//�Q�b�^�[(�X�^���֘A)
	int GetNoDamageTime(void) { return m_NoDamageTime; }
	D3DXVECTOR3 GetStunDir(void) { return m_StunDir; }
	ATTACK_TYPE_ENEMY GetAttackType(void) { return m_AttackType; }
	ATTACK_TYPE_PLAYER GetDamageType(void) { return m_DamageType; }
	bool GetIsBeAttackedFlag(void) { return m_IsBeAttacked; }
	ENEMY_TYPE GetEnemyType(void) { return m_EnemyType; }
	int GetUseID(void) { return m_UseID; }

	//���̊֐�
	void AddLife(int nLife) {
		m_nLife += nLife; 
		m_nLife = max(0, m_nLife);
	}
	void AddPathPoint(const D3DXVECTOR3& pos, int StopFrame);	//���񃋁[�g�|�C���g�̒ǉ�
	void DeletePathPointUsePointer(CPathPoint* pPath);			//�n���ꂽ�|�C���^�Ɋ�Â��폜
	void InsertPathPointUsePointer(const D3DXVECTOR3& pos, int StopFrame, CPathPoint* pPath);
	void DeletePathPoint(void);									//���񃋁[�g�|�C���g���X�g�̖�������v�f���폜
	
	bool FindElementInPathPointList(CPathPoint* pPathPoint) {
		auto Found = std::find(m_PathPoints.begin(), m_PathPoints.end(), pPathPoint);
		if (Found == m_PathPoints.end()) return false;
		return true;
	}

	void EnableHitCircle(void);			//�U�����蔻�苅��L����
	void DisableHitCircle(void);		//�U�����蔻�苅�𖳌���

private:
	//�����o�֐�
	void InitPathPoint(void);
	void UpdatePathPoint(void);
	void DrawPathPoint(void);
	void UninitPathPoint(void);

	void EnemyTypeToSetColliCircle(void);		//�G�̃^�C�v�ɂ���ē��蔻�苅�̐ݒu

	int						m_nLife;
	bool					m_DeadFlag;
	CEnemyControl *			m_Controller;
	std::list<CPathPoint*>	m_PathPoints;					//���񃋁[�g�̃|�C���g
	CPathPoint*				m_PathNumberNow;
	bool					m_IsPathPointIsAscendingOrder;	//����|�C���g�͏����Ȃ�true
	ENEMY_TYPE				m_EnemyType;					//�G�^�C�v
	D3DXVECTOR3				m_InitPos;						//�������W

	//�_���[�W�n�ϐ�
	int						m_NoDamageTime;					//�_���[�W��ʂ�Ȃ�����
	D3DXVECTOR3				m_StunDir;						//�U�����󂯂鎞�̃X�^������
	ATTACK_TYPE_PLAYER		m_DamageType;					//����̍U���^�C�v
	ATTACK_TYPE_ENEMY		m_AttackType;					//�����̍U���^�C�v
	bool					m_IsBeAttacked;					//�U�����ꂽ���̃t���O

	CEnemyState*			m_pEnemyState;					//�X�e�[�g
	CEnemyStateManager*		m_pEnemyStateManager;			//�X�e�[�g�}�l�W���[
	int						m_UseID;						//�g�p����邽�߂�ID
};

#endif