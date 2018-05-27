/*==============================================================================
�w�b�_����:
	�X�e���V���V���h�E�̃w�b�_
===============================================================================*/
#ifndef C_STENCIL_SHADOW_H_
#define C_STENCIL_SHADOW_H_

/*==============================================================================
�C���N���[�h�w�b�_
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

	//�Z�b�^�[
	void SetModelPos(const D3DXVECTOR3& pos) { m_Model.SetPosition(pos); }
private:
	CModel		m_Model;
	CScene2D	m_CScene2D;
};

#endif