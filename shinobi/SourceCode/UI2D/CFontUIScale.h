#pragma once
#ifndef _CSCALE_UI_H_
#define _CSCALE_UI_H_

#include "main.h"
#include "CScene2D.h"

//前方宣言
class CScene2D;

class CFontUIScale : public CScene2D
{
public:
	CFontUIScale();
	~CFontUIScale();
	HRESULT Init(const D3DXVECTOR3 &Pos, const D3DXVECTOR3 &pSize, const char *filePass) override;
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	//ゲッター

	//セッター
	void SetScale(float fScale) { m_Scale = D3DXVECTOR2(max(0.01f, fScale), max(0.01f, fScale)); }
	void SetScaleX(float fScale) { m_Scale.x = max(0.01f, fScale); }
	void SetScaleY(float fScale) { m_Scale.y = max(0.01f, fScale); }
	void SetOriginPos(const D3DXVECTOR3& pos) { m_OriginPos = pos; }
	void SetScalePoint(const D3DXVECTOR3 &ScalePoint) { m_ScalePoint = ScalePoint; }
	void SetAddBlending(bool bAddBlending) { m_bAddBlending = bAddBlending; }

private:
	//スケールのための変数
	D3DXVECTOR3 m_ScalePoint;				//スケール中心
	D3DXVECTOR3 m_OriginPos;				//元の位置
	D3DXVECTOR3 m_OriginSize;				//元のサイズ
	D3DXVECTOR2 m_Scale;							//スケール倍率
	bool m_bAddBlending;						//アルファ加算
};

#endif