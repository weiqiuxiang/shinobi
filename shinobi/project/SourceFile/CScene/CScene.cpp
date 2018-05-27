#include "CScene.h"


//�R���X�g���N�^
CScene::CScene()
{
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

//�f�X�g���N�^
CScene::~CScene()
{
	
}

//-----------------------------------
//��Ԋ֐�
//Init
//-----------------------------------
HRESULT CScene::Init(void)
{
	return S_OK;
}

HRESULT CScene::Init(const D3DXVECTOR3 &Pos, const D3DXVECTOR3 &pSize,const char *filePass)
{
	Pos; pSize; filePass;		//�_�~�[�Q��
	return S_OK;
}

HRESULT CScene::Init(SRP &PosRotScl, const char* filePass)
{
	PosRotScl; filePass;
	return S_OK;
}

//�t�@���N�V�����֐�
D3DXVECTOR3 CScene::GetPosition(void)
{
	return m_pos;
}

void CScene::MoveXZ(const D3DXVECTOR2 &fDirection/*�ړ�����*/, float fDistance/*�ړ�����*/)
{
	D3DXVECTOR2 DirectionVec2;
	D3DXVec2Normalize(&DirectionVec2, &fDirection);

	m_pos.x += DirectionVec2.x * fDistance;
	m_pos.z += DirectionVec2.y * fDistance;
}

void CScene::Move3D(const D3DXVECTOR3 &fDirection/*�ړ�����*/, float fDistance/*�ړ�����*/)
{
	D3DXVECTOR3 DirectionVec3;
	D3DXVec3Normalize(&DirectionVec3, &fDirection);

	m_pos += DirectionVec3 * fDistance;
}