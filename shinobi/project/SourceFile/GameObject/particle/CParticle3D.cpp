#include "CParticle3D.h"
#include "CRenderer.h"

#define EFFECT_SCALE (6.0f)

CParticle3D::CParticle3D()
{
	m_fMass = 1;
	m_Speed = D3DXVECTOR3(0.0f,0.0f,0.0f);
	m_nLife = 0;
	m_DrawTimes_Base = 1;
	m_DrawTimes_Effect = 1;
}
CParticle3D::~CParticle3D()
{
	
}

void CParticle3D::InitCScene3D(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &pSize, const char *BaseFilePass, const char *EffectFilePass)
{
	SRP PosRotScl = { pos,D3DXVECTOR3(0.0f,0.0f,0.0f),D3DXVECTOR3(1.0f,1.0f,1.0f) };
	m_CScene3D_Base.Init(PosRotScl, pSize, BaseFilePass);
	m_CScene3D_Base.SetDrawType(CScene3D::TYPE_BILLBOARD);

	m_CScene3D_Effect.Init(PosRotScl, pSize*EFFECT_SCALE, EffectFilePass);
	m_CScene3D_Effect.SetDrawType(CScene3D::TYPE_BILLBOARD);
}

void CParticle3D::InitParticle(float Mass, const D3DXVECTOR3& Speed, int Life)
{
	m_fMass = Mass;
	m_Speed = Speed;
	m_nLife = Life;
	m_Power = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

void CParticle3D::InitParticleDrawTime(int DrawTimes_Base, int DrawTimes_Effect)
{
	m_DrawTimes_Base = max(1, DrawTimes_Base);
	m_DrawTimes_Effect = max(1, DrawTimes_Effect);
}

void CParticle3D::Uninit(void)
{
	m_CScene3D_Base.Uninit();
	m_CScene3D_Effect.Uninit();
}

void CParticle3D::Update(void)
{
	//速度算出
	if (m_fMass > 0)
	{
		D3DXVECTOR3 Acc = m_Power / m_fMass;   //加速度計算
		m_Speed += Acc;         //速度計算
	}
	else
	{
		m_Speed = D3DXVECTOR3(0.0f,0.0f,0.0f);
	}

	//移動
	m_CScene3D_Base.AddPosition(m_Speed);
	m_CScene3D_Effect.AddPosition(m_Speed);

	m_Power = D3DXVECTOR3(0.0f, 0.0f, 0.0f);      //外力を0にする

	//ライフ減少
	m_nLife--;                //ライフ減る
	m_nLife = max(m_nLife,0); //ライフを0以上維持

	//更新
	m_CScene3D_Base.Update();
	m_CScene3D_Effect.Update();
}

void CParticle3D::Draw(void)
{
	for (int nCntDrawTimes = 0; nCntDrawTimes < m_DrawTimes_Base; nCntDrawTimes++)
	{
		m_CScene3D_Base.Draw();
	}

	for (int nCntDrawTimes = 0; nCntDrawTimes < m_DrawTimes_Effect; nCntDrawTimes++)
	{
		m_CScene3D_Effect.Draw();
	}
}

void CParticle3D::AddSpeed(const D3DXVECTOR3& MoveDir, float AddSpeed)
{
	D3DXVECTOR3 DirNormal;
	D3DXVec3Normalize(&DirNormal, &MoveDir);

	m_Speed += DirNormal*AddSpeed;
}