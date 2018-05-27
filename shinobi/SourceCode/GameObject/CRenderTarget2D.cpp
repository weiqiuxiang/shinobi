#include "CRenderTarget2D.h"
#include "CRenderer.h"
#include "CShaderManager.h"

#define FVF_VERTEX_2D (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

CRenderTarget2D::CRenderTarget2D()
{
	m_Texture = nullptr;
	m_Surface = nullptr;
	m_Depth = nullptr;
}

CRenderTarget2D::~CRenderTarget2D()
{
	if (m_Texture != nullptr) {
		m_Texture->Release();
		m_Texture = nullptr;
	}
	if (m_Surface != nullptr) {
		m_Surface->Release();
		m_Surface = nullptr;
	}
	if (m_Depth != nullptr) {
		m_Depth->Release();
		m_Depth = nullptr;
	}
}

HRESULT CRenderTarget2D::Init( const D3DXVECTOR3 &pSize,D3DFORMAT format)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	//��̃e�N�X�`������
	HRESULT hr;
	hr = D3DXCreateTexture(pDevice, (UINT)pSize.x, (UINT)pSize.y,
		1,D3DUSAGE_RENDERTARGET, format,D3DPOOL_DEFAULT,
		&m_Texture
		);

	if (hr == E_FAIL) 
	{
		MessageBox(nullptr,"�����_�[�^�[�Q�b�g�̃e�N�X�`���������s","ERROR",MB_ICONHAND);
		return false;
	}

	//�e�N�X�`���̃T�[�t�F�[�X�|�C���^�擾
	m_Texture->GetSurfaceLevel(0, &m_Surface);

	//�[�x�o�b�t�@�����
	hr = CRenderer::GetDevice()->CreateDepthStencilSurface((UINT)pSize.x, (UINT)pSize.y, D3DFMT_D24S8, D3DMULTISAMPLE_NONE,
		0, FALSE, &m_Depth, nullptr);

	if (hr == E_FAIL)
	{
		MessageBox(nullptr, "�����_�[�^�[�Q�b�g�̐[�x�o�b�t�@�������s", "ERROR", MB_ICONHAND);
		return E_FAIL;
	}

	return S_OK;
}

void CRenderTarget2D::DrawTest(const D3DXVECTOR2& pos, const D3DXVECTOR2& size)
{
	// �|���S���̕`�揈��
	VERTEX_2D vtx[] = {
		{ D3DXVECTOR4(pos.x, pos.y, 1.0f, 1.0f), 1.0f, D3DCOLOR_RGBA(255, 255, 255, 255), D3DXVECTOR2(0.0f, 0.0f) },
		{ D3DXVECTOR4(pos.x + size.x, pos.y, 1.0f, 1.0f), 1.0f, D3DCOLOR_RGBA(255, 255, 255, 255), D3DXVECTOR2(1.0f, 0.0f) },
		{ D3DXVECTOR4(pos.x, pos.y + size.y, 1.0f, 1.0f), 1.0f, D3DCOLOR_RGBA(255, 255, 255, 255), D3DXVECTOR2(0.0f, 1.0f) },
		{ D3DXVECTOR4(pos.x + size.x, pos.y + size.y, 1.0f, 1.0f), 1.0f, D3DCOLOR_RGBA(255, 255, 255, 255), D3DXVECTOR2(1.0f, 1.0f) },
	};

	// FVF�i������g�p���钸�_���j�̐ݒ�
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	LPDIRECT3DPIXELSHADER9 pPS = CShaderManager::GetInstance()->GetPixelShaderPointer(BASIC_PS);
	LPD3DXCONSTANTTABLE pPSCT = CShaderManager::GetInstance()->GetPixelShaderCTPointer(BASIC_PS);

	pDevice->SetPixelShader(pPS);

	pDevice->SetFVF(FVF_VERTEX_2D);

	//�e�N�X�`���Z�b�g
	UINT TextureSampler = pPSCT->GetSamplerIndex("Sampler0");
	pDevice->SetTexture(TextureSampler, m_Texture);
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &vtx[0], sizeof(VERTEX_2D));

	pDevice->SetPixelShader(nullptr);

}

void CRenderTarget2D::Uninit(void)
{
	if (m_Texture != nullptr) {
		m_Texture->Release();
		m_Texture = nullptr;
	}
	if (m_Surface != nullptr) {
		m_Surface->Release();
		m_Surface = nullptr;
	}
	if (m_Depth != nullptr) {
		m_Depth->Release();
		m_Depth = nullptr;
	}
}