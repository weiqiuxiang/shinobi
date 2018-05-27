#pragma once
#ifndef CMESHFIELD_MANAGER_H_
#define CMESHFIELD_MANAGER_H_

#include "main.h"
#include "CINDEX_SIZE.h"
#include "CMeshField.h"

#define MESH_MAX (100)

class CMeshFieldManager
{
public:
	static void InitAll(void);
	static void UninitAll(void);
	static void UpdateAll(void);
	static void DrawAll(void);
	static void DrawAllOnDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje);
	static void DrawUseDepthMap(const D3DXMATRIX& LightView, const D3DXMATRIX& LightProje,LPDIRECT3DTEXTURE9 DepthTexture);

	//ÉQÉbÉ^Å[
	static CMeshField* GetMesh(int nMeshNumber);

	//ëºÇÃä÷êî
	static CMeshField* CreateMeshField(const D3DXVECTOR3& SclRotPos, const CINDEX_SIZE& IndexSize, const char *TexFilePass);
	static float CalcPosYValueInMeshField(const D3DXVECTOR3& pos);

private:
	static CMeshField *m_MeshField[MESH_MAX];
};

#endif