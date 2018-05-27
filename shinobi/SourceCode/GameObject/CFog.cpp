#include "CFog.h"
#include "CRenderer.h"
#include "SceneTransition.h"

float CFog::m_fFogDensity = 0.05f;

void CFog::FogSetting(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	// �͈̓x�[�X�̃t�H�O���g�p
	pDevice->SetRenderState(D3DRS_RANGEFOGENABLE, TRUE);

	float FogStart = 50.0f, FogEnd = 100.0f;
	pDevice->SetRenderState(D3DRS_FOGENABLE, TRUE); // �t�H�O�L��
	pDevice->SetRenderState(D3DRS_FOGCOLOR, D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f)); // �t�H�O�F
	pDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR); // �o�[�e�b�N�X�t�H�O
	pDevice->SetRenderState(D3DRS_RANGEFOGENABLE, TRUE); // �͈̓x�[�X�̃t�H�O
	pDevice->SetRenderState(D3DRS_FOGSTART, *((DWORD*)(&FogStart))); // �t�H�O�J�n�_
	pDevice->SetRenderState(D3DRS_FOGEND, *((DWORD*)(&FogEnd))); // �t�H�O�I���_
}

void CFog::EnableFog(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();
	pDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_RANGEFOGENABLE, TRUE); // �͈̓x�[�X�̃t�H�O
}

void CFog::DisableFog(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();
	pDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	pDevice->SetRenderState(D3DRS_RANGEFOGENABLE, FALSE); // �͈̓x�[�X�̃t�H�O
}