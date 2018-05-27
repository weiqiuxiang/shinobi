//�����Ԃ��p�p�[�e�B�N��
#ifndef CBLOOD_H_
#define CBLOOD_H_

#include "main.h"
#include "CScene3D.h"

class CBlood : public CScene3D
{
public:
	CBlood();
	~CBlood();

	HRESULT Init(const SRP &worldMtx, const D3DXVECTOR3 &pSize, const char *filePass);
	void Update(void);
	void Draw(void);
	void Uninit(void);

	//�Z�b�^�[
	void SetGravityAcc(float acc) { m_GravityAcc = acc; }
	void SetTime(int Time) { m_Time = Time; }
	void SetIsUse(bool bUse) { m_IsUse = bUse; }
	void SetCoffi(float Coffi) { m_Coffi = Coffi; }

	//�Q�b�^�[
	bool GetIsUse(void) { return m_IsUse; }

	//���̊֐�
	void AddPower(const D3DXVECTOR3& power) { m_Power += power; }

private:
	D3DXVECTOR3		m_Power;		//�O��
	D3DXVECTOR3		m_Speed;		//���x
	float			m_Coffi;		//��R
	int				m_Time;			//�����鎞��
	float			m_GravityAcc;	//�d�͉����x
	bool			m_IsUse;		//�g�p�t���O
};

#endif