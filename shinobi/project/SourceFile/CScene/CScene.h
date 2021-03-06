#pragma once
#ifndef _CSCENE_H_
#define _CSCENE_H_

#include "main.h"

class CScene
{
public:
	CScene();
	~CScene();

	//Init,Uninit,Update,DrawÖ
	virtual HRESULT Init(void);
	virtual HRESULT Init(const D3DXVECTOR3 &Pos, const D3DXVECTOR3 &pSize , const char* filePass);
	virtual HRESULT Init(SRP &PosRotScl , const char* filePass);
	virtual void Uninit(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;

	//Qb^[
	D3DXVECTOR3 GetPosition(void);
	PRIORITY GetPriority(void) { return m_priority; }                     //`æDæxæ¾

	//Zb^[
	void SetPosition(const D3DXVECTOR3& pos) { m_pos = pos; }
	void SetPositionX(float Value) { m_pos.x = Value; }
	void SetPositionY(float Value) { m_pos.y = Value; }
	void SetPositionZ(float Value) { m_pos.z = Value; }
	void SetPriority(const PRIORITY priority) { m_priority = priority; }  //`æDæxÝè

	//¼ÌÖ
	void MoveX(float fDistance/*Ú®£*/) { m_pos.x += fDistance; }
	void MoveY(float fDistance/*Ú®£*/) { m_pos.y += fDistance; }
	void MoveZ(float fDistance/*Ú®£*/) { m_pos.z += fDistance; }
	void MoveXZ(const D3DXVECTOR2 &fDirection/*Ú®ûü*/, float fDistance/*Ú®£*/);
	void Move3D(const D3DXVECTOR3 &fDirection/*Ú®ûü*/, float fDistance/*Ú®£*/);
protected:
	D3DXVECTOR3 m_pos;
	PRIORITY m_priority;
};

#endif