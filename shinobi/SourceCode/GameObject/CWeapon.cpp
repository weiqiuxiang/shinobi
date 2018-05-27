#include "CWeapon.h"

#define LIFE_MAX (1000)
#define STAMINA_MAX (1000)

CWeapon::CWeapon()
{
	m_posOffset =	D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_OtherParentID =	-1;
}

CWeapon::~CWeapon()
{
	
}

void CWeapon::Init(const D3DXVECTOR3& pos, const D3DXVECTOR3& rot, const char* MotionPass)
{
	CHuman::Init(pos, rot, MotionPass);

	//当り判定円のセット
	CHuman::SetCalcEnableCircle(D3DXVECTOR3(0.0f, 0.0f, 0.0f),0,10.0f);

	CHuman::SetCollisionCircle(D3DXVECTOR3(0.0f, 0.5f, 0.0f), CCollisionCircle::TYPE_HIT_PREVIEW, 0, 0.8f);
	CHuman::SetCollisionCircle(D3DXVECTOR3(0.0f, 1.0f, 0.0f), CCollisionCircle::TYPE_HIT_PREVIEW, 0, 0.8f);
	CHuman::SetCollisionCircle(D3DXVECTOR3(0.0f, 1.5f, 0.0f), CCollisionCircle::TYPE_HIT_PREVIEW, 0, 0.8f);
	CHuman::SetCollisionCircle(D3DXVECTOR3(0.0f, 2.0f, 0.0f), CCollisionCircle::TYPE_HIT_PREVIEW, 0, 0.8f);
	CHuman::SetCollisionCircle(D3DXVECTOR3(0.0f, 0.5f, 0.0f), CCollisionCircle::TYPE_HIT_PREVIEW, 1, 0.8f);
	CHuman::SetCollisionCircle(D3DXVECTOR3(0.0f, 1.0f, 0.0f), CCollisionCircle::TYPE_HIT_PREVIEW, 1, 0.8f);
	CHuman::SetCollisionCircle(D3DXVECTOR3(0.0f, 1.5f, 0.0f), CCollisionCircle::TYPE_HIT_PREVIEW, 1, 0.8f);
	CHuman::SetCollisionCircle(D3DXVECTOR3(0.0f, 2.0f, 0.0f), CCollisionCircle::TYPE_HIT_PREVIEW, 1, 0.8f);
}

void CWeapon::Uninit(void)
{
	CHuman::Uninit();
}

void CWeapon::Update(void)
{
	CHuman::Update();								//モデルデータの更新
	CHuman::UpdateCollisionCircle();				//当り判定円の更新
}

void CWeapon::Draw(void)
{
	CHuman::Draw();
}

void CWeapon::EnableHitCircle(void)
{
	for (int i = 0; i < m_ColliCircleMaxNum; i++) {
		if (m_ColliCircle[i].GetUse() == false) continue;
		if (m_ColliCircle[i].GetType() != CCollisionCircle::TYPE_HIT_PREVIEW) continue;
		m_ColliCircle[i].SetType(CCollisionCircle::TYPE_HIT);
	}
}

void CWeapon::DisableHitCircle(void)
{
	for (int i = 0; i < m_ColliCircleMaxNum; i++) {
		if (m_ColliCircle[i].GetUse() == false) continue;
		if (m_ColliCircle[i].GetType() != CCollisionCircle::TYPE_HIT) continue;
		m_ColliCircle[i].SetType(CCollisionCircle::TYPE_HIT_PREVIEW);
	}
}