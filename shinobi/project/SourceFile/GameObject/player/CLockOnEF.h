//���� : �G�Ƀ��b�N�I���������̕\��
#pragma once
#ifndef LOCK_ON_EF_H_
#define LOCK_ON_EF_H_

#include "main.h"
#include "CScene2D.h"
#include "CModel.h"

class CLockOnEF
{
public:
	CLockOnEF();
	~CLockOnEF();

	bool Init(const D3DXVECTOR3& size,const char*TexFilePass);
	void Update(void);
	void Draw(void);
	void Uninit(void);

	//�Z�b�^�[
	void SetPos(const D3DXVECTOR3 &pos) { m_posWorld = pos; }
	void SetUse(bool Use) { m_bUse = Use; }

private:
	int			m_Alpha;		//�A���t�@
	float		m_scl;			//�g��{��
	D3DXVECTOR3	m_size;			//�T�C�Y
	D3DXVECTOR3	m_posWorld;		//���E���W�ʒu
	CScene2D	m_CScene2D;		//�`�惆�j�b�g
	bool		m_bUse;			//�g�p�t���O
};

#endif