//説明 : エルミート曲線の2DUI、曲線補間もしくはミサイルの弾道に使う
#pragma once
#ifndef HERMITIAN_H_
#define HERMITIAN_H_

#include "main.h"
#include "CFontUIScale.h"
#include <vector>

class CHermitian
{
public:
	typedef enum {
		LOCK_NONE = 0,
		LOCK_WINDOW,
		LOCK_START,
		LOCK_END
	}MOUSE_IN_OBJ;

	CHermitian();
	~CHermitian();

	void Init(const D3DXVECTOR2& DrawPos, const D3DXVECTOR2& StartDir, const D3DXVECTOR2& EndDir);
	void Update(void);
	void Draw(void);
	void Uninit(void);

	//ゲッター
	bool GetUsing(void) { return m_bUse; }
	bool GetMouseUsing(void) { return m_MouseUsing; }	//マウスが使われているフラグ
	D3DXVECTOR2 GetStartDir(void) { return m_StartDir; }
	D3DXVECTOR2 GetEndDir(void) { return m_EndDir; }

	bool IsDragedDragPoint(void);						//ドラックポイントを操作したか


	//セッター
	void SetUsing(bool bUse) { m_bUse = bUse; }
	void SetPreview(bool bUse) { m_bPreView = bUse; }
	void SetStartDir(const D3DXVECTOR2& StartDir);
	void SetEndDir(const D3DXVECTOR2& EndDir);

	//他の関数
	void CurveReset(void);

public:
	static float GetChangeValuePercent(const D3DXVECTOR2& StartDir, const D3DXVECTOR2& EndDir, float t); //変化量のパーセンテージ取得

private:
	//関数
	void UpdateCoordinate(void);
	void UpdateCurve(void);			//曲線のアップデートの部分

	void DrawCoordinate(void);		//座標軸描画
	void DrawHermitianCurve(void);	//エルミート曲線描画
	void DrawVectorPoint(void);		//始点終点の方向ベクトルの終点の位置描画

	void MouseControl(void);
	void LimitedPosition(void);		//位置制限
	void MouseHover(const D3DXVECTOR3& MousePos, MOUSE_IN_OBJ *pOut);
	void MousePress(const D3DXVECTOR3& MousePos, MOUSE_IN_OBJ PressFlags);
private:
	//データ部変数
	D3DXVECTOR2 m_DrawPos;			//曲線全体の描画座標(左上)
	D3DXVECTOR2 m_StartPos;			//曲線原点の絶対座標
	D3DXVECTOR2 m_EndPos;			//曲線終点の絶対座標
	D3DXVECTOR2 m_StartDir;			//曲線始点方向ベクトル
	D3DXVECTOR2 m_EndDir;			//曲線終点方向ベクトル
	bool		m_MouseUsing;		//マウス翳すフラグ
	MOUSE_IN_OBJ	m_MouseHoverFlags;		//マウス翳す
	MOUSE_IN_OBJ	m_MousePressFlags;		//マウスクリック
	MOUSE_IN_OBJ	m_MousePressFlagsOld;	//前フレームのマウスクリックフラグ
	D3DXVECTOR2		m_MousePressPos;
	bool			m_bUse;		//描画と更新をするフラグ
	bool			m_bPreView;	//trueなら曲線の操作ができない

	//描画部変数
	std::vector<D3DXVECTOR2> m_CurvePointList;		//曲線を描画用頂点データ
	CScene2D	*m_Background;		//背景
	CScene2D	*m_StartDirPoint;	//始点方向ベクトルのポイント
	CScene2D	*m_EndDirPoint;		//終点方向ベクトルのポイント
};

#endif