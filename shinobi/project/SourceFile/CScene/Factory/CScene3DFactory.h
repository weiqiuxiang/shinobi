#pragma once
#ifndef _CSCENE3D_FACTORY_H_
#define _CSCENE3D_FACTORY_H_

#include "main.h"
#include "CScene3D.h"

class CScene3DFactory
{
public:
	static CScene3D* Create(const SRP &worldMtx, const D3DXVECTOR3 &pSize, const CScene3D::CSCENE3D_TYPE CScene3DType, const char* Texfilepass) {
		CScene3D* pCScene3D;
		pCScene3D = new CScene3D();
		pCScene3D->Init(worldMtx, pSize, Texfilepass);
		pCScene3D->SetDrawType(CScene3DType);
		return pCScene3D;
	}
};

#endif