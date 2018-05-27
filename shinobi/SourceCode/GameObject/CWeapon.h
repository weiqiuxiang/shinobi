#pragma once
#ifndef _CWEAPON_H_
#define _CWEAPON_H_

#include "CHuman.h"

class CWeapon : public CHuman
{
public:
	typedef enum
	{
		MOTION_DEFAULT,			//�f�t�H���g���
		MOTION_CHANGE			//�ω���̏��
	}MOTION_MODE;

	CWeapon();
	~CWeapon();

	virtual void Init(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const char* MotionPass);
	virtual void Uninit(void);
	virtual void Update(void);
	virtual void Draw(void);
	
	//�Q�b�^�[
	int GetOtherParentID(void) { return m_OtherParentID; }
	D3DXVECTOR3 GetOffset(void) { return m_posOffset; }
	D3DXMATRIX GetPartMtx(int PartID) { return m_ModelMotion.GetPartMtx(PartID); }

	//�Z�b�^�[
	void SetParentID(int ID) { m_OtherParentID = ID; }
	void SetOffset(const D3DXVECTOR3 &Offset) { m_posOffset = Offset; }

	//���̊֐�
	void EnableHitCircle(void);
	void DisableHitCircle(void);

private:
	D3DXVECTOR3 m_posOffset;					//�I�t�Z�b�g
	int m_OtherParentID;						//�ʂ̃I�u�W�F�N�g�̃p�[�c��ID
};

#endif