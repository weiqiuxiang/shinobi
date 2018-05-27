#pragma once
#ifndef CCOLLISION_ALL_OBJ3D_HIT_MESH
#define CCOLLISION_ALL_OBJ3D_HIT_MESH

#include "CScene3D.h"
#include "CModel.h"

class CAllObj3DHitMesh
{
public:
	static void ColliAllObj3DAndAllMesh(void);
	static void CScene3DHitAllMesh(CScene3D* pCScene3D);
	static void ModelHitAllMesh(CModel* pModel);
private:
};

#endif