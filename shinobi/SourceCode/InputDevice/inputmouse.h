#pragma once
#ifndef _INPUTMOUSE_H_
#define _INPUTMOUSE_H_

#include "main.h"
#include "input.h"

//�O���錾
class CInput;

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define	NUM_MOUSEKEY		(4)	// �L�[�ő吔
#define MOUSE_LEFT (0)
#define MOUSE_RIGHT (1)
#define MOUSE_CENTER (2)

//*********************************************************
// �L�[�{�[�h���̓N���X
//*********************************************************
class CInputMouse : public CInput
{
public:
	CInputMouse();
	~CInputMouse();

	HRESULT Init(HINSTANCE hInst, HWND hWnd);
	void Uninit(void);
	void Update(void);

	bool GetMousePress(int nKey);             //�ǂ̃{�^���������ꂽ��
	bool GetMouseTrigger(int nKey);           //�{�^���������ꂽ�u��
	bool GetMouseRepeat(int nKey);            //
	bool GetMouseRelease(int nKey);           //�{�^�������ꂽ�u��
	DIMOUSESTATE *GetMouseStates(void);       //�}�E�X��Ԏ擾

private:
	DIMOUSESTATE            m_zdiMouseState;                //�}�E�X��Ԏ擾
	BYTE					m_aMouseState[NUM_MOUSEKEY];			// �}�E�X�̓��͏�񃏁[�N
	BYTE					m_aMouseStateTrigger[NUM_MOUSEKEY];	// �}�E�X�̃g���K�[��񃏁[�N
	BYTE					m_aMouseStateRelease[NUM_MOUSEKEY];	// �}�E�X�̃����[�X��񃏁[�N
	BYTE					m_aMouseStateRepeat[NUM_MOUSEKEY];		// �}�E�X�̃��s�[�g��񃏁[�N
	int						m_aMouseStateRepeatCnt[NUM_MOUSEKEY];	// �}�E�X�̃��s�[�g�J�E���^
};

#endif
