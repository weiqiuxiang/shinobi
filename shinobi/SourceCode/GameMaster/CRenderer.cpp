#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "CRenderer.h"
#include "imgui_impl_dx9.h"

LPDIRECT3D9 CRenderer::m_pD3D = nullptr;
LPDIRECT3DDEVICE9 CRenderer::m_pD3DDevice = nullptr;
D3DPRESENT_PARAMETERS CRenderer::m_d3dpp;
LPDIRECT3DSURFACE9 CRenderer::m_BackBufferSurface = nullptr;
LPDIRECT3DSURFACE9 CRenderer::m_BackDepthSurface = nullptr;

HRESULT CRenderer::Init(HWND hWnd, BOOL bWindow)
{
	D3DDISPLAYMODE d3ddm;

	// Direct3D�I�u�W�F�N�g�̍쐬
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	if (m_pD3D == nullptr)
	{
		return E_FAIL;
	}

	// ���݂̃f�B�X�v���C���[�h���擾
	if (FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	// �f�o�C�X�̃v���[���e�[�V�����p�����[�^�̐ݒ�
	ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));					// ���[�N���[���N���A
	m_d3dpp.BackBufferCount = 1;							// �o�b�N�o�b�t�@�̐�
	m_d3dpp.BackBufferWidth = SCREEN_WIDTH;					// �Q�[����ʃT�C�Y(��)
	m_d3dpp.BackBufferHeight = SCREEN_HEIGHT;				// �Q�[����ʃT�C�Y(����)
	m_d3dpp.BackBufferFormat = d3ddm.Format;				// �J���[���[�h�̎w��
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;				// �f���M���ɓ������ăt���b�v����
	m_d3dpp.EnableAutoDepthStencil = TRUE;					// �f�v�X�o�b�t�@�i�y�o�b�t�@�j�ƃX�e���V���o�b�t�@���쐬
	m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;			// �f�v�X�o�b�t�@(Z�o�b�t�@)�Ƃ���24bit���g���A�X�e���V���o�b�t�@�Ƃ���8bit���g��
	m_d3dpp.Windowed = bWindow;								// �E�B���h�E���[�h
	m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;		// ���t���b�V�����[�g
	m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;	// �C���^�[�o��

	// �f�o�C�X�̐���
	// �f�B�X�v���C�A�_�v�^��\�����߂̃f�o�C�X���쐬
	// �`��ƒ��_�������n�[�h�E�F�A�ōs�Ȃ�
	if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&m_d3dpp, &m_pD3DDevice)))
	{
		// ��L�̐ݒ肪���s������
		// �`����n�[�h�E�F�A�ōs���A���_������CPU�ōs�Ȃ�
		if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&m_d3dpp, &m_pD3DDevice)))
		{
			// ��L�̐ݒ肪���s������
			// �`��ƒ��_������CPU�ōs�Ȃ�
			if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF, hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&m_d3dpp, &m_pD3DDevice)))
			{
				// �������s
				return E_FAIL;
			}
		}
	}

	// �����_�[�X�e�[�g�̐ݒ�
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);			//�e�N�X�`���̉𑜓x���傫���ȃ|���S���ɓ\��t���鎞�̃t�B���^�ݒ�
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC);		//�e�N�X�`���̉𑜓x��菬�����ȃ|���S���ɓ\��t���鎞�̃t�B���^�ݒ�
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);			//�~�b�v�}�b�v�̎��̃t�B���^�ݒ�
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY, 8);						//�ٕ����t�B���^�����O�̃T���v�����O��
	
	//�o�b�N�o�b�t�@�̃T�[�t�F�[�X�|�C���^�擾
	m_pD3DDevice->GetRenderTarget(0, &m_BackBufferSurface);
	m_pD3DDevice->GetDepthStencilSurface(&m_BackDepthSurface);

	//imgui������
	ImGui_ImplDX9_Init(hWnd, m_pD3DDevice);
	
	return S_OK;
}

//����֐�
void CRenderer::Uninit(void)
{
	// �f�o�C�X�̔j��
	if (m_pD3DDevice != nullptr)
	{
		m_pD3DDevice->Release();
		m_pD3DDevice = nullptr;
	}

	// Direct3D�I�u�W�F�N�g�̔j��
	if (m_pD3D != nullptr)
	{
		m_pD3D->Release();
		m_pD3D = nullptr;
	}

	//�o�b�N�o�b�t�@�|�C���^�����
	if (m_BackBufferSurface != nullptr)
	{
		m_BackBufferSurface->Release();
		m_BackBufferSurface = nullptr;
	}
	if (m_BackDepthSurface != nullptr)
	{
		m_BackDepthSurface->Release();
		m_BackDepthSurface = nullptr;
	}

	ImGui_ImplDX9_Shutdown();
}

void CRenderer::DrawClear(void) 
{
	// �o�b�N�o�b�t�@���y�o�b�t�@�̃N���A
	m_pD3DDevice->Clear(0, nullptr, (D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL), 
		D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);
}

void CRenderer::DrawBegin(void)
{
	// Direct3D�ɂ��`��̊J�n
	if (!SUCCEEDED(m_pD3DDevice->BeginScene())) {
		return;
	}
}
void CRenderer::DrawEnd(void)
{
	// Direct3D�ɂ��`��̏I��
	m_pD3DDevice->EndScene();
}
void CRenderer::DrawPresent(void)
{
	// �o�b�N�o�b�t�@�ƃt�����g�o�b�t�@�̓���ւ�
	m_pD3DDevice->Present(nullptr, nullptr, nullptr, nullptr);
}

//�Q�[����ʂ��e�N�X�`���ɏ������ފ֐�
//��������
//pSurface : �T�[�t�F�[�X�|�C���^
//void(*p)(void) : �`��֐��̃|�C���^
void CRenderer::DrawGameSceneOnTexture(LPDIRECT3DSURFACE9 pSurface, CGame* Scene)
{
	if(Scene == nullptr){
		return;
	}
	m_pD3DDevice->SetRenderTarget(0, pSurface); //�`�敽�ʂ��e�N�X�`�����ʂɐݒ�
	DrawClear();
	DrawBegin();
	Scene->DrawInRenderTarget();				//�`�挋�ʂ��e�N�X�`���ɏ�������
	DrawEnd();
	m_pD3DDevice->SetRenderTarget(0, m_BackBufferSurface); //�`�敽�ʂ��o�b�N�o�b�t�@�ɖ߂�

}