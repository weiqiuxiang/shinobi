#pragma once
#ifndef _CMODEL_FACTORY_H_
#define _CMODEL_FACTORY_H_

#include "main.h"
#include "CModel.h"

class CModelFactory
{
public:
	static CModel* Create(const SRP& SclRotPos, const char* ModelFilepass) {
		CModel* pModel;
		pModel = new CModel();
		pModel->Init(SclRotPos, ModelFilepass);
		return pModel;
	}
};

#endif