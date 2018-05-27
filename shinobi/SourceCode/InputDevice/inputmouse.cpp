#include "inputmouse.h"
#include "input.h"

//�}�N����`
#define LIMIT_COUNT_REPEAT (20)

//=============================================================================
// CInputMouse�R���X�g���X�^
//=============================================================================
CInputMouse::CInputMouse()
{
	// �e���[�N�̃N���A
	ZeroMemory(m_aMouseState, sizeof m_aMouseState);
	ZeroMemory(m_aMouseStateTrigger, sizeof m_aMouseStateTrigger);
	ZeroMemory(m_aMouseStateRelease, sizeof m_aMouseStateRelease);
	ZeroMemory(m_aMouseStateRepeat, sizeof m_aMouseStateRepeat);
	ZeroMemory(m_aMouseStateRepeatCnt, sizeof m_aMouseStateRepeatCnt);
}

//=============================================================================
// CInputMouse�f�X�g���X�^
//=============================================================================
CInputMouse::~CInputMouse()
{
}

//=============================================================================
// �L�[�{�[�h�̏���������
//=============================================================================
HRESULT CInputMouse::Init(HINSTANCE hInst, HWND hWnd)
{
	HRESULT ret = S_FALSE;

	// ���͏����̏�����
	CInput::Init(hInst, hWnd);

	// �f�o�C�X�̍쐬
	if (FAILED(m_pDInput->CreateDevice(GUID_SysMouse, &m_pDIDevice, nullptr)))
	{
		MessageBox(hWnd, "�}�E�X���˂��I", "�x���I", MB_ICONWARNING);
		return E_FAIL;
	}

	// �f�[�^�t�H�[�}�b�g��ݒ�
	if (FAILED(m_pDIDevice->SetDataFormat(&c_dfDIMouse)))
	{
		MessageBox(hWnd, "�}�E�X�̃f�[�^�t�H�[�}�b�g��ݒ�ł��܂���ł����B", "�x���I", MB_ICONWARNING);
		return E_FAIL;
	}

	// �������[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j(���ׂ�K�v������)
	if (FAILED(m_pDIDevice->SetCooperativeLevel(hWnd, (DISCL_NONEXCLUSIVE | DISCL_FOREGROUND))))
	{
		MessageBox(hWnd, "�}�E�X�̋������[�h��ݒ�ł��܂���ł����B", "�x���I", MB_ICONWARNING);
		return E_FAIL;
	}

	// �f�o�C�X�̐ݒ�
	DIPROPDWORD diprop;
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwObj = 0;
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.dwData = DIPROPAXISMODE_REL;	// ���Βl���[�h�Őݒ�i��Βl��DIPROPAXISMODE_ABS�j

	ret = m_pDIDevice->SetProperty(DIPROP_AXISMODE, &diprop.diph);

	if (FAILED(ret)) {
		// �f�o�C�X�̐ݒ�Ɏ��s
		MessageBox(hWnd, "�f�o�C�X�̐ݒ莸�s", "�x���I", MB_ICONWARNING);
		return false;
	}

	// �}�E�X�ւ̃A�N�Z�X�����l��(���͐���J�n)
	m_pDIDevice->Acquire();

	return S_OK;
}

//=============================================================================
// �L�[�{�[�h���͏����̊J��
//=============================================================================
void CInputMouse::Uninit(void)
{
	// ���͏����̊J��
	CInput::Uninit();
}

//=============================================================================
// �L�[�{�[�h�̍X�V����
//=============================================================================
void CInputMouse::Update(void)
{
	// �ǎ�O�̒l��ێ����܂�
	DIMOUSESTATE zdiMouseState_bak;	// �}�E�X���(�ω����m�p)
	memcpy(&zdiMouseState_bak, &m_zdiMouseState, sizeof(zdiMouseState_bak));       //zdiMouseState_bak�ɌÂ��f�[�^��ۑ�

	// �f�o�C�X����f�[�^���擾
	if (SUCCEEDED(m_pDIDevice->GetDeviceState(sizeof(DIMOUSESTATE), &m_zdiMouseState)))
	{
		for (int nCnMouse = 0; nCnMouse < NUM_MOUSEKEY; nCnMouse++)
		{
			// �L�[�v���X����ۑ�
			//�L�[�g���K�[�����[�X���̍쐬
			m_aMouseStateTrigger[nCnMouse] = (zdiMouseState_bak.rgbButtons[nCnMouse] ^ m_zdiMouseState.rgbButtons[nCnMouse]) & ~zdiMouseState_bak.rgbButtons[nCnMouse];                          //g_aMouseState�͑O,aMouseState�͍�
			m_aMouseStateRelease[nCnMouse] = (zdiMouseState_bak.rgbButtons[nCnMouse] ^ m_zdiMouseState.rgbButtons[nCnMouse]) & zdiMouseState_bak.rgbButtons[nCnMouse];                        //g_aMouseState�͑O,aMouseState�͍�
			m_aMouseState[nCnMouse] = m_zdiMouseState.rgbButtons[nCnMouse];

			//�L�[���r�[�Ə��̐���
			if (m_zdiMouseState.rgbButtons[nCnMouse])
			{
				if (m_aMouseStateRepeatCnt[nCnMouse] < LIMIT_COUNT_REPEAT)
				{
					m_aMouseStateRepeatCnt[nCnMouse]++;
					if (m_aMouseStateRepeatCnt[nCnMouse] == 1 || m_aMouseStateRepeatCnt[nCnMouse] >= LIMIT_COUNT_REPEAT)
					{
						m_aMouseStateRepeat[nCnMouse] = m_zdiMouseState.rgbButtons[nCnMouse];
						m_aMouseStateRepeatCnt[nCnMouse] = 2;
					}

					else
					{
						m_aMouseStateRepeat[nCnMouse] = 0;
					}
				}
			}

			else
			{
				m_aMouseStateRepeat[nCnMouse] = 0;
				m_aMouseStateRepeatCnt[nCnMouse] = 0;
			}
		}
	}

	//���s�����ꍇ�͂�����x�}�E�X�ւ̃A�X�Z�X��v������
	else
	{
		// �}�E�X�ւ̃A�N�Z�X�����擾
		m_pDIDevice->Acquire();
	}
}

//=============================================================================
// �}�E�X�̃v���X��Ԃ��擾
//=============================================================================
bool CInputMouse::GetMousePress(int nMouse)
{
	return (m_aMouseState[nMouse] & 0x80) ? true : false;
}

//=============================================================================
// �}�E�X�̃g���K�[��Ԃ��擾
//=============================================================================
bool CInputMouse::GetMouseTrigger(int nMouse)
{
	return (m_aMouseStateTrigger[nMouse] & 0x80) ? true : false;
}

//=============================================================================
// �}�E�X�̃��s�[�g��Ԃ��擾
//=============================================================================
bool CInputMouse::GetMouseRepeat(int nMouse)
{
	return (m_aMouseStateRepeat[nMouse] & 0x80) ? true : false;
}

//=============================================================================
// �}�E�X�̃����|�X��Ԃ��擾
//=============================================================================
bool CInputMouse::GetMouseRelease(int nMouse)
{
	return (m_aMouseStateRelease[nMouse] & 0x80) ? true : false;
}

//=============================================================================
//�}�E�X��Ԏ擾
//=============================================================================
DIMOUSESTATE *CInputMouse::GetMouseStates(void)
{
	return &m_zdiMouseState;
}