#pragma once
#ifndef _CSCENE_H_
#define _CSCENE_H_

#include "main.h"

class CScene
{
public:
	CScene();
	~CScene();

	//Init,Uninit,Update,Draw関数
	virtual HRESULT Init(void);
	virtual HRESULT Init(const D3DXVECTOR3 &Pos, const D3DXVECTOR3 &pSize , const char* filePass);
	virtual HRESULT Init(SRP &PosRotScl , const char* filePass);
	virtual void Uninit(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;

	//ゲッター
	D3DXVECTOR3 GetPosition(void);
	PRIORITY GetPriority(void) { return m_priority; }                     //描画優先度取得

	//セッター
	void SetPosition(const D3DXVECTOR3& pos) { m_pos = pos; }
	void SetPositionX(float Value) { m_pos.x = Value; }
	void SetPositionY(float Value) { m_pos.y = Value; }
	void SetPositionZ(float Value) { m_pos.z = Value; }
	void SetPriority(const PRIORITY priority) { m_priority = priority; }  //描画優先度設定

	//他の関数
	void MoveX(float fDistance/*移動距離*/) { m_pos.x += fDistance; }
	void MoveY(float fDistance/*移動距離*/) { m_pos.y += fDistance; }
	void MoveZ(float fDistance/*移動距離*/) { m_pos.z += fDistance; }
	void MoveXZ(const D3DXVECTOR2 &fDirection/*移動方向*/, float fDistance/*移動距離*/);
	void Move3D(const D3DXVECTOR3 &fDirection/*移動方向*/, float fDistance/*移動距離*/);
protected:
	D3DXVECTOR3 m_pos;
	PRIORITY m_priority;
};

#endif