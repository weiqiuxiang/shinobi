#pragma once
#ifndef CGAME_UI_H_
#define CGAME_UI_H_

#include "CFontUIScale.h"
#include "CGame.h"

#define MAGIC_GAUGE_OBJ_MAX (4)

class CGameUI 
{
public:
	typedef enum {
		UI_HP = 0,
		UI_SP,
		UI_HPEF,
		UI_SPEF,
		UI_HP_DAMAGE,
		UI_GAUGE,
		UI_MAX
	}UI_NAME;

	CGameUI(){
		MyNameSpace::For_Null<CFontUIScale**>((int)UI_MAX, m_FontUIScale);
		MyNameSpace::For_Null<CFontUIScale**>((int)MAGIC_GAUGE_OBJ_MAX, m_MagicGauge);
		MyNameSpace::For_Null<CFontUIScale**>((int)MAGIC_GAUGE_OBJ_MAX, m_MagicGaugeEF);
	}
	~CGameUI() {
		MyNameSpace::For_DeleteInitialize<CFontUIScale**>((int)UI_MAX, m_FontUIScale);
		MyNameSpace::For_DeleteInitialize<CFontUIScale**>((int)MAGIC_GAUGE_OBJ_MAX, m_MagicGauge);
		MyNameSpace::For_DeleteInitialize<CFontUIScale**>((int)MAGIC_GAUGE_OBJ_MAX, m_MagicGaugeEF);
	}

	//ゲームUIオブジェクトのInit,Uninit,Update,Draw
	void InitUIObj(void);
	void UninitUIObj(void);
	void UpdateUIObj(void);
	void DrawUIObj(void);
private:
	void MagicGaugeEffect(void);				//魔力ゲージエフェクト
	

	//クリエート系関数
	void CreateUIGauge(void);
	void CreateMagicGauge(void);

	CFontUIScale* m_FontUIScale[UI_MAX];
	CFontUIScale* m_MagicGauge[MAGIC_GAUGE_OBJ_MAX];
	CFontUIScale* m_MagicGaugeEF[MAGIC_GAUGE_OBJ_MAX];
	bool m_bDrawFlag[MAGIC_GAUGE_OBJ_MAX];
	bool m_bMagicFade;
};

#endif