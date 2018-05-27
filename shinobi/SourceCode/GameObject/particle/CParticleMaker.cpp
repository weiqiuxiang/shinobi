#include "CParticleMaker.h"
#include "CRenderer.h"
#include <string.h>

/*******************************
CEnvironmentの部分Start
*******************************/
CEnvironment::CEnvironment()
{
	m_WindDirect = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_WindPower = 0.0f;
	m_fGravityAcc = 0.0f;
	m_fCoeffiResist = 0.0f;
}

CEnvironment::~CEnvironment()
{
	
}

void CEnvironment::SetEnvironment(const D3DXVECTOR3& WindDirect, float WindPower, float fMass, float GravityAcc, float CoeffiResist)
{
	D3DXVec3Normalize(&m_WindDirect,&WindDirect);     //方向代入
	m_WindPower = WindPower;
	m_fGravityAcc = GravityAcc;
	m_fCoeffiResist = CoeffiResist;
	m_fMass = fMass;
}

/*******************************
CEnvironmentの部分End
*******************************/

/*******************************
CShootParaの部分Start
*******************************/
CShootPara::CShootPara()
{
	m_ShootDir = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_fShootRangeAngle = D3DXVECTOR2(0.0f, 0.0f);
	m_fSpeed = 0.1f;
}

CShootPara::~CShootPara()
{
	
}

void CShootPara::SetShootPara(const D3DXVECTOR3& ShootDir, const D3DXVECTOR2& RangeAngle, float Speed)
{
	D3DXVec3Normalize(&m_ShootDir, &ShootDir);     //方向代入
	m_fShootRangeAngle.x = max(0, RangeAngle.x);
	m_fShootRangeAngle.x = min(D3DX_PI, RangeAngle.x);
	m_fShootRangeAngle.y = max(0, RangeAngle.y);
	m_fShootRangeAngle.y = min(D3DX_PI, RangeAngle.y);
	m_fSpeed = max(0.1f,Speed);                    //スピードは0.1f以上
}

void CShootPara::SetShootRangeAngle(const D3DXVECTOR2& RangeAngle)
{
	m_fShootRangeAngle.x = max(0, RangeAngle.x); 
	m_fShootRangeAngle.x = min(D3DX_PI, RangeAngle.x);
	m_fShootRangeAngle.y = max(0, RangeAngle.y); 
	m_fShootRangeAngle.y = min(D3DX_PI, RangeAngle.y);
}
/*******************************
CShootParaの部分End
*******************************/

/*******************************
CParticleの部分Start
*******************************/
CParticleMaker::CParticleMaker()
{
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fFramePerNum = 1;
	m_fCreateCounter = 0;
	m_DrawTimes_Base = 1;
	m_DrawTimes_Effect = 1;
}

CParticleMaker::~CParticleMaker()
{
	
}

//Uninit,Update,Draw関数
void CParticleMaker::Uninit(void)
{
	for (int i = 0; i < PARTICLE3D_MAX; i++)
	{
		if (m_aParticle[i] != nullptr)
		{
			m_aParticle[i]->Uninit();
			delete m_aParticle[i];
			m_aParticle[i] = nullptr;
		}
	}
}

void CParticleMaker::Update(void)
{
	CreateParticleFramePerNum();           //パーティクルを作る

	for (int i = 0; i < PARTICLE3D_MAX; i++)
	{
		if (m_aParticle[i] != nullptr)
		{
			D3DXVECTOR3 Power;
			Power = PowerCalc(m_aParticle[i]);
			m_aParticle[i]->SetPower(Power);
			m_aParticle[i]->Update();
		}
	}

	DeleteDieParticle();
}

void CParticleMaker::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	//zバッファ操作
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);          //ライトをオフにする
	pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);       //zバッファを参照
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);    //z情報残さない

	//加算有効
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	//描画
	for (int i = 0; i < PARTICLE3D_MAX; i++)
	{
		if (m_aParticle[i] != nullptr)
		{
			m_aParticle[i]->Draw();
		}
	}

	//加算無効
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);

	//zバッファ操作(還元)
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);      //z情報残す
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);          //ライトをオンにする
}

//セッター
void CParticleMaker::SetMakerBase(const D3DXVECTOR3& pos, const D3DXVECTOR3& size, float FramePerNum, int nLife, const char* BaseTexPass, const char* EffectTexPass)
{
	m_pos = pos;
	m_size = size;
	m_fFramePerNum = (float)max(0.01, FramePerNum);
	m_nLife = nLife;
	strcpy(m_aBaseTexPass, BaseTexPass);
	strcpy(m_aEffectTexPass, EffectTexPass);
	m_RGBA_Base = {255,255,255,255};
	m_RGBA_Effect = { 255,255,255,255 };
}

void CParticleMaker::SetEnvironment(const D3DXVECTOR3& WindDirect, float WindPower, float fMass, float GravityAcc, float CoeffiResist)
{
	m_Environment.SetEnvironment(WindDirect, WindPower, fMass, GravityAcc, CoeffiResist);
}

