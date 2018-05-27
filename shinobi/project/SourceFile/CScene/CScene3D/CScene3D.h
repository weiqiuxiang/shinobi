#pragma once
#ifndef _CSCENE3D_H_
#define _CSCENE3D_H_

#include "main.h"
#include "CScene.h"
#include "CScene3D_Draw.h"

class CScene3D : public CScene
{
public:
	typedef enum
	{
		TYPE_NORMAL = 0,
		TYPE_BILLBOARD,
		TYPE_BILLBOARD_BOTTOM
	}CSCENE3D_TYPE;

	//メンバ関数
	CScene3D();
	CScene3D(CScene3D_Draw *pDrawType);
	CScene3D(const SRP &worldMtx, const D3DXVECTOR3 &pSize);
	~CScene3D();

	//Init,Update,Draw,Uninit関数
	virtual HRESULT Init(const SRP &worldMtx, const D3DXVECTOR3 &pSize, const char *filePass);
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
	void SetDrawType(CSCENE3D_TYPE drawType);

	//ゲッター
	int GetLayerA(void) { return m_RGBA.a; }       //ALPHAレイヤーの値取得
	D3DXVECTOR3 GetSize(void) { return m_size; }
	CSCENE3D_TYPE GetType(void) { return m_Type; }
	TEX_ID GetTexID(void) { return *m_TexID; }
	SRP GetWorldSRP(void) { SRP SclRotPos = {m_pos,m_rot,m_scl}; return SclRotPos; }
	const D3DXMATRIX* GetWorldMtx(void) { return &m_WorldMtx; }

	//他の関数
	void AddPosition(const D3DXVECTOR3& Speed) { m_pos += Speed; }           //posを徐々変化させる

protected:
	//メンバ関数
	void SetRenderStates(void);          //レンダーステート諸設定

	//メンバ変数
	D3DXVECTOR3 m_rot;
	D3DXVECTOR3 m_scl;
	D3DXVECTOR3 m_size;
	RGBA		m_RGBA;
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuffer;
	CScene3D_Draw *m_DrawType;
	TEX_ID *m_TexID;
	CSCENE3D_TYPE m_Type;
	D3DXMATRIX m_WorldMtx;                         //ワールドマトリクス
};

#endif