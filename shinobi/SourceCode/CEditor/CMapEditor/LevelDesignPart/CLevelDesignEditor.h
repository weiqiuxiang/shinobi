#ifndef OLEVEL_DESIGN_EDITOR_H_
#define OLEVEL_DESIGN_EDITOR_H_

#include "CLevelDesignUI.h"
#include "CEnemy.h"
#include "CPlayer.h"
#include "CMouseEditMeshField.h"
#include "CCoordinate.h"
#include "CPathPoint.h"

class CLevelDesignEditor
{
public:
	CLevelDesignEditor();
	~CLevelDesignEditor();

	typedef enum 
	{
		SET_CREATURE = 0,
		MOVE_ROT_CREATURE
	}SET_CREATURE_MODE;

	typedef enum
	{
		SELECT_CREATURE = 0,
		SELECT_CREATURE_PATH
	}SET_ENEMY_PATH_MODE;

	bool Init(HWND hWnd);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void DrawNavMesh(void);

private:
	void UpdateRightButtonClickTimer(void);	//右クリックのタイマー関連
	void ModeChangeInit(void);				//モード変更時の初期化
	void ModeAction(void);					//各モードの具体的な動作
	bool CreateCreature(void);				//敵のセット及び位置の移動、回転など
	bool MouseSelectEnemy(void);			//マウスで配置済みの敵を選択する
	void SetCreature(void);					//敵の設置
	void MoveCreature(void);				//敵の移動
	void CancelSelectEnemy(void);			//敵の選択状態を解除

	//座標系に関する関数
	void CoordinateAction(void);					//軸のアクション
	void CoordinateAxisSelect(void);				//座標移動カーソルの軸選択
	void SelectedAxisAction(void);					//選択された軸のアクション

	//巡回ポイントに関する関数
	bool SelectPathPoint(void);				//巡回ポイントの巡回
	void MovePathPoint(void);				//巡回ポイントの移動
	void SetPathPointStopFrame(void);		//巡回ポイントの停止フレームを設定
	void AddPathPoint(void);				//巡回ポイントの追加
	bool DeletePathPoint(void);				//巡回ポイントの削除
	void InsertPathPoint(void);				//巡回ポイントの挿入

private:
	CMouseEditMeshField		m_MouseEditMeshField;
	CLevelDesignUI			m_UI;
	CEnemy*					m_pEnemy;
	CPathPoint*				m_pPathPoint;
	CPlayer					m_PlayerInstance;
	int						m_MouseRightButtonFrameTime;//右ボタンを押しっぱなしの時,時刻を保存用変数
	int						m_MouseRightButtonPushTime;	//右ボタンを押しっぱなしの累積時間

	//マテリアル
	D3DMATERIAL9			m_SelectObjMtrl;			//選択しているオブジェクトのマテリアル
	D3DMATERIAL9			m_MouseInAxisMtrl;			//マウスを翳している軸のマテリアル
	D3DMATERIAL9			m_SelectAxisMtrl;			//マウスでドラックしている軸のマテリアル

	//座標軸に関するもの
	CCoordinate				m_Coordinate;			//座標系オブジェクト
	int						m_AxisSelected;				//選択された軸番号

	//子モード
	SET_CREATURE_MODE		m_SetCreatureMode;				//子モード
	SET_ENEMY_PATH_MODE		m_SetEnemyPathMode;				//子モード
};

#endif