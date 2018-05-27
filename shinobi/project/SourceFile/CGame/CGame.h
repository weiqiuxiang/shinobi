#pragma once
#ifndef _CGAME_H_
#define _CGAME_H_

//インクルードヘッダ
#include "SceneManager.h"
#include "CLockOnEF.h"
#include "CSkyDome.h"
#include "CFeedBackBlur.h"
#include "CMeshDataIO.h"
#include "C_Obj3D_DataIO.h"
#include "CCameraLight.h"
#include "CRenderTarget2D.h"

//先方宣言
class CGameUI;
class CPlayerControl;
class CPlayer;

//クラス
class CGame : public CSceneManager
{
public:
	typedef enum {
		TRA_TO_NONE = 0,
		TRA_TO_TITLE,
		TRA_TO_RESULT,
	}SCENE_TRA;

	CGame();
	~CGame();

	bool Init(HWND hWnd);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void DrawInRenderTarget(void);
	void DrawInDepthRenderTarget(D3DXMATRIX ViewMtx, D3DXMATRIX ProjeMtx);
	void DrawSceneUseDepthMap(void);

	void UpdateLockOnEF(void);

	//セッター
	void SceneTraDecision(void);
private:
	void InitStatic(void);		//static型のオブジェクトの初期化
	void InitLight(void);		//ライトの初期化
	void InitEnemy(void);		//敵の初期化

private:
	SCENE_TRA		m_TraFlag;				//シーン遷移フラグ
	CGameUI*		m_GameUI;				//ゲームUI
	CPlayer*		m_Player;				//プレーヤー
	CPlayerControl*	m_PlayerControl;		//プレーヤーコントローラ
	CMeshDataIO		m_MeshLoader;			//メッシュ読み込み役
	C_Obj3D_DataIO	m_ModelLoader;			//モデル読み込み役
	CLockOnEF		m_LockOnEF;				//ロックオン表示
	CSkyDome		m_SkyDoom;				//スカイドーム1
	CSkyDome		m_SkyDoom2;				//スカイドーム2
	CCameraLight	m_CameraLight;			//ライト側のカメラ
	int				m_GoToResultTime;		//敵を全倒すの後のリザルトに行く時間

	CRenderTarget2D m_SceneRenderTarget;			//シーンのレンダーターゲット
};

#endif