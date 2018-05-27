#pragma once
#ifndef CPARTICLE3D_H_
#define CPARTICLE3D_H_

#include "main.h"
#include "CScene3D.h"

class CParticle3D
{
public:
	CParticle3D();
	~CParticle3D();

	//Init,Uninit,Update,Draw
	void InitCScene3D(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &pSize, const char *BaseFilePass, const char *EffectFilePass);     //Scene3D�̐ݒ蕔��
	void InitParticle(float Mass,const D3DXVECTOR3& Speed,int Life);			//Particle�̐ݒ蕔��
	void InitParticleDrawTime(int DrawTimes_Base,int DrawTimes_Effect);			//�`��񐔐ݒ�
	void Uninit(void);
	void Update(void);
	void Draw(void);

	//�Z�b�^�[
	void SetMass(float Mass) { m_fMass = Mass; }   //���ʐݒ�
	void SetSpeed(const D3DXVECTOR3& Speed) { m_Speed = Speed; }      //���x�ݒ�
	void SetLife(int nLife) { m_nLife = nLife; }
	void SetPower(const D3DXVECTOR3& power) { m_Power = power; }
	void SetParticleBaseRGBA(RGBA rgba) { m_CScene3D_Base.SetRGBA(rgba); }
	void SetParticleEffectRGBA(RGBA rgba) { m_CScene3D_Effect.SetRGBA(rgba); }

	//�Q�b�^�[
	float GetMass(void) { return m_fMass; };
	D3DXVECTOR3 GetSpeed(void) { return m_Speed; }
	int GetLife(void) { return m_nLife; }

	//���̊֐�
	void AddSpeed(const D3DXVECTOR3& MoveDir, float AddSpeed);       //�X�s�[�h�����X�ω�������

private:
	CScene3D m_CScene3D_Base;       //CScene3D�`�敔��(�p�[�e�B�N���̃x�[�X����)
	CScene3D m_CScene3D_Effect;     //CScene3D�`�敔��(�p�[�e�B�N���̃G�t�F�N�g����)
	int      m_DrawTimes_Base;      //�`���
	int      m_DrawTimes_Effect;    //�`���
	float m_fMass;              //����
	D3DXVECTOR3 m_Speed;             //���x
	D3DXVECTOR3 m_Power;             //�O��
	int   m_nLife;              //���C�t
};

#endif