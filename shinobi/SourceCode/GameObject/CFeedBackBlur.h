//説明: フィードバックブラー
#ifndef CFEEDBACKBLUR_H_
#define CFEEDBACKBLUR_H_

#include "CRenderTarget2D.h"

//先方宣言
class CGame;

class CFeedBackBlur
{
public:
	CFeedBackBlur();
	~CFeedBackBlur() {}

	bool Init(void);
	void DrawBlurOnDrawTarget(CGame*);	//ブラーターゲットに描画
	void Draw(void);
	void Uninit(void);

	LPDIRECT3DSURFACE9 GetDrawGameSurfacePointer(void) { return m_RenderTargetDrawInGame.GetSurfacePointer(); }

private:
	D3DXVECTOR3			m_TargetBlurPolygonSize;	//ブラーサイズ
	int					m_BlurCount;				//カウント
	CRenderTarget2D		m_RenderTargetBlur;			//ブラー用
	CRenderTarget2D		m_RenderTargetDrawInGame;	//ゲームの中に描画用
};

#endif