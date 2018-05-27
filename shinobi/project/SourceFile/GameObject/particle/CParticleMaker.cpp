#include "CParticleMaker.h"
#include "CRenderer.h"
#include <string.h>

/*******************************
CEnvironment�̕���Start
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
	D3DXVec3Normalize(&m_WindDirect,&WindDirect);     //�������
	m_WindPower = WindPower;
	m_fGravityAcc = GravityAcc;
	m_fCoeffiResist = CoeffiResist;
	m_fMass = fMass;
}

/*******************************
CEnvironment�̕���End
*******************************/

/*******************************
CShootPara�̕���Start
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
	D3DXVec3Normalize(&m_ShootDir, &ShootDir);     //�������
	m_fShootRangeAngle.x = max(0, RangeAngle.x);
	m_fShootRangeAngle.x = min(D3DX_PI, RangeAngle.x);
	m_fShootRangeAngle.y = max(0, RangeAngle.y);
	m_fShootRangeAngle.y = min(D3DX_PI, RangeAngle.y);
	m_fSpeed = max(0.1f,Speed);                    //�X�s�[�h��0.1f�ȏ�
}

void CShootPara::SetShootRangeAngle(const D3DXVECTOR2& RangeAngle)
{
	m_fShootRangeAngle.x = max(0, RangeAngle.x); 
	m_fShootRangeAngle.x = min(D3DX_PI, RangeAngle.x);
	m_fShootRangeAngle.y = max(0, RangeAngle.y); 
	m_fShootRangeAngle.y = min(D3DX_PI, RangeAngle.y);
}
/*******************************
CShootPara�̕���End
*******************************/

/*******************************
CParticle�̕���Start
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

//Uninit,Update,Draw�֐�
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
	CreateParticleFramePerNum();           //�p�[�e�B�N�������

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

	//z�o�b�t�@����
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);          //���C�g���I�t�ɂ���
	pDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);       //z�o�b�t�@���Q��
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);    //z���c���Ȃ�

	//���Z�L��
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	//�`��
	for (int i = 0; i < PARTICLE3D_MAX; i++)
	{
		if (m_aParticle[i] != nullptr)
		{
			m_aParticle[i]->Draw();
		}
	}

	//���Z����
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);

	//z�o�b�t�@����(�Ҍ�)
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);      //z���c��
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);          //���C�g���I���ɂ���
}

//�Z�b�^�[
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

//���̊֐�
//����0�ȉ��̎�����
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
			D3DXVECTOR3 ShootSpeed = ShootDirCalc() * m_ShootPara.GetShootSpeed();                                   //�i�s�����v�Z
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

	//����p�[�e�B�N���͈�ȏ�Ȃ�
	if (CreateParticleAmount > 1) m_fCreateCounter = m_fCreateCounter - CreateParticleAmount;
	else return;

	for(int i = 0; i < CreateParticleAmount;i++)
	{
		CreateParticle();      //�p�[�e�B�N�������
	}
}

D3DXVECTOR3 CParticleMaker::ShootDirCalc(void)
{
	D3DXVECTOR3 ShootDir = m_ShootPara.GetShootDir();
	D3DXVECTOR2 RangeAngle = m_ShootPara.GetShootRangeAngle();

	//�͈͊p�x�����W�A����x�ɕϊ�
	int nAngleDegX = RangeAngle.x * 180 / D3DX_PI;
	int nAngleDegY = RangeAngle.y * 180 / D3DX_PI;

	nAngleDegX = max(1, nAngleDegX);
	nAngleDegY = max(1, nAngleDegY);

	//�����_���Ńx�[�X�p�x�̕ω��ʂ�����
	int nChangeAngleX = rand() % (nAngleDegX * 2) + 1 - nAngleDegX;
	int nChangeAngleY = rand() % (nAngleDegY * 2) + 1 - nAngleDegY;

	//�x�����W�A���ɕϊ�
	double fChangeAngleRadX = nChangeAngleX * D3DX_PI / 180;
	double fChangeAngleRadY = nChangeAngleY * D3DX_PI / 180;

	//�x�[�X�̔��ˊp�x������
	double fVerticalBase = asin(ShootDir.y);          //���݂̋p
	double fHorizonBase = acos(ShootDir.x / cos(fVerticalBase) );  //���݂̕��ʊp

	//���ʔ��ˊp�x������
	double fResultHorizon = fHorizonBase + fChangeAngleRadX;
	double fResultVertical = fVerticalBase + fChangeAngleRadY;

	//���ʔ��˃x�N�g��������
	ShootDir.x = 1 * (float)cos(fResultHorizon) * (float)cos(fResultVertical);
	ShootDir.z = 1 * (float)sin(fResultHorizon) * (float)cos(fResultVertical);
	ShootDir.y = 1 * (float)sin(fResultVertical);

	D3DXVec3Normalize(&ShootDir, &ShootDir);

	return ShootDir;
}

D3DXVECTOR3 CParticleMaker::PowerCalc(CParticle3D* pParticle)
{
	D3DXVECTOR3 Power = { 0.0f,0.0f,0.0f };         //�󂯂��
	D3DXVECTOR3 ParticleSpeed = pParticle->GetSpeed();

	float PowerGravity = m_Environment.GetMass()*m_Environment.GetGravityAcc();    //�d�͌v�Z
	D3DXVECTOR3 WindPower = m_Environment.GetWindDir()*m_Environment.GetWindPower();  //���͌v�Z
	D3DXVECTOR3 ResistPower = -ParticleSpeed*m_Environment.GetCoeffiResist();        //�R�͌v�Z

	Power.x = WindPower.x + ResistPower.x;
	Power.y = WindPower.y + ResistPower.y - PowerGravity;
	Power.z = WindPower.z + ResistPower.z;

	return Power;
}
/*******************************
CParticle�̕���End
*******************************/