//ヒットポイント(HP)をビルボードの形式(3Dポリゴン)で表現する
#ifndef CLIFE_GAUGE_H_
#define CLIFE_GAUGE_H_

#include "main.h"
#include "CScene3D.h"

class CLifeGauge
{
public:
	CLifeGauge();
	~CLifeGauge();

	bool Init(const D3DXVECTOR3& pos, const D3DXVECTOR3& size, float TotalLife);
	void Update(void);
	void Draw(void);
	void Uninit(void);

	//セッター
	void SetTotalLife(float TotalLife) { m_TotalLife = TotalLife; }
	void SetWorldPos(const D3DXVECTOR3& pos) { m_pos = pos; }

	//ゲッター
	D3DXMATRIX GetWorldMtx(void) { return m_WorldMtx; }
	void GetEnemyLife(float Life);

	//他の関数
	void AddDamageLife(float life);

private:
	void LockVertexBuf(float LeftLifeRightSide,float DamageLifeWidth);
	void SetWorldMtxBillBoard(void);

private:
	//テクスチャIDの名前
	typedef enum
	{
		TEX_WINDOW = 0,
		TEX_GAUGE,
		TEX_DAMAGE_GAUGE,
		TEX_ID_MAX
	}LIFE_TEX_ID;

private:
	//描画関係
	D3DXVECTOR3 m_pos;
	D3DXVECTOR3 m_LifeGaugeSize;	//描画時のサイズ
	D3DXVECTOR3 m_LifeWindowSize;	//ゲージを囲む枠のサイズ

	//描画用頂点バッファ
	int						m_Alpha;
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBufferWindow;
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBufferLeftGauge;
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBufferDamageGauge;

	//ライフ関係
	float	m_TotalLife;	//トータルライフ
	float	m_LeftLife;	//残りライフ
	float	m_DamageLife;	//削られたライフ
	int		m_DamageShowTime;

	//ワールド行列
	D3DXMATRIX m_WorldMtx;

	//テクスチャのID
	int m_TexID[TEX_ID_MAX];
};

#endif