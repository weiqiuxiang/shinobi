#pragma once
#ifndef _INPUTMOUSE_H_
#define _INPUTMOUSE_H_

#include "main.h"
#include "input.h"

//前方宣言
class CInput;

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	NUM_MOUSEKEY		(4)	// キー最大数
#define MOUSE_LEFT (0)
#define MOUSE_RIGHT (1)
#define MOUSE_CENTER (2)

//*********************************************************
// キーボード入力クラス
//*********************************************************
class CInputMouse : public CInput
{
public:
	CInputMouse();
	~CInputMouse();

	HRESULT Init(HINSTANCE hInst, HWND hWnd);
	void Uninit(void);
	void Update(void);

	bool GetMousePress(int nKey);             //どのボタンが押されたか
	bool GetMouseTrigger(int nKey);           //ボタンが押された瞬間
	bool GetMouseRepeat(int nKey);            //
	bool GetMouseRelease(int nKey);           //ボタンが離れた瞬間
	DIMOUSESTATE *GetMouseStates(void);       //マウス状態取得

private:
	DIMOUSESTATE            m_zdiMouseState;                //マウス状態取得
	BYTE					m_aMouseState[NUM_MOUSEKEY];			// マウスの入力情報ワーク
	BYTE					m_aMouseStateTrigger[NUM_MOUSEKEY];	// マウスのトリガー情報ワーク
	BYTE					m_aMouseStateRelease[NUM_MOUSEKEY];	// マウスのリリース情報ワーク
	BYTE					m_aMouseStateRepeat[NUM_MOUSEKEY];		// マウスのリピート情報ワーク
	int						m_aMouseStateRepeatCnt[NUM_MOUSEKEY];	// マウスのリピートカウンタ
};

#endif
