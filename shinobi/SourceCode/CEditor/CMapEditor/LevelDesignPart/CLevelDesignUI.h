/*==============================================================================
	ファイル説明:
		マップエディターのUI
===============================================================================*/
#ifndef LEVEL_DESIGN_UI_H_
#define LEVEL_DESIGN_UI_H_

/*==============================================================================
	インクルードヘッダ
===============================================================================*/
#include "main.h"
#include "CModel.h"
#include "CRenderTarget2D.h"

/*==============================================================================
	マクロ
===============================================================================*/
#define NO_PUSH_ANY_IMAGE_BUTTON (-1)

/*==============================================================================
	前方宣言
===============================================================================*/
class CHuman;

/*==============================================================================
	クラス定義
===============================================================================*/
class CLevelDesignUI 
{
public:
	CLevelDesignUI();
	~CLevelDesignUI();

	typedef enum
	{
		SET_CREATURE = 0,
		SET_ENEMY_PATH,
		SET_NAV_MESH
	}MODE;

	bool Init(void);		//初期化処理
	void Update(void);		//更新処理
	void Uninit(void);		//終了処理

	bool AddPathPointUI(void);
	bool DeletePathPointUI(void);
	bool InsertPathPointUI(void);

	//セッター
	bool SetIntputStopFrame(int* pStopFrameNow);

	//ゲッター
	MODE GetMode(void) { return m_Mode; }
	int GetSelectEnemyType(void);
	
	bool CLevelDesignUI::SaveEnemyUI(void);
private:
	void ModeSelectUI(void);
	void EnemyTypeUI(void);

private:
	int				m_PushEnemyButtonNumber;	//押された敵ボタンの番号
	MODE			m_Mode;						//実行中のモード
};

#endif