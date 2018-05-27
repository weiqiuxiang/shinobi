#pragma once
#ifndef CPARTICLEMAKER_H_
#define CPARTICLEMAKER_H_

#include "main.h"
#include "CParticle3D.h"

#define PARTICLE3D_MAX (1024)

class CEnvironment     //���W���N���X
{
public:
	CEnvironment();
	~CEnvironment();

	//�Z�b�^�[
	void SetEnvironment(const D3DXVECTOR3& WindDirect, float WindPower, float fMass, float GravityAcc, float CoeffiResist);   //�p�����[�^�ݒ�
	void SetWindDir(const D3DXVECTOR3& WindDir) { D3DXVec3Normalize(&m_WindDirect, &WindDir); }
	void SetWindPower(float WindPower) { m_WindPower = WindPower; }
	void SetGravityAcc(float GravityAcc) { m_fGravityAcc = GravityAcc; }
	void SetCoeffiResist(float fCoeffiResist) { m_fCoeffiResist = max(0.0f, fCoeffiResist); }
	void SetMass(float fMass) { m_fMass = max(0,fMass); }

	//�Q�b�^�[
	D3DXVECTOR3 GetWindDir(void) { return m_WindDirect; }
	float	GetWindPower(void) { return m_WindPower; }
	float GetGravityAcc(void) { return m_fGravityAcc; }
	float GetCoeffiResist(void) { return m_fCoeffiResist; }
	float GetMass(void) { return m_fMass; }

private:
	//�����o�ϐ�
	D3DXVECTOR3 m_WindDirect;        //������
	float    m_WindPower;            //����
	float    m_fGravityAcc;          //�d�͉����x
	float    m_fCoeffiResist;        //��R�W��
	float    m_fMass;                //����
};

class CShootPara     //�V���b�g�p�����[�^
{
public:
	CShootPara();
	~CShootPara();

	//�Z�b�^�[
	void SetShootPara(const D3DXVECTOR3& ShootDir,const D3DXVECTOR2& RangeAngle,float Speed);
	void SetShootDir(const D3DXVECTOR3& ShootDir){ D3DXVec3Normalize(&m_ShootDir, &ShootDir); }
	void SetShootRangeAngle(const D3DXVECTOR2& RangeAngle);
	void SetShootRangeAngleX(float RangeAngleX) { m_fShootRangeAngle.x = max(0, RangeAngleX); m_fShootRangeAngle.x = min(D3DX_PI, RangeAngleX); }
	void SetShootRangeAngleY(float RangeAngleY) { m_fShootRangeAngle.y = max(0, RangeAngleY); m_fShootRangeAngle.y = min(D3DX_PI, RangeAngleY); }
	void SetShootSpeed(float fSpeed) { m_fSpeed = fSpeed; }

	//�Q�b�^�[
	D3DXVECTOR3 GetShootDir(void) { return m_ShootDir; }
	D3DXVECTOR2 GetShootRangeAngle(void) { return m_fShootRangeAngle; }
	float GetShootSpeed(void) { return m_fSpeed; }

private:
	//�����o�ϐ�
	D3DXVECTOR3 m_ShootDir;          //���ˊ����
	D3DXVECTOR2 m_fShootRangeAngle;  //���˔͈͊p�x(x�͐���,y�͐���)
	float    m_fSpeed;               //���ˑ��x
};

class CParticleMaker
{
public:
	CParticleMaker();
	~CParticleMaker();

	void Uninit(void);
	void Update(void);
	void Draw(void);
	
	//�Z�b�^�[
	void SetMakerBase(const D3DXVECTOR3& pos, const D3DXVECTOR3& size, float PerFrameNum,int nLife, const char* BaseTexPass, const char* EffectTexPass);
	void SetEnvironment(const D3DXVECTOR3& WindDirect, float WindPower,float fMass, float GravityAcc, float CoeffiResist);
	void SetShootPara(const D3DXVECTOR3& ShootDir, const D3DXVECTOR2& RangeAngle, float Speed);
	void SetParticleDrawTime(int DrawTimes_Base, int DrawTimes_Effect){ m_DrawTimes_Base = max(1, DrawTimes_Base); m_DrawTimes_Effect = max(1, DrawTimes_Effect);}
	void SetPerFrameNum(double PerNum) { m_fFramePerNum = (float)max(0.01,PerNum); }
	void SetPosition(const D3DXVECTOR3& pos) { m_pos = pos; }
	void SetParticleBaseRGBA(RGBA rgba) { m_RGBA_Base = rgba; }
	void SetParticleEffectRGBA(RGBA rgba) { m_RGBA_Effect = rgba; }

	//�Q�b�^�[
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	CEnvironment* GetEnvironment(void) { return &m_Environment; };
	CShootPara* GetShootPara(void){ return &m_ShootPara; }

	//���̊֐�
	void ChangeSize(const D3DXVECTOR3& size) {m_size.x = max(0, size.x); m_size.y = max(0, size.y);}
	void ChangeSizeX(float SizeX) { m_size.x = max(0,SizeX); }
	void ChangeSizeY(float SizeY) { m_size.y = max(0,SizeY); }

private:
	void DeleteDieParticle(void);         //���C�t��0�ȉ��̃p�[�e�B�N���폜
	void CreateParticle(void);            //�p�[�e�B�N�������
	void CreateParticleFramePerNum(void);
	D3DXVECTOR3 ShootDirCalc(void);       //�p�[�e�B�N���i�s�����v�Z
	D3DXVECTOR3 PowerCalc(CParticle3D* pParticle);  //�󂯂�͌v�Z

	D3DXVECTOR3 m_pos;                  //�ʒu
	D3DXVECTOR3 m_size;                 //�p�[�e�B�N���̃T�C�Y
	RGBA        m_RGBA_Base;            //�J���[
	RGBA        m_RGBA_Effect;          //�J���[
	int         m_nLife;                //�����l
	char        m_aBaseTexPass[256];     //�e�N�X�`���p�X
	char        m_aEffectTexPass[256];   //�e�N�X�`���p�X
	int      m_DrawTimes_Base;           //�`���
	int      m_DrawTimes_Effect;         //�`���
	float       m_fFramePerNum;          //���t���[��1���˂��邩
	float       m_fCreateCounter;        //�t���[���J�E���^�[
	CEnvironment m_Environment;         //���p�����[�^
	CShootPara m_ShootPara;             //���˃p�����[�^
	CParticle3D *m_aParticle[PARTICLE3D_MAX];             //�p�[�e�B�N���}�K�W��
};

#endif