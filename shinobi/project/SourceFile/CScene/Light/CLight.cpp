#include "CLight.h"
#include "CRenderer.h"

CLight::CLight()
{
	//ライトの設定
	ZeroMemory(&m_Light, sizeof(D3DLIGHT9));                      //ライト初期化
	m_Light.Type = D3DLIGHT_DIRECTIONAL;                          //ライトのタイプを指定
	m_Light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);          //ディレクショナルライトの色
	m_Light.Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);          //アンビエントライト
	D3DXVECTOR3 vecDir(1.0f, -1.0f, 1.0f);                         //ライト方向
	D3DXVec3Normalize((D3DXVECTOR3*)&m_Light.Direction, &vecDir); //ライト方向を設定したライトに入れる
}

CLight::~CLight()
{
	
}

void CLight::Init(const D3DLIGHT9& Light)
{
	//ライトの設定
	ZeroMemory(&m_Light, sizeof(D3DLIGHT9));                      //ライト初期化
	m_Light = Light; 
}

void CLight::UseLight(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();						//デバイス取得	

	pDevice->SetLight(0, &m_Light);											//デバイスのライトスロット0番に格納
	pDevice->LightEnable(0, TRUE);											//ライトスロット0番ライト起用
	pDevice->SetRenderState(D3DRS_AMBIENT, (D3DXCOLOR)m_Light.Ambient);		// アンビエントライト設定
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