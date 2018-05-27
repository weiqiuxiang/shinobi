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
	void InitCScene3D(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &pSize, const char *BaseFilePass, const char *EffectFilePass);     //Scene3Dの設定部分
	void InitParticle(float Mass,const D3DXVECTOR3& Speed,int Life);			//Particleの設定部分
	void InitParticleDrawTime(int DrawTimes_Base,int DrawTimes_Effect);			//描画回数設定
	void Uninit(void);
	void Update(void);
	void Draw(void);

	//セッター
	void SetMass(float Mass) { m_fMass = Mass; }   //質量設定
	void SetSpeed(const D3DXVECTOR3& Speed) { m_Speed = Speed; }      //速度設定
	void SetLife(int nLife) { m_nLife = nLife; }
	void SetPower(const D3DXVECTOR3& power) { m_Power = power; }
	void SetParticleBaseRGBA(RGBA rgba) { m_CScene3D_Base.SetRGBA(rgba); }
	void SetParticleEffectRGBA(RGBA rgba) { m_CScene3D_Effect.SetRGBA(rgba); }

	//ゲッター
	float GetMass(void) { return m_fMass; };
	D3DXVECTOR3 GetSpeed(void) { return m_Speed; }
	int GetLife(void) { return m_nLife; }

	//他の関数
	void AddSpeed(const D3DXVECTOR3& MoveDir, float AddSpeed);       //スピードを徐々変化させる

private:
	CScene3D m_CScene3D_Base;       //CScene3D描画部分(パーティクルのベース部分)
	CScene3D m_CScene3D_Effect;     //CScene3D描画部分(パーティクルのエフェクト部分)
	int      m_DrawTimes_Base;      //描画回数
	int      m_DrawTimes_Effect;    //描画回数
	float m_fMass;              //質量
	D3DXVECTOR3 m_Speed;             //速度
	D3DXVECTOR3 m_Power;             //外力
	int   m_nLife;              //ライフ
};

#endif