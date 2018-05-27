#pragma once
#ifndef COORDINATE_H_
#define COORDINATE_H_

#include "main.h"
#include "CModel.h"

class CCoordinate 
{
public:
	typedef enum 
	{
		OBJ_X_AXIS,
		OBJ_Y_AXIS,
		OBJ_Z_AXIS,
		OBJ_BOX,
		OBJ_MAX
	}OBJ_NAME;

	CCoordinate();
	~CCoordinate();

	void Init(const D3DXVECTOR3& pos);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	//セッター
	void SetPos(const D3DXVECTOR3& pos)		{ m_pos = pos; }
	void SetScl(const D3DXVECTOR3& scl)		{ m_scl = scl; }
	void SetDrawUse(bool DrawUse);
	void SetMaterial(int name,const D3DMATERIAL9& Mtrl)	{ m_Obj[name].SetMaterial(Mtrl); }
	void SetDrawType(int name,CModel::DRAW_TYPE DrawType)	{ m_Obj[name].SetDrawType(DrawType); }

	//ゲッター
	LPD3DXMESH GetMesh(int name)	{ return m_Obj[name].GetMesh(); }
	D3DXMATRIX GetMatrix(int name)		{ return m_Obj[name].GetResultMtx(); }
	D3DXVECTOR3 GetMoveDist (void){ return m_MoveDist; }
	D3DXVECTOR3 GetPos(void) { return m_pos; }

	//他の関数
	void AddPosX(float Value) { m_pos.x += Value; }
	void AddPosZ(float Value) { m_pos.z += Value; }

private:
	D3DXVECTOR3		m_pos;
	D3DXVECTOR3		m_posOld;		//古い座標
	D3DXVECTOR3		m_MoveDist;		//移動距離
	D3DXVECTOR3		m_scl;
	CModel			m_Obj[OBJ_MAX];
	bool			m_DrawUse;
};

#endif