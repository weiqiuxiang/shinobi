#pragma once
#ifndef _CSCENE2D_H_
#define _CSCENE2D_H_

#include "main.h"
#include "CScene.h"

class CScene2D : public CScene
{
public:
	//メンバ関数
	CScene2D();
	CScene2D(const D3DXVECTOR3 &Pos, const D3DXVECTOR3 &pSize);
	~CScene2D();

	//Init,Update,Draw,Uninit関数
	virtual HRESULT Init(const D3DXVECTOR3 &Pos, const D3DXVECTOR3 &pSize, const char *filePass);
	virtual void Update(void);
	virtual void Draw(void);
	virtual void Uninit(void);

	//セッター
	void SetSize(const D3DXVECTOR3& size) { m_size = size; }
	void SetLayerR(int Value);                     //REDレイヤーの値設定
	void SetLayerG(int Value);                     //GREENレイヤーの値設定
	void SetLayerB(int Value);                     //BLUEレイヤーの値設定
	void SetLayerA(int Value);                     //ALPHAレイヤーの値設定
	void SetRGBA(const RGBA& rgba);                       //RGBA一括設定
	void ChangeAlpha(int ChangeValue);             //ALPHAレイヤーの値を徐々変更させる
	void SetUV(const D3DXVECTOR2& UV) { m_UV = UV; }
	void SetUVWH(const D3DXVECTOR2& UVWH) { m_UVWH = UVWH; }

	//ゲッター
	int GetLayerA(void) { return m_RGBA.a; }       //ALPHAレイヤーの値取得
	D3DXVECTOR3 GetSize(void) { return m_size;}

	//他の関数
	void AddLayerA(int Value) { 
		m_RGBA.a += Value; 
		m_RGBA.a = min(255, m_RGBA.a);
		m_RGBA.a = max(0,m_RGBA.a);
	}
	void AddPosition(const D3DXVECTOR3& AddPos) { m_pos += AddPos; }

protected:
	//メンバ関数
	void SetRenderStates(void);				//レンダーステート諸設定
	void VertexBufferLock(void);			//頂点ロック
	void VertexBufferLockUV(void);			//頂点ロック

	//メンバ変数
	D3DXVECTOR2 m_UV;
	D3DXVECTOR2 m_UVWH;
	D3DXVECTOR3 m_size;
	RGBA m_RGBA;
	TEX_ID *m_TexID;
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;
};

#endif