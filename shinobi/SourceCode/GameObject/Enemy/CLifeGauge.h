//�q�b�g�|�C���g(HP)���r���{�[�h�̌`��(3D�|���S��)�ŕ\������
#ifndef CLIFE_GAUGE_H_
#define CLIFE_GAUGE_H_

#include "main.h"
#include "CScene3D.h"

class CLifeGauge
{
public:
	CLifeGauge();
	~CLifeGauge();

	bool Init(const D3DXVECTOR3& pos, const D3DXVECTOR3& size, float TotalLife);
	void Update(void);
	void Draw(void);
	void Uninit(void);

	//�Z�b�^�[
	void SetTotalLife(float TotalLife) { m_TotalLife = TotalLife; }
	void SetWorldPos(const D3DXVECTOR3& pos) { m_pos = pos; }

	//�Q�b�^�[
	D3DXMATRIX GetWorldMtx(void) { return m_WorldMtx; }
	void GetEnemyLife(float Life);

	//���̊֐�
	void AddDamageLife(float life);

private:
	void LockVertexBuf(float LeftLifeRightSide,float DamageLifeWidth);
	void SetWorldMtxBillBoard(void);

private:
	//�e�N�X�`��ID�̖��O
	typedef enum
	{
		TEX_WINDOW = 0,
		TEX_GAUGE,
		TEX_DAMAGE_GAUGE,
		TEX_ID_MAX
	}LIFE_TEX_ID;

private:
	//�`��֌W
	D3DXVECTOR3 m_pos;
	D3DXVECTOR3 m_LifeGaugeSize;	//�`�掞�̃T�C�Y
	D3DXVECTOR3 m_LifeWindowSize;	//�Q�[�W���͂ޘg�̃T�C�Y

	//�`��p���_�o�b�t�@
	int						m_Alpha;
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBufferWindow;
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBufferLeftGauge;
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBufferDamageGauge;

	//���C�t�֌W
	float	m_TotalLife;	//�g�[�^�����C�t
	float	m_LeftLife;	//�c�胉�C�t
	float	m_DamageLife;	//���ꂽ���C�t
	int		m_DamageShowTime;

	//���[���h�s��
	D3DXMATRIX m_WorldMtx;

	//�e�N�X�`����ID
	int m_TexID[TEX_ID_MAX];
};

#endif