#pragma once
#ifndef RAY_PICK_TO_MESH_H_
#define RAY_PICK_TO_MESH_H_

#include "main.h"

class RayPickToMesh 
{
private:
	//..�C���f�b�N�X�o�b�t�@�p�\���̂̐錾
	struct _XFileIndexBuffer
	{
		short p[3];
	};

	//..���_�o�b�t�@�p�\���̂̐錾
	struct _XFileVertexBuffer_p_n
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR3 normal;
	};
	struct _XFileVertexBuffer_p_n_t
	{
		D3DXVECTOR3 pos;
		D3DXVECTOR3 normal;
		float		tu, tv;
	};
public:
	static bool RaypicktoMesh(bool moveflag, D3DXVECTOR3 *resultpos, D3DXVECTOR3 pos_before, D3DXVECTOR3 pos_after, LPD3DXMESH mesh, D3DXMATRIX matrix);
	static bool RaypicktoMesh(bool moveflag, D3DXVECTOR3 *resultpos, D3DXVECTOR3 pos, LPD3DXMESH mesh, D3DXMATRIX matrix_befoe, D3DXMATRIX matrix_afiter);
};

#endif

