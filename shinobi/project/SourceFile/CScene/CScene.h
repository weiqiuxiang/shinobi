#pragma once
#ifndef _CSCENE_H_
#define _CSCENE_H_

#include "main.h"

class CScene
{
public:
	CScene();
	~CScene();

	//Init,Uninit,Update,Draw�֐�
	virtual HRESULT Init(void);
	virtual HRESULT Init(const D3DXVECTOR3 &Pos, const D3DXVECTOR3 &pSize , const char* filePass);
	virtual HRESULT Init(SRP &PosRotScl , const char* filePass);
	virtual void Uninit(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;

	//�Q�b�^�[
	D3DXVECTOR3 GetPosition(void);
	PRIORITY GetPriority(void) { return m_priority; }                     //�`��D��x�擾

	//�Z�b�^�[
	void SetPosition(const D3DXVECTOR3& pos) { m_pos = pos; }
	void SetPositionX(float Value) { m_pos.x = Value; }
	void SetPositionY(float Value) { m_pos.y = Value; }
	void SetPositionZ(float Value) { m_pos.z = Value; }
	void SetPriority(const PRIORITY priority) { m_priority = priority; }  //�`��D��x�ݒ�

	//���̊֐�
	void MoveX(float fDistance/*�ړ�����*/) { m_pos.x += fDistance; }
	void MoveY(float fDistance/*�ړ�����*/) { m_pos.y += fDistance; }
	void MoveZ(float fDistance/*�ړ�����*/) { m_pos.z += fDistance; }
	void MoveXZ(const D3DXVECTOR2 &fDirection/*�ړ�����*/, float fDistance/*�ړ�����*/);
	void Move3D(const D3DXVECTOR3 &fDirection/*�ړ�����*/, float fDistance/*�ړ�����*/);
protected:
	D3DXVECTOR3 m_pos;
	PRIORITY m_priority;
};

#endif