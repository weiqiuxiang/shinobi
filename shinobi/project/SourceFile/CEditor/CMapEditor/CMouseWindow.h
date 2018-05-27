#ifndef CMOUSE_WINDOW_H_
#define CMOUSE_WINDOW_H_

#include "main.h"

class CMouseWindow 
{
public:
	CMouseWindow();
	~CMouseWindow();

	bool Init(const RGBA& color,float FrameWidht);
	void UpdateDragWindow(bool IsMousePress,const D3DXVECTOR2& MousePos);
	void Uninit(void);
	void DrawWindow(void);

	//ゲッター
	D3DXVECTOR2 GetStartPoint(void) { return m_StartPoint; }
	D3DXVECTOR2 GetEndPoint(void) { return m_EndPoint; }
	bool GetUsingFlag(void) { return m_UsingMouseOld; }

private:
	void DrawLine(void);				//枠線描画
	void LockVertex(void);				//頂点ロック
	void CalcStartEndPoint(void);		

private:
	bool			m_DrawFlag;				//描画フラグ
	bool			m_UsingMouseOld;		//マウス使用古いフラグ
	D3DXVECTOR2		m_PosStart;				//マウスの開始座標
	D3DXVECTOR2		m_PosEnd;				//マウスの終了座標
	D3DXVECTOR2		m_StartPoint;			//開始座標
	D3DXVECTOR2		m_EndPoint;				//終了座標

	//ウィンドウパラメータ
	RGBA			m_RGBA;					//ウィンドウの色
	float			m_FrameWidht;			//枠の太さ

	LPDIRECT3DVERTEXBUFFER9		m_pVtxBuffer;	//頂点バッファ(ウィンドウ描画用)
};

#endif