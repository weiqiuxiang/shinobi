//ヘッダ説明 : AIの巡回ポイントの描画ユニット
#ifndef CCUBE_H_
#define CCUBE_H_

#include "main.h"

class CCube
{
public:
	CCube();
	~CCube();

	HRESULT Init(float X,float Y,float Z);
	void Update(void);
	void Draw(void);					//デバックモードのみ描画する
	void DrawSelectedByEditor(void);	//エディター用描画
	void Uninit(void);

	//ゲッター
	D3DXVECTOR3 GetPos(void) { return m_pos; }
	LPD3DXMESH GetMesh(void) { return m_pMesh; }
	D3DXMATRIX GetWorldMtx(void) { return m_WorldMtx; }

	//セッター
	void SetPos(const D3DXVECTOR3& pos) { m_pos = pos; }

private:
	//メンバ関数
	void MtxWorldSetting(void);

private:
	//メンバ変数
	D3DXVECTOR3 m_pos;						//位置
	D3DXVECTOR3 m_scl;						//スケール
	D3DXMATRIX	m_WorldMtx;					//ワールドマトリクス
	float		m_X, m_Y, m_Z;				//キューブのサイズ
	LPD3DXMESH	m_pMesh;						//メッシュポインタ
	D3DMATERIAL9 m_Mtrl;					//マテリアル
	D3DMATERIAL9 m_MtrlSelectedByEditor;	//エディター時、選択された状態のマテリアル
};

#endif