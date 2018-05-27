#include "CBlood.h"
#include "CRenderer.h"
#include "TexManager.h"
#include "CCameraManager.h"

#define DEFAULT_GRAVITY_VALUE (0.01f)
#define DEFAULT_MASS (1.0f)

CBlood::CBlood()
{
	m_Power = D3DXVECTOR3(0, 0, 0);
	m_Speed = D3DXVECTOR3(0, 0, 0);
	m_Coffi = 0.0f;
	m_Time = 0;			
	m_GravityAcc = DEFAULT_GRAVITY_VALUE;
	m_IsUse = false;
}

CBlood::~CBlood()
{
	
}

HRESULT CBlood::Init(const SRP &worldMtx, const D3DXVECTOR3 &pSize, const char *filePass)
{
	CScene3D::Init(worldMtx, pSize, filePass);

	m_Power = D3DXVECTOR3(0, 0, 0);
	m_Speed = D3DXVECTOR3(0, 0, 0);
	m_Coffi = 0.0f;
	m_Time = 0;
	m_GravityAcc = DEFAULT_GRAVITY_VALUE;
	m_IsUse = false;

	return S_OK;
}

void CBlood::Update(void)
{
	CScene3D::Update();

	//�d�̓v���X
	m_Power.y -= DEFAULT_MASS * m_GravityAcc;

	//�����x�v�Z
	D3DXVECTOR3 Acc = m_Power / DEFAULT_MASS;

	//���x�v�Z
	m_Speed += Acc;
	
	//���x����
	m_Speed -= m_Speed * m_Coffi;

	//�ʒu�v�Z
	m_pos += m_Speed;

	//�O�̓��Z�b�g
	m_Power = D3DXVECTOR3(0,0,0);

	//�o�����ԃJ�E���g�_�E��
	m_Time--;
	if (m_Time < 0) {
		m_Time = 0;
		m_IsUse = false;
	}
}

void CBlood::Draw(void)
{
	CScene3D::Draw();
}

void CBlood::Uninit(void)
{
	CScene3D::Uninit();
}