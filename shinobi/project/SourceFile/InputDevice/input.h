//=============================================================================
//
// ���͏��� [input.h]
// Author : AKIRA TANAKA
//
//=============================================================================
#ifndef _INPUT_H_
#define _INPUT_H_

#include "main.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	NUM_KEY_MAX		(256)	// �L�[�ő吔

//*********************************************************
// ���̓N���X
//*********************************************************
class CInput
{
public:
	CInput();
	virtual ~CInput();

	virtual HRESULT Init(HINSTANCE hInst, HWND hWnd);
	virtual void Uninit(void);
	virtual void Update(void) = 0;		// ---> �������z�֐���

protected:
	static LPDIRECTINPUT8 m_pDInput;	// DirectInput�I�u�W�F�N�g
	LPDIRECTINPUTDEVICE8 m_pDIDevice;	// Device�I�u�W�F�N�g(���͂ɕK�v)
};

//*********************************************************
// �L�[�{�[�h���̓N���X
//*********************************************************
class CInputKeyboard : public CInput
{
public:
	CInputKeyboard();
	~CInputKeyboard();

	HRESULT Init(HINSTANCE hInst, HWND hWnd);
	void Uninit(void);
	void Update(void);

	bool GetKeyPress(int nKey);
	bool GetKeyTrigger(int nKey);
	bool GetKeyRelease(int nKey);
	bool GetKeyRepeat(int nKey);
	void FlushKeyTrigger(int nKey);

private:
	BYTE	m_aKeyState[NUM_KEY_MAX];			// �L�[�{�[�h�̏�Ԃ��󂯎�郏�[�N
	BYTE	m_aKeyStateTrigger[NUM_KEY_MAX];	// �g���K�[���[�N
	BYTE	m_aKeyStateRelease[NUM_KEY_MAX];	// �����[�X���[�N
	BYTE	m_aKeyStateRepeat[NUM_KEY_MAX];		// ���s�[�g���[�N
	int		m_aKeyStateRepeatCnt[NUM_KEY_MAX];	// ���s�[�g�J�E���^
};

#endif