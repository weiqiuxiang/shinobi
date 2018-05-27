#include "CLight.h"
#include "CRenderer.h"

CLight::CLight()
{
	//���C�g�̐ݒ�
	ZeroMemory(&m_Light, sizeof(D3DLIGHT9));                      //���C�g������
	m_Light.Type = D3DLIGHT_DIRECTIONAL;                          //���C�g�̃^�C�v���w��
	m_Light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);          //�f�B���N�V���i�����C�g�̐F
	m_Light.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);          //�A���r�G���g���C�g
	D3DXVECTOR3 vecDir(1.0f, -1.0f, 1.0f);                         //���C�g����
	D3DXVec3Normalize((D3DXVECTOR3*)&m_Light.Direction, &vecDir); //���C�g������ݒ肵�����C�g�ɓ����
}

CLight::~CLight()
{
	
}

void CLight::Init(const D3DLIGHT9& Light)
{
	//���C�g�̐ݒ�
	ZeroMemory(&m_Light, sizeof(D3DLIGHT9));                      //���C�g������
	m_Light = Light; 
}

void CLight::UseLight(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();						//�f�o�C�X�擾	

	pDevice->SetLight(0, &m_Light);											//�f�o�C�X�̃��C�g�X���b�g0�ԂɊi�[
	pDevice->LightEnable(0, TRUE);											//���C�g�X���b�g0�ԃ��C�g�N�p
	pDevice->SetRenderState(D3DRS_AMBIENT, (D3DXCOLOR)m_Light.Ambient);		// �A���r�G���g���C�g�ݒ�
}

void CLight::SetDiffuse(const D3DXCOLOR &DiffuseColor)
{
	m_Light.Diffuse = DiffuseColor;
}

void CLight::SetAmbient(const D3DXCOLOR &AmbientColor)
{
	m_Light.Ambient = AmbientColor;
}

D3DXVECTOR3 CLight::GetLightDiretion(void)
{
	return m_Light.Direction;
}