void CParticleMaker::SetShootPara(const D3DXVECTOR3& ShootDir, const D3DXVECTOR2& RangeAngle, float Speed)
{
	m_ShootPara.SetShootPara(ShootDir, RangeAngle, Speed);
}

//他の関数
//命が0以下の時消す
void CParticleMaker::DeleteDieParticle(void)
{
	for (int i = 0; i < PARTICLE3D_MAX; i++)
	{
		if (m_aParticle[i] != nullptr)
		{
			if (m_aParticle[i]->GetLife() > 0) continue;
			m_aParticle[i]->Uninit();
			delete m_aParticle[i];
			m_aParticle[i] = nullptr;
		}
	}
}

void CParticleMaker::CreateParticle(void)
{
	for (int i = 0; i < PARTICLE3D_MAX; i++)
	{
		if (nullptr == m_aParticle[i])
		{
			m_aParticle[i] = new CParticle3D;
			m_aParticle[i]->InitCScene3D(m_pos, m_size, m_aBaseTexPass, m_aEffectTexPass);
			D3DXVECTOR3 ShootSpeed = ShootDirCalc() * m_ShootPara.GetShootSpeed();                                   //進行方向計算
			m_aParticle[i]->InitParticle(m_Environment.GetMass(), ShootSpeed,m_nLife);
			m_aParticle[i]->SetParticleBaseRGBA(m_RGBA_Base);
			m_aParticle[i]->SetParticleEffectRGBA(m_RGBA_Effect);
			m_aParticle[i]->InitParticleDrawTime(m_DrawTimes_Base,m_DrawTimes_Effect);
			break;
		}
	}
}

void CParticleMaker::CreateParticleFramePerNum(void)
{
	m_fCreateCounter += m_fFramePerNum;
	int CreateParticleAmount = (int)m_fCreateCounter;

	//作れるパーティクルは一個以上なら
	if (CreateParticleAmount > 1) m_fCreateCounter = m_fCreateCounter - CreateParticleAmount;
	else return;

	for(int i = 0; i < CreateParticleAmount;i++)
	{
		CreateParticle();      //パーティクルを作る
	}
}

D3DXVECTOR3 CParticleMaker::ShootDirCalc(void)
{
	D3DXVECTOR3 ShootDir = m_ShootPara.GetShootDir();
	D3DXVECTOR2 RangeAngle = m_ShootPara.GetShootRangeAngle();

	//範囲角度をラジアンを度に変換
	int nAngleDegX = RangeAngle.x * 180 / D3DX_PI;
	int nAngleDegY = RangeAngle.y * 180 / D3DX_PI;

	nAngleDegX = max(1, nAngleDegX);
	nAngleDegY = max(1, nAngleDegY);

	//ランダムでベース角度の変化量を求め
	int nChangeAngleX = rand() % (nAngleDegX * 2) + 1 - nAngleDegX;
	int nChangeAngleY = rand() % (nAngleDegY * 2) + 1 - nAngleDegY;

	//度をラジアンに変換
	double fChangeAngleRadX = nChangeAngleX * D3DX_PI / 180;
	double fChangeAngleRadY = nChangeAngleY * D3DX_PI / 180;

	//ベースの発射角度を求め
	double fVerticalBase = asin(ShootDir.y);          //現在の仰角
	double fHorizonBase = acos(ShootDir.x / cos(fVerticalBase) );  //現在の方位角

	//結果発射角度を求め
	double fResultHorizon = fHorizonBase + fChangeAngleRadX;
	double fResultVertical = fVerticalBase + fChangeAngleRadY;

	//結果発射ベクトルを求め
	ShootDir.x = 1 * (float)cos(fResultHorizon) * (float)cos(fResultVertical);
	ShootDir.z = 1 * (float)sin(fResultHorizon) * (float)cos(fResultVertical);
	ShootDir.y = 1 * (float)sin(fResultVertical);

	D3DXVec3Normalize(&ShootDir, &ShootDir);

	return ShootDir;
}

D3DXVECTOR3 CParticleMaker::PowerCalc(CParticle3D* pParticle)
{
	D3DXVECTOR3 Power = { 0.0f,0.0f,0.0f };         //受ける力
	D3DXVECTOR3 ParticleSpeed = pParticle->GetSpeed();

	float PowerGravity = m_Environment.GetMass()*m_Environment.GetGravityAcc();    //重力計算
	D3DXVECTOR3 WindPower = m_Environment.GetWindDir()*m_Environment.GetWindPower();  //風力計算
	D3DXVECTOR3 ResistPower = -ParticleSpeed*m_Environment.GetCoeffiResist();        //抗力計算

	Power.x = WindPower.x + ResistPower.x;
	Power.y = WindPower.y + ResistPower.y - PowerGravity;
	Power.z = WindPower.z + ResistPower.z;

	return Power;
}
/*******************************
CParticleの部分End
*******************************/