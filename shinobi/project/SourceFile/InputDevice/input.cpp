//=============================================================================
//
// ���͏��� [input.cpp]
// Author : AKIRA TANAKA
//
//=============================================================================
#include "input.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	COUNT_WAIT_REPEAT	(20)	// ���s�[�g�҂�����

//*****************************************************************************
// �ÓI�ϐ�
//*****************************************************************************
LPDIRECTINPUT8	CInput::m_pDInput = nullptr;	// DirectInput�I�u�W�F�N�g

//=============================================================================
// CInput�R���X�g���X�^
//=============================================================================
CInput::CInput()
{
	m_pDIDevice = nullptr;
}

//=============================================================================
// CInput�f�X�g���X�^
//=============================================================================
CInput::~CInput()
{
}

//=============================================================================
// ���͏����̏���������
//=============================================================================
HRESULT CInput::Init(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr = S_FALSE;
	hWnd;			//�_�~�[�Q��

	// DirectInput�I�u�W�F�N�g�̍쐬
	if(!m_pDInput)
	{
		hr = DirectInput8Create(hInst, DIRECTINPUT_VERSION,
									IID_IDirectInput8, (void**)&m_pDInput, nullptr);
	}

	return hr;
}

//=============================================================================
// ���͏����̍X�V����
//=============================================================================
#if 0	// ---> �������z�֐���
HRESULT CInput::Update(void)
{
	return S_OK;
}
#endif

//=============================================================================
// ���͏����̏I������
//=============================================================================
void CInput::Uninit(void)
{
	if(m_pDIDevice)
	{// �f�o�C�X�I�u�W�F�N�g�̊J��
		m_pDIDevice->Unacquire();

		m_pDIDevice->Release();
		m_pDIDevice = nullptr;
	}

	if(m_pDInput)
	{// DirectInput�I�u�W�F�N�g�̊J��
		m_pDInput->Release();
		m_pDInput = nullptr;
	}
}

//=============================================================================
// CInputKeyboard�R���X�g���X�^
//=============================================================================
CInputKeyboard::CInputKeyboard()
{
	// �e���[�N�̃N���A
	ZeroMemory(m_aKeyState, sizeof m_aKeyState);
	ZeroMemory(m_aKeyStateTrigger, sizeof m_aKeyStateTrigger);
	ZeroMemory(m_aKeyStateRelease, sizeof m_aKeyStateRelease);
	ZeroMemory(m_aKeyStateRepeat, sizeof m_aKeyStateRepeat);
	ZeroMemory(m_aKeyStateRepeatCnt, sizeof m_aKeyStateRepeatCnt);
}

//=============================================================================
// CInputKeyboard�f�X�g���X�^
//=============================================================================
CInputKeyboard::~CInputKeyboard()
{
}

//=============================================================================
// �L�[�{�[�h�̏���������
//=============================================================================
HRESULT CInputKeyboard::Init(HINSTANCE hInst, HWND hWnd)
{
	HRESULT hr;

	// ���͏����̏�����
	CInput::Init(hInst, hWnd);

	// �f�o�C�X�I�u�W�F�N�g���쐬
	hr = m_pDInput->CreateDevice(GUID_SysKeyboard, &m_pDIDevice, nullptr);
	if(FAILED(hr) || m_pDIDevice == nullptr)
	{
		MessageBox(hWnd, "�L�[�{�[�h���˂��I", "�x���I", MB_ICONWARNING);
		return hr;
	}

	// �f�[�^�t�H�[�}�b�g��ݒ�
	hr = m_pDIDevice->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(hr))
	{
		MessageBox(hWnd, "�L�[�{�[�h�̃f�[�^�t�H�[�}�b�g��ݒ�ł��܂���ł����B", "�x���I", MB_ICONWARNING);
		return hr;
	}

	// �������[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j
	hr = m_pDIDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE));
	if(FAILED(hr))
	{
		MessageBox(hWnd, "�L�[�{�[�h�̋������[�h��ݒ�ł��܂���ł����B", "�x���I", MB_ICONWARNING);
		return hr;
	}

	// m_pDIDevice�A�N�Z�X�����l��(���͐���J�n)
	m_pDIDevice->Acquire();

	return hr;
}

//=============================================================================
// �L�[�{�[�h���͏����̊J��
//=============================================================================
void CInputKeyboard::Uninit(void)
{
	// ���͏����̊J��
	CInput::Uninit();
}

//=============================================================================
// �L�[�{�[�h�̍X�V����
//=============================================================================
void CInputKeyboard::Update(void)
{
	HRESULT hr;
	BYTE aKeyState[NUM_KEY_MAX];

	if(!m_pDIDevice)
	{
		return;
	}

	// �f�o�C�X����f�[�^���擾
	hr = m_pDIDevice->GetDeviceState(sizeof(aKeyState), aKeyState);
	if(SUCCEEDED(hr))
	{
		for(int nCntKey = 0; nCntKey < NUM_KEY_MAX; nCntKey++)
		{
			m_aKeyStateTrigger[nCntKey] = (m_aKeyState[nCntKey] ^ aKeyState[nCntKey]) & aKeyState[nCntKey];
			m_aKeyStateRelease[nCntKey] = (m_aKeyState[nCntKey] ^ aKeyState[nCntKey]) & ~aKeyState[nCntKey];
			m_aKeyStateRepeat[nCntKey] = m_aKeyStateTrigger[nCntKey];

			if(aKeyState[nCntKey])
			{
				m_aKeyStateRepeatCnt[nCntKey]++;
				if(m_aKeyStateRepeatCnt[nCntKey] >= COUNT_WAIT_REPEAT)
				{
					m_aKeyStateRepeat[nCntKey] = aKeyState[nCntKey];
				}
			}
			else
			{
				m_aKeyStateRepeatCnt[nCntKey] = 0;
				m_aKeyStateRepeat[nCntKey] = 0;
			}

			m_aKeyState[nCntKey] = aKeyState[nCntKey];
		}
	}
	else
	{
		m_pDIDevice->Acquire();
	}
}

//=============================================================================
// �L�[�{�[�h�f�[�^�擾(�v���X)
//=============================================================================
bool CInputKeyboard::GetKeyPress(int nKey)
{
	return (m_aKeyState[nKey] & 0x80) ? true: false;
}

//=============================================================================
// �L�[�{�[�h�f�[�^�擾(�g���K�[)
//=============================================================================
bool CInputKeyboard::GetKeyTrigger(int nKey)
{
	return (m_aKeyStateTrigger[nKey] & 0x80) ? true : false;
}

//=============================================================================
// �L�[�{�[�h�f�[�^�擾(�����[�X)
//=============================================================================
bool CInputKeyboard::GetKeyRelease(int nKey)
{
	return (m_aKeyStateRelease[nKey] & 0x80) ? true : false;
}

//=============================================================================
// �L�[�{�[�h�f�[�^�擾(���s�[�g)
//=============================================================================
bool CInputKeyboard::GetKeyRepeat(int nKey)
{
	return (m_aKeyStateRepeat[nKey] & 0x80) ? true : false;
}

//=============================================================================
// �L�[�{�[�h�f�[�^�폜(�g���K�[)
//=============================================================================
void CInputKeyboard::FlushKeyTrigger(int nKey)
{
	m_aKeyStateTrigger[nKey] = 0;
}

