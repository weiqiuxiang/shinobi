#pragma once
#ifndef CPARTICLEMAKER_H_
#define CPARTICLEMAKER_H_

#include "main.h"
#include "CParticle3D.h"

#define PARTICLE3D_MAX (1024)

class CEnvironment     //環境係数クラス
{
public:
	CEnvironment();
	~CEnvironment();

	//セッター
	void SetEnvironment(const D3DXVECTOR3& WindDirect, float WindPower, float fMass, float GravityAcc, float CoeffiResist);   //パラメータ設定
	void SetWindDir(const D3DXVECTOR3& WindDir) { D3DXVec3Normalize(&m_WindDirect, &WindDir); }
	void SetWindPower(float WindPower) { m_WindPower = WindPower; }
	void SetGravityAcc(float GravityAcc) { m_fGravityAcc = GravityAcc; }
	void SetCoeffiResist(float fCoeffiResist) { m_fCoeffiResist = max(0.0f, fCoeffiResist); }
	void SetMass(float fMass) { m_fMass = max(0,fMass); }

	//ゲッター
	D3DXVECTOR3 GetWindDir(void) { return m_WindDirect; }
	float	GetWindPower(void) { return m_WindPower; }
	float GetGravityAcc(void) { return m_fGravityAcc; }
	float GetCoeffiResist(void) { return m_fCoeffiResist; }
	float GetMass(void) { return m_fMass; }

private:
	//メンバ変数
	D3DXVECTOR3 m_WindDirect;        //風方向
	float    m_WindPower;            //風力
	float    m_fGravityAcc;          //重力加速度
	float    m_fCoeffiResist;        //抵抗係数
	float    m_fMass;                //質量
};

class CShootPara     //シュットパラメータ
{
public:
	CShootPara();
	~CShootPara();

	//セッター
	void SetShootPara(const D3DXVECTOR3& ShootDir,const D3DXVECTOR2& RangeAngle,float Speed);
	void SetShootDir(const D3DXVECTOR3& ShootDir){ D3DXVec3Normalize(&m_ShootDir, &ShootDir); }
	void SetShootRangeAngle(const D3DXVECTOR2& RangeAngle);
	void SetShootRangeAngleX(float RangeAngleX) { m_fShootRangeAngle.x = max(0, RangeAngleX); m_fShootRangeAngle.x = min(D3DX_PI, RangeAngleX); }
	void SetShootRangeAngleY(float RangeAngleY) { m_fShootRangeAngle.y = max(0, RangeAngleY); m_fShootRangeAngle.y = min(D3DX_PI, RangeAngleY); }
	void SetShootSpeed(float fSpeed) { m_fSpeed = fSpeed; }

	//ゲッター
	D3DXVECTOR3 GetShootDir(void) { return m_ShootDir; }
	D3DXVECTOR2 GetShootRangeAngle(void) { return m_fShootRangeAngle; }
	float GetShootSpeed(void) { return m_fSpeed; }

private:
	//メンバ変数
	D3DXVECTOR3 m_ShootDir;          //発射基準方向
	D3DXVECTOR2 m_fShootRangeAngle;  //発射範囲角度(xは水平,yは垂直)
	float    m_fSpeed;               //発射速度
};

class CParticleMaker
{
public:
	CParticleMaker();
	~CParticleMaker();

	void Uninit(void);
	void Update(void);
	void Draw(void);
	
	//セッター
	void SetMakerBase(const D3DXVECTOR3& pos, const D3DXVECTOR3& size, float PerFrameNum,int nLife, const char* BaseTexPass, const char* EffectTexPass);
	void SetEnvironment(const D3DXVECTOR3& WindDirect, float WindPower,float fMass, float GravityAcc, float CoeffiResist);
	void SetShootPara(const D3DXVECTOR3& ShootDir, const D3DXVECTOR2& RangeAngle, float Speed);
	void SetParticleDrawTime(int DrawTimes_Base, int DrawTimes_Effect){ m_DrawTimes_Base = max(1, DrawTimes_Base); m_DrawTimes_Effect = max(1, DrawTimes_Effect);}
	void SetPerFrameNum(double PerNum) { m_fFramePerNum = (float)max(0.01,PerNum); }
	void SetPosition(const D3DXVECTOR3& pos) { m_pos = pos; }
	void SetParticleBaseRGBA(RGBA rgba) { m_RGBA_Base = rgba; }
	void SetParticleEffectRGBA(RGBA rgba) { m_RGBA_Effect = rgba; }

	//ゲッター
	D3DXVECTOR3 GetPosition(void) { return m_pos; }
	CEnvironment* GetEnvironment(void) { return &m_Environment; };
	CShootPara* GetShootPara(void){ return &m_ShootPara; }

	//他の関数
	void ChangeSize(const D3DXVECTOR3& size) {m_size.x = max(0, size.x); m_size.y = max(0, size.y);}
	void ChangeSizeX(float SizeX) { m_size.x = max(0,SizeX); }
	void ChangeSizeY(float SizeY) { m_size.y = max(0,SizeY); }

private:
	void DeleteDieParticle(void);         //ライフが0以下のパーティクル削除
	void CreateParticle(void);            //パーティクルを作る
	void CreateParticleFramePerNum(void);
	D3DXVECTOR3 ShootDirCalc(void);       //パーティクル進行方向計算
	D3DXVECTOR3 PowerCalc(CParticle3D* pParticle);  //受ける力計算

	D3DXVECTOR3 m_pos;                  //位置
	D3DXVECTOR3 m_size;                 //パーティクルのサイズ
	RGBA        m_RGBA_Base;            //カラー
	RGBA        m_RGBA_Effect;          //カラー
	int         m_nLife;                //生命値
	char        m_aBaseTexPass[256];     //テクスチャパス
	char        m_aEffectTexPass[256];   //テクスチャパス
	int      m_DrawTimes_Base;           //描画回数
	int      m_DrawTimes_Effect;         //描画回数
	float       m_fFramePerNum;          //何フレーム1個発射するか
	float       m_fCreateCounter;        //フレームカウンター
	CEnvironment m_Environment;         //環境パラメータ
	CShootPara m_ShootPara;             //発射パラメータ
	CParticle3D *m_aParticle[PARTICLE3D_MAX];             //パーティクルマガジン
};

#endif