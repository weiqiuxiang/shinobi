#pragma once
#ifndef SCENETRANSITION_H_
#define SCENETRANSITION_H_

#include "main.h"
#include "SceneManager.h"
#include "CScene2D.h"
#include "input.h"
#include "inputmouse.h"
#include "XInputControl.h"

class CSceneTransition 
{
public:
	typedef enum
	{
		FADE_NONE = 0,
		FADE_OUT,
		FADE_IN
	}FADE_MODE;

	//コンストラクタとデストラクタ
	CSceneTransition();
	~CSceneTransition();

	//Init,Uninit,Update,Draw関数
	void Init(HINSTANCE hInst, HWND hWnd, BOOL bWindow);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	//ゲッター
	static CInputKeyboard *GetInputKeyboard(void) { return m_pInputKeyboard; }
	static CInputMouse *GetInputMouse(void){ return m_pInputMouse; }
	static XInputControl *GetXInput(void) { return m_pXInput; }

	//セッター
	static void SetScene(FADE_MODE fadeMode, CSceneManager *pMode);       //遷移したいシーン

private:
	void SceneManagerAndFadeUpdate(void);
	void CSceneTransition::SceneTransition(void);

	CSceneManager*	m_SceneManager;
	HWND			m_Wnd;
	static CSceneManager *m_StorageBeforeFade;
	
	static CScene2D m_Fade;									//フェード
	static FADE_MODE m_FadeMode;							//フェードの状態

	//ゲームの進行役
	static CInputKeyboard *m_pInputKeyboard;
	static CInputMouse *m_pInputMouse;
	static XInputControl *m_pXInput;
};

#endif