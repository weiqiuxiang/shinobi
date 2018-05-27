#define _CRT_SECURE_NO_WARNINGS
#include "Tex.h"
#include "CRenderer.h"

//�R���X�g���N�^�ƃf�X�g���N�^
TEX::TEX()
{
	m_pTexture = nullptr;
	memset(&m_pFilepass[0], '\0', sizeof(&m_pFilepass[0]));
}

TEX::~TEX()
{
	if (m_pTexture != nullptr) 
	{
		m_pTexture->Release(); 
		m_pTexture = nullptr;
	}
}

HRESULT TEX::Init(void)
{
	if (m_pTexture != nullptr) 
	{ 
		m_pTexture->Release(); 
		m_pTexture = nullptr; 
	}

	m_pTexture = nullptr;
	memset(&m_pFilepass[0], '\0', sizeof(&m_pFilepass[0]));

	return S_OK;
}

void TEX::Uninit(void)
{
	//�e�N�X�`���|�C���^���
	if (m_pTexture != nullptr) {
		m_pTexture->Release(); 
		m_pTexture = nullptr; 
	}
}

void TEX::SetTexPass(const char* filepass)
{
	strcpy(&m_pFilepass[0], filepass);     //�t�@�C���p�X�L�^
}

const char *TEX::GetTexPass(void)
{
	return m_pFilepass;
}

const LPDIRECT3DTEXTURE9 TEX::GetTexturePoint(void)
{
	return m_pTexture;
}

void TEX::CreateTexObj(void)
{
	LPDIRECT3DDEVICE9 pDevice = CRenderer::GetDevice();

	if(m_pFilepass[0] == '\0')
	{
		MessageBox(nullptr, "�t�@�C���p�X���ǂݍ��܂�ĂȂ��I�e�N�X�`���������s", "�G���[", MB_OK | MB_ICONHAND);
		return;
	}

	//�e�N�X�`�����[�h
	if (FAILED(D3DXCreateTextureFromFile(pDevice, m_pFilepass, &m_pTexture)))
	{
		MessageBox(nullptr, "�e�N�X�`���̓ǂݍ��݂����s���܂���", "�G���[", MB_OK | MB_ICONHAND);
		return;
	}
}