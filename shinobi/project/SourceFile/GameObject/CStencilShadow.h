/*==============================================================================
ヘッダ説明:
	ステンシルシャドウのヘッダ
===============================================================================*/
#ifndef C_STENCIL_SHADOW_H_
#define C_STENCIL_SHADOW_H_

/*==============================================================================
インクルードヘッダ
===============================================================================*/
#include "main.h"
#include "CModel.h"
#include "CScene2D.h"

class CStencilShadow
{
public:
	CStencilShadow();
	~CStencilShadow();

	HRESULT Init(const SRP& SclRotPos, const char* ModelFilepass);
	void Update(void);
	void Draw(void);
	void Uninit(void);

	//セッター
	void SetModelPos(const D3DXVECTOR3& pos) { m_Model.SetPosition(pos); }
private:
	CModel		m_Model;
	CScene2D	m_CScene2D;
};

#endif