#pragma once
#ifndef _CSCALE_UI_FACTORY_H_
#define _CSCALE_UI_FACTORY_H_

#include "main.h"
#include "CFontUIScale.h"

class CFontUIScaleFactory
{
public:
	static CFontUIScale *Create(const D3DXVECTOR3 &pos, const D3DXVECTOR3 &size, const D3DXVECTOR3 &ScalePoint,const D3DXVECTOR2& UV, const D3DXVECTOR2& UVWH,const RGBA& rgba, bool bAddBlending, const char* filePass) {
		CFontUIScale *pScaleUI;
		pScaleUI = new CFontUIScale();
		pScaleUI->Init(pos, size, filePass);
		pScaleUI->SetRGBA(rgba);
		pScaleUI->SetAddBlending(bAddBlending);
		pScaleUI->SetScalePoint(ScalePoint);
		pScaleUI->SetUV(UV);
		pScaleUI->SetUVWH(UVWH);
		return pScaleUI;
	}
};

#endif