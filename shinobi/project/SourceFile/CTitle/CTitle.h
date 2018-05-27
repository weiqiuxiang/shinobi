#pragma once
#ifndef _CTITLE_H_
#define _CTITLE_H_

#include "SceneManager.h"
#include "CFontUIScale.h"

class CTitle : public CSceneManager
{
public:
	typedef enum {
		UI_START = 0,
		UI_MAP_EDITOR,
		UI_MOTION_EDITOR,
		UI_EXIT,
		UI_MAX
	}UI_NAME;

	typedef enum {
		OBJ_LOGO = 0,
		OBJ_SOULGEM,
		OBJ_SOULGEM_EF,
		OBJ_MAX
	}OTHER_OBJ;

	CTitle();
	~CTitle();

	bool Init(HWND hWnd) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	//セッター
	static void SetWnd(HWND hWnd) { m_hWnd = hWnd; }

private:
	void InitTitleObj(void);
	void UninitTitleObj(void);
	void UpdateTitleObj(void);
	void DrawTitleObj(void);

	//他の関数
	void ChangeSelector(void);
	void TransitionToOtherScene(void);
	void SoulGemFadeEffect(void);

	int m_Selector;
	CFontUIScale* m_FontUIScale[UI_MAX];
	CFontUIScale* m_OtherObj[OBJ_MAX];
	bool m_SoulJamFade;					//falseはfade_in
	static HWND m_hWnd;						//ウィンドウハンドル
};

#endif